#include "qdbdata.h"
#include <QDateTime>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <qfile.h>
#include <QImage>
#include <QApplication>
#include <QDir>
#include <QtMath>
#include <QMessageBox>
#include "QCommon/qexcel.h"
#include <QDir>
#include <QDirIterator>

QDBData::QDBData(QObject *parent) : QObject(parent)
{
    _pErrCodeProcess  =new QErrCodeProcess();
}

//连接数据库
bool QDBData::connectDataBase()
{
    //数据库初始化
    _pDbProcess = new QDatabaseProcess();
    if(!_pDbProcess->connectDatabase("FluxMeasureDB",M_DATABASE_SQLSERVER,"caep-PC","sa","123456"))
    {
        return false;
    }

    return true;
}

bool QDBData::getAllDevConfigInfo()
{

    //电机设备的配置信息
    getMotorConfigInfo();
    getMotorPostionConfig();
    getEnergyMeterConfigInfo();
    return true;
}

//获取实验信息
bool QDBData::onGetExpInfo4Show(QVector<ExpInfo4Show> &ExpInfos)
{
    bool bOk = false;
    ExpInfos.clear();
    QStringList Infolist = _pDbProcess->QueryTable("select* from dbo.T_EXP_INFO order by DateTime desc");
    foreach (QString item, Infolist)
    {
        QStringList strArray = item.split('|');
        if (strArray.size() < 9)
        {
            return false;
        }
        ExpInfo4Show t_expinfo;
        t_expinfo.sExpNo = strArray[0];
        t_expinfo.ExpType = strArray[1].toInt(&bOk);
        t_expinfo.SampleSize = strArray[3];
        t_expinfo.DataTime = strArray[10];
        t_expinfo.SampleName = strArray[2];
        ExpInfos.append(t_expinfo);
    }
    return true;
}


bool QDBData::onGetExpData4Show(QVector<AllExpData4show> &ExpData, QString sExpNo)
{
    bool bOk = false;
    ExpData.clear();
    QString strsql1 = QString("select* from dbo.T_ENERGY_DATA where ExpNo='%1'order by PointNo, Times").arg(sExpNo);
    QStringList configlist1 =  _pDbProcess->QueryTable(strsql1);

    foreach (QString item, configlist1)
    {
        QStringList strArray = item.split('|');

        AllExpData4show t_expData;
        t_expData.m_nPointNo = strArray[4].toInt(&bOk);
        t_expData.m_nShotNo = strArray[3].toInt(&bOk);
        t_expData.m_dEnergy = strArray[2].toDouble(&bOk);
        t_expData.m_dFlux = strArray[5].toDouble(&bOk);
        ExpData.append(t_expData);
    }
    qDebug()<<"FINISH QUERY DATA ENERGY";
    emit signal_queryfinished(0);
    return true;
}




bool QDBData::onGetDefectInfo4Show(QVector<int> &DefectInfo, QString sExpNo)
{
    qDebug()<<"defect info: "<<sExpNo;
    bool bOk = false;
    DefectInfo.clear();
    QString strsql1 = QString("select* from dbo.T_IDS_IMG_DATA where ExpNo='%1' and ImageType='%2'order by PointNo, Times").arg(sExpNo).arg(1);
    QStringList configlist1 =  _pDbProcess->QueryTable(strsql1);

    foreach (QString item, configlist1)
    {
        QStringList strArray = item.split('|');

        DefectInfo.append(strArray[5].toInt(&bOk));
    }
    emit signal_queryfinished(1);
    return true;
}

bool QDBData::onGetDevInfo4Show(QVector<DevInfo4Show> &DevInfos)
{
    DevInfos.clear();
    QStringList Infolist = _pDbProcess->QueryTable("select* from dbo.T_DEVICE_INFO order by DevName");
    foreach (QString item, Infolist)
    {
        QStringList strArray = item.split('|');
        if (strArray.size() < 5)
        {
            return false;
        }

        DevInfo4Show t_DevInfo;
        t_DevInfo.DevName = strArray[0];
        t_DevInfo.DevType = strArray[1];
        t_DevInfo.DevNo = strArray[2];
        t_DevInfo.DevCertiNo = strArray[3];
        t_DevInfo.DevComment = strArray[4];
        DevInfos.append(t_DevInfo);
    }
    return true;
}

bool QDBData::onGetClientInfo(QVector<ClientInfo> &ClientInfos)
{
    ClientInfos.clear();
    QStringList Infolist = _pDbProcess->QueryTable("select* from dbo.T_CLIENT_INFO order by ClientNo desc");
    foreach (QString item, Infolist)
    {
        QStringList strArray = item.split('|');
        if (strArray.size() < 3)
        {
            return false;
        }
        ClientInfo t_ClientInfo;
        t_ClientInfo.CName = strArray[1];
        t_ClientInfo.CAddress = strArray[2];
        t_ClientInfo.CPhone = strArray[3];
        ClientInfos.append(t_ClientInfo);
    }
    return true;
}


