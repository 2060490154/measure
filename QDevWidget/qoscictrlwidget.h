#ifndef QOSCIWIDGET_H
#define QOSCIWIDGET_H

#include <QWidget>
#include "QOsciCtrl/QOsciDevCtrl.h"
#include <QTextEdit>
#include <QComboBox>
#include <QTabWidget>
#include "qcustomplot.h"




class QOsciCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QOsciCtrlWidget(QOsciDevCtrl* pCtrl,QWidget *parent = 0);

signals:

public slots:
    void onOpenButtonClick();
    void onRefreshButtonClick();
    void onSetParamButtonClick();
    void onCapButtonClick();

    void onSelectFileButtonClick();


public:
    QOsciDevCtrl* m_pOsciDevCtrl;

    //UI相关
    QTextEdit* m_pOsciNameEdit;
    QComboBox* m_pTriggerModeCombox;//触发模式
    QComboBox* m_pTriggerChannelCombox;//触发通道
    QComboBox* m_pTriggerSlopeCombox;//触发沿类型
    QTextEdit* m_pFilePathEdit;


    QTableWidget* m_pOsciParamTable;//参数设置
    QTableWidget* m_pOsciTableStatusTable;//示波器当前设置状态显示列表
    QComboBox* m_pVoltDivCombox[M_OSCI_CHANNEL_NUM];//电压档位

    QListWidget* m_pLogListWidget;


    QPushButton* m_pSelectFileButton;//选择文件按钮

    QPushButton* m_pOpenButton;//打开按钮
    QPushButton* m_pRefreshButton;//刷新按钮
    QPushButton* m_pSetParamButton;//设置按钮
    QPushButton* m_pCapDataButton;//采集按钮

    //画图控件
    QCustomPlot* m_pCustomPlot;
    //显示波形相关数据
    int m_nRecordLen;
    float m_lfRecordSampleRate;
     QVector<double> m_pTimeData;//波形时间数据
     QVector<double> m_pWaveData;//波形数据

     float m_lfMaxWaveData;
     float m_lfMinWaveData;

     float m_lfMaxTimeData;



public:
    void InitUI();

    void InitStatusTableWidget();//初始化列表
    void InitParamTableWidget();//初始化列表

    void showWaveData(QString sFilePath,int nChannelNum = 0,QString sChannelName ="");//显示波形数据

    void showLog(QString str);
};

#endif // QOSCIWIDGET_H
