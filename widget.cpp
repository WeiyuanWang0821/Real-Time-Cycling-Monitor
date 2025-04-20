////////////////////////////////////////
// File: widget.cpp
// Description: Main Qt widget for real-time physiological data display.
//              Integrates O2, CO2, and flow sensors via multithreading.
//              Provides dynamic chart plotting and table interface.
////////////////////////////////////////

#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Initialize O2 sensor thread
    pO2 = new O2;                             // Allocate sensor handler
    pThread1 = new QThread(this);             // Create worker thread
    pO2->moveToThread(pThread1);              // Move handler to thread
    connect(pO2, &O2::signalsO2Data,
            this, &Widget::dealO2Fun);       // Sensor data -> handler
    connect(this, &Widget::startO2Thread,
            pO2, &O2::runO2ThreadFun);       // Start/stop signal

    // Initialize SFM3300 sensor thread
    pSFM3300 = new SFM3300;//Dynamic threads allocate space, can't specify a parent object
    pThread2 =new QThread(this);//Creating sub-threads
    pSFM3300->moveToThread(pThread2);//Adding custom threads to subthreads
    connect(pSFM3300,&SFM3300::signalsSFM3300Data ,this,&Widget::dealSFM3300Fun);//Connecting thread signals with main thread handler functions
    connect(this,&Widget::startSFM3300Thread,pSFM3300,&SFM3300::runSFM3300ThreadFun);//Connecting the main thread to start a sub-thread

    // Initialize SCD4X sensor thread
    pSCD4X = new SCD4X;//Dynamic threads allocate space, can't specify a parent object
    pThread3 =new QThread(this);//Creating sub-threads
    pSCD4X->moveToThread(pThread3);//Adding custom threads to subthreads
    connect(pSCD4X,&SCD4X::signalsSCD4XData ,this,&Widget::dealSCD4XFun);//Connecting thread signals with main thread handler functions
    connect(this,&Widget::startSCD4XThread,pSCD4X,&SCD4X::runSCD4XThreadFun);//Connecting the main thread to start a sub-thread


    // Get UI slider values
    mXCount = ui->horizontalSliderX->value();
    ui->lineEditX->setText(QString::number(mXCount));
    mYLCount = ui->verticalSliderYL->value();
    mYRCount = ui->verticalSliderYR->value();

    QFont legendFont;
    legendFont.setPointSize(11);              // Legend font size

    m_Series.Time = 1000;                     // Data interval (ms)
    
    // Create chart series
    m_Series.O2C = new QSplineSeries();
    m_Series.CO2C = new QSplineSeries();
    m_Series.GF = new QSplineSeries();
    m_Series.VCO2 = new QSplineSeries();
    m_Series.VO2 = new QSplineSeries(); 
    m_Series.RER = new QSplineSeries();
    m_Series.VE = new QSplineSeries();
    
    // Setup chart
    pChart = new QChart();
    pChart->setTitle("");
    legendFont = pChart->legend()->font();
    legendFont.setPointSize(11);  // Set the font size to 11
    pChart->legend()->setFont(legendFont);

    // Configure X Axis (Time)
    pTimeAxis = new QDateTimeAxis();
    //pTimeAxis->setTitleText("Time");
    pTimeAxis->setFormat("HH:mm:ss");  // Set the time format to hours, minutes and seconds
    mStartTime = QDateTime::fromSecsSinceEpoch(0+16*3600);//
    pTimeAxis->setRange( mStartTime,  mStartTime.addSecs(mXCount)); // Only the most recent mXCount data points are displayed.
    pTimeAxis->setLabelsAngle(60);  // Set the rotation angle of the label
    //pTimeAxis->setTickCount(mXCount+1);// Set the number of labels on the axis
    pTimeAxis->setTickCount(11);// Set the number of labels on the axis
    QFont font = pTimeAxis->labelsFont();  // Get current font
    font.setPointSize(11);  // Set the font size to 11
    pTimeAxis->setLabelsFont(font);  // Apply new font settings
    pChart->addAxis(pTimeAxis, Qt::AlignBottom);

    // Configure Left Y Axis
    pDataAxis = new QValueAxis();
    pDataAxis->setTitleText("GF/VO2/VCO2/VE");
    pDataAxis->setLabelsAngle(90);
    font = pDataAxis->labelsFont();// Get current font
    font.setPointSize(11);// Set the font size to 11
    pDataAxis->setLabelsFont(font);// Apply new font settings
    pChart->addAxis(pDataAxis, Qt::AlignLeft);
    pDataAxis->setRange(0, mYLCount); // Setting range 0 - 1000
    
    // Configure Right Y Axis
    pDataAxis1 = new QValueAxis();
    pDataAxis1->setTitleText("O2C/CO2C");
    pDataAxis1->setLabelsAngle(90);
    font = pDataAxis1->labelsFont();// Get current font
    font.setPointSize(11);// Set the font size to 11
    pDataAxis1->setLabelsFont(font);// Apply new font settings
    pDataAxis1->setRange(0, mYRCount); // Setting range 0 - 100
    pChart->addAxis(pDataAxis1, Qt::AlignRight);

    // Setup chart view
    pChartView = new QChartView(pChart);
    pChartView->setRenderHint(QPainter::Antialiasing);
    ui->scrollArea->setWidget(pChartView);// Using pChartView as the content of a QScrollArea
    ui->scrollArea->setWidgetResizable(true);// Make pChartView resizable to fit the size of the scrolling area
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // Always show horizontal scrollbar

    // Set baseline gas concentration
    ui->doubleSpinBoxO2CI->setValue(21);
    ui->doubleSpinBoxCO2CI->setValue(4);
    mO2CI =  ui->doubleSpinBoxO2CI->value();
    mCO2CI =  ui->doubleSpinBoxCO2CI->value();

    // Initialize timer and data counters
    mTime = 0;
    pTimer = new QTimer(this);
    connect(pTimer, &QTimer::timeout, this, &Widget::updateData);

    // Configure data table
    ui->tableWidget->setRowCount(2);  // Setting 2 rows
    ui->tableWidget->setColumnCount(9);  // Setting 8 columns

    ui->tableWidget->verticalHeader()->setVisible(false);// Hide line number
    ui->tableWidget->horizontalHeader()->setVisible(false);// Hide column number

    // Make all columns stretch automatically to evenly distribute available space
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Adjusting line height to fit the new font size
    ui->tableWidget->resizeRowsToContents();
    //Setting the container height
    ui->tableWidget->setFixedHeight(ui->tableWidget->rowHeight(0)*2.1);

    // Setting up form content
     ui->tableWidget->setItem(0, 0, new QTableWidgetItem("O2C(%)"));
     ui->tableWidget->setItem(0, 1, new QTableWidgetItem("CO2C(%)"));
     ui->tableWidget->setItem(0, 2, new QTableWidgetItem("GF"));
     ui->tableWidget->setItem(0, 3, new QTableWidgetItem("VO2(mL/min)"));
     ui->tableWidget->setItem(0, 4, new QTableWidgetItem("VCO2(mL/min)"));
     ui->tableWidget->setItem(0, 5, new QTableWidgetItem("RER"));
     ui->tableWidget->setItem(0, 6, new QTableWidgetItem("VE(L/min)"));
     ui->tableWidget->setItem(0, 7, new QTableWidgetItem("VO2MAX"));
     ui->tableWidget->setItem(0, 8, new QTableWidgetItem("HR"));

     ui->tableWidget->setItem(1, 0, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 1, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 2, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 3, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 4, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 5, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 6, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 7, new QTableWidgetItem("0"));
     ui->tableWidget->setItem(1, 8, new QTableWidgetItem("0"));

     // Setting the content of each cell to be centered
     for (int row = 0; row < 2; ++row) {
         for (int col = 0; col <9; ++col) {
             QTableWidgetItem *item =  ui->tableWidget->item(row, col);
             item->setTextAlignment(Qt::AlignCenter);  // Setting Center Alignment
         }
     }
}

