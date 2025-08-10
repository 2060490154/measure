/**************************************************
*文件说明:西光所光仪室科学、普通CCD控制类
*
*维护记录:
*2019-01-25  create by lg
******************************************************/
#include "qkxccdctrl.h"
#include <QTime>
#include <QCoreApplication>
#include <QMessageBox>
#include <qeventloop.h>
#include <windows.h>

QKxccdCtrl::QKxccdCtrl(QObject *parent) : QObject(parent)
{
    m_nCameraStatus = M_CAMERA_DISCONNECT;
    m_tDevSocket.m_pCmdSocket = NULL;
    m_tDevSocket.m_pDataSocket = NULL;

    memset(&m_tRcvKxccdData,0,sizeof(m_tRcvKxccdData));
    m_tRcvKxccdData.m_pImageData = NULL;

    m_nDevWorkMode = M_CAPTYPE_NONE;

    _pSearchDevSendSocket = NULL;
    _pSearchDevRcvSocket = NULL;


    m_nRcvFrameCount = 0;
    m_nLineCnt = 0;

    tParamInfo l_st_paramInfo;
    l_st_paramInfo.sParamName = "曝光时间";
    l_st_paramInfo.nCmdVaule = M_CMD_SET_EXPOSURE;
    l_st_paramInfo.nMinValue = 1;
    l_st_paramInfo.nMaxValue = 400;
    l_st_paramInfo.bSetOk = false;
    l_st_paramInfo.nCurrentValue = 1;
    m_st_ParamInfoList.append(l_st_paramInfo);

    tParamInfo l_st_paramInfo1;
    l_st_paramInfo1.sParamName = "增益";
    l_st_paramInfo1.nCmdVaule = M_CMD_SET_GAIN;
    l_st_paramInfo1.nMinValue = 1;
    l_st_paramInfo1.nMaxValue = 63;
    l_st_paramInfo1.nCurrentValue = 1;
    l_st_paramInfo1.bSetOk = false;
    m_st_ParamInfoList.append(l_st_paramInfo1);

    tParamInfo l_st_paramInfo2;
    l_st_paramInfo2.sParamName = "偏置";
    l_st_paramInfo2.nCmdVaule = M_CMD_SET_OFFSET;
    l_st_paramInfo2.nMinValue = 1;
    l_st_paramInfo2.nMaxValue = 511;
    l_st_paramInfo2.nCurrentValue = 1;
    l_st_paramInfo2.bSetOk = false;
    m_st_ParamInfoList.append(l_st_paramInfo2);



}

QKxccdCtrl::~QKxccdCtrl()
{
    closeDev();
}

/******************************************
* 功能:判断是否连接
* 输入:无
* 输出:无
* 返回值:是否连接
* 维护记录:
******************************************/
bool QKxccdCtrl:: isDevOpen()
{
    if (m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return false;
    }
    return true;
}
/*******************************************************************
**功能：设置设备的IP地址
**输入：
**输出：
**返回值：
*******************************************************************/
bool QKxccdCtrl::setDevIPAndPort(QString sIP,int nPort)
{
    m_tDevSocket.m_sDevIP = sIP;
    m_tDevSocket.m_nInitPort = nPort;
    return true;
}


//打开设备
bool QKxccdCtrl::openDev()
 {
    bool bRel = false;

    if(m_nCameraStatus != M_CAMERA_DISCONNECT)
    {
        emit signal_DevConnectStatus(bRel);
        return true;
    }

    if (m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        createSocket(m_tDevSocket.m_sDevIP,m_tDevSocket.m_nInitPort);

        sendCmd(M_CMD_LINK_DEV_OPEN);

        //延时等待是否连接成功
        QTime dieTime = QTime::currentTime();
        dieTime.start();
        while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            if(dieTime.elapsed()%200 == 0)
            {
                sendCmd(M_CMD_LINK_DEV_OPEN);//每200ms发送一次
            }

            if(m_nCameraStatus == M_CAMERA_CONNECT)
            {
                bRel = true;                
                break;
            }
        }
    }

    emit signal_DevConnectStatus(bRel);

    return bRel;

 }

//断开连接
bool QKxccdCtrl::closeDev()
{
    bool bRel = false;

    if (m_nCameraStatus != M_CAMERA_DISCONNECT)
    {
        sendCmd(M_CMD_LINK_DEV_CLOSE);

        //延时等待是否连接成功
        QTime dieTime = QTime::currentTime();
        dieTime.start();
        while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            if(dieTime.elapsed()%200 == 0)
            {
                sendCmd(M_CMD_LINK_DEV_CLOSE);//每200ms发送一次
            }

        }
    }


    //防止中间断电 无法重连
    m_nCameraStatus = M_CAMERA_DISCONNECT;
    deleteSocket();
    bRel = true;


    return bRel;

}

