#include "Energymeterdevicemanager.h"
#include <QDebug>
QEnergyMeterDeviceManager::QEnergyMeterDeviceManager(QObject *parent) : QObject(parent)
{
    m_pDevicCom = new CoLMMeasurement();
    // 实例化COM组件对象
       bool rst = m_pDevicCom->setControl("{b180613c-e514-4739-aadc-cad4493910d7}");
       if (!rst)
       {
           // COM组件还未在系统注册，注册dll，需要当前exe程序以管理员权限运行
               m_pDevicCom = NULL;
       }
}

QEnergyMeterDeviceManager::~QEnergyMeterDeviceManager()
{
    for (int i = m_tEnergyMerterArray.size()-1;i >0;i--)
    {
        m_pDevicCom->Close(i);
        delete m_tEnergyMerterArray[i];
    }
    m_tEnergyMerterArray.clear();

    m_pDevicCom->CloseAll();


}

/*************************************************************************
功能：获取所有的设备信息
输入：无
输出:_m_tEnergyMerterArray 设备数组
返回值：true打开成功，false打开失败
****************************************************************************/
bool QEnergyMeterDeviceManager::ScanAllDevice()
{
#if 1
    bool bRel = false;
    QVariant vserialNumbers;
    if(m_pDevicCom == NULL)
    {
        return false;
    }

    m_pDevicCom->ScanUSB(vserialNumbers);

    QStringList serialNumbers = vserialNumbers.value<QStringList>();

    m_nDevicNum = serialNumbers.size();
    if(m_nDevicNum >0)
    {
        bRel = true;
    }
    CEnergyMeter *tem_storePtr[2];
    for(int i = 0;i < m_nDevicNum;i++)
    {
        CEnergyMeter* pEnergyMeter = new CEnergyMeter(m_pDevicCom,serialNumbers[i]);

        //将监视能量计设置为0号，主能量计设置为1号
        for (int j=0; j<m_tEnergyConfigInfoList->size();j++)
        {
            if (serialNumbers[i] == m_tEnergyConfigInfoList->at(j)->sSerialNUmber)
            {
                pEnergyMeter->m_nIndex = m_tEnergyConfigInfoList->at(j)->nDevIndex;
                tem_storePtr[pEnergyMeter->m_nIndex] = pEnergyMeter;
            }
        }
    }
    //200108yi
    for (int i=0; i<m_nDevicNum; i++)
    {
        m_tEnergyMerterArray.push_back(tem_storePtr[i]);
    }
#else
    bool bRel = false;
    QVariant vserialNumbers;
    if(m_pDevicCom == NULL)
    {
        return false;
    }

    m_pDevicCom->ScanUSB(vserialNumbers);

    QStringList serialNumbers = vserialNumbers.value<QStringList>();

    m_nDevicNum = serialNumbers.size();
    if(m_nDevicNum >0)
    {
        bRel = true;
    }
    for(int i = 0;i < m_nDevicNum;i++)
    {
        CEnergyMeter* pEnergyMeter = new CEnergyMeter(m_pDevicCom,serialNumbers[i]);

        //将监视能量计设置为0号，主能量计设置为1号
        for (int j=0; j<m_tEnergyConfigInfoList->size();j++)
        {
            if (serialNumbers[i] == m_tEnergyConfigInfoList->at(j)->sSerialNUmber)
            {
                pEnergyMeter->m_nIndex = m_tEnergyConfigInfoList->at(j)->nDevIndex;
            }
        }
        m_tEnergyMerterArray.push_back(pEnergyMeter);
    }
    for(int i = 0;i < m_nDevicNum;i++)
    {
        m_tEnergyMerterArray.move(i,m_tEnergyMerterArray.at(i)->m_nIndex);
    }
#endif
    return bRel;
}




CEnergyMeter* QEnergyMeterDeviceManager::GetSelectEneryMeterDev(int nIndex)
{
    CEnergyMeter* pEnergyMeter = NULL;
    int nSize = m_tEnergyMerterArray.size();
    for (int i = 0; i < nSize; i++)
    {
        if (m_tEnergyMerterArray[i]->m_nIndex == nIndex)
        {
            pEnergyMeter = m_tEnergyMerterArray[i];
            break;
        }
    }

    return pEnergyMeter;
}






