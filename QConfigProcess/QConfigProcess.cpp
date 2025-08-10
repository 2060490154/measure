#include "QConfigProcess.h"
#include <QtXml>
#include <QDomElement>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>

QConfigProcess::QConfigProcess(QObject *parent) : QObject(parent)
{
     m_sPlatformConfigPath = QApplication::applicationDirPath()+"/config/PlatformConfig.xml";
     m_sDevConfigPath = QApplication::applicationDirPath()+"/config/DevConfig.xml";

}

//获取设备配置信息
bool QConfigProcess::getDevConfig()
{

    bool bOk = false;

    QDomDocument configDoc;
    QFile file(m_sDevConfigPath);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    if(!configDoc.setContent(&file))
    {
        file.close();
        return false;
    }


    QDomElement root = configDoc.documentElement();

    QDomNodeList nodelist = root.childNodes();
    for(int i =0;i < nodelist.count();i++)
    {
        QDomNode subitem = nodelist.at(i);

        if(subitem.nodeName() == "KxCCD")
        {
            m_tDevConfigParam.m_sKxccdIPAddress =  subitem.attributes().namedItem("DevIP").nodeValue();
            m_tDevConfigParam.m_nKxccdPort = subitem.attributes().namedItem("Port").nodeValue().toInt(&bOk);
        }

        if(subitem.nodeName() == "OSCII")
        {
            m_tDevConfigParam.m_sOSCIIIPAddress =  subitem.attributes().namedItem("DevIP").nodeValue();
            if(subitem.hasChildNodes())
            {
                QDomNodeList osciiChannelNodelist = subitem.childNodes();
                for(int k =0;k < osciiChannelNodelist.count();k++)
                {
                    QDomNode channelitem = osciiChannelNodelist.at(k);
                    tOsciiChannel channelparam;
                    channelparam.m_nChannelNum = channelitem.attributes().namedItem("No").nodeValue().toInt(&bOk);
                    channelparam.m_sChannelName = channelitem.attributes().namedItem("Name").nodeValue();
                    m_tDevConfigParam.m_tOSCIIVaildChannelList.append(channelparam);
                }

            }
        }

        if(subitem.nodeName() == "SHUTTER")
        {
            m_tDevConfigParam.m_nShutterPort =  subitem.attributes().namedItem("Port").nodeValue().toInt(&bOk);
            int nDevNum = subitem.attributes().namedItem("DEVNUM").nodeValue().toInt(&bOk);
            for(int nshutterIndex = 0; nshutterIndex < nDevNum;nshutterIndex++)
            {
                QString sDevName;
                sDevName = QString("DevIP%1").arg(nshutterIndex+1);
                m_tDevConfigParam.m_tShutterDevIPList.append(subitem.attributes().namedItem(sDevName).nodeValue());
            }

        }

        if(subitem.nodeName() == "ENERGYMETER")
        {
            tEnergyMeterConfigInfo*l_pEnergyMeterConfig = new tEnergyMeterConfigInfo();
            l_pEnergyMeterConfig->nDevIndex =  subitem.attributes().namedItem("DevIndex").nodeValue().toInt(&bOk);
            l_pEnergyMeterConfig->sChannelName = subitem.attributes().namedItem("Name").nodeValue();
            l_pEnergyMeterConfig->lfCoeff = subitem.attributes().namedItem("Coeff").nodeValue().toFloat(&bOk);
            l_pEnergyMeterConfig->sSerialNUmber = subitem.attributes().namedItem("SerialNum").nodeValue();
            m_tDevConfigParam.m_tEnergyConfigInfoList.append(l_pEnergyMeterConfig);

        }

    }
    file.close();

    return true;

}

//获取平台配置信息
bool QConfigProcess::getPlatformConfig()
{
    bool bOk = false;

    QDomDocument configDoc;
    QFile file(m_sPlatformConfigPath);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    if(!configDoc.setContent(&file))
    {
        file.close();
        return false;
    }


    QDomElement root = configDoc.documentElement();

    QDomNodeList nodelist = root.childNodes();
    for(int i =0;i < nodelist.count();i++)
    {
        QDomNode subitem = nodelist.at(i);

        if(subitem.nodeName() == "Status")
        {
            m_tPlatformConfig.m_nCurrentWaveLength =  subitem.attributes().namedItem("WaveLength").nodeValue().toInt(&bOk);
        }

        if(subitem.nodeName() == "EnergyAdjCoeff")
        {
            int nIndex =  subitem.attributes().namedItem("WaveLength").nodeValue().toInt(&bOk);

            m_tPlatformConfig.m_st_Coeff[nIndex].m_lfK = subitem.attributes().namedItem("Coeff_k").nodeValue().toFloat(&bOk);
            m_tPlatformConfig.m_st_Coeff[nIndex].m_lfB = subitem.attributes().namedItem("Coeff_b").nodeValue().toFloat(&bOk);
        }

    }

    file.close();
    return true;

}

