#ifndef QKXCCDCTRL_H
#define QKXCCDCTRL_H
#include "kxccddefine.h"
#include "qhigimage.h"
#include <QMutex>
#include <QSet>
#include "define.h"

class QKxccdCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QKxccdCtrl(QObject *parent = 0);
    ~QKxccdCtrl();

signals:
    void signal_showKxccdImag(uchar* pBuff,int nWidth,int nHigh,bool bCnt = true);
    void signal_ReCap(int nDevIndex,bool bStart,bool bFinished,uchar* pData=NULL,int nlen=0);

    void signal_DevConnectStatus(bool bConnect);

public slots:


public:
    bool setDevIPAndPort(QString sIP,int nPort);//设置设备的Ip和端口
    bool openDev();//连接设备
    bool closeDev();//断开设备
    bool setWorkMode(int nType);//设置工作模式
    bool startCap();//开始采集
    bool stopCap();//停止采集
    bool startReCap();//开始重传
    bool setDevParam(int nIndex,int nValue);//设置参数 nindex为参数索引
    bool modifyIPAddress(QString sIP);


    //用于显示
    QString getDevStatusInfo();//获取设备状态信息  用于显示
    QString getDevWorkModeInfo(int nDevWorkMode);//获取设备的工作模式


    bool isDevOpen();//是否已打开

    bool getDevStatus();

    //参数相关
    bool getDevParam();
    bool saveDevParam();

    //更新窗位 窗宽
    void updateWindowInfo(int nWindowPos,int nWindowWidth);

    //搜索相关功能
    bool initSearchDevResource();//初始化搜索相关资源
    bool startSearchDev();//周期性调用 每调用一次 发送一个广播消息
    void clearSearchDevResource();//清理资源

    //保存图片
    bool saveImgeToFile(QString sFilePathName);


private:
    void rcvCmdData();//接收命令
    void cmdProcess(char* pbuff,int nLen);
    bool sendCmd(int nCmdID,int nValue = 0,bool bGetParam = false);
    bool sendDataRequset(int nCmdID);

    void createSocket(QString sIP,int nPort);//创建socket
    void deleteSocket();
    void rcvDevData();//接收数据

     bool sendRecapRequest(int nCmdID);

    //数据解析
    int getDataOffset();
    void parseRowData(char* pBuf,int nLen);//解析数据行头信息
    bool parseDataHeader(char* pBuf,int nLen);//解析数据头
    void parseRcvData(char* pBuf,int nLen);

    //其他功能
    void mergeWorkMode();

    //搜索功能
    void rcvSearchData();

public:
    bool m_bOpenDev;//是否已打开
    bool m_bModifyIP;
    int m_nCameraStatus;
    int m_nDevWorkMode;//工作方式 根据 workemode 和 translate 融合之后的状态


    int m_nTriggerMode;//工作方式
    int m_nTransLateMode;//传输方式

    int m_nRcvFrameCount;

    struct DevSocket m_tDevSocket;
    tRcvKxccdData m_tRcvKxccdData;
    QHigImage m_higImg;

    QList<tParamInfo> m_st_ParamInfoList;

    QSet<QString> m_sSearchDevIPList;//搜索到的IP地址列表  用于显示
    int m_nLineCnt;


 private:
    QMutex _MutexLock;

    QUdpSocket* _pSearchDevSendSocket;
    QUdpSocket* _pSearchDevRcvSocket;

};

#endif // QKXCCDCTRL_H
