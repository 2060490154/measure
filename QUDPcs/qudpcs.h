#ifndef QUDPCS_H
#define QUDPCS_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QWidget>
#include <QTimer>

class QUDPcs : public QWidget
{
    Q_OBJECT

public:
    explicit QUDPcs(QWidget *parent = nullptr);
    bool lastDealMsgResult;
    bool yesorno;
    QString scanIndex;//扫描点索引
    QString frameInfo;//发次信息

signals:
    void dataProcessed(); // 数据处理完成的信号

public slots:
    void dealMsg();//处理对方发过来的数据
    void waitForDealMsg();//等待处理数据
    void SEND_PLAN(const int& m_nMeasureType,const QString& m_sExpNumber);//发布实验信息
    void SEND_INFO();//扫描前准备
    void GET_DATA();
    void GET_one();//yhy0319
    void START_DEAL();//开始进行损伤诊断
    void END_DEAL();
private:
    QUdpSocket *udpSocket;//udp链接
    QTimer *timer = new QTimer(this);//定时器
    QHostAddress receiverAddress;//目标IP地址
    quint16 receiverPort;//目标端口
};

#endif // QUDPCS_H
