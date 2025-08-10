#ifndef QDBDATA_H
#define QDBDATA_H

#include <QObject>
#include <QList>
#include "dbdatadefine.h"
#include "qdatabaseprocess.h"
#include "define.h"
#include "qerrcodeprocess.h"


class QDBData : public QObject
{
    Q_OBJECT
public:
    explicit QDBData( QObject *parent = 0);

signals:
    void signal_queryfinished(int nType);

public slots:

    void onWirteIdsImageToDb(QVariant TaskExpInfo, int nImageType, QString sPath);
    void onWriteExpInfoToDb(QString sExpNo, QString sCompSize, int ExpType, int CompentType, QString sSmapleName, int PointNumPerEnergy, float MinimunEnergy, float EnergyIncrecment, int EnergyIncreNum,int TimesPerPoint);

    void onWirteOsciDataToDb(QVariant TaskExpInfo,QString sFilePath,float lfPlusWidth);
    void onWirteKxccdImageToDb(char* pbuff,int nwidth,int nhigh,QVariant TaskExpInfo);
    void onWriteEnergyMeterDataToDb(int nIndex,QString sEnergyData, QVariant tTaskExpInfo);
    void onWriteDefectStatus(QVariant tTaskExpInfo, bool bstatus);
    void onWriteFluxData(QVariant tTaskExpInfo, float lfFluxData);
    bool onGetExpInfo4Show(QVector<ExpInfo4Show> &ExpInfos);
    bool onGetDefectInfo4Show(QVector<int> &DefectInfo, QString sExpNo);
    bool onGetExpData4Show(QVector<AllExpData4show> &ExpData, QString sExpNo);
    bool onSaveNewDevInfo2Db(DevInfo4Show DevInfo);
    bool onSaveNewClientInfo2Db(int index, ClientInfo ClientInfos);
    bool onGetDevInfo4Show(QVector<DevInfo4Show> &DevInfos);
    bool onGetClientInfo(QVector<ClientInfo> &ClientInfos);
    bool onTestFinished(QString sSampleName, QString sExpNo, QVector<float> &lfFlux, QVector<float> &lfDefect, QVector<double> &dEnergyAver, QVector<double> &dEnergyRMS, int nMeasureType, double &AverFlux, double &RMSFlux, QString sPath, int nWaveLengthType);
    bool onGetFinalResultData(QString sSampleName, QString sExpNo, int nWaveType);
    //data query
//    bool onGetExpInfo4Show(QVector<ExpInfo4Show> &ExpInfos);

public:
    bool connectDataBase();//连接数据库
    bool getAllDevConfigInfo();

public:
    tAllDevConfigInfo m_tAllDevConfigInfo;
    QString m_sSavePath;
public:
    QVector<int> m_tPtNo;
    QVector<int> m_tShotNo;
    QVector<double> m_tEnergy;
    QVector<int> m_tDefect;


private:
    QDatabaseProcess* _pDbProcess;
    QErrCodeProcess* _pErrCodeProcess;

    bool getMotorConfigInfo();//查询获取电机设备信息
    bool getMotorPostionConfig();//获取配置的档位信息
    bool getEnergyMeterConfigInfo();//获取能量计配置信息

    bool getEngeryAdjCoeff();//获取能量调节拟合系数
};

#endif // QDBDATA_H
