#ifndef QOSCII_DEV__H
#define QOSCII_DEV__H

#include "define.h"
#include "activedsolib.h"
#include <QHash>

using namespace ACTIVEDSOLib;

const int M_MAX_POINTS=500000;


enum _TriggerMode{
    M_TRIGGER_MODE_AUTO = 0,
    M_TRIGGER_MODE_NORMAL = 1,
    M_TRIGGER_MODE_SIGNAL = 2,
};

class QOsciDevCtrl:public QObject
{

        Q_OBJECT
public:
    QOsciDevCtrl(QString sIP, QList<tOsciiChannel> vaildChannellist);
    ~QOsciDevCtrl();

signals:
    void signal_showOsciData(QString sPath,int nChannelNum,QString sChannelName);

public slots:


    //属性
public:

    int m_nTriggerChannel;//触发通道
    int m_nTriggerMode;//触发类型 0--auto 1-normal 2-signal
    int m_nTriggerSlope[4];//触发类型 0-上升沿 1-下降沿
    float m_lfTriggerlevelEx;//外触发电平
    float m_lfTriggerlevel[4];//触发电平
    float m_lfTriggerDelay;//触发延时 s
    float m_lfSampleRate;//采样率 s
    float m_lfDivTime;//时间分辨 s

    float m_lfOffset[4];//基线位置 单位mv
    int m_nVoltDiv[4];//电压档位  单位V


    QHash<QString,int>m_tTriggerChannel;//触发通道
    QHash<QString,int>m_tTriggerMode;//触发模式
    QHash<QString,int>m_tTriggerSlope;//触发类型--上升沿 下降沿
    QHash<QString,int>m_tVoltDiv;//基线电压
    QHash<QString,int>m_tdisplayVoltDiv;//基线电压 显示内容


    //方法
public:
    bool openDev();//建立连接
    bool closeDev();//断开连接
    bool setTriggerMode(int nTriggerMode);//设置触发模式
    bool setSampleRate(float lfSampleRate);//设置采样率
    bool setTimeDiv(float lfTimeDiv);//设置时间分辨率
    bool setTirggerDelay(float lfDelayTime);//设置触发延时
    bool setTirggerLevelEx(float lfTriggerLevel);//设置外触发电平
    bool setTirggerLevel(int nChannel,float lfTriggerLevel);//设外触发电平
    bool setTriggerSlope(int nChannelNum,int nTriggerSlope=0);//设置触发通道的触发沿类型  默认触发沿类型为pos
    bool setTriggerChannel(int nChannelNum);//设置触发通道
    bool setChannelLevel(int nChannelNum,int nLevel);//设置通道的电压档位
    bool setOffset(int nChannel,float lfOffset);//设置基线位置
    bool capChannelData(int nChannelNum);//采集数据
    bool capChannelDataFile(int nChannelNum, QString curtime);


    bool getDevStatus();//获取示波器当前的连接状态
    bool getDevParam();//获取示波器当前的工作参数

    double getMeasureParam(int nParamIndex);//读取测量参数 脉宽 幅值

    QString getDevName();
    void setDevName(QString sName);



 private:
 ActiveDSO  _OsciDso;

 QString _sVisaName;//连接字符串
 bool _bOpenStatus;//是否打开
 QString _sIpAddress;
  QList<tOsciiChannel> _vaildChannellist;




};

#endif // QOsciDevCtrl_H