//设置采集模式
bool QKxccdCtrl::setWorkMode(int nType)
{
    bool bRel = false;
    bool bSendCmd1 = false;
    bool bSendCmd2 = false;

    if(m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return bRel;
    }

    m_nDevWorkMode = M_CAPTYPE_NONE;
    m_nTransLateMode = M_TRANS_NONE;
    m_nTriggerMode = M_TRIGGER_NONE;


    switch(nType)
    {
        case M_CAPTYPE_SYN_IN_ACTIVE://内同步
        {
            bSendCmd1 = sendCmd(M_CMD_DEV_SYN_MODE,0x00);
            Sleep(100);
            bSendCmd2 = sendCmd(M_CMD_DEV_TRANSLATE_MODE,0x00);
        }
        break;

        case M_CAPTYPE_SYN_OUT_ACTIVE://主动 外同步
        {
            bSendCmd1 = sendCmd(M_CMD_DEV_SYN_MODE,0x01);
            Sleep(100);
            bSendCmd2 = sendCmd(M_CMD_DEV_TRANSLATE_MODE,0x00);
        }

        break;

        case M_CAPTYPE_SYN_OUT_NATIVE://被动 外同步
        {
            bSendCmd1 = sendCmd(M_CMD_DEV_SYN_MODE,0x01);
            Sleep(100);
            bSendCmd2 = sendCmd(M_CMD_DEV_TRANSLATE_MODE,0x01);
        }
        break;
    }

    if(bSendCmd1 == false || bSendCmd2 == false)
    {
        return false;
    }

    //延时等待是否设置成功
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if(m_nDevWorkMode == nType)
        {
            bRel = true;
            break;
        }
    }

    return bRel;
}