Widget::~Widget()
{
    stopO2ThreadFun();
    pThread1->quit();//Exit subthreads
    pThread1->wait();//Recycling resources

    stopSFM3300ThreadFun();
    pThread2->quit();//Exit subthreads
    pThread2->wait();//Recycling resources

    stopSCD4XThreadFun();
    pThread3->quit();//Exit subthreads
    pThread3->wait();//Recycling resources
    delete ui;
}


// Compute VO2 over time window
void Widget::getVO2(float *ret , float *o2c ,float *co2c ,float *gasflow)
{
    mVO2Z[mTime] =(*gasflow) * ( ui->doubleSpinBoxO2CI->value()- (*o2c)) /100 ;
    float sum = 0;
    if(mTime < 60){
        for (int var = 0; var < mTime+1; ++var) {
            sum +=mVO2Z[var];
        }
        *ret = sum;
    }
    else{
        for (int var = mTime-60; var < mTime+1; ++var) {
            sum += mVO2Z[var];
        }
        *ret = sum;
    }
}


// Compute VCO2 over time window
void Widget::getVCO2(float *ret ,float *o2c ,float *co2c ,float *gasflow)
{
    mVCO2Z[mTime] = (*gasflow) * ((*co2c) - ui->doubleSpinBoxCO2CI->value()) /100 ;
    float sum = 0;
    if(mTime < 60){
        for (int var = 0; var < mTime+1; ++var) {
            sum +=mVCO2Z[var];
        }
        *ret = sum;
    }
    else{
        for (int var = mTime-60; var < mTime+1; ++var) {
            sum +=mVCO2Z[var];
        }
        *ret = sum;
    }
}