//更新当前的波长信息
bool QConfigProcess::updateWaveLengthStatus()
{
    QDomDocument configDoc;
    QFile file(m_sPlatformConfigPath);
    if(!file.open(QIODevice::ReadWrite))
    {
        return false;
    }

    if(!configDoc.setContent(&file))
    {
        file.close();
        return false;
    }


    QDomElement root = configDoc.documentElement();

    QDomNodeList nodelist = root.childNodes();
    for(int i =0;i < nodelist.count();i++)
    {
        QDomNode subitem = nodelist.at(i);

        if(subitem.nodeName() == "Status")
        {
            subitem.attributes().namedItem("WaveLength").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_nCurrentWaveLength));
        }

    }


//保存数据
    file.close();

    QFile savefile(m_sPlatformConfigPath);
    if(!savefile.open(QFile::ReadWrite | QFile::Truncate))
    {
        return false;
    }
    QTextStream stream(&savefile);

    stream.reset();
    stream.setCodec("utf-8");
    configDoc.save(stream,4,QDomNode::EncodingFromTextStream);
    file.close();

    return true;

}

//更新能量调节拟合系数
bool QConfigProcess::updateEnergyAdjCoeff()
{
    bool bOk = false;

    QDomDocument configDoc;
    QFile file(m_sPlatformConfigPath);
    if(!file.open(QIODevice::ReadWrite))
    {
        return false;
    }

    if(!configDoc.setContent(&file))
    {
        file.close();
        return false;
    }


    QDomElement root = configDoc.documentElement();

    QDomNodeList nodelist = root.childNodes();
    for(int i =0;i < nodelist.count();i++)
    {
        QDomNode subitem = nodelist.at(i);

        if(subitem.nodeName() == "EnergyAdjCoeff")
        {
            int nIndex =  subitem.attributes().namedItem("WaveLength").nodeValue().toInt(&bOk);

            subitem.attributes().namedItem("Coeff_b").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_st_Coeff[nIndex].m_lfB));
            subitem.attributes().namedItem("Coeff_k").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_st_Coeff[nIndex].m_lfK));
        }

    }


//保存数据
    file.close();

    QFile savefile(m_sPlatformConfigPath);
    if(!savefile.open(QFile::ReadWrite | QFile::Truncate))
    {
        return false;
    }
    QTextStream stream(&savefile);

    stream.reset();
    stream.setCodec("utf-8");
    configDoc.save(stream,4,QDomNode::EncodingFromTextStream);
    file.close();

    return true;

}

//更新能量计设备的系数
bool QConfigProcess::updateEnergyDevCoeff()
{
    bool bOk = false;

    QDomDocument configDoc;
    QFile file(m_sDevConfigPath);
    if(!file.open(QIODevice::ReadWrite))
    {
        return false;
    }

    if(!configDoc.setContent(&file))
    {
        file.close();
        return false;
    }


    QDomElement root = configDoc.documentElement();

    QDomNodeList nodelist = root.childNodes();
    for(int i =0;i < nodelist.count();i++)
    {
        QDomNode subitem = nodelist.at(i);

        if(subitem.nodeName() == "ENERGYMETER")
        {
            int nIndex =  subitem.attributes().namedItem("DevIndex").nodeValue().toInt(&bOk);
            subitem.attributes().namedItem("Coeff").setNodeValue(QString("%1").arg(m_tDevConfigParam.m_tEnergyConfigInfoList.at(nIndex)->lfCoeff));
        }

    }


//保存数据
    file.close();

    QFile savefile(m_sDevConfigPath);
    if(!savefile.open(QFile::ReadWrite | QFile::Truncate))
    {
        return false;
    }
    QTextStream stream(&savefile);

    stream.reset();
    stream.setCodec("utf-8");
    configDoc.save(stream,4,QDomNode::EncodingFromTextStream);
    file.close();

    return true;

}