//创建socket
void QKxccdCtrl::createSocket(const QString sIP,int nPort)
{
    deleteSocket();
    qDebug()<<this<<"Current:";
    m_tDevSocket.m_nDevCmdPort = nPort;
    m_tDevSocket.m_nDevDataPort = nPort + 1;
    m_tDevSocket.m_tHostIP.setAddress(sIP);
    m_tDevSocket.m_pCmdSocket = new QUdpSocket(this);
    m_tDevSocket.m_pCmdSocket->bind(m_tDevSocket.m_nDevCmdPort,QUdpSocket::ShareAddress);
    //m_tDevSocket.m_pCmdSocket->bind( m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevCmdPort);
    QObject::connect(m_tDevSocket.m_pCmdSocket,&QUdpSocket::readyRead,this,&QKxccdCtrl::rcvCmdData);//绑定的是rcver的消息 对应给出响应的处理槽
    m_tDevSocket.m_pDataSocket = new QUdpSocket(this);
    m_tDevSocket.m_pDataSocket->bind(m_tDevSocket.m_nDevDataPort,QUdpSocket::ShareAddress);
   m_tDevSocket.m_pDataSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,1024*1032*50);
    //m_tDevSocket.m_pDataSocket->bind( m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevDataPort);
    QObject::connect(m_tDevSocket.m_pDataSocket,&QUdpSocket::readyRead,this,&QKxccdCtrl::rcvDevData);//绑定的是rcver的消息 对应给出响应的处理槽

}
//删除socket
void QKxccdCtrl::deleteSocket()
{
    if(m_tDevSocket.m_pCmdSocket != NULL)
    {
        m_tDevSocket.m_pCmdSocket->deleteLater();
        m_tDevSocket.m_pCmdSocket = NULL;
    }
    if(m_tDevSocket.m_pDataSocket != NULL)
    {
        m_tDevSocket.m_pDataSocket->deleteLater();
        m_tDevSocket.m_pDataSocket = NULL;
    }

}
 //命令接收
 void QKxccdCtrl::rcvCmdData()
 {
     QHostAddress sender;
     quint16 senderport;
     char buff[100];
     while(m_tDevSocket.m_pCmdSocket->hasPendingDatagrams())
     {
         int nSize = m_tDevSocket.m_pCmdSocket->pendingDatagramSize();
         m_tDevSocket.m_pCmdSocket->readDatagram(buff,sizeof(buff),&sender,&senderport);

         cmdProcess(buff,nSize);
     }

 }
 //命令处理
 void QKxccdCtrl::cmdProcess(char* pbuff,int nlen)
 {

     PACK_CMD* pCmd = (PACK_CMD*)pbuff;
     switch(pCmd->m_cmd)
     {
        case  M_CMD_LINK_DEV_OPEN:
        {
         m_nCameraStatus = M_CAMERA_CONNECT;
        }
        break;
        case  M_CMD_LINK_DEV_CLOSE:
        {
            m_nCameraStatus = M_CAMERA_DISCONNECT;
        }
        break;
        case M_CMD_CAP_START:
        {
            m_nCameraStatus = M_CAMERA_START_CAP;
        }
         break;
        case M_CMD_CAP_CLOSE:
        {
            m_nCameraStatus = M_CAMERA_STOP_CAP;
        }
         break;
        case M_CMD_CAP_ONE:
        {
            m_nCameraStatus = M_CAMERA_START_RECAP;
        }
         break;

        case M_CMD_DEV_SYN_MODE:
        {
             if(pCmd->m_tCmdData.m_ndata == 0x01)
             {
                 m_nTriggerMode = M_TRIGGER_SYN_OUT;
             }
             else if(pCmd->m_tCmdData.m_ndata == 0x00)
             {
                  m_nTriggerMode = M_TRIGGER_SYN_IN;
             }
             mergeWorkMode();
        }
         break;
        case M_CMD_DEV_TRANSLATE_MODE:
        {
            if(pCmd->m_tCmdData.m_ndata == 0x00)
            {
                m_nTransLateMode = M_TRANS_ACTIVE;
            }
            else
                m_nTransLateMode = M_TRANS_NATIVE;

            mergeWorkMode();
        }
          break;
        case M_CMD_DEV_MODIFY_IP:
        {
            QString sIP =QString("%1.%2.%3.%4").arg(pCmd->m_tCmdData.m_data[3]).arg(pCmd->m_tCmdData.m_data[2]).arg(pCmd->m_tCmdData.m_data[1]).arg(pCmd->m_tCmdData.m_data[0]);
            m_tDevSocket.m_sDevIP = sIP;

            //重新绑定
            m_tDevSocket.m_tHostIP.setAddress(sIP);
            m_tDevSocket.m_pCmdSocket->bind( m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevCmdPort);
            m_tDevSocket.m_pDataSocket->bind( m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevDataPort);
            m_bModifyIP = true;
        }
            break;
        case M_CMD_SET_EXPOSURE://设置曝光时间
        {
            for(int i = 0; i < m_st_ParamInfoList.size();i++)
            {
                if(m_st_ParamInfoList[i].nCmdVaule == pCmd->m_cmd)
                {
                    m_st_ParamInfoList[i].bSetOk = true;
                    break;
                }

            }
        }
        break;
        case M_CMD_SET_GAIN://设置增益
        {
            for(int i = 0; i < m_st_ParamInfoList.size();i++)
            {
                if(m_st_ParamInfoList[i].nCmdVaule == pCmd->m_cmd)
                {
                    m_st_ParamInfoList[i].bSetOk = true;
                    break;
                }

            }
        }
        break;
        case M_CMD_SET_OFFSET://设置偏置
        {
            for(int i = 0; i < m_st_ParamInfoList.size();i++)
            {
                if(m_st_ParamInfoList[i].nCmdVaule == pCmd->m_cmd)
                {
                    m_st_ParamInfoList[i].bSetOk = true;
                    break;
                }
            }
        }
        break;
     }

 }

 //发送请求重传数据命令
 bool QKxccdCtrl::sendRecapRequest(int nCmdID)
 {
     bool bRel = false;
     char* pbuff = new char[2];
     pbuff[0] = nCmdID;
     pbuff[1] = '\0';


      int nlen = m_tDevSocket.m_pDataSocket->writeDatagram(pbuff,1,m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevDataPort);

      if(nlen > 0 )
      {
          bRel =  true;
      }
      return bRel;
 }



 //发送  bGetParam = true为查询
 bool QKxccdCtrl::sendCmd(int nCmdID,int nValue,bool bGetParam)
 {
     bool bRel = false;
     PACK_CMD tCmd;
     memset(&tCmd,0,sizeof(PACK_CMD));
     tCmd.m_cmd = nCmdID;
     tCmd.m_tCmdData.m_ndata = nValue;
     if(bGetParam == true)
     {
        tCmd.m_response = 0x01;
     }


      int nlen = m_tDevSocket.m_pCmdSocket->writeDatagram((char*)&tCmd,sizeof(PACK_CMD),m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevCmdPort);

      if(nlen > 0 )
      {
          bRel =  true;
      }

      return bRel;
 }

 //开始采集
 bool QKxccdCtrl::startCap()
 {
     bool bRel = false;

     if(m_nCameraStatus == M_CAMERA_DISCONNECT)
     {
         return bRel;
     }

     sendCmd(M_CMD_CAP_START);


     //延时等待是否设置成功
     QTime dieTime = QTime::currentTime();
     dieTime.start();
     while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
     {
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

         if( m_nCameraStatus == M_CAMERA_START_CAP)
         {
             bRel = true;
             break;
         }
     }

     return bRel;
 }

 /******************************************
* 功能:停止采集
* 输入:
* 输出:
* 返回值:
* 维护记录:
* 2019-01-25 create by lg
******************************************/
 bool QKxccdCtrl::stopCap()
 {
     bool bRel = false;

     if(m_nCameraStatus == M_CAMERA_DISCONNECT)
     {
         return bRel;
     }

     sendCmd(M_CMD_CAP_CLOSE);


     //延时等待是否设置成功
     QTime dieTime = QTime::currentTime();
     dieTime.start();
     while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
     {
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

         if( m_nCameraStatus == M_CAMERA_STOP_CAP)
         {
             bRel = true;
             break;
         }
     }

     return bRel;
 }

 //开始重传