// Compute RER
void Widget::getRER(float *ret ,float *vo2 ,float *vco2)
{
    *ret = (*vco2) * (*vo2);
}


// Compute VE (minute ventilation)
void Widget::getVE(float *ret ,float *gasflow)
{
    mVEZ[mTime] = (*gasflow);
    float sum = 0;
    if(mTime < 60){
        for (int var = 0; var < mTime+1; ++var) {
            sum +=mVEZ[var];
        }
        *ret = sum /500;
    }
    else{
        for (int var = mTime-60; var < mTime+1; ++var) {
            sum +=mVEZ[var];
        }
        *ret = sum /500;
    }
}

// O2 sensor thread control functions ------------------------------
void Widget::startO2ThreadFun()
{
    if(pThread1->isRunning()== true)
    {
        emit startO2Thread(true);
        return;
    }
    else
    {
        pThread1->start();
        emit startO2Thread(true);
        return;
    }
}

void Widget::stopO2ThreadFun()
{
    if(pThread1->isRunning()== true)
    {
        emit startO2Thread(false);
        return;
    }
}

void Widget::dealO2Fun(float data)
{
    qDebug() << "data = " <<data;
    mO2C = data;
}

// SFM3300 sensor thread control functions ----------------------------
void Widget::startSFM3300ThreadFun()
{
    if(pThread2->isRunning()== true)
    {
        emit startSFM3300Thread(true);
        return;
    }
    else
    {
        pThread2->start();
        emit startSFM3300Thread(true);
        return;
    }

}

void Widget::stopSFM3300ThreadFun()
{
    if(pThread2->isRunning()== true)
    {
        emit startSFM3300Thread(false);
        return;
    }
}

void Widget::dealSFM3300Fun(float data)
{
    qDebug() << "SFM3300data = " <<data;
    mGasFlow = data;
}

// SCD4X sensor thread control functions ------------------------------
void Widget::startSCD4XThreadFun()
{
    if(pThread3->isRunning()== true)
    {
        emit startSCD4XThread(true);
        return;
    }
    else
    {
        pThread3->start();
        emit startSCD4XThread(true);
        return;
    }
}

void Widget::stopSCD4XThreadFun()
{
    if(pThread3->isRunning()== true)
    {
        emit startSCD4XThread(false);
        return;
    }
}

void Widget::dealSCD4XFun(float data)
{
    qDebug() << "SCD4Xdata = " <<data;
    mCO2C = data;
}

