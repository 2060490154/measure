/***************************************************************************
**                                                                                               **
**QShutterDevCtrl为光闸设备控制类                                     **
** 提供的功能函数为：
**
****************************************************************************
**创建人：李刚
**创建时间：2018.11.17
**修改记录：
**
****************************************************************************/
#include "QShutterManagerCtrl.h"
#include <QApplication>
#include <QMessageBox>
#include <QTime>
#include <QDebug>


QShutterDevCtrl::QShutterDevCtrl(int nPort,QStringList tDevIPList)
{
    _nPort = nPort;

    m_pUdpSocket = new QUdpSocket(this);
    connect(m_pUdpSocket,&QUdpSocket::readyRead,this,&QShutterDevCtrl::ReceiveData);

    bool result=m_pUdpSocket->bind(_nPort);		//绑定到指定的端口上
    if(!result)
    {
        return;
    }



    //初始化设备信息
   // memset(&m_tShutterDev[0],0,sizeof(tShutterDevInfo)*M_SHUTTER_TOTAL_NUM);
    if(tDevIPList.size() <=M_SHUTTER_TOTAL_NUM)
    {
        for(int i = 0; i < tDevIPList.size(); i++)
        {
            m_tShutterDev[i].tHostAddress.setAddress(tDevIPList.at(i));
            m_tShutterDev[i].bProcessStart = false;
            m_tShutterDev[i].bProcessEnd = false;
            m_tShutterDev[i].nCurrentVoltData = 0;
            m_tShutterDev[i].nSetLimitData = 0;
            m_tShutterDev[i].nCurrentStatus = M_SHUTTER_STATUS_NONE;
        }

    }



    _bDevStatus = checkDevStatus();
}

QShutterDevCtrl::~QShutterDevCtrl()
{

}

/*******************************************************************
**功能：获取状态
**输入：
**输出：
**返回值：
*******************************************************************/
bool QShutterDevCtrl::getDevStatus()
{
    return _bDevStatus;
}

/*******************************************************************
**功能：判断是否通信正常
**输入：
**输出：
**返回值：
*******************************************************************/
bool QShutterDevCtrl::checkDevStatus()
{
    bool bRel = false;

    tShutterCmd cmddata;
    cmddata.startflag = 0xff;
    cmddata.status = 0x01;
    cmddata.cmdType = 0x00;//查询
    cmddata.data[0] = 0x77;
    cmddata.data[1] = 0x0B;
    cmddata.endflag = 0xee;

    for(int i = 0; i < M_SHUTTER_TOTAL_NUM;i++)
    {
        m_tShutterDev[i].nCurrentStatus = M_SHUTTER_STATUS_NONE;//清除当前状态
        m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDev[i].tHostAddress,_nPort);//发送查询命令
    }


    //延时等待是否连接成功
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < 1000 )
    {
         QCoreApplication::processEvents();
        for(int i = 0; i < M_SHUTTER_TOTAL_NUM;i++)
        {
            if( m_tShutterDev[i].nCurrentStatus == M_SHUTTER_STATUS_NONE)
            {
                  bRel = false;
            }
            else
                bRel = true;
        }

        if(bRel == true)
        {
            break;
        }
        Sleep(10);
    }

    return bRel;
}


