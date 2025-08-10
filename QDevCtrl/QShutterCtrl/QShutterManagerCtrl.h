#ifndef QSHUTTERCTRL_H
#define QSHUTTERCTRL_H

#include <QObject>
#include <synchapi.h>
#include "define.h"
#include <QtNetwork/QUdpSocket>

#define M_SHUTTER_FREAM_LENGTH 8
#define M_SHUTTER_LOG_FILE QString("FILE:ShutterLog.txt:FILE")
#define M_OUT_MSG(sFileName, msg) sFileName + msg

enum _ShutterStatus
{
    M_SHUTTER_STATUS_NONE = 0,
    M_SHUTTER_STATUS_OPENED = 1,
    M_SHUTTER_STATUS_CLOSED = 2
};

typedef struct _shutterDevInfo
{
    QHostAddress tHostAddress;
    int nCurrentStatus;
    unsigned int nSetLimitData;//设定的门限值
    int nCurrentVoltData;//当前采集的值

    bool bProcessStart;//流程是否开启
    bool bProcessEnd;//流程是否结束
}tShutterDevInfo;

typedef struct _shutterCmd
{
    char startflag;//起始标志
    char status;    //0x00打开；0x01关闭
    char cmdType;//命令类型 11位设置状态 00 为查询  01位设置电压阈值 02为设置流程打开模式
    char data[2];//数据
    char endflag;//结束标志

}tShutterCmd;

typedef struct _shutterRcvData
{
    char startflag;//起始标志
    char status;
    char cmdType;//命令类型 11位设置状态 00 为查询  01位设置电压阈值
    char threshold[2];
    char currentdata[2];
    char endflag;//结束标志

}tShutterRcvData;




class QShutterDevCtrl:public QObject
{

        Q_OBJECT
public:
    QShutterDevCtrl(int nPort,QStringList tDevIPList);
    ~QShutterDevCtrl();

signals:


public slots:
    void ReceiveData();


    //属性
public:

    QUdpSocket* m_pUdpSocket;

    tShutterDevInfo m_tShutterDev[M_SHUTTER_TOTAL_NUM];


    //方法
public:

    bool SetDevStatus(int nIndex,bool bOPen);//设置光闸状态

    bool SetDevLimit(int nIndex,int ndata);//设置阈值  单位为mv

    bool getDevStatus();//查询光闸状态

    void  ParseRcvData(QString sIPAddress,int nPort,char* pbuff,int nSize);

    bool checkDevStatus();//检查是否在线

    bool setProcessStatus(int nIndex,int nPlusCnt,int nDeleay);//设置执行流程状态，并等待流程执行结束

private:


private:
 int         _nPort;
 int _nDevTotalNum;
 bool _bDevStatus;

};

#endif // QShutterDevCtrl_H
