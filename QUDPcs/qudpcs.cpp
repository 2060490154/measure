#include "qudpcs.h"
#include "define.h"
#include <QObject>
#include <QNetworkDatagram>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>

QUDPcs::QUDPcs(QWidget *parent)
    : QWidget(parent)
{
    //初始化变量
    receiverAddress = QHostAddress("192.168.0.140");
    receiverPort = 6101;
    scanIndex = "01";
    frameInfo = "01";
    //创建udp链接
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 6101);

    connect(timer, &QTimer::timeout, this, &QUDPcs::dealMsg);
}

void QUDPcs::waitForDealMsg()
{
    QEventLoop loop;
    QTimer timerForLoop;
    QObject::connect(&timerForLoop, &QTimer::timeout, [&]() {
//        QMessageBox::warning(nullptr, "超时警告", "等待数据报超时，请检查嵌入式系统连接状态！");
        lastDealMsgResult = false;
        loop.quit();
    });
    timerForLoop.start(10000);
    QObject::connect(this, &QUDPcs::dataProcessed, &loop, &QEventLoop::quit);
    QObject::connect(this, &QUDPcs::dataProcessed, &timerForLoop, &QTimer::stop);
    timer->start(20);
    loop.exec();
    timer->stop();
    timerForLoop.stop();
}//yhy0320

void QUDPcs::dealMsg()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QNetworkDatagram data = udpSocket->receiveDatagram();
        QByteArray datagram = data.data();
        QString receivedData = QString::fromLatin1(datagram.toHex());
        qDebug() << "Received data: " << receivedData;
        // 判断接收数据的前两位和第三位第四位
        if (datagram[0] == char(0xa0))
        {
            if(datagram[1] == char(0x01))
            {

            }
            else
            {
                // 创建一个QMessageBox对象
                QMessageBox msgBox;
                // 设置对话框的标题
                msgBox.setWindowTitle("错误");
                // 设置对话框显示的文本信息
                msgBox.setText("损伤诊断系统连接失败，请重新尝试！");
                // 设置对话框的标准按钮（这里只设置一个“确定”按钮）
                msgBox.setStandardButtons(QMessageBox::Ok);
                // 显示对话框，并等待用户响应
                // exec()函数会阻塞当前线程，直到对话框关闭
                msgBox.exec();
            }
        }
        else if (datagram[0] == char(0xa1))
        {
            if(datagram[1] == char(0x01))
            {
                lastDealMsgResult = true;// 接收成功
            }
            else
            {
                lastDealMsgResult = false;// 接收失败
            }
        }
        else if (datagram[0] == char(0xa2))
        {
            if(datagram[1] == char(0x01))
            {
                lastDealMsgResult = true;
                yesorno = false;
            }
            else if(datagram[1] == char(0x02))
            {
                lastDealMsgResult = true;
                yesorno = true;
            }
            else if(datagram[1] == char(0x03))
            {
                lastDealMsgResult = false;
            }
        }
        else if (datagram[0] == char(0xa3))
        {
            if(datagram[1] == char(0x01))
            {
                lastDealMsgResult = true;
            }
            else if(datagram[1] == char(0x02))
            {
                lastDealMsgResult = false;
            }
        }
        else if (datagram[0] == char(0xa4))
        {
            if(datagram[1] == char(0x01))
            {
                lastDealMsgResult = true;// 接收成功
            }
            else if(datagram[1] == char(0x02))
            {
                lastDealMsgResult = false;// 接收失败
            }
        }
        else if (datagram[0] == char(0xa5))
        {
            if(datagram[1] == char(0x01))
            {
                lastDealMsgResult = true;
            }
            else if(datagram[1] == char(0x02))
            {
                lastDealMsgResult = false;
            }
        }//yhy0319
        emit dataProcessed();
    }
}

void QUDPcs::SEND_PLAN(const int& m_nMeasureType,const QString& m_sExpNumber)//发布实验信息
{
    QByteArray datagram;
    datagram.append(QByteArray::fromHex("A0"));
    if (m_nMeasureType == M_MEASURETYPE_1On1) {
        datagram.append(QByteArray::fromHex("01"));
        datagram.append(m_sExpNumber);
    }
    else if (m_nMeasureType == M_MEASURETYPE_SOn1) {
        datagram.append(QByteArray::fromHex("02"));
        datagram.append(m_sExpNumber);
    }
    else if (m_nMeasureType == M_MEASURETYPE_ROn1) {
        datagram.append(QByteArray::fromHex("03"));
        datagram.append(m_sExpNumber);
    }
    else if (m_nMeasureType == M_MEASURETYPE_RASTER) {
        datagram.append(QByteArray::fromHex("04"));
        datagram.append(m_sExpNumber);
    }
    udpSocket->writeDatagram(datagram, receiverAddress, receiverPort);
}

void QUDPcs::GET_DATA()
{
    QByteArray datagram;
    datagram.append(QByteArray::fromHex("A2"));
    datagram.append(QByteArray::fromHex(scanIndex.toUtf8()));
    datagram.append(QByteArray::fromHex(frameInfo.toUtf8()));//yhy0319
    datagram.append(QByteArray::fromHex("010000000000000000000000000000000000"));
    udpSocket->writeDatagram(datagram, receiverAddress, receiverPort);
}

void QUDPcs::GET_one()
{
    QByteArray datagram;
    datagram.append(QByteArray::fromHex("A50000000000000000000000000000000000000000"));
    udpSocket->writeDatagram(datagram, receiverAddress, receiverPort);
}//yhy0319

void QUDPcs::SEND_INFO()//扫描前准备
{
    QByteArray datagram;
    datagram.append(QByteArray::fromHex("A1"));
    datagram.append(QByteArray::fromHex(scanIndex.toUtf8()));
    datagram.append(QByteArray::fromHex(frameInfo.toUtf8()));//yhy0320
    datagram.append(QByteArray::fromHex("000000000000000000000000000000000000"));
    udpSocket->writeDatagram(datagram, receiverAddress, receiverPort);
}

void QUDPcs::START_DEAL()
{
    QByteArray datagram;
    datagram.append(QByteArray::fromHex("A3"));
    datagram.append(QByteArray::fromHex(scanIndex.toUtf8()));
    datagram.append(QByteArray::fromHex(frameInfo.toUtf8()));//yhy0320
    datagram.append(QByteArray::fromHex("000000000000000000000000000000000000"));
    udpSocket->writeDatagram(datagram, receiverAddress, receiverPort);
}

void QUDPcs::END_DEAL()
{
    QByteArray datagram;
    datagram.append(QByteArray::fromHex("A40000000000000000000000000000000000000000"));
    udpSocket->writeDatagram(datagram, receiverAddress, receiverPort);
}