/*******************************************************************
**功能：设置打开关闭状态
**输入：
**输出：
**返回值：
*******************************************************************/
 bool QShutterDevCtrl::SetDevStatus(int nIndex,bool bOPen)
 {
     bool bRel = false;
     tShutterCmd cmddata;
     int nResend = 0;

     if(nIndex > M_SHUTTER_TOTAL_NUM - 1)
     {
         return false;
     }

     cmddata.startflag = 0xff;
     cmddata.status = 0x00;
     cmddata.cmdType = 0x11;//设置
     cmddata.data[0] = 0x77;
     cmddata.data[1] = 0x0B;
     cmddata.endflag = 0xee;

     if(bOPen == true)//打开
     {
        cmddata.status = 0x01;
     }

     m_tShutterDev[nIndex].nCurrentStatus = M_SHUTTER_STATUS_NONE;//清除当前状态

     m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDev[nIndex].tHostAddress,_nPort);

     //延时等待是否连接成功
     QTime dieTime = QTime::currentTime();
     dieTime.start();
     while( dieTime.elapsed() < 4000 )
     {
         QCoreApplication::processEvents();

         if(bOPen == true &&  m_tShutterDev[nIndex].nCurrentStatus == M_SHUTTER_STATUS_OPENED)
         {
             bRel = true;
             break;
         }

         if(bOPen == false &&  m_tShutterDev[nIndex].nCurrentStatus == M_SHUTTER_STATUS_CLOSED)
         {
             bRel = true;
             break;
         }

         //resend
         if (dieTime.elapsed() > 2000 && m_tShutterDev[nIndex].nCurrentStatus == M_SHUTTER_STATUS_NONE && nResend == 0)
         {
             m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDev[nIndex].tHostAddress,_nPort);
             nResend = 1;
         }
     }

     return bRel;
 }

 /*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
  bool QShutterDevCtrl::SetDevLimit(int nIndex,int ndata)
  {
      bool bRel = false;
      tShutterCmd cmddata;

      if(nIndex > M_SHUTTER_TOTAL_NUM - 1)
      {
          return false;
      }

      cmddata.startflag = 0xff;
      cmddata.status = 0x00;
      cmddata.cmdType = 0x01;//设置电压门限
      cmddata.data[0] = ndata&0xff;
      cmddata.data[1] = (ndata>>8)&0xff;
      cmddata.endflag = 0xee;

      m_tShutterDev[nIndex].nSetLimitData  = 0;

      m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDev[nIndex].tHostAddress,_nPort);

      //延时等待是否连接成功
      QTime dieTime = QTime::currentTime();
      dieTime.start();
      while( dieTime.elapsed() < 1000 )
      {
          QCoreApplication::processEvents();

          if(m_tShutterDev[nIndex].nSetLimitData != 0)
          {
              bRel = true;
              break;
          }
      }

      return bRel;

  }

/*******************************************************************
**功能：接收消息处理
**输入：
**输出：
**返回值：
*******************************************************************/
void QShutterDevCtrl::ReceiveData()
{
    QHostAddress sender;
    quint16 senderport;
    char buff[256];

    while(m_pUdpSocket->hasPendingDatagrams())
    {
        int nSize = m_pUdpSocket->pendingDatagramSize();
        m_pUdpSocket->readDatagram(buff,nSize,&sender,&senderport);

        ParseRcvData(sender.toString(), senderport,buff,nSize);
    }

}

/*******************************************************************
**功能：解析接收的数据 更新对应设备的状态
**输入：
**输出：
**返回值：
*******************************************************************/
void  QShutterDevCtrl::ParseRcvData(QString sIPAddress,int nPort,char* pbuff,int nSize)
{
    int nIndex = -1;
    if(nPort != _nPort )
    {
        return;
    }
    QString sIp = sIPAddress.split(":")[3];


    //查找对应的设备
    for(int i = 0; i < M_SHUTTER_TOTAL_NUM;i++)
    {
        if(m_tShutterDev[i].tHostAddress.toString() == sIp)
        {
            nIndex = i;
            break;
        }
    }

    if(nIndex == -1)
    {
        return;
    }

    //解析帧头帧尾
    int tem_nHeadPos = -1;
    for (int i=0; i<8; i++)
    {
        if (pbuff[i]&0xff == 0xff)
        {
            tem_nHeadPos = i;
            if (i != 0)
            {
                QString tem_curFrame = QString::number(pbuff[i]&0xff,16);
                for (int j=1; j<8; j++)
                {
                    tem_curFrame += (QString(" ") + QString::number(pbuff[i+j]&0xff,16));
                }
                qDebug()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[检测到帧头不在0位，在第%1位；当前帧为：%2]").arg(i).arg(tem_curFrame));
            }
            if (pbuff[i + M_SHUTTER_FREAM_LENGTH - 1]&0xff != 0xee) //非正常帧尾
            {
                QString tem_curFrame = QString::number(pbuff[i]&0xff,16);
                for (int j=1; j<8; j++)
                {
                    tem_curFrame += (QString(" ") + QString::number(pbuff[i+j]&0xff,16));
                }
                qDebug()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[检测到不正常的帧尾，当前帧为：%1]").arg(tem_curFrame));
                return;
            }
            else
            {
                break;
            }
        }
    }
    if (tem_nHeadPos == -1)
    {
        QString tem_curFrame = QString::number(pbuff[0]&0xff,16);
        for (int j=1; j<8; j++)
        {
            tem_curFrame += (QString(" ") + QString::number(pbuff[0+j]&0xff,16));
        }
        qDebug()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[当前帧未检测到帧头，当前帧为：%1]").arg(tem_curFrame));
        return;
    }


    //解析数据 ff 01 11 00 00(设定值) 00 00 (采集值) ee   第二个字节为状态打开/关闭  第三个字节为设定/查询
    if( pbuff[tem_nHeadPos+1] == 0x00 && pbuff[tem_nHeadPos+2] != 0x01)
    {
        m_tShutterDev[nIndex].nCurrentStatus = M_SHUTTER_STATUS_CLOSED;
    }
    if( pbuff[tem_nHeadPos+1] == 0x01&& pbuff[tem_nHeadPos+2] != 0x01)
    {
        m_tShutterDev[nIndex].nCurrentStatus = M_SHUTTER_STATUS_OPENED;
    }

    if( pbuff[tem_nHeadPos+2] == 0x01 || pbuff[tem_nHeadPos+2] == 0x00)
    {
        unsigned short data1 =  (pbuff[tem_nHeadPos+6]<<8)&0xff00;
        unsigned short data2 = pbuff[tem_nHeadPos+5]&0xff;

        m_tShutterDev[nIndex].nCurrentVoltData =data1  + data2 ;

        data1 =  (pbuff[tem_nHeadPos+4]<<8)&0xff00;
        data2 = pbuff[tem_nHeadPos+3]&0xff;
        m_tShutterDev[nIndex].nSetLimitData = data1+data2;
//        m_tShutterDev[nIndex].nCurrentStatus = M_SHUTTER_STATUS_QUERYFINISH;
    }

    if( pbuff[tem_nHeadPos+2] == 0x22)//流程
    {
        QString tem_curFrame = QString::number(pbuff[0]&0xff,16);
        for (int j=1; j<8; j++)
        {
            tem_curFrame += (QString(" ") + QString::number(pbuff[0+j]&0xff,16));
        }
//        qDebug()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[解析到当前帧为：%1  时间：%2]").arg(tem_curFrame).arg(QDateTime::currentDateTime().toString("hh_mm_ss_zzz")));

        unsigned short data =  (pbuff[tem_nHeadPos+5])&0xff;

        if(data == 0xAA)//开始
        {
             m_tShutterDev[nIndex].bProcessStart = true ;
             m_tShutterDev[nIndex].bProcessEnd = false ;
        }

        if(data == 0xBB)//结束
        {
             m_tShutterDev[nIndex].bProcessEnd = true ;
        }
    }

    if ( ((pbuff[tem_nHeadPos+2])&0xff) == 0xAA)  //外部触发关断光闸
    {
        m_tShutterDev[nIndex].bProcessEnd = true ;
    }

}


