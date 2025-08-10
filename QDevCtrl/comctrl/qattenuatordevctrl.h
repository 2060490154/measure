#ifndef QATTENUATORDEVCTRL_H
#define QATTENUATORDEVCTRL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QMessageBox>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QTimer>

#define M_COM_TOTAL_NUM 3
typedef struct _ComDevInfo
{
    bool ComPortFreeFlag;  //串口是否空闲 空闲：ComPortFreeFlag = 1
    bool ComPortOpenFlag;  //串口是否打开，【打开意味着不空闲】
    int nCurrentStatus;
    unsigned int nSetLimitData;//设定的门限值
}tComDevInfo;
enum _COMStatus
{
    M_COM_STATUS_NONE = 0,
    M_COM_STATUS_Free = 1,
    M_COM_STATUS_OPENED = 2
};

class QAttenuatorDevCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QAttenuatorDevCtrl(QString ComPort,QObject *parent = nullptr);
    ~QAttenuatorDevCtrl();

    void sendMsg(const QString &msg1);
    void findFreePorts();
    bool initSerialPort(QString ComPort);
    QStringList portnameList;
    QSerialPort *serialPort;
    tComDevInfo m_tCOMDev[M_COM_TOTAL_NUM];
    QByteArray LeastestReceivedMsg;
protected:
    QString acsii_to_Hex(QString str);
public slots:
    void recvMsg();
    void closePorts();
    void waitForDealMsg();//等待处理数据
signals:
    void portsreadsignal();
    void dataProcessed(); // 处理完成的信号
private:
    QMutex mutex;
    QTimer* timer;
};

#endif // QATTENUATORDEVCTRL_H
