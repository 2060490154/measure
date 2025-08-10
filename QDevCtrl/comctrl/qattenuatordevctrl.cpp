#include "qattenuatordevctrl.h"
#include <QEventLoop>
QAttenuatorDevCtrl::QAttenuatorDevCtrl(QString ComPort,QObject *parent) : QObject(parent)
{
    this->serialPort = new QSerialPort;
    findFreePorts();
    initSerialPort(ComPort);
//    connect(ui->openCom, &QCheckBox::toggled, [=](bool checked){
//        if (checked){
//            initSerialPort();
//            ui->btnSend->setEnabled(true);
//        }else{
//            this->serialPort->close();
//            ui->btnSend->setEnabled(false);
//            ui->openCom->setChecked(false);
//        }
//    });
    connect(this->serialPort, SIGNAL(readyRead()), this, SLOT(recvMsg()));
//    connect(ui->btnSend, &QPushButton::clicked, [=](){
//        sendMsg(ui->message->toPlainText());
//    });
//    connect(timer, &QTimer::timeout, this, &QAttenuatorDevCtrl::dealMsg);
}

QAttenuatorDevCtrl::~QAttenuatorDevCtrl()
{

}

//寻找空闲状态串口
void QAttenuatorDevCtrl::findFreePorts(){
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (int i = 0; i < ports.size(); ++i){
//        if (ports.at(i).isBusy()){
//            ports.removeAt(i);
//            continue;
//        }
        portnameList.append(ports.at(i).portName());
//      ui->portName->addItem(ports.at(i).portName());
    }
    if (!ports.size()){
        QMessageBox::warning(NULL,"Tip",QStringLiteral("找不到空闲串口"));
        return;
    }
}

void QAttenuatorDevCtrl::closePorts(){
    this->serialPort->close();
}

void QAttenuatorDevCtrl::waitForDealMsg()
{
    QEventLoop loop;
    connect(this, &QAttenuatorDevCtrl::dataProcessed, &loop, &QEventLoop::quit);
    timer->start(20); // 开始等待数据报
    loop.exec(); // 进入局部事件循环，等待 dataProcessed 信号
    timer->stop(); // 退出事件循环后停止计时器
}

//初始化串口
bool QAttenuatorDevCtrl::initSerialPort(QString ComPort){
//  this->serialPort->setPortName(ui->portName->currentText());
    for(int i = 0; i < portnameList.size(); i++){
        if(portnameList.at(i) == ComPort){
              this->serialPort->setPortName(ComPort);
              if (!this->serialPort->open(QIODevice::ReadWrite)){
                  QMessageBox::warning(NULL,"Tip",QStringLiteral("串口打开失败"));
                  return false;
              }
              this->serialPort->setBaudRate(115200);
              this->serialPort->setDataBits(QSerialPort::Data8);
              this->serialPort->setStopBits(QSerialPort::OneStop);
              this->serialPort->setParity(QSerialPort::NoParity);
               return true;


        }
    }
    QMessageBox::warning(NULL,"Tip",QStringLiteral("串口不存在或被占用"));
    return false;
}



void QAttenuatorDevCtrl::sendMsg(const QString &msg){
    QMutexLocker locker(&mutex);
    this->serialPort->write(QByteArray::fromHex(acsii_to_Hex(msg).toLatin1()));

}
//接受来自串口的信息
void QAttenuatorDevCtrl::recvMsg(){
    QMutexLocker locker(&mutex);
    LeastestReceivedMsg = this->serialPort->readAll();  //lbz
    emit portsreadsignal();
}

QString QAttenuatorDevCtrl::acsii_to_Hex(QString str)
{
    QString finalString("");
    for(int i = 0; i < str.size();i++){
        finalString.append(QString(QString(str.at(i)).toLatin1().toHex()));
        finalString.append(" ");
    }
    finalString = finalString + "0d 0a";

    return finalString;
}