bool QDBData::onTestFinished(QString sSampleName,QString sExpNo, QVector<float> &lfFlux, QVector<float> &lfDefect, QVector<double> &dEnergyAver, QVector<double> &dEnergyRMS, int nMeasureType, double &AverFlux, double &RMSFlux,QString sPath,int nWaveLengthType)
{
    m_sSavePath = sPath;
    //保存原始数据
    onGetFinalResultData(sSampleName,sExpNo,nWaveLengthType);

    //计算平均通量和损伤概率
    if (nMeasureType == M_MEASURETYPE_1On1)
    {
        //统计各能量段下的平均通量、平均能量和能量RMS
        QString strsql1 = QString("select* from dbo.T_ENERGY_DATA where ExpNo='%1'").arg(sExpNo);
        QStringList configlist1 =  _pDbProcess->QueryTable(strsql1);

        float ESection[30] = {0.0}; //默认最多30个能量段
        int ESecPtNum[30] = {0};
        double EnergySection[30] = {0.0};
        double EnergySectionRMS[30] = {0.0};
        foreach (QString item, configlist1)
        {
            QStringList strArray = item.split('|');

            if(strArray.size() < 3)
            {
                return false;
            }
            //计算每一能量段的平均通量
            ESection[QString(strArray[6]).toInt()] += QString(strArray[5]).toFloat();//同能量段的通量累加
            ESecPtNum[QString(strArray[6]).toInt()]++;//同能量段点数累计
            EnergySection[QString(strArray[6]).toInt()] += QString(strArray[2]).toDouble();//同能量段能量累加
        }

        foreach (QString item, configlist1)
        {
            QStringList strArray = item.split('|');

            if(strArray.size() < 3)
            {
                return false;
            }
            //RMS
            EnergySectionRMS[QString(strArray[6]).toInt()] += qPow(QString(strArray[2]).toDouble() - EnergySection[QString(strArray[6]).toInt()]/ESecPtNum[QString(strArray[6]).toInt()],2);
        }



        for (int i=0; i<30;i++)
        {
            if (ESecPtNum[i] != 0)
            {
                lfFlux.push_back(ESection[i]/ESecPtNum[i]);
                dEnergyAver.push_back(EnergySection[i]/ESecPtNum[i]);
                dEnergyRMS.push_back(qSqrt(EnergySectionRMS[i]/ESecPtNum[i]));
            }
        }

        //统计各能量段下的损伤概率
        QString strsql2 = QString("select* from dbo.T_IDS_IMG_DATA where ExpNo='%1' and ImageType=%2").arg(sExpNo).arg(1);
        QStringList configlist2 =  _pDbProcess->QueryTable(strsql2);
        int ImgSection[30] = {0}; //默认最多30个能量段
        int ImgSecDefectNum[30] = {0};
        foreach (QString item, configlist2)
        {
            QStringList strArray = item.split('|');

            if(strArray.size() < 3)
            {
                return false;
            }
            ImgSection[QString(strArray[6]).toInt()]++;
            if (QString(strArray[5]).toInt() == 1)
            {
                ImgSecDefectNum[QString(strArray[6]).toInt()]++;
            }
        }

        for (int i=0; i<30;i++)
        {
            if (ImgSection[i] != 0)
            {
                lfDefect.push_back((float)ImgSecDefectNum[i]/(float)ImgSection[i]);
            }
        }
        return true;
    }
    else if (nMeasureType == M_MEASURETYPE_ROn1)
    {
        //最大点号，最大发次号
        int tem_nMaxPtIndex = 0;
        int tem_nMaxTimes = 0;

        QString strsql2 = QString("select* from dbo.T_IDS_IMG_DATA where ExpNo='%1' and ImageType=%2 order by PointNo, Times").arg(sExpNo).arg(1);
        QStringList configlist2 =  _pDbProcess->QueryTable(strsql2);
        QVector<int> tem_point;
        QVector<int> tem_Times;
        QVector<int> tem_IsDefect;
        foreach (QString item, configlist2)
        {
            QStringList strArray = item.split('|');

            if(strArray.size() < 3)
            {
                return false;
            }
            tem_point.push_back(QString(strArray[1]).toInt());
            tem_Times.push_back(QString(strArray[2]).toInt());
            tem_IsDefect.push_back(QString(strArray[5]).toInt());

            if (QString(strArray[1]).toInt() > tem_nMaxPtIndex) { tem_nMaxPtIndex = QString(strArray[1]).toInt();}
            if (QString(strArray[2]).toInt() > tem_nMaxTimes) {tem_nMaxTimes = QString(strArray[2]).toInt();}
        }


        //在能量表中查找通量
        QString strsql1 = QString("select* from dbo.T_ENERGY_DATA where ExpNo='%1' order by PointNo, Times").arg(sExpNo);
        QStringList configlist1 =  _pDbProcess->QueryTable(strsql1);

        QVector<int> tem_pointE;
        QVector<int> tem_TimesE;
        QVector<float> tem_Flux;
        foreach (QString item, configlist1)
        {
            QStringList strArray = item.split('|');

            if(strArray.size() < 3)
            {
                return false;
            }
            tem_pointE.push_back(QString(strArray[4]).toInt());
            tem_TimesE.push_back(QString(strArray[3]).toInt());
            tem_Flux.push_back(QString(strArray[5]).toFloat());
        }

        //求每个能量段的平均通量
        for (int i=0; i<tem_nMaxTimes+1; i++)
        {
            double tem_dSumFlux = 0;
            double tem_nTimesCnt = 0;
            //初始化损伤与未损伤统计值
            int tem_nUndefect = 0;
            int tem_nDefect = 0;

            for (int j=0; j<tem_nMaxPtIndex+1; j++)
            {
                bool tem_bNotFoundTimes = false;
                for (int k=0; k<tem_pointE.size(); k++)
                {
                    //通量统计
                    if (tem_pointE.at(k) == j && tem_TimesE.at(k) == i)
                    {
                        tem_dSumFlux += tem_Flux.at(k);
                        tem_nTimesCnt++;
                    }
                    //损伤统计
                    if (tem_point.at(k) == j && tem_Times.at(k) == i)
                    {
                        if (tem_IsDefect.at(k) == 0) {tem_nUndefect++;}
                        if (tem_IsDefect.at(k) == 1) {tem_nDefect++;}
                        tem_bNotFoundTimes = true;
                        break;
                    }
                    else
                    {
                        tem_bNotFoundTimes = false;
                    }
                }

                if (tem_bNotFoundTimes == false)
                {
                    tem_nDefect++;
                }
            }

            lfFlux.push_back(tem_dSumFlux/(double)tem_nTimesCnt);
            lfDefect.push_back((double)tem_nDefect/(double)(tem_nDefect + tem_nUndefect));
        }

        //求Flux的平均值和RMS
//        AverFlux = lfFlux;
//        for (int i=0; i<lfFlux.size(); i++)
//        {
//            AverFlux
//        }
//        double tem_dFluxSqureSum = 0;
//        for (int i=1; i<lfFlux.size(); i++)
//        {

//        }
    }


    //        for (int j=0; j<tem_CriticlePt.size(); j++)
    //        {
    //            for (int i=0; i<tem_pointE.size(); i++)
    //            {
    //                if (tem_pointE[i] == tem_CriticlePt[j] && tem_TimesE[i] == tem_CriticleTi[j])
    //                {
    //                    if (tem_Times[i] == 0)
    //                    {
    //                        lfFlux.push_back(0);
    //                        lfDefect.push_back((float)tem_CriticlePt[j]);
    //                    }
    //                    else
    //                    {
    //                        lfFlux.push_back(tem_Flux[i]);
    //                        lfDefect.push_back((float)tem_CriticlePt[j]);
    //                    }
    //                }
    //            }
    //        }

//    QVector<int> t_sortvector = tem_CriticleTi;
//    qSort(t_sortvector.begin(),t_sortvector.end());
//    int minTimesvalue = t_sortvector[0];
//    double tem_AverFlux = 0.0;
//    double tem_RMSFlux = 0.0;
//    QVector<float> tem_minTimesFlux;
//    int tem_Cnt = 0;
//    for (int i=0; i<tem_pointE.size(); i++)
//    {
//        if (tem_pointE.at(i) == i && tem_TimesE.at(i) == minTimesvalue)
//        {
//            tem_minTimesFlux.push_back(tem_Flux.at(i));
//            tem_AverFlux += tem_Flux.at(i);
//            tem_Cnt++;
//        }
//    }
//    tem_AverFlux = tem_AverFlux/tem_Cnt;
//    for (int i=0; i<tem_Cnt; i++)
//    {
//        tem_RMSFlux += qPow((tem_minTimesFlux.at(i) - tem_AverFlux),2);
//    }
//    tem_RMSFlux = qSqrt(tem_RMSFlux/tem_Cnt);
//    AverFlux = tem_AverFlux;
//    RMSFlux = tem_RMSFlux;
    return true;
    //;
}