bool QKxccdCtrl::startReCap()
{
#if 0
    bool bRel = false;
    if(m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return bRel;
    }


    sendCmd(M_CMD_CAP_ONE,0);
    Sleep(500);

    m_tRcvKxccdData.m_bImageStart = false;
    m_tRcvKxccdData.m_bImageReCapOK = false;

    //延时等待是否连接成功
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    if(m_nCameraStatus == M_CAMERA_START_RECAP)
    {
        if( m_tRcvKxccdData.m_bImageStart == false)
        {
            sendRecapRequest(M_CMD_RECAP_TRANS_IMG_HEADER);//发送请求数据头
            qDebug()<<"===发送请求数据头";
        }
         Sleep(300);
    }
    while( dieTime.elapsed() < 3000 )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        if( m_tRcvKxccdData.m_bImageReCapOK == true)
        {
            bRel = true;
        }


    }

    return bRel;
#else
    bool bRel = false;
    if(m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return bRel;
    }


    sendCmd(M_CMD_CAP_ONE,0);
    Sleep(100);

    m_tRcvKxccdData.m_bImageStart = false;
    m_tRcvKxccdData.m_bImageReCapOK = false;

    //延时等待是否连接成功
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < 10000 )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        if(m_nCameraStatus == M_CAMERA_START_RECAP)
        {
            if( m_tRcvKxccdData.m_bImageStart == false)
            {
                sendRecapRequest(M_CMD_RECAP_TRANS_IMG_HEADER);//发送请求数据头
            }
             Sleep(100);
        }

        if( m_tRcvKxccdData.m_bImageReCapOK == true)
        {
            bRel = true;
            break;
        }
    }

    return bRel;
    //;
