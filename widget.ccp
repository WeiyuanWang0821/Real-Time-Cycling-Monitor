#include "widget.h"
 #include "ui_widget.h"
 
 
 
 
 Widget::Widget(QWidget *parent)
     : QWidget(parent)
     , ui(new Ui::Widget)
 {
     ui->setupUi(this);
 
     mXCount = ui->horizontalSliderX->value();
     ui->lineEditX->setText(QString::number(mXCount));
 
     mYLCount = ui->verticalSliderYL->value();
 
     mYRCount = ui->verticalSliderYR->value();
 
     pIICThread = new IICThread;//Dynamic thread allocation space, parent object cannot be specified
     pThread2 =new QThread(this);//Creating a child thread
     pIICThread->moveToThread(pThread2);//Add the custom thread to the child thread
     connect(pIICThread,&IICThread::signalsIICThread ,this,&Widget::dealIICThreadFun);//Connect thread signal with main thread processing function
     connect(this,&Widget::startIICThread,pIICThread,&IICThread::runIICThreadFun);//Connect to the main thread to start the child thread
 
     QFont legendFont;
     legendFont.setPointSize(11);  //Set the font size to 11
 
     m_Series.Time =1000;//Collection interval 1000ms
 
     m_Series.O2C = new QSplineSeries();// Create Curve
     m_Series.CO2C = new QSplineSeries();// Create Curve
     m_Series.GF = new QSplineSeries();// Create Curve
     m_Series.VCO2 = new QSplineSeries();// Create Curve
     m_Series.VO2 = new QSplineSeries();// Create Curve
     m_Series.RER = new QSplineSeries();// Create Curve
     m_Series.VE = new QSplineSeries();// Create Curve
 
     pChart = new QChart();// Create a chart
     pChart->setTitle("");// Set Title
 
     // Set the legend font size
     legendFont = pChart->legend()->font();
     legendFont.setPointSize(11);  //Set the font size  11
     pChart->legend()->setFont(legendFont);
 
     // set up X Axis: Time (seconds)
     pTimeAxis = new QDateTimeAxis();
     //pTimeAxis->setTitleText("Time");
     pTimeAxis->setFormat("HH:mm:ss");  // Set the time format to hours, minutes, and seconds
     mStartTime = QDateTime::fromSecsSinceEpoch(0+16*3600);//
     pTimeAxis->setRange( mStartTime,  mStartTime.addSecs(mXCount)); // Only show the most recent mXCount data points
     pTimeAxis->setLabelsAngle(60);  // Set the rotation angle of the label
     //pTimeAxis->setTickCount(mXCount+1);// Set the number of labels on the axis
     pTimeAxis->setTickCount(11);// Set the number of labels on the axis
     QFont font = pTimeAxis->labelsFont();  // Get the current font
     font.setPointSize(11);  // Set the font size to 11
     pTimeAxis->setLabelsFont(font);  // Apply new font settings
     pChart->addAxis(pTimeAxis, Qt::AlignBottom);
 
     // Set the Y axis to data values
     pDataAxis = new QValueAxis();
     pDataAxis->setTitleText("GF/VO2/VCO2/VE");
     pDataAxis->setLabelsAngle(90);
     font = pDataAxis->labelsFont();// Get the current font
     font.setPointSize(11);// Set the font size to 11
     pDataAxis->setLabelsFont(font);// Apply new font settings
     pChart->addAxis(pDataAxis, Qt::AlignLeft);
     pDataAxis->setRange(0, mYLCount); // Setting Range 0 - 1000
 
     pDataAxis1 = new QValueAxis();
     pDataAxis1->setTitleText("O2C/CO2C");
     pDataAxis1->setLabelsAngle(90);
     font = pDataAxis1->labelsFont();// Get the current font
     font.setPointSize(11);// Set the font size to 11
     pDataAxis1->setLabelsFont(font);// Apply new font settings
     pDataAxis1->setRange(0, mYRCount); // Setting Range 0 - 100
     pChart->addAxis(pDataAxis1, Qt::AlignRight);
 
     // Create a chart view and set up the chart
     pChartView = new QChartView(pChart);
     pChartView->setRenderHint(QPainter::Antialiasing);
 
     ui->scrollArea->setWidget(pChartView);// Use pChartView as the content of QScrollArea
     ui->scrollArea->setWidgetResizable(true);// Make pChartView resizable to fit the scrolling area
     ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // Always show horizontal scroll bar
 
     ui->doubleSpinBoxO2CI->setValue(21);
     ui->doubleSpinBoxCO2CI->setValue(4);
     mO2CI =  ui->doubleSpinBoxO2CI->value();
     mCO2CI =  ui->doubleSpinBoxCO2CI->value();
 
     // Simulate data changes
     mTime = 0;
     // Timer
     pTimer = new QTimer(this);
     connect(pTimer, &QTimer::timeout, this, &Widget::updateData);
 
     // Set the number of rows and columns
     ui->tableWidget->setRowCount(2);  // Set 2 rows
     ui->tableWidget->setColumnCount(9);  // Set 8 columns
 
     ui->tableWidget->verticalHeader()->setVisible(false);// Hide row numbers
     ui->tableWidget->horizontalHeader()->setVisible(false);// Hide row numbers
 
     // Make all columns stretch automatically to evenly distribute available space
     ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
     // Adjust row height to accommodate new font size
     ui->tableWidget->resizeRowsToContents();
     //Set the container height
     ui->tableWidget->setFixedHeight(ui->tableWidget->rowHeight(0)*2.1);
 
     // Set the table content
      ui->tableWidget->setItem(0, 0, new QTableWidgetItem("O2C(%)"));
      ui->tableWidget->setItem(0, 1, new QTableWidgetItem("CO2C(%)"));
      ui->tableWidget->setItem(0, 2, new QTableWidgetItem("GF(mL/S)"));
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
 
      // Center the contents of each cell
      for (int row = 0; row < 2; ++row) {
          for (int col = 0; col <9; ++col) {
              QTableWidgetItem *item =  ui->tableWidget->item(row, col);
              item->setTextAlignment(Qt::AlignCenter);  // Set center alignment
          }
      }
 
 }
 
 Widget::~Widget()
 {
     pIICThread->setIICThreadFlag(false);
     pThread2->quit();//Exit child thread
     pThread2->wait();//Recycling
     delete ui;
 }
 
 void Widget::getVO2(float *ret , float *o2c ,float *co2c ,float *gasflow)
 {
 //    *ret = (*gasflow) * ( ui->doubleSpinBoxO2CI->value()- (*o2c)) *10 ;
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
 
 void Widget::getVCO2(float *ret ,float *o2c ,float *co2c ,float *gasflow)
 {
    // *ret = (*gasflow) * ((*co2c) - ui->doubleSpinBoxCO2CI->value()) *10 ;
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
 
 void Widget::getRER(float *ret ,float *vo2 ,float *vco2)
 {
     *ret = (*vco2) * (*vo2);
 }
 
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
 
 void Widget::updateData() {
     qDebug()<<"updateData";
     // Data processing
     pIICThread->getData(&mO2C ,&mCO2C ,&mGasFlow);
     getVO2(&mVO2,&mO2C ,&mCO2C ,&mGasFlow);
     getVCO2(&mVCO2,&mO2C ,&mCO2C ,&mGasFlow);
     getRER(&mRER,&mVO2 ,&mVCO2);
     getVE(&mVE,&mGasFlow);
 
     if(mVO2MAX < mVO2)
     {
         mVO2MAX = mVO2;
     }
 
     // The time increases every second, and the data value changes according to a certain rule
     if(mTime > mXCount) // Dynamically update the horizontal axis range
     {
         pTimeAxis->setRange( mStartTime.addSecs(mTime - mXCount),  mStartTime.addSecs(mTime)); // Only the most recent 10 data points are displayed
     }
     else
     {
         pTimeAxis->setRange( mStartTime.addSecs(0),  mStartTime.addSecs(mXCount)); // Show only the most recent data points
     }
 
     m_Series.O2C->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mO2C );
     m_Series.CO2C->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mCO2C );
     m_Series.GF->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mGasFlow );
     m_Series.VO2->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVO2 );
     m_Series.VCO2->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVCO2 );
     m_Series.RER->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mRER );
     m_Series.VE->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVE );
 
 
     // Refresh the table in real time
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
             item->setTextAlignment(Qt::AlignCenter);  // Set center alignment
         }
     }
 
     mTime++;
 
 }
 
 void Widget::startIICThreadFun()
 {
     if(pThread2->isRunning()== true)//Thread starting, return
     {
         return;
     }
     pThread2->start();//Starting a Thread
     pIICThread->setIICThreadFlag(true);//Set the loop flag
     emit startIICThread();//Send start-up signal
 }
 
 void Widget::dealIICThreadFun()
 {
     qDebug()<<"dealIICThreadFun线程号"<<QThread::currentThreadId();
 }
 
 void Widget::stopIICThreadFun()
 {
     if(pThread2->isRunning() == false)//The thread is stopped, return
     {
         return;
     }
     pIICThread->setIICThreadFlag(false);
     pThread2->quit();//Exit child thread
     pThread2->wait();//Recycling
 }
 
 void Widget::on_btnStart_clicked()//Start Update
 {
     if(m_Series.StartPauseStopFalg != 1 )//Not executed in pause mode
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
     pTimer->start(m_Series.Time);  // Updated once per second
     startIICThreadFun();
 }
 
 void Widget::on_btnPause_clicked()// Pause Updates
 {
     pTimer->stop();
     m_Series.StartPauseStopFalg = 1;
 }
 
 
 void Widget::on_btnStop_clicked()// Stop updating
 {
     pTimer->stop();
     m_Series.StartPauseStopFalg = 2;
     stopIICThreadFun();
 }
 
 
 void Widget::on_checkBoxVE_clicked(bool checked)
 {
     if(checked == true)
     {
         m_Series.VE->setName("VE");// Set the curve name
         m_Series.VE->setColor(QColor(128, 200, 110));//Set the curve color
         pChart->addSeries(m_Series.VE);// Writing data to chart
         m_Series.VE->attachAxis(pTimeAxis);// Set the curve X axis
         m_Series.VE->attachAxis(pDataAxis);// Set the Y axis of the curve
     }
     else
     {
         pChart->removeSeries(m_Series.VE);//Delete Curve
     }
 }
 
 void Widget::on_checkBoxO2C_clicked(bool checked)
 {
     if(checked == true)
     {
         m_Series.O2C->setName("O2C");// Set the curve name
         m_Series.O2C->setColor(Qt::red);//Set the curve color
         pChart->addSeries(m_Series.O2C);// Writing data to chart
         m_Series.O2C->attachAxis(pTimeAxis);// Set the curve X axis
         m_Series.O2C->attachAxis(pDataAxis1);// Set the Y axis of the curve
     }
     else
     {
         pChart->removeSeries(m_Series.O2C);//Delete Curve
     }
 }
 
 
 void Widget::on_checkBoxCO2C_clicked(bool checked)
 {
     if(checked == true)
     {
         m_Series.CO2C->setName("CO2C");// Set the curve name
         m_Series.CO2C->setColor(Qt::blue);//Set the curve color
         pChart->addSeries(m_Series.CO2C);// Writing data to chart
         m_Series.CO2C->attachAxis(pTimeAxis);// Set the curve X axis
         m_Series.CO2C->attachAxis(pDataAxis1);// Set the Y axis of the curve
     }
     else
     {
         pChart->removeSeries(m_Series.CO2C);//Delete Curve
     }
 }
 
 void Widget::on_checkBoxGF_clicked(bool checked)
 {
     if(checked == true)
     {
         m_Series.GF->setName("GF");// Set the curve name
         m_Series.GF->setColor(Qt::green);//Set the curve color
         pChart->addSeries(m_Series.GF);// Writing data to chart
         m_Series.GF->attachAxis(pTimeAxis);// Set the curve X axis
         m_Series.GF->attachAxis(pDataAxis);// Set the Y axis of the curve
     }
     else
     {
         pChart->removeSeries(m_Series.GF);//Delete Curve
     }
 }
 
 void Widget::on_checkBoxVO2_clicked(bool checked)
 {
     if(checked == true)
     {
         m_Series.VO2->setName("VO2");// Set the curve name
         m_Series.VO2->setColor(Qt::magenta);
         pChart->addSeries(m_Series.VO2);
         m_Series.VO2->attachAxis(pTimeAxis);
         m_Series.VO2->attachAxis(pDataAxis);
     }
     else
     {
         pChart->removeSeries(m_Series.VO2);
     }
 }
 
 
 void Widget::on_checkBoxVCO2_clicked(bool checked)
 {
     if(checked == true)
     {
         m_Series.VCO2->setName("VCO2");// Set the curve name
         m_Series.VCO2->setColor(QColor(128, 0, 128));
         pChart->addSeries(m_Series.VCO2);
         m_Series.VCO2->attachAxis(pTimeAxis);
         m_Series.VCO2->attachAxis(pDataAxis);
     }
     else
     {
         pChart->removeSeries(m_Series.VCO2);
     }
 }
 
 void Widget::on_checkBoxRER_clicked(bool checked)
 {
     if(checked == true)
     {
         m_Series.RER->setName("RER");
         m_Series.RER->setColor(Qt::yellow);
         pChart->addSeries(m_Series.RER);
         m_Series.RER->attachAxis(pTimeAxis);
         m_Series.RER->attachAxis(pDataAxis);
     }
     else
     {
         pChart->removeSeries(m_Series.RER);
 }
 
 
 void Widget::on_doubleSpinBoxO2CI_valueChanged(double arg1)
 {
     mO2CI =  arg1;
 }
 
 
 void Widget::on_doubleSpinBoxCO2CI_valueChanged(double arg1)
 {
     mCO2CI =  arg1;
 }
 
 
 void Widget::on_horizontalSliderX_valueChanged(int value)
 {
    if(mXCount == value)
        return;
    mXCount = value;
    ui->lineEditX->setText(QString::number(mXCount));
    if(mTime > mXCount) // Dynamically update the horizontal axis range
    {
        pTimeAxis->setRange( mStartTime.addSecs(mTime - mXCount),  mStartTime.addSecs(mTime)); // Show only the most recent data points
    }
    else
    {
        pTimeAxis->setRange( mStartTime.addSecs(0),  mStartTime.addSecs(mXCount)); // Show only the most recent data points
    }
 }
 
 
 void Widget::on_verticalSliderYL_valueChanged(int value)
 {
     pDataAxis->setRange(0, value); // Setting Range
 }
 
 
 void Widget::on_verticalSliderYR_valueChanged(int value)
 {
     pDataAxis1->setRange(0, value); // Setting Range
 }