/******************************
*功能:获取实验监视能量数据和损伤情况
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDBData::onGetFinalResultData(QString sSampleName,QString sExpNo,int nWaveType)
{
    m_tPtNo.clear();
    m_tShotNo.clear();
    m_tDefect.clear();
    m_tEnergy.clear();


    bool bOk = false;
    QStringList Infolist = _pDbProcess->QueryTable(QString("select* from dbo.T_ENERGY_DATA where ExpNo='%1' order by PointNo, Times").arg(sExpNo));
    foreach (QString item, Infolist)
    {
        QStringList strArray = item.split('|');
        if (strArray.size() < 8)
        {
            return false;
        }
        int tem_PtNo = strArray[4].toInt(&bOk);
        int tem_ShotNo = strArray[3].toInt(&bOk);
        double tem_Energy = strArray[2].toDouble(&bOk);

        m_tPtNo.append(tem_PtNo);
        m_tShotNo.append(tem_ShotNo);
        m_tEnergy.append(tem_Energy);
    }
    QStringList Infolist2;
    for (int i=0; i<m_tEnergy.size(); i++)
    {
        Infolist2.clear();
        Infolist2 = _pDbProcess->QueryTable(QString("select* from dbo.T_IDS_IMG_DATA where ExpNo='%1'and PointNo=%2 and Times=%3 and ImageType=%4 order by PointNo, Times").\
                                            arg(sExpNo).arg(m_tPtNo.at(i)).arg(m_tShotNo.at(i)).arg(1));
        foreach (QString item, Infolist2)
        {
            QStringList strArray = item.split('|');
            if (strArray.size() < 8)
            {
                return false;
            }
            int tem_Defect = strArray[5].toInt(&bOk);
            m_tDefect.append(tem_Defect);
        }

    }
    //保存文件
    QDir tem_NewSampleDir(m_sSavePath + QString("/IDSIMAGE/DATA"));//修改保存路径 by lg 20190611
    if (!(tem_NewSampleDir.exists()))
    {
        tem_NewSampleDir.mkdir(m_sSavePath + QString("/IDSIMAGE/DATA"));
    }
    QFile file(m_sSavePath + QString("/IDSIMAGE/DATA/%1_%2.txt").arg(sSampleName).arg(sExpNo));
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        return false;

    }


    //191018
    QExcel *m_pExcel = new QExcel();
    m_pExcel->initSheet();
    m_pExcel->setSheetName("实验数据");

    QTextStream in(&file);
    //nWaveType
    QString tem_Energy;
    //实验编号
    in<<"======"<<sExpNo<<"======"<<"\n";
    in<<"PointNO."<<"     ShotNO."<<"     Energy"<<"     Defect\n";
    //ydc21427
    //判断能量计数据精度：1064nm使用200mJ档位，保留一位小数；355nm使用20mJ档，保留两位小数
    int _nEneryRangeType = -1;  //默认-1，0表示一位小数，1表示两位小数
    for (int i=0; i< m_tEnergy.size(); i++)
    {
        tem_Energy = QString::number(m_tEnergy.at(i));
        //如果当前实验数据表中存在两位小数的数据，则认为实验使用了20mJ档位，应保留两位小数
        if (tem_Energy.contains('.') && (tem_Energy.length() - (tem_Energy.indexOf('.')+1) == 2))
        {
            _nEneryRangeType = 1;
            break;
        }
    }
    if(_nEneryRangeType == -1)
    {
        _nEneryRangeType = 0;
    }

    QString _sDataFormat = "";
    if (_nEneryRangeType == 1)
    {
        _sDataFormat = "0.00";
    }
    else
    {
        _sDataFormat = "0.0";
    }
    for (int i=0; i<m_tEnergy.size(); i++)
    {
        tem_Energy = QString::number(m_tEnergy.at(i));
        in<<m_tPtNo.at(i)+1<<"            "<<m_tShotNo.at(i)+1<<"            "<<tem_Energy<<"            "<<m_tDefect.at(i)<<"\n";
        m_pExcel->insertContent((m_tShotNo.at(i)+1),(m_tPtNo.at(i)*2+1),tem_Energy,_sDataFormat);
        m_pExcel->insertContent((m_tShotNo.at(i)+1),(m_tPtNo.at(i)*2+2),QString::number(m_tDefect.at(i)),"0");
    }
    file.close();

    //try excel
    m_pExcel->saveSheet(m_sSavePath + QString("/IDSIMAGE/DATA/%1_%2.xlsx").arg(sSampleName).arg(sExpNo));
    m_pExcel->closeSheet();

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
        msgBox.setWindowTitle("数据库連接失败");
        msgBox.setText("!db.open()");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }else
    {
        QString sSql = QString("select * from dbo.T_PROCESS_DATA where ExpNo='%1'").arg(sExpNo);
        QSqlQuery sqlQuery(db);
        sqlQuery.prepare(sSql);
        if (sqlQuery.exec())
        {
            QString path_csv = m_sSavePath + QString("/IDSIMAGE/DATA/%1_%2.csv").arg(sSampleName).arg(sExpNo);
            QFile file_csv(path_csv);
                if (file_csv.open(QIODevice::WriteOnly | QIODevice::Text)) {
                     QTextStream out(&file_csv);
                     out << "ExpNo,pointNo,shotNo,energydata,defect\n";
                     while (sqlQuery.next()) {
                         out << sqlQuery.value(0).toString() << ","
                             << sqlQuery.value(1).toString() << ","
                             << sqlQuery.value(2).toString() << ","
                             << sqlQuery.value(3).toString() << ","
                             << sqlQuery.value(4).toString() << "\n"
                             ;
                     }
                }
                file_csv.close();

        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("表格存储");
            msgBox.setText(sqlQuery.lastError().text());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        //wzj0324
         db.close();

    }

    delete m_pExcel;
#if 0
    QExcel *m_pExcel = new QExcel();
    m_pExcel->initSheet();
    m_pExcel->setSheetName("实验数据");
    for (int i=0; i<m_tEnergy.size(); i++)
    {
        m_pExcel->insertContent((m_tShotNo.at(i)+1),(m_tPtNo.at(i)*2+1),tem_Energy);
        m_pExcel->insertContent((m_tShotNo.at(i)+1),(m_tPtNo.at(i)*2+2),QString::number(m_tDefect.at(i)));
    }

    m_pExcel->saveSheet(m_sSavePath + QString("/IDSIMAGE/DATA/%1_%2.xlsx").arg(sSampleName).arg(sExpNo));
    m_pExcel->closeSheet();

    delete m_pExcel;
#endif



    return true;
}



//获取配置的电机信息
bool QDBData::getMotorConfigInfo()
{
    bool bOk = false;
   QStringList configlist =  _pDbProcess->QueryTable("select* from dbo.T_MOTOR_CONFIG_INFO order by  MotorCtrlIndex ");

   foreach (QString item, configlist)
   {
       QStringList strArray = item.split('|');

       if(strArray.size() < 3)
       {
           return false;
       }

       tMotorConfigInfo configInfo;

       configInfo.sMotorName = strArray[0];
       configInfo.nMotorCtrlIndex = strArray[1].toInt(&bOk);
       configInfo.nMotorIndex = strArray[2].toInt(&bOk);
       configInfo.nMotorCoeff = strArray[3].toInt(&bOk);
       configInfo.nMortorType = strArray[4].toInt(&bOk);

       m_tAllDevConfigInfo.m_tMotorConfigInfoList.append(configInfo);
   }
    return true;
}

//获取配置的电机档位信息
bool QDBData::getMotorPostionConfig()
{
    bool bOk = false;
    tMotorConfigInfo* pTempConfig = NULL;
   QStringList postionlist =  _pDbProcess->QueryTable("select* from dbo.T_MOTOR_POSTION_CONFIG");

   foreach (QString item, postionlist)
   {
       pTempConfig = NULL;
       QStringList strArray = item.split('|');

       if(strArray.size() < 4)
       {
           return false;
       }

       tMotorPostionInfo postionConfigInfo;

       for(int i = 0; i < m_tAllDevConfigInfo.m_tMotorConfigInfoList.size();i++)
       {
           tMotorConfigInfo* pMotorConfigItem = &m_tAllDevConfigInfo.m_tMotorConfigInfoList[i];

           int nMotorCtrlIndex = strArray[0].toInt(&bOk);
           int nMotorIndex = strArray[1].toInt(&bOk);

           if(pMotorConfigItem->nMotorIndex == nMotorIndex && pMotorConfigItem->nMotorCtrlIndex == nMotorCtrlIndex )
           {
               pTempConfig = pMotorConfigItem;
               break;
           }
       }

       if(pTempConfig != NULL)
       {
           postionConfigInfo.sPostionName = strArray[2];
           postionConfigInfo.lfPostionNum = strArray[3].toFloat(&bOk);
           pTempConfig->tPostionConfig.append(postionConfigInfo);
       }
   }

    return true;

}

/*******************************************************************
**功能：获取能量计配置信息
**输入：
**输出：
**返回值：
** 修改记录：
** 2019-01-21 能量计配置信息移植到配置文件中 不在数据库中
*******************************************************************/
bool QDBData::getEnergyMeterConfigInfo()
{
    //bool bOk = false;

//   QStringList configlist =  _pDbProcess->QueryTable("select* from dbo.T_ENERGYMETER_CONFIG_INFO order by DevIndex");

//   foreach (QString item, configlist)
//   {
//       QStringList strArray = item.split('|');

//       if(strArray.size() < 3)
//       {
//           return false;
//       }

//       tEnergyMeterConfigInfo tconfigInfo;

//       tconfigInfo.nDevIndex = ((QString)strArray.at(0)).toInt(&bOk);
//       tconfigInfo.lfCoeff = ((QString)strArray.at(1)).toInt(&bOk);
//       tconfigInfo.sChannelName = strArray.at(2);

//       m_tAllDevConfigInfo.m_tEnergyConfigInfoList.append(tconfigInfo);
//   }

    return true;
}

