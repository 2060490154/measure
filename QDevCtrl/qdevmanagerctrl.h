#ifndef QDEVMANAGERCTRL_H
#define QDEVMANAGERCTRL_H


#define DETECTION_MING 0
#define DETECTION_AN 1

#include <QObject>
#include "define.h"
#include "EnergyMeterDeviceManager.h"
#include "qidscamerdevctrl.h"
#include "qmotordevmanager.h"
#include "QKxccdCtrl/qkxccdctrl.h"
#include "QOsciCtrl/QOsciDevCtrl.h"
#include "QShutterCtrl/QShutterManagerCtrl.h"
#include "QDataProcessCtrl/qdataprocessctrl.h"
#include "QConfigProcess.h"
#include "qdefectwidget.h"
#include "comctrl/qattenuatordevctrl.h"





struct _devStatus
{
    bool bEngerMeter;
    bool bIDSCamer;
    bool bMotor;
    bool bOscii;
    bool bKxccd;//科学CCD
    bool bshutterStatus;

};

class QDevManagerCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QDevManagerCtrl(tDevConfigParam* tDevConfig,QObject *parent = 0);
    ~QDevManagerCtrl();

public:
     void getDevStatus();
     void LogShutter(QString slog);
     void showFullScreenModalDialog(const QString &path1, const QString &path2);

signals:
    void getMotorStatus();
    void signal_capIDSImageBuff(QVariant tTaskExpInfo,int nImageType,QString sPath);//IDS  nImageType=0 打之前 nImageType=1 打之后的图像
    void signal_capKxccdImageBuff(char* pbuff,int nwidth,int nhigh,QVariant tTaskExpInfo);//kxccd图像
    void signal_wirteOsciDataToDb(QVariant tTaskExpInfo,QString sPath,float lfPlusWidth);//sPath = ../TimeWave/
    void signal_updateEnergyData(int nIndex,QString sEnergyData,QVariant tTaskExpInfo);//能量计数据
    void signal_showCheckStatus(QWorkTask* pTask);
    void singal_showCalcFluxStatus(QVariant tTaskExpInfo,float lfFluxData);
    void singal_showDefectStatus(QVariant tTaskExpInfo,bool bstatus);
    void signal_sendArea(double dArea,double dArea_e2);
    void signal_UserEnterAction();
    void signal_UserShowDefect(QVariant tTaskExpInfo);
    void signal_UserConfirmDef(QVariant tTaskExpInfo);
    void signal_UserDenyDef(QVariant tTaskExpInfo);
    void signal_NewShutterTestResult(int nCntNum, int result);
public slots:
    void onUserChooseYes(QVariant tTaskExpInfo);
    void onUserChooseNo(QVariant tTaskExpInfo);

public:
    //设备信息
    QEnergyMeterDeviceManager* m_pEnergyMeterDevManager;
    QIDSCamerDevCtrl* m_pIDSDevCtrl;
 //   QMotorCtrl* m_pMotorCtrl;
    QMotorDevManager* m_pMotorDevManager;
    QOsciDevCtrl* m_pOsciDevCtrl;
    QKxccdCtrl* m_pKxccdCtrl;
    QShutterDevCtrl* m_pShutterCtrl;
    QDataProcessCtrl* m_pDataProcessCtrl;

    //配置文件
    QConfigProcess* m_pDevConfigProcess;
    tDevConfigParam* m_pDevConfigData;


    //设备状态信息
    struct _devStatus m_tdevStatus;

    //配置信息
    tAllDevConfigInfo* m_pAllDevConfigInfo;

    //数据存储目录
    QString m_sDataSavePath;

    QAttenuatorDevCtrl *m_pPortCtrl1;
    QAttenuatorDevCtrl *m_pPortCtrl2;
    QAttenuatorDevCtrl *m_pPortCtrl3;//ywc

    //能量计数据 后续需要优化
    float m_lfCurrentEngData[4];
    int m_nCurrentRange;

    //任务相关
    bool  RunIDSTask(QWorkTask* pTask);//IDS任务
    bool  RunOsciTask(QWorkTask* pTask);//示波器任务
    bool  RunMotorTask(QWorkTask* pTask);//电机任务
    bool  RunEnergyMeterTask(QWorkTask* pTask);//能量计
    bool  RunKxccdTask(QWorkTask* pTask);
    bool  RunShutterTask(QWorkTask* pTask);
    bool  RunDataProcess(QWorkTask* pTask);
public:
    int m_nStopPoint;   //Ron1 和 Son1流程中途停止的点号和发次
    int m_nStopTimes;
    bool m_bManualChangeResult;
    bool m_bIsReady;
public:
    double m_dManualArea;
    bool m_bIsRasterDefect;
    QString m_sSavePath;
public:
    QDefectWidget *m_pDefWidget;
    int m_nDetectionMethod;
    QAttenuatorDevCtrl *m_pAttenuator;
};

#endif // QDEVMANAGERCTRL_H