// Timer slot: gather and update all data every interval ---------------------------
void Widget::updateData() {
    qDebug()<<"updateData";
    // Compute physiological metrics

    getVO2(&mVO2,&mO2C ,&mCO2C ,&mGasFlow);
    getVCO2(&mVCO2,&mO2C ,&mCO2C ,&mGasFlow);
    getRER(&mRER,&mVO2 ,&mVCO2);
    getVE(&mVE,&mGasFlow);

    // Track peak VO2
    if(mVO2MAX < mVO2)
    {
        mVO2MAX = mVO2;
    }

    // Update time axis range dynamically
    if(mTime > mXCount) // Dynamically update the horizontal coordinate range
    {
        pTimeAxis->setRange( mStartTime.addSecs(mTime - mXCount),  mStartTime.addSecs(mTime)); // Only the last 10 data points are displayed
    }
    else
    {
        pTimeAxis->setRange( mStartTime.addSecs(0),  mStartTime.addSecs(mXCount)); // Only the most recent data point is displayed
    }

    // Append new data points to each series
    m_Series.O2C->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mO2C );
    m_Series.CO2C->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mCO2C );
    m_Series.GF->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mGasFlow );
    m_Series.VO2->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVO2 );
    m_Series.VCO2->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVCO2 );
    m_Series.RER->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mRER );
    m_Series.VE->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVE );


    // Refresh table with formatted values
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem(QString::number(mO2C, 'g' ,3)));
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::number(mCO2C, 'g' ,3)));
    ui->tableWidget->setItem(1, 2, new QTableWidgetItem(QString::number(mGasFlow, 'g' ,3)));
    ui->tableWidget->setItem(1, 3, new QTableWidgetItem(QString::number(mVO2, 'g' ,3)));
    ui->tableWidget->setItem(1, 4, new QTableWidgetItem(QString::number(mVCO2, 'g' ,3)));
    ui->tableWidget->setItem(1, 5, new QTableWidgetItem(QString::number(mRER, 'g' ,3)));
    ui->tableWidget->setItem(1, 6, new QTableWidgetItem(QString::number(mVE, 'g' ,3)));
    ui->tableWidget->setItem(1, 7, new QTableWidgetItem(QString::number(mVO2MAX, 'g' ,3)));
    ui->tableWidget->setItem(1, 8, new QTableWidgetItem(QString::number(mHR, 'g' ,3)));

//    ui->tableWidget->setItem(1, 7, new QTableWidgetItem(QString::number(m_Series.O2C->at(mTime).y())));
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col <9; ++col) {
            QTableWidgetItem *item =  ui->tableWidget->item(row, col);
            item->setTextAlignment(Qt::AlignCenter);  // Setting Center Alignment
        }
    }

    mTime++;

}



void Widget::on_btnStart_clicked()// Start data updates
{
    if(m_Series.StartPauseStopFalg != 1 )// Only if not paused
    {
        m_Series.O2C->clear();
        m_Series.CO2C->clear();
        m_Series.GF->clear();
        m_Series.VCO2->clear();
        m_Series.VO2->clear();
        m_Series.RER->clear();
        m_Series.VE->clear();
        mTime = 0;
    }
    m_Series.StartPauseStopFalg = 0;
    pTimer->start(m_Series.Time);  // Trigger update every interval
    startO2ThreadFun();
    startSFM3300ThreadFun();
    startSCD4XThreadFun();
}

void Widget::on_btnPause_clicked()// Pause data updates
{
{
    pTimer->stop();
    m_Series.StartPauseStopFalg = 1;
}


void Widget::on_btnStop_clicked()// Stop data updates
{
    pTimer->stop();
    m_Series.StartPauseStopFalg = 2;
    stopO2ThreadFun();
    stopSFM3300ThreadFun();
    stopSCD4XThreadFun();
}

// Toggle VE curve
void Widget::on_checkBoxVE_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.VE->setName("VE");// Setting the curve name Chen
        m_Series.VE->setColor(QColor(128, 200, 110));//Setting the curve color
        pChart->addSeries(m_Series.VE);// Data writing to charts
        m_Series.VE->attachAxis(pTimeAxis);// Setting the curve X-axis
        m_Series.VE->attachAxis(pDataAxis);// Setting the curve Y-axis
    }
    else
    {
        pChart->removeSeries(m_Series.VE);//Delete Curve
    }
}

// Toggle O2C curve
void Widget::on_checkBoxO2C_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.O2C->setName("O2C");// Setting the curve name Chen
        m_Series.O2C->setColor(Qt::red);//Setting the curve color
        pChart->addSeries(m_Series.O2C);// Data writing to charts
        m_Series.O2C->attachAxis(pTimeAxis);// Setting the curve X-axis
        m_Series.O2C->attachAxis(pDataAxis1);// Setting the curve Y-axis
    }
    else
    {
        pChart->removeSeries(m_Series.O2C);//Delete Curve
    }
}