#endif

}
 //发送数据请求
 bool QKxccdCtrl::sendDataRequset(int nCmdID)
 {
     bool bRel = false;
     char sendbuff[8];
     memset(sendbuff,0,8);

     sendbuff[0] = nCmdID;
     sendbuff[1] = '\0';

      int nlen = m_tDevSocket.m_pDataSocket->writeDatagram(sendbuff,8,m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevDataPort);

      if(nlen > 0 )
      {
          bRel =  true;
      }

      return bRel;
 }

 //接收数据
 void QKxccdCtrl::rcvDevData()
 {
     QHostAddress sender;
     quint16 senderport;
     char buff[4096];
     while(m_tDevSocket.m_pDataSocket->hasPendingDatagrams())
     {
         int nSize = m_tDevSocket.m_pDataSocket->pendingDatagramSize();
         m_tDevSocket.m_pDataSocket->readDatagram(buff,nSize,&sender,&senderport);

         parseRcvData(buff,nSize);
     }

 }

 //解析接收到的数据  根据工作模式不同进行区分
 void QKxccdCtrl::parseRcvData(char* buff,int nlen)
 {
     int nBitLenArray[3] = {8,12,16};

     if (nlen == M_DATA_HEADER_LENTH)
     {
        m_tRcvKxccdData.m_bImageReCapOK = false;
        m_tRcvKxccdData.m_bImageStart = parseDataHeader(buff,nlen);
        if(m_nCameraStatus == M_CAMERA_START_RECAP)//重传状态 需要请求数据
        {
            sendRecapRequest(M_CMD_RECAP_TRANS_DATA);
        }
     }
     else if(m_tRcvKxccdData.m_bImageStart == true)
     {
         if(nlen != 56)
         {
             m_nLineCnt++;
             parseRowData(buff,nlen);
         }
         else  if (m_tRcvKxccdData.m_nRevLen == 0 && nlen == 56)//所有数据接收完毕
         {
             _MutexLock.lock();
            m_nLineCnt = 0;
            if (m_tRcvKxccdData.m_tRcvDataHeaderInfo.bBit > 0)
            {
                  m_higImg.initDataBuf(m_tRcvKxccdData.m_tRcvDataHeaderInfo.wCol,m_tRcvKxccdData.m_tRcvDataHeaderInfo.wRow,nBitLenArray[m_tRcvKxccdData.m_tRcvDataHeaderInfo.bBit-1],(char*)m_tRcvKxccdData.m_pImageData);
            }
            else
            {
                m_tRcvKxccdData.m_bImageStart = false;
                m_tRcvKxccdData.m_bImageReCapOK = false;
                m_tRcvKxccdData.m_nRevLen = 0;
                return;
            }

            m_higImg.processHig2Bmp();
            m_tRcvKxccdData.m_bImageReCapOK = true;
            m_nRcvFrameCount ++;//帧计数
            m_tRcvKxccdData.m_bImageStart = false;
             //显示数据
            if(m_nDevWorkMode != M_CAPTYPE_NONE)
            {
                emit signal_showKxccdImag(m_higImg.m_pBmpData,m_higImg.m_pHeaderInfo->w,m_higImg.m_pHeaderInfo->h);
            }
            _MutexLock.unlock();
         }
         else
         {
             qDebug()<<"重传失败信息："<<m_tRcvKxccdData.m_nRevLen<<nlen;
             //数据错误  接收到最后一帧  但是数据未全到
             m_tRcvKxccdData.m_bImageStart = false;
             m_tRcvKxccdData.m_bImageReCapOK = false;
             m_tRcvKxccdData.m_nRevLen = 0;
             m_nLineCnt = 0;
         }

     }

}


 //数据头解析
 bool QKxccdCtrl::parseDataHeader(char* pBuf,int nLen)
 {
     m_tRcvKxccdData.m_nDataLen = 0;
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.wStartFlag, pBuf,sizeof(WORD));
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.nRevDataLen, pBuf+2,sizeof(int));
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.wRow, pBuf+6,sizeof(WORD));
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.wCol, pBuf+8,sizeof(WORD));
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.bBit,pBuf+10,1);
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.bOffset,pBuf+11,1);
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.wFrameCount,pBuf+12,sizeof(WORD));
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.wRowDataCnt,pBuf+14,2);
     memcpy(&m_tRcvKxccdData.m_tRcvDataHeaderInfo.bReserve,pBuf+16,8);

     if (m_tRcvKxccdData.m_tRcvDataHeaderInfo.wStartFlag !=0xAFAF)
     {
         return false;
     }

     int nBit = m_tRcvKxccdData.m_tRcvDataHeaderInfo.bBit;
     if(nBit == 0)
     {
         m_tRcvKxccdData.m_nDataLen = ((int)m_tRcvKxccdData.m_tRcvDataHeaderInfo.wCol)* ((int)m_tRcvKxccdData.m_tRcvDataHeaderInfo.wRow-m_tRcvKxccdData.m_nInvaildRowNum) ;
     }
     else
     {
         m_tRcvKxccdData.m_nDataLen = ((int)m_tRcvKxccdData.m_tRcvDataHeaderInfo.wCol)* ((int)m_tRcvKxccdData.m_tRcvDataHeaderInfo.wRow-m_tRcvKxccdData.m_nInvaildRowNum) * 2;
     }


     if(m_tRcvKxccdData.m_pImageData !=NULL)
     {
        delete [] m_tRcvKxccdData.m_pImageData;
        m_tRcvKxccdData.m_pImageData = NULL;
     }


     m_tRcvKxccdData.m_pImageData = new uchar[m_tRcvKxccdData.m_nDataLen];
     memset(m_tRcvKxccdData.m_pImageData,0x00,m_tRcvKxccdData.m_nDataLen);
     m_tRcvKxccdData.m_nRevLen = m_tRcvKxccdData.m_tRcvDataHeaderInfo.nRevDataLen;

     return true;
 }

 //数据行信息
 void QKxccdCtrl::parseRowData(char* pBuf,int nLen)
 {
     memcpy(&m_tRcvKxccdData.m_tRcvRowHeaderInfo.wStartFlag ,pBuf,sizeof(WORD));
     memcpy(&m_tRcvKxccdData.m_tRcvRowHeaderInfo.wFrameCount ,pBuf+2,sizeof(WORD));
     memcpy(&m_tRcvKxccdData.m_tRcvRowHeaderInfo.wRowCount ,pBuf+4,sizeof(WORD));
     memcpy(&m_tRcvKxccdData.m_tRcvRowHeaderInfo.bRowflag,pBuf+6,1);
     memcpy(&m_tRcvKxccdData.m_tRcvRowHeaderInfo.bReserve,pBuf+7,1);

     if(m_tRcvKxccdData.m_tRcvRowHeaderInfo.wFrameCount == m_tRcvKxccdData.m_tRcvDataHeaderInfo.wFrameCount) //同一帧
     {
         int offset = getDataOffset();
         if (offset >=0)
         {
             memcpy(m_tRcvKxccdData.m_pImageData + offset,pBuf + M_DATA_ROW_HEADER_LENTH, nLen - M_DATA_ROW_HEADER_LENTH);
         }
         m_tRcvKxccdData.m_nRevLen = m_tRcvKxccdData.m_nRevLen - nLen;
     }
     else
     {
         qDebug()<<"!!!!m_tRcvRowHeaderInfo: "<<m_tRcvKxccdData.m_tRcvRowHeaderInfo.wFrameCount;
         qDebug()<<"!!!!m_tRcvDataHeaderInfo: "<<m_tRcvKxccdData.m_tRcvDataHeaderInfo.wFrameCount;
     }

 }

 /********************************************
 @功能：获取偏移量
 **********************************************/
 int QKxccdCtrl::getDataOffset()
 {
     int nOffset = 0;
     int nRowDataCnt =m_tRcvKxccdData.m_tRcvDataHeaderInfo.wRowDataCnt ;

     if(nRowDataCnt == 0)
     {
         nRowDataCnt = m_tRcvKxccdData.m_tRcvDataHeaderInfo.wCol/2;
     }

     if(m_tRcvKxccdData.m_tRcvDataHeaderInfo.bBit >= 1 )
     {
         nOffset = m_tRcvKxccdData.m_tRcvRowHeaderInfo.wRowCount*m_tRcvKxccdData.m_tRcvDataHeaderInfo.wCol*2 +  (m_tRcvKxccdData.m_tRcvRowHeaderInfo.bRowflag - 1) * nRowDataCnt*2;//10 12 16 均为2字节
     }
     else if(m_tRcvKxccdData.m_tRcvDataHeaderInfo.bBit == 1 )
     {
         nOffset = m_tRcvKxccdData.m_tRcvRowHeaderInfo.wRowCount*m_tRcvKxccdData.m_tRcvDataHeaderInfo.wCol +  (m_tRcvKxccdData.m_tRcvRowHeaderInfo.bRowflag - 1) * nRowDataCnt;
     }

     return nOffset;
 }

