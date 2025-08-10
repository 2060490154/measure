#ifndef QMEASUREWIDGET_H
#define QMEASUREWIDGET_H

#include <QWidget>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qimagereader.h>
#include <qpushbutton.h>
#include <QTabWidget>
#include "qcustomplot.h"
#include "qpointareawidget.h"
#include "define.h"
#include "qworkflowtab.h"
#include "qworkflowparamwidget.h"
#include "qimagelabel.h"
#include "qruntaskthread.h"
#include "qworkflow.h"
#include "qdevmonitorwidget.h"
#include "qdbdata.h"
#include "QConfigProcess.h"
#include <QDialog>
#define M_STR_START_STANDBY  ("开始准备流程")
#define M_STR_STOP_STANDBY  ("停止准备流程")

#define M_STR_START_MEASURE  (" 开始测试 ")
#define M_STR_STOP_MEASURE  (" 停止测试 ")
#define M_STR_SUSPEND_MEASURE  (" 暂停测试 ")
#define M_STR_CONTINUE_MEASURE  (" 继续测试 ")

class QWorkFlowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QWorkFlowWidget(int nMeasureType,QDevManagerCtrl* pCtrl, QConfigProcess* pConfigProcess,QWidget *parent = 0);
    ~QWorkFlowWidget();

signals:
    void UpdateWorkStatus(int nWorkType,bool bwork);

    void signal_writeExpInfoToDb(QString sExpNo,QString sCompSize,int ExpType,int CompentType, QString sSmapleName, int PointNumPerEnergy, float MinimunEnergy, float EnergyIncrecment, int EnergyIncreNum, int TimesPerPoint);
    void signal_TestFinished(QString sSampleName,QString sExpNo,QVector<float> &lfFlux, QVector<float> &lfDefect, QVector<double> &dEnergyAver, QVector<double> &dEnergyRMS, int nMeasureType, double &AverFlux, double &RMSFlux,QString sPath,int nWaveLengthType);
    void singal_updateUserDefectStatus(QVariant tTaskExpInfo,bool bstatus);
public slots:

    void onUpdateWorkflow();//创建或更新测试流程
    void onUpdateRaster();//更新Raster测试流程
    void OnStartStandbyTest();//开始测试流程
    void OnStartTest();//开始测试流程
    void OnRepeatTest();//raster循环流程
    void OnSuspendTest();   //暂停流程
    void onThreadFinish();
    void onManualTest();//手动通量测试流程
    void onAverArea();  //平均光斑面积
    void onNewShutterTest();    //新光闸测试
    void onNewOneclicktest();//yhy0320
    void onNewPulsewidthtest();//yhy0320
    void onNewShutterLog(int nCntNum, int result);
    void onSampleStage2Zero();
    void onAreaRecord(double dArea, double dArea_e2);
    void onEnergyDataRecord(int nIndex,QString sEnergyData,QVariant tTaskExpInfo);

    void onShowFluxStatus(QVariant tTaskExpInfo, float lfdata);
    void onShowDefectStatus(QVariant tTaskExpInfo, bool bStatus);
    void onAlterData();
    void onMeasureEnergyRatio();
    void onManualEnterAction();
    void onManualYesBtnClicked();
    void onManualNoBtnClicked(QVariant tTaskExpInfo);

public:
    void InitUI();

    void setProcessTab(int nIndex);
    bool checkWavelengthChange();//检测当前选择的波长与实际波长是否一致
    void showResultInDialog();
    void outputResultFile();

    void manualTestFinished();
    void AverAreaTestFinished();
    void AverEnergyMeterTestFinished();
    void AverPulsewidthTestFinished();//yhy0320
    QWidget* createEnergyDevInfoWidget();
    void showLog(QString slog);



public:

    QWorkFlowParamWidget* m_pWorkflowParamWidget;


    QTabWidget* m_pParamSetTab;//参数设置和打靶点设置Tab切换
    QDevMonitorWidget* m_pDevMonitorWidget;//设备当前数据显示

    QTabWidget* m_pWorkflowTab;//自动流程和手动流程切换
    QConfigProcess* m_pConfigProcess;
//    tPlatformConfig m_tPlatformConfig;
//    QString m_sPlatformConfigPath;
    QDialog *m_pResultDlg;
    QList<QLineEdit*> m_stLineEditList;
    QPushButton *m_pAlterBtn;


    int m_nMeasureType;
    bool m_bInitDev;//标志有没有执行准备流程  每次执行时 必须从准备流程开始


    QPushButton* m_pRefreshParamBtn;//更新步长 尺寸
    QPushButton* m_pStandbyStartTestBtn;
    QPushButton* m_pStartTestBtn;
    QPushButton* m_pSuspendBtn;
    QPushButton* m_pManualYesBtn;
    QPushButton* m_pManualNoBtn;
    //手动测试相关
    QTextEdit* m_pManualTestEngeryEdit;
    QTextEdit* m_pCurrentPointEdit;//当前点号
    QPushButton* m_pManualTestBtn;
    QListWidget* m_pManualLogListWidget;
    QTableWidget* m_pManualWorkflowListWidget;//测试流程显示


    //光斑平均面积测试相关
    QPushButton *m_pAverArea;
    QLineEdit *m_pAverCntEdit;
    double m_dAreaRecord;
    double m_dAreaRecord_e2;
    double m_dAreaSquareRecord;
    double m_dAreaSquareRecord_e2;
    double m_dAreaAver;
    double m_dAreaAver_e2;
    double m_dAreaRMS;
    double m_dAreaRMS_e2;
    QVector<double> m_tAreaVector;
    QVector<double> m_tAreaVector_e2;


    //新关闸测试
    QPushButton *m_pNewShutterBtn;
    QLineEdit *m_pNewShutCntEdit;

    //样品台归零相关
    QPushButton *m_pSampleStageMove2Zero;

    //测量分光比相关
    QPushButton *m_pMeasureEnergyMeterRatio;
    QLineEdit *m_pEnergyAverCnt;
    double m_dEnergyRatio;
    double m_dEnergyRatioRMS;
    QVector<double> m_tEnergyDevData[2];

    QLineEdit *m_pNumber;
    QPushButton *m_pOneclicktest;//yhy0320

    QPushButton *m_pPulsewidthtest;//yhy0320
    int m_nLastPointNum;

    QPointAreaWidget* m_pPointAreaWidget;//点图区域



    QWorkFlowThread* m_pWorkFlowThread;
    QWorkflowTab* m_pStandbyWorkflowTab;//准备流程
    QWorkflowTab* m_pMeasureWorkflowTab;//打把流程


private:
    QDevManagerCtrl* _pDevManagerCtrl;

};

#endif // QMEASUREWIDGET_H
