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

    pIICThread = new IICThread;//动态线程分配空间，不能指定父对象
    pThread2 =new QThread(this);//创建子线程
    pIICThread->moveToThread(pThread2);//把自定义的线程加入到子线程中
    connect(pIICThread,&IICThread::signalsIICThread ,this,&Widget::dealIICThreadFun);//连接线程信号与主线程处理函数
    connect(this,&Widget::startIICThread,pIICThread,&IICThread::runIICThreadFun);//连接主线程启动子线程

    QFont legendFont;
    legendFont.setPointSize(11);  // 设置字体大小为 11

    m_Series.Time =1000;//采集间隔1000ms

    m_Series.O2C = new QSplineSeries();// 创建曲线
    m_Series.CO2C = new QSplineSeries();// 创建曲线
    m_Series.GF = new QSplineSeries();// 创建曲线
    m_Series.VCO2 = new QSplineSeries();// 创建曲线
    m_Series.VO2 = new QSplineSeries();// 创建曲线
    m_Series.RER = new QSplineSeries();// 创建曲线
    m_Series.VE = new QSplineSeries();// 创建曲线

    pChart = new QChart();// 创建图表
    pChart->setTitle("");// 设置标题

    // 设置图例字体大小
    legendFont = pChart->legend()->font();
    legendFont.setPointSize(11);  // 设置字体大小为 11
    pChart->legend()->setFont(legendFont);

    // 设置 X 轴为时间（秒）
    pTimeAxis = new QDateTimeAxis();
    //pTimeAxis->setTitleText("Time");
    pTimeAxis->setFormat("HH:mm:ss");  // 设置时间格式为时分秒
    mStartTime = QDateTime::fromSecsSinceEpoch(0+16*3600);//
    pTimeAxis->setRange( mStartTime,  mStartTime.addSecs(mXCount)); // 只显示最近的mXCount个数据点
    pTimeAxis->setLabelsAngle(60);  // 设置标签的旋转角度
    //pTimeAxis->setTickCount(mXCount+1);// 设置坐标轴上的标签数量
    pTimeAxis->setTickCount(11);// 设置坐标轴上的标签数量
    QFont font = pTimeAxis->labelsFont();  // 获取当前字体
    font.setPointSize(11);  // 设置字体大小为11
    pTimeAxis->setLabelsFont(font);  // 应用新的字体设置
    pChart->addAxis(pTimeAxis, Qt::AlignBottom);

    // 设置 Y 轴为数据值
    pDataAxis = new QValueAxis();
    pDataAxis->setTitleText("GF/VO2/VCO2/VE");
    pDataAxis->setLabelsAngle(90);
    font = pDataAxis->labelsFont();// 获取当前字体
    font.setPointSize(11);// 设置字体大小为11
    pDataAxis->setLabelsFont(font);// 应用新的字体设置
    pChart->addAxis(pDataAxis, Qt::AlignLeft);
    pDataAxis->setRange(0, mYLCount); // 设置范围 0 - 1000

    pDataAxis1 = new QValueAxis();
    pDataAxis1->setTitleText("O2C/CO2C");
    pDataAxis1->setLabelsAngle(90);
    font = pDataAxis1->labelsFont();// 获取当前字体
    font.setPointSize(11);// 设置字体大小为11
    pDataAxis1->setLabelsFont(font);// 应用新的字体设置
    pDataAxis1->setRange(0, mYRCount); // 设置范围 0 - 100
    pChart->addAxis(pDataAxis1, Qt::AlignRight);

    // 创建图表视图并设置图表
    pChartView = new QChartView(pChart);
    pChartView->setRenderHint(QPainter::Antialiasing);

    ui->scrollArea->setWidget(pChartView);// 将pChartView作为QScrollArea的内容
    ui->scrollArea->setWidgetResizable(true);// 使pChartView大小可调整，适应滚动区域大小
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // 始终显示水平滚动条

    ui->doubleSpinBoxO2CI->setValue(21);
    ui->doubleSpinBoxCO2CI->setValue(4);
    mO2CI =  ui->doubleSpinBoxO2CI->value();
    mCO2CI =  ui->doubleSpinBoxCO2CI->value();

    // 模拟数据变化
    mTime = 0;
    // 定时器
    pTimer = new QTimer(this);
    connect(pTimer, &QTimer::timeout, this, &Widget::updateData);

    // 设置行数和列数
    ui->tableWidget->setRowCount(2);  // 设置2行
    ui->tableWidget->setColumnCount(9);  // 设置8列

    ui->tableWidget->verticalHeader()->setVisible(false);// 隐藏行序号
    ui->tableWidget->horizontalHeader()->setVisible(false);// 隐藏列序号

    // 使所有列自动拉伸，均匀分配可用空间
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 调整行高以适应新的字体大小
    ui->tableWidget->resizeRowsToContents();
    //设置容器高度
    ui->tableWidget->setFixedHeight(ui->tableWidget->rowHeight(0)*2.1);

    // 设置表格内容
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

     // 设置每个单元格的内容居中
     for (int row = 0; row < 2; ++row) {
         for (int col = 0; col <9; ++col) {
             QTableWidgetItem *item =  ui->tableWidget->item(row, col);
             item->setTextAlignment(Qt::AlignCenter);  // 设置居中对齐
         }
     }

}