/******************************************
* 功能:获取当前设备状态  用于显示
* 输入:无
* 输出:无
* 返回值:当前状态描述信息
* 维护记录:
* 2019-01-25 create by lg
******************************************/
 QString QKxccdCtrl::getDevStatusInfo()
 {
     QString sRel = "";

     if(m_tDevSocket.m_sDevIP.isEmpty())
     {
         sRel += "设备IP地址为:无 \n\n";
     }
     else
        sRel += "设备IP地址为:" + m_tDevSocket.m_sDevIP+"\n\n";

     if(m_nCameraStatus != M_CAMERA_DISCONNECT)
     {
         sRel += "设备连接状态：已连接 \n\n";
     }
     else
     {
        sRel += "设备连接状态：未连接 \n\n";
     }
     sRel = sRel + "工作模式为："+ getDevWorkModeInfo(m_nDevWorkMode)+"\n\n";

     return sRel;

 }


/******************************************
* 功能:获取当前工作模式 用于显示
* 输入:nDevWorkMode--状态类型
* 输出:无
* 返回值:状态文字描述
* 维护记录:
*  2019-01-25 create by lg
******************************************/
 QString QKxccdCtrl::getDevWorkModeInfo(int nDevWorkMode)
 {
     QString sRel;
     switch(nDevWorkMode)
     {
         case M_CAPTYPE_NONE:
            sRel ="无";
         break;
         case M_CAPTYPE_SYN_IN_ACTIVE:
            sRel ="连续内同步模式";
         break;

         case M_CAPTYPE_SYN_OUT_ACTIVE:
            sRel ="连续外触发模式";
         break;

         case M_CAPTYPE_SYN_OUT_NATIVE:
            sRel ="帧存采集模式";
         break;
     }

     return sRel;

 }

 /******************************************
* 功能:状态融合
* 输入:无
* 输出:融合后的状态数据 m_nDevWorkMode
* 返回值:无
* 维护记录:
*  2019-01-25 create by lg
******************************************/
 void QKxccdCtrl::mergeWorkMode()
 {
     m_nDevWorkMode = M_CAPTYPE_NONE;

     if(m_nTriggerMode == M_TRIGGER_SYN_IN && m_nTransLateMode == M_TRANS_ACTIVE)
     {
         m_nDevWorkMode = M_CAPTYPE_SYN_IN_ACTIVE;
     }
    else  if(m_nTriggerMode == M_TRIGGER_SYN_OUT && m_nTransLateMode == M_TRANS_ACTIVE)
     {
         m_nDevWorkMode = M_CAPTYPE_SYN_OUT_ACTIVE;
     }
     else if(m_nTriggerMode == M_TRIGGER_SYN_OUT && m_nTransLateMode == M_TRANS_NATIVE)
     {
         m_nDevWorkMode = M_CAPTYPE_SYN_OUT_NATIVE;
     }
 }

 /******************************************
* 功能:修改IP地址
* 输入:sIP---新的IP地址
* 输出:无
* 返回值:是否成功
* 维护记录:
*  2019-01-25 create by lg
******************************************/
bool QKxccdCtrl::modifyIPAddress(QString sIP)
{
    bool bOk = false;
    bool bRel = false;
    uchar cIPAddress[4];
    QStringList IP = sIP.split('.');
    if(IP.length() != 4 || m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return bRel;
    }

    m_bModifyIP = false;

    cIPAddress[0] = IP.at(0).toInt(&bOk,10);
    cIPAddress[1] = IP.at(1).toInt(&bOk,10);
    cIPAddress[2] = IP.at(2).toInt(&bOk,10);
    cIPAddress[3] = IP.at(3).toInt(&bOk,10);

    int nValue = cIPAddress[0]<<24 |cIPAddress[1]<<16|cIPAddress[2]<<8|cIPAddress[3];
    sendCmd(M_CMD_DEV_MODIFY_IP,nValue);

    //延时等待是否设置成功
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if( m_bModifyIP == true)
        {
            bRel = true;
            break;
        }
    }

    return bRel;
}

