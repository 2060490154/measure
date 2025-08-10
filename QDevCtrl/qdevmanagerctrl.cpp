/***************************************************************************
**                                                                                               **
**QDevManagerCtrl为所有设备管理类                                       **
** 包括电机、显微镜、科学CCD、示波器等
** 提供的功能函数为：
** getDevStatus                              获取所有设备的状态
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qdevmanagerctrl.h"
#include "qudpcs.h"
#include "imagedialog.h"
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QTime>
#include <QDateTime>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
QDevManagerCtrl::QDevManagerCtrl(tDevConfigParam* tDevConfig,QObject *parent) : QObject(parent)
{
    m_nDetectionMethod = DETECTION_MING;    //默认使用明场照明
    m_pDefWidget = new QDefectWidget();
    connect(m_pDefWidget,&QDefectWidget::signal_YesBtnClicked,this,&QDevManagerCtrl::onUserChooseYes);
    connect(m_pDefWidget,&QDefectWidget::signal_NoBtnClicked,this,&QDevManagerCtrl::onUserChooseNo);
    m_pDevConfigData = tDevConfig;

    memset(&m_tdevStatus,0,sizeof(m_tdevStatus));
    //能量计设备
    m_pEnergyMeterDevManager = new QEnergyMeterDeviceManager();
    m_pEnergyMeterDevManager->m_tEnergyConfigInfoList = &m_pDevConfigData->m_tEnergyConfigInfoList;
    if(m_pEnergyMeterDevManager->m_pDevicCom == NULL)
    {
        QMessageBox::critical(NULL,"","能量计Com组件未注册",QMessageBox::Ok);
    }

    m_pEnergyMeterDevManager->ScanAllDevice();//获取设备个数

    //显微探测
    m_pIDSDevCtrl = new QIDSCamerDevCtrl();
    m_pIDSDevCtrl->OpenCamera();//打开设备

    //电机设备
    m_pMotorDevManager = new QMotorDevManager();

    //示波器设备
    m_pOsciDevCtrl = new QOsciDevCtrl(m_pDevConfigData->m_sOSCIIIPAddress,m_pDevConfigData->m_tOSCIIVaildChannelList);

    //科学CCD
    m_pKxccdCtrl = new QKxccdCtrl(this);
    m_pKxccdCtrl->setDevIPAndPort(m_pDevConfigData->m_sKxccdIPAddress,m_pDevConfigData->m_nKxccdPort);
    m_pKxccdCtrl->openDev();

    //光闸
    m_pShutterCtrl = new QShutterDevCtrl(m_pDevConfigData->m_nShutterPort,m_pDevConfigData->m_tShutterDevIPList);
    m_pPortCtrl1 = new QAttenuatorDevCtrl("COM5");
    m_pPortCtrl1->serialPort->close();
    m_pPortCtrl2 = new QAttenuatorDevCtrl("COM6");
    m_pPortCtrl2->serialPort->close();
    m_pPortCtrl3 = new QAttenuatorDevCtrl("COM7");
    m_pPortCtrl3->serialPort->close();
    //数据处理
    m_nStopPoint = -1;
    m_nStopTimes = -1;

    //显示
    getDevStatus();
}

/******************************************
*功能：析构函数 资源清理
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
QDevManagerCtrl::~QDevManagerCtrl()
{
    delete m_pIDSDevCtrl;
    delete m_pKxccdCtrl;
    delete m_pMotorDevManager;
    delete m_pOsciDevCtrl;

}


/******************************************
*功能：获取设备当前状态
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
void QDevManagerCtrl::getDevStatus()
{
    m_tdevStatus.bEngerMeter = m_pEnergyMeterDevManager->m_nDevicNum == 0?false:true;

    m_tdevStatus.bIDSCamer = m_pIDSDevCtrl->getDevStatus();

    m_tdevStatus.bMotor = m_pMotorDevManager->getAllMotorDevStatus();

    m_tdevStatus.bOscii = m_pOsciDevCtrl->getDevStatus();

    m_tdevStatus.bKxccd = m_pKxccdCtrl->getDevStatus();

    m_tdevStatus.bshutterStatus = m_pShutterCtrl->getDevStatus();

}

/******************************
*功能:关闸log
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDevManagerCtrl::LogShutter(QString slog)
{
    QFile tem_log(QApplication::applicationDirPath() + "/Log/shutterLog.txt");
    if (!(tem_log.open(QIODevice::Append)))
    {
        qDebug()<<"open failed";
    }
    else
    {
        QTextStream in(&tem_log);
        in<<slog<<"\n";
        tem_log.close();
    }
}

/*******************************************************************
**功能：IDS任务
**输入：
**输出：
**返回值：
*******************************************************************/
bool QDevManagerCtrl::  RunIDSTask(QWorkTask* pTask)
{
    int  nStartTime = GetTickCount();
    int nEndTime = nStartTime;

    QUDPcs *UDPcs = new QUDPcs();

    if(!m_pIDSDevCtrl->OpenCamera())
    {
        pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
        return false;
    }
    else
        pTask->m_nStepStatus = M_STEP_STATUS_RUNNING;

    switch (pTask->m_nAction)
    {
    case M_SET_TRIGGER:
    {
        if(m_pIDSDevCtrl->SetTriggerMode(pTask->m_nParam))
        {
            bool bCap =  m_pIDSDevCtrl->StartCapImage(false);
            pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
        }
        else
        {
            pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
            qDebug()<<"IDS相机：设置触发模式失败！";
        }

        }
                      break;
    case M_CAP_IMAG:
            {
                m_pIDSDevCtrl->m_bHasNewImage = false;
                //判断是否捕获到数据
                while(nEndTime - nStartTime <= M_WAITFOR_IDS_CAP_IMAG_TIME)
                {
                    nEndTime =  GetTickCount();
                    if(m_pIDSDevCtrl->m_bHasNewImage == true)
                    {
                        //存入本地文件夹IDSIMAGE中,以实验编号为子文件夹
                        QDir tem_NewSampleDir(m_sSavePath + QString("/IDSIMAGE"));
                        if (!(tem_NewSampleDir.exists()))
                        {
                            tem_NewSampleDir.mkdir(m_sSavePath + QString("/IDSIMAGE"));
                        }
                        QString sImgName = m_sSavePath + QString("/IDSIMAGE/%1_%2_%3_%4.jpg").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
                        //m_pIDSDevCtrl->SaveImage(sImgName);

                        QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
                        QString dsn = QString::fromLocal8Bit("QtLocalDSN");
                        db.setDatabaseName(dsn);
                        db.setHostName("local");  //数据库主机名
                        db.setPort(1433);
                        db.setUserName("sa");
                        db.setPassword("123456");
                        if(!db.open())
                        {
                            qDebug("=== %s",qPrintable(db.lastError().text()));
                            QMessageBox msgBox;

                            msgBox.setWindowTitle("連接失败");

                            msgBox.setText("!db.open()");
                            msgBox.setStandardButtons(QMessageBox::Ok);

                            msgBox.exec();

                        }else
                        {
                            //QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and PointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
                            Sleep(100);
                            QString sSql = QString("select top 1 imagedata from T_DEFECT_SYSTEM_DATA order by time desc");
                            QSqlQuery sqlQuery(db);
                            bool bOk = sqlQuery.exec(sSql);
                            if (bOk==true)
                            {

                            }
                            else
                            {
                                QMessageBox msgBox;

                                msgBox.setWindowTitle("操作失敗");

                                msgBox.setText(sqlQuery.lastError().text());
                                msgBox.setStandardButtons(QMessageBox::Ok);

                                msgBox.exec();
                            }
                            while(sqlQuery.next())
                            {
                                QByteArray l_byteArray;
                                l_byteArray = sqlQuery.value(0).toByteArray();
                                if(l_byteArray.size()>0)
                                {
                                    QFile l_file(QString("%1").arg(sImgName));
                                    l_file.open(QFile::ReadWrite);
                                    l_file.write(l_byteArray);
                                    l_file.close();
                                }
                            }
                            //wzj0324
                             db.close();

                        }

                        //数据到了  写入数据库中 同时通知上层 显示图片
                        QVariant var;
                        var.setValue(pTask->m_tExpInfo);

                        emit signal_capIDSImageBuff(var,pTask->m_nParam,sImgName);//pTask->m_nParam 0为打靶前图像 1为打靶后图像
                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                        return true;
                    }
                }

                if(nEndTime - nStartTime > M_WAITFOR_STEP_TIME)
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                    qDebug()<<"IDS相机：捕获图像超时！";
                    return false;
                }
            }
                break;
        case M_SEND_PLAN:
        {
            break;
        }
        case M_SEND_INFO:
        {
            UDPcs->scanIndex = QString::number(pTask->m_tExpInfo.m_nPointNo,16);
            UDPcs->frameInfo = QString::number(pTask->m_tExpInfo.m_nTimes,16);//yhy0321
//            QString sImgName = QString("%1_%2").arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes);
//            QMessageBox::warning(nullptr, "超时警告", sImgName);

            QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
            QString dsn = QString::fromLocal8Bit("QtLocalDSN");
            db.setDatabaseName(dsn);
            db.setHostName("local");  //数据库主机名
            db.setPort(1433);
            db.setUserName("sa");
            db.setPassword("123456");
            if(!db.open())
            {
                qDebug("=== %s",qPrintable(db.lastError().text()));
                QMessageBox msgBox;
                msgBox.setWindowTitle("連接失败");
                msgBox.setText("!db.open()");
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();
            }
            else
            {
                //QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and PointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
                QString sSql1 = QString("insert into T_PROCESS_DATA (ExpNo,pointNo,shotNo) values('%1',%2,%3)").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes);
                QSqlQuery sqlQuery1(db);
                bool bOk1 = sqlQuery1.exec(sSql1);
                if (bOk1==true)
                {

                }
                else
                {
                    QMessageBox msgBox;

                    msgBox.setWindowTitle("操作失敗1");

                    msgBox.setText(sqlQuery1.lastError().text());
                    msgBox.setStandardButtons(QMessageBox::Ok);

                    msgBox.exec();
                }

            }

            UDPcs->SEND_INFO();
            UDPcs->waitForDealMsg();
            bool res = UDPcs->lastDealMsgResult;

            if(res)
            {
                //存入本地文件夹IDSIMAGE中,以实验编号为子文件夹
                QDir tem_NewSampleDir(m_sSavePath + QString("/IDSIMAGE"));
                if (!(tem_NewSampleDir.exists()))
                {
                    tem_NewSampleDir.mkdir(m_sSavePath + QString("/IDSIMAGE"));
                }
                QString sImgName = m_sSavePath + QString("/IDSIMAGE/%1_%2_%3_%4.jpg").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);

                //m_pIDSDevCtrl->SaveImage(sImgName);
//                QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
//                QString dsn = QString::fromLocal8Bit("QtLocalDSN");
//                db.setDatabaseName(dsn);
//                db.setHostName("local");  //数据库主机名
//                db.setPort(1433);
//                db.setUserName("sa");
//                db.setPassword("123456");
//                if(!db.open())
//                {
//                    qDebug("=== %s",qPrintable(db.lastError().text()));
//                    QMessageBox msgBox;
//                    msgBox.setWindowTitle("連接失败");
//                    msgBox.setText("!db.open()");
//                    msgBox.setStandardButtons(QMessageBox::Ok);
//                    msgBox.exec();
//                }
//                else
//                {
                    //QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and PointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
//                    QString sSql = QString("insert into T_PROCESS_DATA (ExpNo,pointNo,shotNo) values('%1',%2,%3)").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes);
//                    QSqlQuery sqlQuery(db);
//                    bool bOk = sqlQuery.exec(sSql);
//                    if (bOk==true)
//                    {

//                    }
//                    else
//                    {
//                        QMessageBox msgBox;

//                        msgBox.setWindowTitle("操作失敗1");

//                        msgBox.setText(sqlQuery.lastError().text());
//                        msgBox.setStandardButtons(QMessageBox::Ok);

//                        msgBox.exec();
//                    }

                    Sleep(100);

                    QString sSql2 = QString("select top 1 imagedata from T_DEFECT_SYSTEM_DATA order by time desc");
                    QSqlQuery sqlQuery2(db);
                    bool bOk2 = sqlQuery2.exec(sSql2);
                    if (bOk2==true)
                    {

                    }
                    else
                    {
                        QMessageBox msgBox;

                        msgBox.setWindowTitle("操作失敗");

                        msgBox.setText(sqlQuery2.lastError().text());
                        msgBox.setStandardButtons(QMessageBox::Ok);

                        msgBox.exec();
                    }
                    while(sqlQuery2.next())
                    {
                        QByteArray l_byteArray;
                        l_byteArray = sqlQuery2.value(0).toByteArray();
                        if(l_byteArray.size()>0)
                        {
                            QFile l_file(QString("%1").arg(sImgName));
                            l_file.open(QFile::ReadWrite);
                            l_file.write(l_byteArray);
                            l_file.close();
                        }
                    }
                    //wzj0324
//                     db.close();

//                }

                //数据到了  写入数据库中 同时通知上层 显示图片
                QVariant var;
                var.setValue(pTask->m_tExpInfo);
                emit signal_capIDSImageBuff(var,pTask->m_nParam,sImgName);//pTask->m_nParam 0为打靶前图像 1为打靶后图像

            }
            db.close();
            if(res){
                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
            }else{
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
            }
            return res;
            break;
        }
        case M_GET_DATA:
        {
            UDPcs->scanIndex = QString::number(pTask->m_tExpInfo.m_nPointNo,16);
            UDPcs->frameInfo = QString::number(pTask->m_tExpInfo.m_nTimes,16);//yhy0321
            UDPcs->GET_DATA();
            UDPcs->waitForDealMsg();
            bool res = UDPcs->lastDealMsgResult;
            if(UDPcs->yesorno){
                m_nStopPoint = pTask->m_tExpInfo.m_nPointNo;
                m_nStopTimes = pTask->m_tExpInfo.m_nTimes;
            }
            if(res){
                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
            }else{
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
            }
            return res;
            break;
        }
        case M_START_DEAL:
        {
//            Sleep(1000);
            UDPcs->scanIndex = QString::number(pTask->m_tExpInfo.m_nPointNo,16);
            UDPcs->frameInfo = QString::number(pTask->m_tExpInfo.m_nTimes,16);//yhy0321
            UDPcs->START_DEAL();
            UDPcs->waitForDealMsg();
            bool res = UDPcs->lastDealMsgResult;
            if(res)
            {
                //存入本地文件夹IDSIMAGE中,以实验编号为子文件夹
                QDir tem_NewSampleDir(m_sSavePath + QString("/IDSIMAGE"));
                if (!(tem_NewSampleDir.exists()))
                {
                    tem_NewSampleDir.mkdir(m_sSavePath + QString("/IDSIMAGE"));
                }
                QString sImgName = m_sSavePath + QString("/IDSIMAGE/%1_%2_%3_%4.jpg").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);

                //m_pIDSDevCtrl->SaveImage(sImgName);
                QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
                QString dsn = QString::fromLocal8Bit("QtLocalDSN");
                db.setDatabaseName(dsn);
                db.setHostName("local");  //数据库主机名
                db.setPort(1433);
                db.setUserName("sa");
                db.setPassword("123456");
                if(!db.open())
                {
                    qDebug("=== %s",qPrintable(db.lastError().text()));
                    QMessageBox msgBox;
                    msgBox.setWindowTitle("連接失败");
                    msgBox.setText("!db.open()");
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.exec();

                }else
                {
                    //QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and pointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
                    //Sleep(100);
                    while(1)
                    {


                    //QString sSql = QString("select top 1 imagedata from T_DEFECT_SYSTEM_DATA order by time desc");
                    QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and pointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
                    //QString sSql = QString("select top 1 imagedata from T_DEFECT_SYSTEM_DATA order by time desc");
                    QSqlQuery sqlQuery(db);
                    bool bOk = sqlQuery.exec(sSql);
                    if (bOk==true)
                    {

                    }
                    else
                    {
                        QMessageBox msgBox;
                        msgBox.setWindowTitle("操作失敗");
                        msgBox.setText(sqlQuery.lastError().text());
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.exec();
                    }
                    sqlQuery.next();
                    QByteArray l_byteArray;
                    l_byteArray = sqlQuery.value(0).toByteArray();
//                  //qDebug()<<l_byteArray.size();
                    if(l_byteArray.size()>0)
                    {

//                    if (bOk==true)
//                    {

//                    }
//                    else
//                    {
//                        QMessageBox msgBox;
//                        msgBox.setWindowTitle("操作失敗");
//                        msgBox.setText(sqlQuery.lastError().text());
//                        msgBox.setStandardButtons(QMessageBox::Ok);
//                        msgBox.exec();
//                    }
//                    while(sqlQuery.next())
//                    {
//                        QByteArray l_byteArray;
//                        l_byteArray = sqlQuery.value(0).toByteArray();
                        if(l_byteArray.size()>0)
                        {
                            QFile l_file(QString("%1").arg(sImgName));
                            l_file.open(QFile::ReadWrite);
                            l_file.write(l_byteArray);
                            l_file.close();
                        }
//                    }
                    break;
                    }
                    }
                    //wzj0324
                     db.close();
                }

                QVariant var;
                var.setValue(pTask->m_tExpInfo);

                emit signal_capIDSImageBuff(var,pTask->m_nParam,sImgName);//pTask->m_nParam 0为打靶前图像 1为打靶后图像

            }

            if(res){
                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
            }else{
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
            }
            return res;
            break;
        }
        case M_END_DEAL:
        {
            UDPcs->END_DEAL();
            UDPcs->waitForDealMsg();
            bool res = UDPcs->lastDealMsgResult;
            if(res){
                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
            }else{
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
            }
            return res;
            break;
        }
    case M_COM:
    {
//ywc
 if(pTask->m_nParam1  ==1){
//        QMutexLocker locker(&mutex);
        m_pAttenuator=new QAttenuatorDevCtrl("COM5");
        m_pAttenuator->sendMsg(QString("LPA>PWR!_%1").arg(pTask->m_nParam));
//        m_pAttenuator->waitForDealMsg();
        QEventLoop loop;
        connect(m_pAttenuator,&QAttenuatorDevCtrl::portsreadsignal,
                m_pAttenuator,&QAttenuatorDevCtrl::closePorts);
        connect(m_pAttenuator, &QAttenuatorDevCtrl::portsreadsignal, &loop, &QEventLoop::quit);

        loop.exec(); // 进入局部事件循环，等待 dataProcessed 信号
//        m_pAttenuator->timer->stop();

        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
}
 if(pTask->m_nParam1  ==2){
//        QMutexLocker locker(&mutex);
        m_pAttenuator=new QAttenuatorDevCtrl("COM6");
        m_pAttenuator->sendMsg(QString("LPA>PWR!_%1").arg(pTask->m_nParam));
//        m_pAttenuator->waitForDealMsg();
        QEventLoop loop;
        connect(m_pAttenuator,&QAttenuatorDevCtrl::portsreadsignal,
                m_pAttenuator,&QAttenuatorDevCtrl::closePorts);
        connect(m_pAttenuator, &QAttenuatorDevCtrl::portsreadsignal, &loop, &QEventLoop::quit);

        loop.exec(); // 进入局部事件循环，等待 dataProcessed 信号
//        m_pAttenuator->timer->stop();

        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
}
 if(pTask->m_nParam1  ==3){
//        QMutexLocker locker(&mutex);
        m_pAttenuator=new QAttenuatorDevCtrl("COM7");
        m_pAttenuator->sendMsg(QString("LPA>PWR!_%1").arg(pTask->m_nParam));
//        m_pAttenuator->waitForDealMsg();
        QEventLoop loop;
        connect(m_pAttenuator,&QAttenuatorDevCtrl::portsreadsignal,
                m_pAttenuator,&QAttenuatorDevCtrl::closePorts);
        connect(m_pAttenuator, &QAttenuatorDevCtrl::portsreadsignal, &loop, &QEventLoop::quit);

        loop.exec(); // 进入局部事件循环，等待 dataProcessed 信号
//        m_pAttenuator->timer->stop();

        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
}


//        QMessageBox::warning(NULL,"Tip",QStringLiteral("发送"));
        return true;
        break;
    }







        }

        return true;
    }

        /*******************************************************************
**功能：执行示波器任务
**输入：pTask 任务配置
**输出：无
**返回值：true 成功 false 失败
*******************************************************************/
        bool QDevManagerCtrl::RunOsciTask(QWorkTask* pTask)
        {
            if(!m_pOsciDevCtrl->openDev())
            {
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                return false;
            }


            switch (pTask->m_nAction)
            {
            case M_SET_OSCI_PARAM://参数设置
            {
                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;

                if(!m_pOsciDevCtrl->setTriggerMode(M_TRIGGER_MODE_AUTO))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }

                if(!m_pOsciDevCtrl->setSampleRate(1e12/m_pOsciDevCtrl->m_lfSampleRate ))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }

                if(!m_pOsciDevCtrl->setTirggerLevelEx(m_pOsciDevCtrl->m_lfTriggerlevelEx))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }

                if(!m_pOsciDevCtrl->setTirggerDelay(m_pOsciDevCtrl->m_lfTriggerDelay))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }

                if(!m_pOsciDevCtrl->setTriggerChannel(m_pOsciDevCtrl->m_nTriggerChannel))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }

                if(!m_pOsciDevCtrl->setTriggerSlope(m_pOsciDevCtrl->m_nTriggerChannel,m_pOsciDevCtrl->m_nTriggerSlope[m_pOsciDevCtrl->m_nTriggerChannel]))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }

                if(!m_pOsciDevCtrl->setTimeDiv(m_pOsciDevCtrl->m_lfDivTime*1e-9))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }

            }
                break;
            case M_CAP_OSCI_DATA:
            {

                bool bRel = false;
                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
//                QMessageBox::warning(nullptr, "執行", "for前");
//                for(int i = 0; i < 4;i++)
//                {
//                    bRel = m_pOsciDevCtrl->capChannelData(i);
//                    if(bRel == false)
//                    {
//                        pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
//                    }
//                }
//                QMessageBox::warning(nullptr, "執行", "for后");
                //读取脉宽
                float lfPlusWidth = m_pOsciDevCtrl->getMeasureParam(0x01);

                Sleep(300);
                //发送信号 激活写数据库操作
                if( pTask->m_nStepStatus  == M_STEP_STATUS_SUCCESS)
                {                  
                    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
                    QString dsn = QString::fromLocal8Bit("QtLocalDSN");
                    db.setDatabaseName(dsn);
                    db.setHostName("local");  //数据库主机名
                    db.setPort(1433);
                    db.setUserName("sa");
                    db.setPassword("123456");
                    if (!db.open())
                    {
                        QMessageBox msgBox;
                        msgBox.setWindowTitle("notopen");
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.exec();
                    }
                    QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                    QString sSql = QString("insert into dbo.T_OSCI_DATA(ExpNo,DateTime,PulseWidth) values('%1',?,%2)").arg(pTask->m_tExpInfo.m_sExpNo).arg(lfPlusWidth*1000000000);

                    QSqlQuery sqlQuery(db);
                    sqlQuery.prepare(sSql);
//                    sqlQuery.addBindValue(pTask->m_tExpInfo.m_sExpNo);
                    sqlQuery.addBindValue(CurrentTime);
//                    sqlQuery.addBindValue(lfPlusWidth);
                    bool bOk = sqlQuery.exec();
                    if (bOk==true)
                    {

                    }
                    else
                    {
                        QMessageBox msgBox;
                        msgBox.setWindowTitle("fail");
                        msgBox.setText(sqlQuery.lastError().text());
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.exec();
                    }
                    //wzj0324
                     db.close();


//                    QString sFilePath = QApplication::applicationDirPath()+"\\TimeWave\\" ;
//                    QVariant var;
//                    var.setValue(pTask->m_tExpInfo);
//                    emit signal_wirteOsciDataToDb(var, sFilePath,lfPlusWidth);


                }
            }
                break;
            }

            return true;

        }


        /*******************************************************************
**功能：执行电机任务
**输入：pTask为任务信息
**输出：运动状态
**返回值：
*******************************************************************/
        bool QDevManagerCtrl::RunMotorTask(QWorkTask* pTask)
        {
            int nMotorCtrlIndex = (pTask->m_nDevNo>>8)&0xff;
            QMotorCtrl* pMotorCtrl = m_pMotorDevManager->m_pMotorCtrl[nMotorCtrlIndex];//驱动器索引
            int nMotorIndex = pTask->m_nDevNo & 0x00ff;//电机在驱动器中的索引

            pMotorCtrl-> m_bStopRunTask = false;
            pTask->m_nStepStatus = M_STEP_STATUS_RUNNING;

            if(!pMotorCtrl->checkIsConnect())
            {
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                return false;
            }

            //根据控制盒编号和电机编号查找电机系数、电机类型
            for (int i=0; i<m_pAllDevConfigInfo->m_tMotorConfigInfoList.size(); i++)
            {
                if ( nMotorCtrlIndex == m_pAllDevConfigInfo->m_tMotorConfigInfoList[i].nMotorCtrlIndex && nMotorIndex == m_pAllDevConfigInfo->m_tMotorConfigInfoList[i].nMotorIndex)
                {
                    pMotorCtrl->m_pMotorTypeCoeff[nMotorIndex].m_nMotorType = m_pAllDevConfigInfo->m_tMotorConfigInfoList[i].nMortorType;
                    pMotorCtrl->m_pMotorTypeCoeff[nMotorIndex].m_nMotorCoeff = m_pAllDevConfigInfo->m_tMotorConfigInfoList[i].nMotorCoeff;
                }
            }
            if(pTask->m_nParam1 == M_MOVE_ABS)//绝对位置运动
            {
                pMotorCtrl->MoveToAbsPosition(nMotorIndex,pTask->m_nParam);
            }

            if(pTask->m_nParam1 == M_MOVE_RELATIVE)//默认为相对位置运动
            {
                pMotorCtrl->MoveToRelativePosition(nMotorIndex,pTask->m_nParam);
            }


            pTask->m_nStepStatus = pMotorCtrl->m_tMotorStatusdata[nMotorIndex].m_nRunState;

            return true;
        }

        /*******************************************************************
**功能：执行能量计任务
**输入：
**输出：
**返回值：
** 20190516增加自动调节量程功能
*******************************************************************/
        bool QDevManagerCtrl::RunEnergyMeterTask(QWorkTask* pTask)
        {
            pTask->m_nStepStatus = M_STEP_STATUS_RUNNING;

            if(m_pEnergyMeterDevManager->m_tEnergyMerterArray.size() == 0)
            {
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                return false;
            }

            CEnergyMeter* pEnergyMeterCtrl = m_pEnergyMeterDevManager->m_tEnergyMerterArray[pTask->m_nDevNo];

            if(!pEnergyMeterCtrl->OpenDevice())
            {
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                return false;
            }

            //获取当前量程
            m_nCurrentRange = pEnergyMeterCtrl->GetCurrentRange();
            switch(pTask->m_nAction)
            {
            case M_SET_PARAM://设置参数
            {
                if(!pEnergyMeterCtrl->SetMeasureMode(1))//根据实际修改 测量模式----1为Energy模式
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                    return false;
                }

                if( !pEnergyMeterCtrl->SetMeasureRange(3))//测量范围 根据实际修改-----3为20mJ量程
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                    return false;
                }

                if( !pEnergyMeterCtrl->SetMeasureWavelength(3-pTask->m_nParam))//测量波长----通过m_nParam参数传入，通过“3-”运算转换成能量计中对应波长的index
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                    return false;
                }

                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;

            }

                break;

            case M_CAP_DATA://采集数据
            {
//                Sleep(1000);
                if(!pEnergyMeterCtrl->GetMeasureData())
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                    return false;
                }



                QVariant var;
                var.setValue(pTask->m_tExpInfo);
                emit signal_updateEnergyData(pTask->m_nDevNo,pEnergyMeterCtrl->m_tEnergyMeterParam.m_sMeasureValue,var);

                if(pTask->m_nDevNo < 4)
                {
                    bool bok = false;
                    m_lfCurrentEngData[pTask->m_nDevNo] = pEnergyMeterCtrl->m_tEnergyMeterParam.m_sMeasureValue.toFloat(&bok)*m_pDevConfigData->m_tEnergyConfigInfoList[pTask->m_nDevNo]->lfCoeff;
                }
                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                //判断是否需要提升量程
                //2020-01-06Yi修改：不需要对能量计的量程自动调整
                //            if (pEnergyMeterCtrl->m_tEnergyMeterParam.m_sMeasureValue.toDouble()/(double)m_nCurrentRange > 0.5)
                //            {
                //                int tem_nUpIndex = pEnergyMeterCtrl->m_tEnergyMeterParam.m_nCurrentRangeIndex - 1;
                //                pEnergyMeterCtrl->SetMeasureRange(tem_nUpIndex<0?0:tem_nUpIndex);
                //            }
            }
                break;
            }
            return true;
        }


        /*******************************************************************
**功能：执行kxccd任务
**输入：
**输出：
**返回值：
*******************************************************************/
        bool QDevManagerCtrl::RunKxccdTask(QWorkTask* pTask)
        {
            if(!m_pKxccdCtrl->openDev())
            {
                pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                return false;
            }
            else
                pTask->m_nStepStatus = M_STEP_STATUS_RUNNING;

            switch (pTask->m_nAction)
            {
            case M_SET_TRIGGER:
            {
                if(m_pKxccdCtrl->setWorkMode(M_CAPTYPE_SYN_OUT_NATIVE))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                }
                else
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;

            }
                break;
            case M_CAP_IMAG:
            {
                if (!(m_dManualArea == 0))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                    return true;
                }
                else
                {
                    if(m_pKxccdCtrl->startReCap())
                    {
                        m_pKxccdCtrl->saveImgeToFile(QApplication::applicationDirPath() + QString("/CCDIMG/ccdimg.bmp"));
                        //上传图像成功
                        QVariant var;
                        var.setValue(pTask->m_tExpInfo);
                        //                 emit signal_capKxccdImageBuff((char*)m_pKxccdCtrl->m_higImg.m_pHigImage,m_pKxccdCtrl->m_higImg.m_pHeaderInfo->w,m_pKxccdCtrl->m_higImg.m_pHeaderInfo->h,var);
                        //                m_higImg.m_pBmpData
                        //存ccd图像到本地
                        //            m_pKxccdCtrl->m_higImg.processHig2Bmp();
                        emit signal_capKxccdImageBuff((char*)m_pKxccdCtrl->m_higImg.m_pBmpData,m_pKxccdCtrl->m_higImg.m_pHeaderInfo->w,m_pKxccdCtrl->m_higImg.m_pHeaderInfo->h,var);
                        //            QImage qIm = QImage(m_pKxccdCtrl->m_higImg.m_pBmpData, m_pKxccdCtrl->m_higImg.m_pHeaderInfo->w, m_pKxccdCtrl->m_higImg.m_pHeaderInfo->h, QImage::Format_Indexed8);  //封装QImage
                        //            qIm.save(QApplication::applicationDirPath() + QString("/CCDIMG/ccdimg.bmp"));
                        //            qIm.setColorTable(m_pKxccdCtrl->m_higImg.m_vcolorTable); //设置颜色表
                        //            QPixmap qPixImg;
                        //            qPixImg.fromImage(qIm);
                        //            qPixImg.save(QApplication::applicationDirPath() + QString("/CCDIMG/ccdimg.bmp"));
                        //            qIm.save(QApplication::applicationDirPath() + QString("/CCDIMG/ccdimg.bmp"));


                        //            m_ImageBuff = QImage(pbuff, bufWidth, bufHight, QImage::Format_Indexed8);  //封装QImage
                        //            m_ImageBuff.setColorTable(m_vcolorTable); //设置颜色表
                        //            m_Pixmap = QPixmap::fromImage(m_ImageTempBuff)

                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                        return true;

                    }
                    else
                    {
                        pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                        return false;
                    }
                }

            }
                break;
            case M_NORECAP_IMAG:
            {
                if(m_pKxccdCtrl->startCap())
                {
                    m_pKxccdCtrl->saveImgeToFile(QApplication::applicationDirPath() + QString("/CCDIMG/ccdimg.bmp"));
                    pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                    if (pTask->m_nParam == 1)
                    {
                        m_pKxccdCtrl->stopCap();
                    }
                    return true;
                }
                else
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                    return false;
                }
            }
                break;
            }

            return true;
        }

        /*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
        bool QDevManagerCtrl:: RunShutterTask(QWorkTask* pTask)
        {
            pTask->m_nStepStatus = M_STEP_STATUS_RUNNING;

            switch (pTask->m_nAction)
            {
            case M_CLOSE_SHUTTER:
            {
#if 0 //yidongchi 20210426
                if (pTask->m_nDevNo == 1)
                {
                    if(m_pShutterCtrl->SetDevStatus(pTask->m_nDevNo,true))
                    {
                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                    }
                    else
                        pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }
                else
                {
                    if(m_pShutterCtrl->SetDevStatus(pTask->m_nDevNo,false))
                    {
                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                    }
                    else
                        pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                }
#endif
                if(m_pShutterCtrl->SetDevStatus(pTask->m_nDevNo,false))
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                }
                else
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;

            }
                break;
            case M_OPEN_SHUTTER:
            {

                int tem_nRepeatCnt = 0;
                if(m_pShutterCtrl->setProcessStatus(pTask->m_nDevNo,pTask->m_nParam,pTask->m_nParam1))
                {

                    pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                    if (pTask->m_nDevNo == 1)
                    {
                        emit signal_NewShutterTestResult(pTask->m_tExpInfo.m_nPointNo, 1);
                    }
                }
                else
                {
                    //查询监视能量计以进行进一步判断
                    if (m_pEnergyMeterDevManager->m_tEnergyMerterArray[0]->GetEnergyFlag()) //执行成功但状态未更新
                    {
                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                        if (pTask->m_nDevNo == 1)
                        {
                            emit signal_NewShutterTestResult(pTask->m_tExpInfo.m_nPointNo, 2);
                        }
                        LogShutter(QString::number(pTask->m_nDevNo) + " ENERGYMETER_USED: SUCCESS" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                    }
                    else    //执行失败
                    {
                        LogShutter("==============================UNOPEND============================");
                        while (tem_nRepeatCnt < 5)
                        {
                            LogShutter(QString("try %1 time: ").arg(tem_nRepeatCnt));
                            Sleep(300);
                            if(m_pShutterCtrl->setProcessStatus(pTask->m_nDevNo,pTask->m_nParam,pTask->m_nParam1))
                            {
                                pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
                                if (pTask->m_nDevNo == 1)
                                {
                                    emit signal_NewShutterTestResult(pTask->m_tExpInfo.m_nPointNo, 3);
                                }
                                LogShutter(QString::number(pTask->m_nDevNo) + " SEND_CMD_AGAIN: SUCCESS AT" + QString::number(tem_nRepeatCnt) + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                                break;
                            }
                            else
                            {
                                LogShutter(QString::number(pTask->m_nDevNo) + " SEND_CMD_AGAIN: FAILD AT" + QString::number(tem_nRepeatCnt) + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                            }
                            tem_nRepeatCnt++;
                        }
                        if (pTask->m_nStepStatus == M_STEP_STATUS_RUNNING)
                        {
                            if (pTask->m_nDevNo == 1)
                            {
                                emit signal_NewShutterTestResult(pTask->m_tExpInfo.m_nPointNo, 0);
                            }
                            pTask->m_nStepStatus = M_STEP_STATUS_FAILD;
                        }
                    }
                }


            }



                break;
            }

            return true;
        }


        /*******************************************************************
**功能：数据处理功能
**输入：
**输出：
**返回值：
*******************************************************************/
        bool QDevManagerCtrl:: RunDataProcess(QWorkTask* pTask)
        {
            pTask->m_nStepStatus = M_STEP_STATUS_RUNNING;
            switch (pTask->m_nAction)
            {
            case M_IMG_1ON1:
            {
                //实验编号pTask->m_tExpInfo.m_sExpNo
                //点编号m_tExpInfo.m_nPointNo
                //发次pTask->m_tExpInfo.m_nTimes
                QVariant var;
                var.setValue(pTask->m_tExpInfo);
                QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
                QString dsn = QString::fromLocal8Bit("QtLocalDSN");
                db.setDatabaseName(dsn);
                db.setHostName("local");  //数据库主机名
                db.setPort(1433);
                db.setUserName("sa");
                db.setPassword("123456");
                int res = 0;
                if(!db.open())
                {
                    qDebug("=== %s",qPrintable(db.lastError().text()));
                    QMessageBox msgBox;
                    msgBox.setWindowTitle("連接失败");
                    msgBox.setText("!db.open()");
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.exec();

                }
                else
                {
                    //QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and PointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
                    QString sSql = QString("select defect from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and pointNo=%2 and imagetype=1").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo);
                    QSqlQuery sqlQuery(db);
                    bool bOk = sqlQuery.exec(sSql);
                    if (bOk==true)
                    {

                    }
                    else
                    {
                        QMessageBox msgBox;
                        msgBox.setWindowTitle("操作失敗");
                        msgBox.setText(sqlQuery.lastError().text());
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.exec();
                    }
                    while(sqlQuery.next())
                    {
                        res=sqlQuery.value(0).toInt();
                    }
                    //wzj0324
                     db.close();

                }

                if(1){
                    QString strOriImg = m_sSavePath + QString("/IDSIMAGE/%1_%2_%3_0.jpg").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes);
                    QString strDefImg = m_sSavePath + QString("/IDSIMAGE/%1_%2_%3_1.jpg").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes);
                    emit signal_UserEnterAction();
                    showFullScreenModalDialog(strOriImg,strDefImg);
                }
            }
                break;
            case M_CALC_FLUX:
            {
                QVariant var;
                var.setValue(pTask->m_tExpInfo);

                if(!(m_dManualArea == 0))
                {
                    double lfArea = m_dManualArea;//单位平方厘米
                    //调用能量计数据
                    double lfFlux = (m_lfCurrentEngData[0]/1000)/lfArea;   //m_lfCurrentEngData[0]是乘过分光比后的能量(单位焦耳每平方厘米)
                    emit singal_showCalcFluxStatus(var,lfFlux);
                    pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;   //计算面积成功
                    return true;
                }
                else
                {
                    QDataProcessCtrl *pDataprocess = NULL;
                    pDataprocess = new QDataProcessCtrl();
                    pDataprocess->m_sSavePath = m_sSavePath;
                    double lfArea = 0;
                    double lfArea_e2 = 0;
                    if(pDataprocess->CalclSpotArea(lfArea,lfArea_e2))
                    {
                        lfArea = lfArea*(13*13)/(1e8);//单位平方厘米
                        //调用能量计数据
                        double lfFlux = (m_lfCurrentEngData[0]/1000)/lfArea;   //m_lfCurrentEngData[0]是乘过分光比后的能量(单位焦耳每平方厘米)
                        emit singal_showCalcFluxStatus(var,lfFlux);
                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;   //计算面积成功
                        delete pDataprocess;
                        return true;
                    }
                    else
                    {
                        pTask->m_nStepStatus = M_STEP_STATUS_FAILD;   //计算面积失败，打开文件失败
                        delete pDataprocess;
                        return false;
                    }
                }
            }
                break;
            case M_CALC_AREA:
            {
                QDataProcessCtrl *pDataprocess = NULL;
                pDataprocess = new QDataProcessCtrl();
                pDataprocess->m_phigImg = &m_pKxccdCtrl->m_higImg;
                double lfArea = 0;
                double lfArea_e2 = 0;
                if(pDataprocess->CalclSpotArea(lfArea,lfArea_e2))
                {
                    //判断光斑面积大于图像一半的面积，即认为当前发次未拍摄到光斑，应剔除当前数据
                    if (lfArea > m_pKxccdCtrl->m_higImg.m_pHeaderInfo->w * m_pKxccdCtrl->m_higImg.m_pHeaderInfo->h / 2)
                    {
                        emit signal_sendArea(0,0);
                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;   //计算面积成功
                        delete pDataprocess;
                        return true;
                    }
                    else
                    {
                        lfArea = lfArea*(13*13)/(1e8);//单位平方厘米
                        lfArea_e2 = lfArea_e2*(13*13)/(1e8);
                        emit signal_sendArea(lfArea,lfArea_e2);
                        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;   //计算面积成功
                        delete pDataprocess;
                        return true;
                    }
                }
                else
                {
                    pTask->m_nStepStatus = M_STEP_STATUS_FAILD;   //计算面积失败，打开文件失败
                    delete pDataprocess;
                    return false;
                }
            }
                break;
            }
            return true;
        }


        /******************************
*功能:用户认为没有损伤
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
        void QDevManagerCtrl::onUserChooseNo(QVariant tTaskExpInfo)
        {
            emit signal_UserDenyDef(tTaskExpInfo);
            m_pDefWidget->hide();
        }

        /******************************
*功能:用户认为有损伤
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
        void QDevManagerCtrl::onUserChooseYes(QVariant tTaskExpInfo)
        {
            emit signal_UserConfirmDef(tTaskExpInfo);
            m_pDefWidget->hide();
        }
/******************************
*功能:显示两张图像
*输入:
*输出:
*返回值:
*修改记录:yhy0323
*******************************/
void QDevManagerCtrl::showFullScreenModalDialog(const QString &path1, const QString &path2) {
    ImageDialog *dialog = new ImageDialog();
    dialog->showMaximized();
    dialog->setImagePaths(path1, path2);
    dialog->exec();
}