/*******************************************************************
**功能：将IDS数据写入到数据库中
**输入：pbuff 图片数据 nwidth 图像宽度 nhigh 图像高度
**          nImageType 图像类型  0--打之前图像 1--打之后图像
**输出：
**返回值：
*******************************************************************/
void QDBData::onWirteIdsImageToDb(QVariant varTaskExpInfo,int nImageType,QString sPath)
{
    tExpInfo TaskExpInfo = varTaskExpInfo.value<tExpInfo>();
    if(TaskExpInfo.m_bAutoTest == false)
    {
        return;
    }

    QSqlQuery insertImgSql(_pDbProcess->m_db);
    QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sql = QString("insert into dbo.T_IDS_IMG_DATA(ExpNo,PointNo,Times,ImageType,IsDefect,EnergySection,DateTime,ImgData) values('%1',%2,%3,%4,%5,%6,?,?)").\
            arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes).arg(nImageType).arg(2).arg(TaskExpInfo.m_nEnergySection);

    //转换图像数据
    if(!QFile(sPath).exists())
    {
        return;
    }

    QImage Img;
    Img.load(sPath);
    insertImgSql.prepare(sql);
    insertImgSql.addBindValue(CurrentTime);
    insertImgSql.addBindValue(Img);
    insertImgSql.exec();
}