/******************************************
* 功能:获取参数数据
* 输入:无
* 输出:参数数据
* 返回值:true成功 false为失败
* 维护记录:
*  2019-01-25 create by lg
******************************************/
bool QKxccdCtrl::getDevParam()
{
    if(m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return false;
    }


    m_st_ParamInfoList[0].bSetOk = false;
    m_st_ParamInfoList[1].bSetOk = false;
    m_st_ParamInfoList[2].bSetOk = false;

    if (!sendCmd(M_CMD_GET_EXPOSURE, 0,true))
    {
        return false;
    }
    Sleep(100);

    if (!sendCmd(M_CMD_GET_GAIN, 0,true))
    {
        return false;
    }
    Sleep(100);

    if (!sendCmd(M_CMD_GET_OFFSET, 0,true))
    {
        return false;
    }
    Sleep(100);

    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if( m_st_ParamInfoList[0].bSetOk == true && m_st_ParamInfoList[1].bSetOk == true && m_st_ParamInfoList[2].bSetOk == true)
        {
            return true;
        }
    }
    return true;

}

//保存参数
bool QKxccdCtrl::saveDevParam()
{
    if(m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return false;
    }

    return sendCmd(M_CMD_SAVE_PARAM);
}

//显示用
bool QKxccdCtrl::getDevStatus()
{
    return isDevOpen();
}


/******************************************
* 功能:设置参数值，根据参数索引查找对应的参数值
* 输入:nIndex为参数索引
* 输出:无
* 返回值:是否设置成功
* 修改记录:
*  2019-01-25 create by lg
******************************************/
bool QKxccdCtrl::setDevParam(int nIndex,int nValue)
{
    bool bRel = false;
    if(m_nCameraStatus == M_CAMERA_DISCONNECT)
    {
        return bRel;
    }

    m_st_ParamInfoList[nIndex].nCurrentValue = nValue;

    tParamInfo* l_st_ParamInfo = &m_st_ParamInfoList[nIndex];

    l_st_ParamInfo->bSetOk = false;

    sendCmd(l_st_ParamInfo->nCmdVaule,l_st_ParamInfo->nCurrentValue);

    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < M_KXCCD_WAITFOR_TIME )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if( l_st_ParamInfo->bSetOk == true)
        {
            bRel = true;
            break;
        }
    }

    return bRel;
}