/*******************************************************************
**功能：设置执行流程状态，并等待流程执行结束
**输入：nPlusCnt为脉冲计数  nDeleay为延时时间（ms）
**输出：
**返回值：
*******************************************************************/
bool QShutterDevCtrl::setProcessStatus(int nIndex,int nPlusCnt,int nDeleay)
{
    bool bRel = false;
    tShutterCmd cmddata;
    int nReSendFlag = 0;
    int nAASUCFlag = 0;

    if(nIndex > M_SHUTTER_TOTAL_NUM - 1)
    {
        return false;
    }

    cmddata.startflag = 0xff;
    cmddata.status = 0x00;  //open
    cmddata.cmdType = 0x22;//设置流程状态
    cmddata.data[0] = nPlusCnt&0xff;
    cmddata.data[1] = nDeleay&0xff;
    cmddata.endflag = 0xee;

    m_tShutterDev[nIndex].bProcessStart = false ;
    m_tShutterDev[nIndex].bProcessEnd = false ;


    m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDev[nIndex].tHostAddress,_nPort);
    qDebug()<<m_tShutterDev[nIndex].tHostAddress.toString()<<_nPort<<"TESTY";

    //延时等待
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < 5000 )//5000
    {
        QCoreApplication::processEvents();

        if(m_tShutterDev[nIndex].bProcessStart  == true && nAASUCFlag == 0)
        {
            nAASUCFlag += 1;
        }

        if(m_tShutterDev[nIndex].bProcessStart  == true && m_tShutterDev[nIndex].bProcessEnd == true)
        {
            bRel = true;
            break;
        }

        //如果发送流程打开命令3秒后没有收到AA信号，则再次发送打开命令
        if (dieTime.elapsed()> 4000 && m_tShutterDev[nIndex].bProcessStart == false && nReSendFlag == 0)//4000
        {
            qint64 tem_resendSize = m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDev[nIndex].tHostAddress,_nPort);
            qDebug()<<M_OUT_MSG( M_SHUTTER_LOG_FILE, QString("[SHUTTER AGAIN] IP：%1 脉冲数：%2 延时：%3 发送长度：%4  发送时间：%5").arg(m_tShutterDev[nIndex].tHostAddress.toString()).arg(nPlusCnt).arg(nDeleay).arg(tem_resendSize).arg(QDateTime::currentDateTime().toString("hh_mm_ss_zzz")));
            nReSendFlag = 1;
        }

    }

    if (bRel == false)
    {
        qDebug()<<M_OUT_MSG( M_SHUTTER_LOG_FILE, QString("失败超时：%1").arg(dieTime.elapsed()));
    }

    return bRel;
}