// Toggle CO2C curve
void Widget::on_checkBoxCO2C_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.CO2C->setName("CO2C");// Setting the curve name Chen
        m_Series.CO2C->setColor(Qt::blue);//Setting the curve color
        pChart->addSeries(m_Series.CO2C);// Data writing to charts
        m_Series.CO2C->attachAxis(pTimeAxis);// Setting the curve X-axis
        m_Series.CO2C->attachAxis(pDataAxis1);// Setting the curve Y-axis
    }
    else
    {
        pChart->removeSeries(m_Series.CO2C);//Delete Curve
    }
}

// Toggle GF curve
void Widget::on_checkBoxGF_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.GF->setName("GF");// Setting the curve name Chen
        m_Series.GF->setColor(Qt::green);// Setting the curve color
        pChart->addSeries(m_Series.GF);// Data writing to charts
        m_Series.GF->attachAxis(pTimeAxis);// Setting the curve X-axis
        m_Series.GF->attachAxis(pDataAxis);// Setting the curve Y-axis
    }
    else
    {
        pChart->removeSeries(m_Series.GF);//Delete Curve
    }
}

// Toggle VO2 curve
void Widget::on_checkBoxVO2_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.VO2->setName("VO2");// Setting the curve name Chen
        m_Series.VO2->setColor(Qt::magenta);//Setting the curve color
        pChart->addSeries(m_Series.VO2);// Data writing to charts
        m_Series.VO2->attachAxis(pTimeAxis);// Setting the curve X-axis
        m_Series.VO2->attachAxis(pDataAxis);// Setting the curve Y-axis
    }
    else
    {
        pChart->removeSeries(m_Series.VO2);//Delete Curve
    }
}

 // Toggle VCO2 curve
void Widget::on_checkBoxVCO2_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.VCO2->setName("VCO2");// Setting the curve name Chen
        m_Series.VCO2->setColor(QColor(128, 0, 128));//Setting the curve color
        pChart->addSeries(m_Series.VCO2);// Data writing to charts
        m_Series.VCO2->attachAxis(pTimeAxis);// Setting the curve X-axis
        m_Series.VCO2->attachAxis(pDataAxis);// Setting the curve Y-axis
    }
    else
    {
        pChart->removeSeries(m_Series.VCO2);//Delete Curve
    }
}

// Toggle RER curve
void Widget::on_checkBoxRER_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.RER->setName("RER");// Setting the curve name Chen
        m_Series.RER->setColor(Qt::yellow);//Setting the curve color
        pChart->addSeries(m_Series.RER);// Data writing to charts
        m_Series.RER->attachAxis(pTimeAxis);// Setting the curve X-axis
        m_Series.RER->attachAxis(pDataAxis);// Setting the curve Y-axis
    }
    else
    {
        pChart->removeSeries(m_Series.RER);//Delete Curve
    }
}

// O2 calibration input
void Widget::on_doubleSpinBoxO2CI_valueChanged(double arg1)
{
    mO2CI =  arg1;
}

// CO2 calibration input
void Widget::on_doubleSpinBoxCO2CI_valueChanged(double arg1)
{
    mCO2CI =  arg1;
}

// Adjust time window
void Widget::on_horizontalSliderX_valueChanged(int value)
{
   if(mXCount == value)
       return;
   mXCount = value;
   ui->lineEditX->setText(QString::number(mXCount));
   if(mTime > mXCount) // Dynamically update the horizontal coordinate range
   {
       pTimeAxis->setRange( mStartTime.addSecs(mTime - mXCount),  mStartTime.addSecs(mTime)); // Only the most recent data point is displayed
   }
   else
   {
       pTimeAxis->setRange( mStartTime.addSecs(0),  mStartTime.addSecs(mXCount)); // Only the most recent data point is displayed
   }
}

// Adjust left Y-axis range
void Widget::on_verticalSliderYL_valueChanged(int value)
{
    pDataAxis->setRange(0, value); // Setting range
}

// Adjust right Y-axis range
void Widget::on_verticalSliderYR_valueChanged(int value)
{
    pDataAxis1->setRange(0, value); // Setting range
}