/******************************************
* 功能:更改窗位、窗宽
* 输入:无
* 输出:无
* 返回值:
* 维护记录:
* 2019-01-25 create by lg
******************************************/
void QKxccdCtrl::updateWindowInfo(int nWindowPos,int nWindowWidth)
{
    _MutexLock.lock();
    m_higImg.setHiGrayTab(nWindowPos,nWindowWidth,16);
    if(m_higImg.m_pHigImage != NULL)
    {
        m_higImg.processHig2Bmp();
        emit signal_showKxccdImag(m_higImg.m_pBmpData,m_higImg.m_pHeaderInfo->w,m_higImg.m_pHeaderInfo->h,false);
    }
    _MutexLock.unlock();
}

/******************************************
* 功能:初始化搜索功能相关资源
* 输入:无
* 输出:iplist清空 sokect初始化
* 返回值:true 成功  false 失败
* 维护记录:
* 2019-01-25 create by lg
******************************************/
bool QKxccdCtrl::initSearchDevResource()
{
    m_sSearchDevIPList.clear();

    if(_pSearchDevSendSocket != NULL)
    {
        _pSearchDevSendSocket->close();
        delete _pSearchDevSendSocket;
        _pSearchDevSendSocket = NULL;
    }
    if(_pSearchDevRcvSocket != NULL)
    {
        _pSearchDevRcvSocket->close();
        delete _pSearchDevRcvSocket;
        _pSearchDevRcvSocket = NULL;
    }

    _pSearchDevSendSocket = new QUdpSocket(this);

    _pSearchDevRcvSocket = new QUdpSocket(this);
    if(!_pSearchDevRcvSocket->bind(M_SEARCH_DEV_RCV_PORT,QUdpSocket::ShareAddress))
    {
        return false;
    }
    else
    {
        QObject::connect(_pSearchDevRcvSocket,&QUdpSocket::readyRead,this,&QKxccdCtrl::rcvSearchData);//绑定的是rcver的消息 对应给出响应的处理槽
    }

    return true;
}

/******************************************
*功能:接收搜索到的消息
*输入:无
*输出:iplist
*返回值:无
*维护记录:
* 2019-01-25 create by lg
******************************************/
void QKxccdCtrl::rcvSearchData()
{
    QHostAddress sender;
    quint16 senderport;
    char buff[4096];
    while(_pSearchDevRcvSocket->hasPendingDatagrams())
    {
        int nSize = _pSearchDevRcvSocket->pendingDatagramSize();
        _pSearchDevRcvSocket->readDatagram(buff,nSize,&sender,&senderport);
        QString sIPAddress = sender.toString();
        QStringList sIPlist = sIPAddress.split(':');

        m_sSearchDevIPList.insert(sIPlist.at(sIPlist.size()-1));
    }
}

/******************************************
* 功能:发送搜索命令
* 输入:无
* 输出:无
* 返回值:true 成功  false 失败
* 维护记录:
*  2019-01-25 create by lg
******************************************/
bool QKxccdCtrl::startSearchDev()
{
    char buf[8] = {0x0,0x1,0x0,0x0,0x01,0x17,0x0,0x0};  //发送查询曝光时间命令

   int nLen = _pSearchDevSendSocket->writeDatagram(buf,8,QHostAddress::Broadcast,M_SEARCH_DEV_SEND_PORT);
   if(nLen == 0)
   {
       return false;
   }
   else
   {
       return true;
   }
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdCtrl::clearSearchDevResource()
{
    if(_pSearchDevSendSocket != NULL)
    {
        _pSearchDevSendSocket->close();
        delete _pSearchDevSendSocket;
        _pSearchDevSendSocket = NULL;
    }
    if(_pSearchDevRcvSocket != NULL)
    {
        _pSearchDevRcvSocket->close();
        delete _pSearchDevRcvSocket;
        _pSearchDevRcvSocket = NULL;
    }

}

/******************************************
* 功能:保存图片
* 输入:
* 输出:
* 返回值:
* 维护记录:
* 2019-01-25 create by lg
* 2019-01-26 增加同步锁 by lg
******************************************/
bool QKxccdCtrl::saveImgeToFile(QString sFilePathName)
{
    _MutexLock.lock();
    if(sFilePathName.length() != 0 && (sFilePathName.endsWith(".bmp")||sFilePathName.endsWith(".BMP")))//根据类型存储
    {
         m_higImg.saveBmpImage(sFilePathName);
    }
    else if(sFilePathName.length() != 0 &&( sFilePathName.endsWith(".hig") ||sFilePathName.endsWith(".HIG") ))//根据类型存储
    {
        m_higImg.saveHigImage(sFilePathName);
    }
    _MutexLock.unlock();

    return true;
}