Widget::~Widget()
{
    pIICThread->setIICThreadFlag(false);
    pThread2->quit();//退出子线程
    pThread2->wait();//回收资源
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
    // 数据处理
    pIICThread->getData(&mO2C ,&mCO2C ,&mGasFlow);
    getVO2(&mVO2,&mO2C ,&mCO2C ,&mGasFlow);
    getVCO2(&mVCO2,&mO2C ,&mCO2C ,&mGasFlow);
    getRER(&mRER,&mVO2 ,&mVCO2);
    getVE(&mVE,&mGasFlow);

    if(mVO2MAX < mVO2)
    {
        mVO2MAX = mVO2;
    }

    // 每秒增加时间，数据值根据某种规则变化
    if(mTime > mXCount) // 动态更新横坐标范围
    {
        pTimeAxis->setRange( mStartTime.addSecs(mTime - mXCount),  mStartTime.addSecs(mTime)); // 只显示最近的10个数据点
    }
    else
    {
        pTimeAxis->setRange( mStartTime.addSecs(0),  mStartTime.addSecs(mXCount)); // 只显示最近的个数据点
    }

    m_Series.O2C->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mO2C );
    m_Series.CO2C->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mCO2C );
    m_Series.GF->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mGasFlow );
    m_Series.VO2->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVO2 );
    m_Series.VCO2->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVCO2 );
    m_Series.RER->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mRER );
    m_Series.VE->append(mStartTime.addSecs(mTime).toMSecsSinceEpoch() , mVE );


    // 实时刷新表格
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
            item->setTextAlignment(Qt::AlignCenter);  // 设置居中对齐
        }
    }

    mTime++;

}

void Widget::startIICThreadFun()
{
    if(pThread2->isRunning()== true)//线程启动中，返回
    {
        return;
    }
    pThread2->start();//启动线程
    pIICThread->setIICThreadFlag(true);//设置循环标志位
    emit startIICThread();//发送启动现场信号
}

void Widget::dealIICThreadFun()
{
    qDebug()<<"dealIICThreadFun线程号"<<QThread::currentThreadId();
}

void Widget::stopIICThreadFun()
{
    if(pThread2->isRunning() == false)//线程停止中，返回
    {
        return;
    }
    pIICThread->setIICThreadFlag(false);
    pThread2->quit();//退出子线程
    pThread2->wait();//回收资源
}

void Widget::on_btnStart_clicked()//开始更新
{
    if(m_Series.StartPauseStopFalg != 1 )//暂停模式下不执行
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
    pTimer->start(m_Series.Time);  // 每秒更新一次
    startIICThreadFun();
}

void Widget::on_btnPause_clicked()// 暂停更新
{
    pTimer->stop();
    m_Series.StartPauseStopFalg = 1;
}


void Widget::on_btnStop_clicked()// 停止更新
{
    pTimer->stop();
    m_Series.StartPauseStopFalg = 2;
    stopIICThreadFun();
}


