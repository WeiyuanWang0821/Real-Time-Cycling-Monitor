#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtCharts>//图表

#include <QThread>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <QDebug>
#include <QLineSeries>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTimer>
#include <QDateTime>
#include <QLegend>

#include "iicthread.h"//IIC线程
#include "serialportthread.h"//串口线程

//曲线结构体
struct m_SeriesStruct {
    QSplineSeries *O2C = nullptr;
    QSplineSeries *CO2C = nullptr;
    QSplineSeries *GF = nullptr;
    QSplineSeries *VCO2 = nullptr;
    QSplineSeries *VO2 = nullptr;
    QSplineSeries *RER = nullptr;
    QSplineSeries *VE = nullptr;

    int VO2MAX = 0;
    int StartPauseStopFalg = 0;
    int Time = 1000;//采集间隔ms
};


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


public slots:
    void updateData();



    void dealIICThreadFun();
    void startIICThreadFun();
    void stopIICThreadFun();
    void getVCO2(float *ret, float *o2c, float *co2c, float *gasflow);
    void getVO2(float *ret, float *o2c, float *co2c, float *gasflow);
    void getRER(float *ret, float *vo2, float *vco2);
    void getVE(float *ret, float *gasflow);
signals:
       void startIICThread();
       void startSerialPortThread();

private slots:
       void on_btnStart_clicked();
       void on_btnPause_clicked();
       void on_btnStop_clicked();

       void on_checkBoxO2C_clicked(bool checked);
       void on_checkBoxCO2C_clicked(bool checked);
       void on_checkBoxGF_clicked(bool checked);
       void on_checkBoxVO2_clicked(bool checked);
       void on_checkBoxVCO2_clicked(bool checked);
       void on_checkBoxRER_clicked(bool checked);
       void on_checkBoxVE_clicked(bool checked);

       void on_doubleSpinBoxO2CI_valueChanged(double arg1);
       void on_doubleSpinBoxCO2CI_valueChanged(double arg1);
       void on_horizontalSliderX_valueChanged(int value);

       void on_verticalSliderYL_valueChanged(int value);

       void on_verticalSliderYR_valueChanged(int value);

private:
    Ui::Widget *ui;
    //QSerialPort *pSerialPort = nullptr;  //串口对象

    IICThread *pIICThread = nullptr;//IIC线程指针
    SerialPortThread *pSerialPortThread = nullptr;//串口线程指针
    QThread *pThread1 = nullptr;
    QThread *pThread2 = nullptr;

    m_SeriesStruct m_Series;

    QChart *pChart = nullptr;

    QChartView *pChartView = nullptr;
    QDateTimeAxis *pTimeAxis = nullptr;
    QValueAxis *pDataAxis = nullptr;
    QValueAxis *pDataAxis1 = nullptr;
    int mTime = 0;
    int mTimeAxisCount = 20;

    QTimer *pTimer = nullptr;
    QDateTime mStartTime;
    QDateTime mCurrentTime;

    float mO2CI = 0,mCO2CI = 0;
    float mO2C = 0,mCO2C = 0,mGasFlow = 0;
    float mVO2 = 0,mVCO2 = 0,mRER = 0;
    float mVO2Z[60*20] = {0};
    float mVCO2Z[60*20] = {0};
    float mVO2MAX = 0;
    float mHR = 0;
    float mVE = 0;
    float mVEZ[60*20] = {0};

    int mXCount = 0;
    int mYLCount = 0;
    int mYRCount = 0;

};
#endif // WIDGET_H