/*******************************************************************
**功能：存储实验基本信息
**输入：sExpNo 实验编号 ExpType实验类型 CompentType为元器件类型 sCompSize为尺寸（h,w）或r:xx
**输出：存储数据库内容
**返回值：无
*******************************************************************/
 void QDBData::onWriteExpInfoToDb(QString sExpNo, QString sCompSize, int ExpType, int CompentType , QString sSmapleName, int PointNumPerEnergy, float MinimunEnergy, float EnergyIncrecment, int EnergyIncreNum, int TimesPerPoint)
 {

    QSqlQuery insertImgSql(_pDbProcess->m_db);
    QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sql;
    if (ExpType == M_MEASURETYPE_1On1)
    {
        sql = QString("insert into dbo.T_EXP_INFO(ExpNo,ExpType,CompentName,CompentSize,CompentType,MinEnergy,EnergyIncrease,PointNumPerEnergy,DateTime) values('%1',%2,'%3','%4',%5,%6,%7,%8,?)").\
                arg(sExpNo).arg(ExpType).arg(sSmapleName).arg(sCompSize).arg(CompentType).arg(MinimunEnergy).arg(EnergyIncrecment).arg(PointNumPerEnergy);
    }
    else if (ExpType == M_MEASURETYPE_ROn1)
    {
        sql = QString("insert into dbo.T_EXP_INFO(ExpNo,ExpType,CompentName,CompentSize,CompentType,MinEnergy,PointNumPerEnergy,EnergyIncreNum,DateTime) values('%1',%2,'%3','%4',%5,%6,%7,%8,?)").\
                arg(sExpNo).arg(ExpType).arg(sSmapleName).arg(sCompSize).arg(CompentType).arg(MinimunEnergy).arg(PointNumPerEnergy).arg(EnergyIncreNum);
    }
    else if (ExpType == M_MEASURETYPE_SOn1)
    {
        sql = QString("insert into dbo.T_EXP_INFO(ExpNo,ExpType,CompentName,CompentSize,CompentType,MinEnergy,PointNumPerEnergy,TimesPerPoint,DateTime) values('%1',%2,'%3','%4',%5,%6,%7,%8,?)").\
                arg(sExpNo).arg(ExpType).arg(sSmapleName).arg(sCompSize).arg(CompentType).arg(MinimunEnergy).arg(PointNumPerEnergy).arg(TimesPerPoint);
    }


    insertImgSql.prepare(sql);
    insertImgSql.addBindValue(CurrentTime);
    insertImgSql.exec();

 }


 //添加新的设备信息
 bool QDBData::onSaveNewDevInfo2Db(DevInfo4Show DevInfo)
 {
     QSqlQuery insertImgSql(_pDbProcess->m_db);
     QString sql = QString("insert into dbo.T_DEVICE_INFO(DevName,DevType,DevNo,DevCertiNo,DevSpecification) values('%1','%2','%3','%4','%5')").\
      arg(DevInfo.DevName).arg(DevInfo.DevType).arg(DevInfo.DevNo).arg(DevInfo.DevCertiNo).arg(DevInfo.DevComment);
     insertImgSql.prepare(sql);
     insertImgSql.exec();
     return true;
 }


 bool QDBData::onSaveNewClientInfo2Db(int index, ClientInfo ClientInfos)
 {
     QSqlQuery insertImgSql(_pDbProcess->m_db);
     QString sql = QString("insert into dbo.T_CLIENT_INFO(ClientNo,ClientName,ClientAddress,ClientContact) values(%1,'%2','%3','%4')").\
      arg(QString::number(index)).arg(ClientInfos.CName).arg(ClientInfos.CAddress).arg(ClientInfos.CPhone);
     insertImgSql.prepare(sql);
     insertImgSql.exec();
     return true;
 }


 /*******************************************************************
**功能：写入示波器数据到数据库中
**输入：
**输出：
**返回值：
*******************************************************************/
 void QDBData::onWirteOsciDataToDb(QVariant varTaskExpInfo,QString sFilePath,float lfPlusWidth)
{
     tExpInfo TaskExpInfo = varTaskExpInfo.value<tExpInfo>();
     if(TaskExpInfo.m_bAutoTest == false)
     {
         return;
     }

    QSqlQuery insertImgSql(_pDbProcess->m_db);
    QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sql = QString("insert into dbo.T_OSCI_DATA(ExpNo,PointNo,Times,DateTime,Data,ChannelNo,PlusWidth) values('%1',%2,%3,?,?,?,?)").\
         arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes);

    QFile dataFile;

     for(int nChannel = 0; nChannel < 4; nChannel ++)
     {
         QString sFileName = QString("%1C%2.txt").arg(sFilePath).arg(nChannel+1);
         dataFile.setFileName(sFileName);

         bool bRel = dataFile.open(QFile::ReadOnly);
         if(bRel == true)
         {
             QByteArray  channelData = dataFile.readAll();

            insertImgSql.prepare(sql);
            insertImgSql.addBindValue(CurrentTime);
            insertImgSql.addBindValue(channelData);
            insertImgSql.addBindValue(nChannel+1);
            insertImgSql.addBindValue(lfPlusWidth);
            insertImgSql.exec();
         }

         dataFile.close();
     }

}

 /******************************
*功能:将能量计数据写入数据库中
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
 void QDBData::onWriteEnergyMeterDataToDb(int nIndex, QString sEnergyData, QVariant tTaskExpInfo)
 {
     tExpInfo TaskExpInfo = tTaskExpInfo.value<tExpInfo>();
//     QSqlQuery insertImgSql(_pDbProcess->m_db);
     QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
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
         QString sSql = QString("insert into dbo.T_ENERGY_DATA(ExpNo,EnergyDevIndex,EnergyData,EnergyFlux,EnergySection,Times,PointNo,DateTime) values('%1',%2,%3,'%4',%5,?,?,?)").\
                 arg(TaskExpInfo.m_sExpNo).arg(nIndex).arg(sEnergyData).arg(QString("0.0")).arg(TaskExpInfo.m_nEnergySection);
         QSqlQuery sqlQuery(db);
         sqlQuery.prepare(sSql);
         sqlQuery.addBindValue(TaskExpInfo.m_nTimes);
         sqlQuery.addBindValue(TaskExpInfo.m_nPointNo);
         sqlQuery.addBindValue(CurrentTime);
         if (sqlQuery.exec())
         {

         }
         else
         {
             QMessageBox msgBox;
             msgBox.setWindowTitle("能量计数据插入失败1");
             msgBox.setText(sqlQuery.lastError().text());
             msgBox.setStandardButtons(QMessageBox::Ok);
             msgBox.exec();
         }
         //wzj0323
//         sSql = QString("insert into dbo.T_PROCESS_DATA(ExpNo,pointNo,shotNo,EnergyData) values('%1',%2,%3,%4)").\
//                 arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes).arg(sEnergyData);
         sSql = QString("update dbo.T_PROCESS_DATA set EnergyData=%1 where ExpNo='%2' and pointNo=%3 and shotNo=%4").\
                          arg(sEnergyData).arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes);
         sqlQuery.prepare(sSql);
         if (sqlQuery.exec())
         {

         }
         else
         {
             QMessageBox msgBox;
             msgBox.setWindowTitle("能量计数据插入失败2");
             msgBox.setText(sqlQuery.lastError().text());
             msgBox.setStandardButtons(QMessageBox::Ok);
             msgBox.exec();
         }

//         sSql = QString("select defect from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and pointNo=%2 and shotNo=%3 and imagetype=1").\
//                 arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes);
//         sqlQuery.prepare(sSql);
//         int defect=1;
//         if (sqlQuery.exec())
//         {
//            while(sqlQuery.next())
//            {
//                defect=sqlQuery.value(0).toInt();
//            }
//         }
//         else
//         {
//             QMessageBox msgBox;
//             msgBox.setWindowTitle("能量计数据插入失败3");
//             msgBox.setText(sqlQuery.lastError().text());
//             msgBox.setStandardButtons(QMessageBox::Ok);
//             msgBox.exec();
//         }
//         sSql = QString("update T_PROCESS_DATA set defect=%1 where ExpNo='%2' and pointNo=%3 and shotNo=%4").\
//                 arg(defect).arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes);
//         sqlQuery.prepare(sSql);
//         if (sqlQuery.exec())
//         {

//         }
//         else
//         {
//             QMessageBox msgBox;
//             msgBox.setWindowTitle("能量计数据插入失败3");
//             msgBox.setText(sqlQuery.lastError().text());
//             msgBox.setStandardButtons(QMessageBox::Ok);
//             msgBox.exec();
//         }
         //wzj0324
         db.close();

     }
//     QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
//     QString sql = QString("insert into dbo.T_ENERGY_DATA(ExpNo,EnergyDevIndex,EnergyData,EnergyFlux,EnergySection,Times,PointNo,DateTime) values('%1',%2,%3,'%4',%5,?,?,?)").\
//          arg(TaskExpInfo.m_sExpNo).arg(nIndex).arg(sEnergyData).arg(QString("0.0")).arg(TaskExpInfo.m_nEnergySection);
//     QSqlQuery sqlQuery(db);
//     insertImgSql.prepare(sql);
//     insertImgSql.addBindValue(TaskExpInfo.m_nTimes);
//     insertImgSql.addBindValue(TaskExpInfo.m_nPointNo);
//     insertImgSql.addBindValue(CurrentTime);
//     insertImgSql.exec();
 }

 /*******************************************************************
 **功能：将当前发次的损伤结果写入IDS数据库
 **输入：
 **输出：
 **返回值：
 *******************************************************************/
 void  QDBData::onWriteDefectStatus(QVariant tTaskExpInfo, bool bstatus)
 {
     tExpInfo TaskExpInfo = tTaskExpInfo.value<tExpInfo>();
     if(TaskExpInfo.m_bAutoTest == false)
     {
         return;
     }

     QSqlQuery insertImgSql(_pDbProcess->m_db);
     QString sql = QString("update dbo.T_IDS_IMG_DATA set IsDefect=? where ExpNo='%1'and PointNo=%2 and Times=%3 and ImageType=%4").\
             arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes).arg(1);
     qDebug()<<"==YDC==: "<<"update ids status into ---->"<<bstatus;
     //转换图像数据
     insertImgSql.prepare(sql);
     insertImgSql.addBindValue((int)bstatus);
     insertImgSql.exec();
 }


 /*******************************************************************
 **功能：将当前发次的通量写入能量值数据库
 **输入：
 **输出：
 **返回值：
 *******************************************************************/
 void QDBData::onWriteFluxData(QVariant tTaskExpInfo, float lfFluxData)
 {
     tExpInfo TaskExpInfo = tTaskExpInfo.value<tExpInfo>();
     if(TaskExpInfo.m_bAutoTest == false)
     {
         return;
     }

     QSqlQuery insertImgSql(_pDbProcess->m_db);
     QString sql = QString("update dbo.T_ENERGY_DATA set EnergyFlux=? where ExpNo='%1' and PointNo=%2 and Times=%3 and EnergyDevIndex=%4").\
             arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes).arg(0);
     //转换图像数据
     insertImgSql.prepare(sql);
     insertImgSql.addBindValue(QString::number(lfFluxData));
     insertImgSql.exec();
     //;
 }

 /*******************************************************************
 **功能：将kxccd数据写入到数据库中
 **输入：pbuff 图片数据 nwidth 图像宽度 nhigh 图像高度
 **输出：
 **返回值：
 *******************************************************************/
 void QDBData::onWirteKxccdImageToDb(char* pbuff,int nwidth,int nhigh,QVariant varTaskExpInfo)
 {
     tExpInfo TaskExpInfo = varTaskExpInfo.value<tExpInfo>();
     if(TaskExpInfo.m_bAutoTest == false)
     {
         return;
     }
     QByteArray imgData;
     QSqlQuery insertImgSql(_pDbProcess->m_db);
     QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
     QString sql = QString("insert into dbo.T_KXCCD_IMG_DATA(ExpNo,PointNo,Times,ImgData,DateTime) values('%1',%2,%3,?,?)").\
             arg(TaskExpInfo.m_sExpNo).arg(TaskExpInfo.m_nPointNo).arg(TaskExpInfo.m_nTimes);
     //转换图像数据
     for(int i = 0; i < nwidth*nhigh;i++)
     {
         imgData.append(pbuff[i]);
     }

     insertImgSql.prepare(sql);
     insertImgSql.addBindValue(imgData);
     insertImgSql.addBindValue(CurrentTime);
     insertImgSql.exec();
 }