void Widget::on_checkBoxVE_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.VE->setName("VE");// 设置曲线名陈
        m_Series.VE->setColor(QColor(128, 200, 110));//设置曲线颜色
        pChart->addSeries(m_Series.VE);// 数据写入图表
        m_Series.VE->attachAxis(pTimeAxis);// 设置曲线X轴
        m_Series.VE->attachAxis(pDataAxis);// 设置曲线Y轴
    }
    else
    {
        pChart->removeSeries(m_Series.VE);//删除曲线
    }
}

void Widget::on_checkBoxO2C_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.O2C->setName("O2C");// 设置曲线名陈
        m_Series.O2C->setColor(Qt::red);//设置曲线颜色
        pChart->addSeries(m_Series.O2C);// 数据写入图表
        m_Series.O2C->attachAxis(pTimeAxis);// 设置曲线X轴
        m_Series.O2C->attachAxis(pDataAxis1);// 设置曲线Y轴
    }
    else
    {
        pChart->removeSeries(m_Series.O2C);//删除曲线
    }
}


void Widget::on_checkBoxCO2C_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.CO2C->setName("CO2C");// 设置曲线名陈
        m_Series.CO2C->setColor(Qt::blue);//设置曲线颜色
        pChart->addSeries(m_Series.CO2C);// 数据写入图表
        m_Series.CO2C->attachAxis(pTimeAxis);// 设置曲线X轴
        m_Series.CO2C->attachAxis(pDataAxis1);// 设置曲线Y轴
    }
    else
    {
        pChart->removeSeries(m_Series.CO2C);//删除曲线
    }
}

void Widget::on_checkBoxGF_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.GF->setName("GF");// 设置曲线名陈
        m_Series.GF->setColor(Qt::green);//设置曲线颜色
        pChart->addSeries(m_Series.GF);// 数据写入图表
        m_Series.GF->attachAxis(pTimeAxis);// 设置曲线X轴
        m_Series.GF->attachAxis(pDataAxis);// 设置曲线Y轴
    }
    else
    {
        pChart->removeSeries(m_Series.GF);//删除曲线
    }
}

void Widget::on_checkBoxVO2_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.VO2->setName("VO2");// 设置曲线名陈
        m_Series.VO2->setColor(Qt::magenta);//设置曲线颜色
        pChart->addSeries(m_Series.VO2);// 数据写入图表
        m_Series.VO2->attachAxis(pTimeAxis);// 设置曲线X轴
        m_Series.VO2->attachAxis(pDataAxis);// 设置曲线Y轴
    }
    else
    {
        pChart->removeSeries(m_Series.VO2);//删除曲线
    }
}


void Widget::on_checkBoxVCO2_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.VCO2->setName("VCO2");// 设置曲线名陈
        m_Series.VCO2->setColor(QColor(128, 0, 128));//设置曲线颜色
        pChart->addSeries(m_Series.VCO2);// 数据写入图表
        m_Series.VCO2->attachAxis(pTimeAxis);// 设置曲线X轴
        m_Series.VCO2->attachAxis(pDataAxis);// 设置曲线Y轴
    }
    else
    {
        pChart->removeSeries(m_Series.VCO2);//删除曲线
    }
}

void Widget::on_checkBoxRER_clicked(bool checked)
{
    if(checked == true)
    {
        m_Series.RER->setName("RER");// 设置曲线名陈
        m_Series.RER->setColor(Qt::yellow);//设置曲线颜色
        pChart->addSeries(m_Series.RER);// 数据写入图表
        m_Series.RER->attachAxis(pTimeAxis);// 设置曲线X轴
        m_Series.RER->attachAxis(pDataAxis);// 设置曲线Y轴
    }
    else
    {
        pChart->removeSeries(m_Series.RER);//删除曲线
    }
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
   if(mTime > mXCount) // 动态更新横坐标范围
   {
       pTimeAxis->setRange( mStartTime.addSecs(mTime - mXCount),  mStartTime.addSecs(mTime)); // 只显示最近的个数据点
   }
   else
   {
       pTimeAxis->setRange( mStartTime.addSecs(0),  mStartTime.addSecs(mXCount)); // 只显示最近的个数据点
   }
}


void Widget::on_verticalSliderYL_valueChanged(int value)
{
    pDataAxis->setRange(0, value); // 设置范围
}


void Widget::on_verticalSliderYR_valueChanged(int value)
{
    pDataAxis1->setRange(0, value); // 设置范围
}

