// EnergyMeter.cpp : 实现文件
//

#include "EnergyMeter.h"
#include <QDebug>
#include <QtMath>


// CEnergyMeter

CEnergyMeter::CEnergyMeter(CoLMMeasurement* pDevicCom,QString sDevicserialNumber)
{
	
	_hDevice = 0;//初始化
	_pDevicCom = pDevicCom;
	_sDevicserialNumber = sDevicserialNumber;

	m_nIndex = 0;
    m_nChannel = 0;


    //memset(&m_tEnergyMeterParam,0,sizeof(m_tEnergyMeterParam));
    m_tEnergyMeterParam.m_sDeviceName="";
    m_tEnergyMeterParam.m_sRomVersion="";
    m_tEnergyMeterParam.m_sSerialNumber="";

    //̽ͷ��Ϣ
    m_tEnergyMeterParam.m_sHeadSN="";
    m_tEnergyMeterParam.m_sHeadType="";
    m_tEnergyMeterParam.m_sHeadName="";

    m_tEnergyMeterParam.m_nCurrentWavelengthIndex=0;

    m_tEnergyMeterParam.m_nCurrentRangeIndex=0;


    m_tEnergyMeterParam.m_nCurrentModeIndex=0;

    m_tEnergyMeterParam.m_nCurrentAvgIndex=0;
    m_tEnergyMeterParam.m_sMeasureValue="";

	
}

CEnergyMeter::~CEnergyMeter()
{
}


// CEnergyMeter 成员函数

/*************************************************************************
功能：打开能量计
输入：设备串号 
输出:_hDevice  设备句柄
返回值：true打开成功，false打开失败
****************************************************************************/
bool  CEnergyMeter::OpenDevice()
{
	bool bRel = false;

    if(_pDevicCom == NULL)
    {
        return false;
    }
    if(_hDevice != 0)
    {
        return true;
    }

    _pDevicCom->OpenUSBDevice(_sDevicserialNumber, _hDevice);
	if(_hDevice !=0)
	{
		bRel = true;
	}
    else
        bRel = false;

	return bRel;
}

/*************************************************************************
功能：关闭能量计
输入：设备串号
输出:_hDevice  设备句柄
返回值：true打开成功，false打开失败
****************************************************************************/
bool  CEnergyMeter::CloseDevice()
{

    if(_pDevicCom == NULL || _hDevice == 0)
    {
        return false;
    }

    _pDevicCom->Close( _hDevice);
    _hDevice = 0;

    return true;
}
/*************************************************************************
功能：获取能量计设备信息
输入：无 
输出:
返回值：true获取成功，false获取失败
****************************************************************************/
bool  CEnergyMeter::GetDeviceInfo()
{
	bool bRel = false;
    QString deviceName, romVersion, serialNumber;

	if (_hDevice == 0)
	{
		return bRel;
	}

	_pDevicCom->GetDeviceInfo(_hDevice, deviceName, romVersion, serialNumber);//设备信息

    m_tEnergyMeterParam.m_sDeviceName = deviceName;
    m_tEnergyMeterParam.m_sRomVersion = romVersion;
    m_tEnergyMeterParam.m_sSerialNumber = serialNumber;

    if (!m_tEnergyMeterParam.m_sDeviceName.isEmpty()&&!m_tEnergyMeterParam.m_sRomVersion.isEmpty()&&!m_tEnergyMeterParam.m_sSerialNumber.isEmpty())//均不为空 则认为获取成功
	{
		bRel = true;
	}

	return bRel;

}


/*************************************************************************
功能：获取探头信息
输入：无 
输出:
返回值：true获取成功，false获取失败
****************************************************************************/
bool  CEnergyMeter::GetSensorInfo()
{
	bool bRel = false;
    QString  headSN, headType,headName;

	if (_hDevice == 0)
	{
		return bRel;
	}


    _pDevicCom->GetSensorInfo(_hDevice, m_nChannel, headSN, headType, headName);//传感器信息
    m_tEnergyMeterParam.m_sHeadSN = headSN;
    m_tEnergyMeterParam.m_sHeadName = headName;
    m_tEnergyMeterParam.m_sHeadType = headType;

    if (!m_tEnergyMeterParam.m_sHeadSN.isEmpty()&&!m_tEnergyMeterParam.m_sHeadName.isEmpty()&&!m_tEnergyMeterParam.m_sHeadType.isEmpty())//均不为空 则认为获取成功
	{
		bRel = true;
	}

	return bRel;

}

/*************************************************************************
功能：获取波长信息
输入：无 
输出: m_tWavelengthArray 波长数组
返回值：true获取成功，false获取失败
****************************************************************************/
bool  CEnergyMeter::GetMeasureWavelength()
{
	bool bRel = false;
    int nIndex = 0;
    QVariant vWavelength;
    QStringList tWavelength;

  //  tWavelength = QVariant(QStringList);
	if (_hDevice == 0)
	{
		return bRel;
	}

    _pDevicCom->GetWavelengths(_hDevice,m_nChannel,nIndex,vWavelength);

    tWavelength = vWavelength.value<QStringList>();


	int nlen = tWavelength.size();
	for (int i = 0; i < nlen;i++)
	{
        m_tEnergyMeterParam.m_tWavelengthArray.push_back(tWavelength[i]);
	}

	m_tEnergyMeterParam.m_nCurrentWavelengthIndex = nIndex;

	if (nlen > 0)
	{
		bRel = true;
	}

	return bRel;

}

/*************************************************************************
功能：获取测量范围
输入：无 
输出: m_tRangeArray 测量范围数组
返回值：true获取成功，false获取失败
****************************************************************************/
bool  CEnergyMeter::GetMeasureRange()
{
	bool bRel = false;
    int index = 0;
    QVariant vRange;
	if (_hDevice == 0)
	{
		return bRel;
	}

    _pDevicCom->GetRanges(_hDevice, m_nChannel,index,vRange);//获取测量范围 index为当前设置的参数

    QStringList tRange = vRange.value<QStringList>();

	int nlen =  tRange.size();

	for(int i = 0;i <nlen; i++)
	{
        m_tEnergyMeterParam.m_tRangeArray.push_back(tRange[i]);
	}

	m_tEnergyMeterParam.m_nCurrentRangeIndex = index;

	if (nlen> 0 )
	{
		bRel = true;
	}
		

	return bRel;

}

/******************************
*功能:获取当前能量计范围(单位mJ)
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
int CEnergyMeter::GetCurrentRange()
{
    switch (m_tEnergyMeterParam.m_nCurrentRangeIndex)
    {
    case 0:
    {return 10000;}break;
    case 1:
    {return 2000;}break;
    case 2:
    {return 200;}break;
    case 3:
    {return 20;}break;
    case 4:
    {return 2;}break;
    }
}
/*************************************************************************
功能：获取测量模式
输入：无 
输出: m_tMeasureModeArray 测量模式数组
返回值：true获取成功，false获取失败
****************************************************************************/
bool  CEnergyMeter::GetMeasureMode()
{
	bool bRel = false;
    int index = 0;
    QVariant vMode;
	if (_hDevice == 0)
	{
		return bRel;
	}

    _pDevicCom->GetMeasurementMode(_hDevice, m_nChannel,index,vMode);//获取测量范围

    QStringList tMode = vMode.value<QStringList>();
	int nlen =  tMode.size();

	for(int i = 0;i <nlen; i++)
	{
        m_tEnergyMeterParam.m_tMeasureModeArray.push_back(tMode[i]);
	}
	m_tEnergyMeterParam.m_nCurrentModeIndex = index;

	if (nlen> 0 )
	{
		bRel = true;
	}

	return bRel;
}


/*************************************************************************
功能：获取平均值
输入：无 
输出: m_tMeasureAverageArray 平均值数组
返回值：true获取成功，false获取失败
****************************************************************************/
bool  CEnergyMeter::GetMeasureAverage()
{
	bool bRel = false;
    bool bOk;
    QString sCmd,sRelValue;
    QString sReadValue,sTemp;
	if (_hDevice == 0)
	{
		return bRel;
	}

    sCmd ="AQ";
	_pDevicCom->Write(_hDevice,sCmd);
	_pDevicCom->Read(_hDevice,sRelValue);

    QStringList tValue = sRelValue.split(' ');

    for(int i = 0; i < tValue.size();i++)
    {
        if(tValue[i]!="*"&&tValue[i]!=""&&tValue[i]!="\n"){
            if(i == 1)
            {
                m_tEnergyMeterParam.m_nCurrentAvgIndex = tValue[i].toInt(&bOk);//0---currentindex
            }
            else
                m_tEnergyMeterParam.m_tMeasureAverageArray.push_back(tValue[i]);
        }
    }


    if (m_tEnergyMeterParam.m_tMeasureAverageArray.size() > 0)
	{
		bRel = true;
	}


	return bRel;

}


/*************************************************************************
功能：获取测量数据
输入：无 
输出: m_sMeasureValue 测量值 输出的值单位为J
返回值：true获取成功，false获取失败
****************************************************************************/
bool  CEnergyMeter::GetMeasureData()
{
    //bool bOk;
	bool bRel = false;
    QString sCmd,sRelValue;
    QString sReadValue,sTemp;
	if (_hDevice == 0)
	{
		return bRel;
	}

    sCmd ="SE";
   _pDevicCom->Write(_hDevice,"EF");
   _pDevicCom->Read(_hDevice,sRelValue);

   _pDevicCom->Write(_hDevice,"SE");




    _pDevicCom->Read(_hDevice,sRelValue);

//    sRelValue.toLatin1().mid(1,6).toDouble();
//    qDebug()<<sRelValue.toLatin1().mid(1,6);

   // m_tEnergyMeterParam.m_sMeasureValue = sRelValue;//测量数据


//    double fdata = sRelValue.toLatin1().mid(1,6);//输出以J为单位的数据
//    qDebug()<<sRelValue.toLatin1().mid(2,5);
//    qDebug()<<fdata;

    //单位：mJ
//    qDebug()<<sRelValue.toLatin1().mid(2,5)<<sRelValue.toLatin1().mid(8,2)<<sRelValue.toLatin1().mid(2,5).toFloat() * qPow(10,sRelValue.toLatin1().mid(8,2).toFloat()) *1000;
    m_tEnergyMeterParam.m_sMeasureValue = QString("%1").arg(sRelValue.toLatin1().mid(2,5).toFloat() * qPow(10,sRelValue.toLatin1().mid(8,2).toFloat()) *1000);

    if (!m_tEnergyMeterParam.m_sMeasureValue.isEmpty())
	{
		bRel = true;
	}


	return bRel;

}

/******************************
*功能:获取能量计Flag
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool CEnergyMeter::GetEnergyFlag()
{
    bool bRel = false;
    if (_hDevice == 0)
    {
        return bRel;
    }
   _pDevicCom->Write(_hDevice,"EF");
   _pDevicCom->Read(_hDevice,m_sEnergyFlag);
   if (m_sEnergyFlag.mid(1,1).toInt() == 0)
   {
       return false;
   }
   else if (m_sEnergyFlag.mid(1,1).toInt() == 1)
   {
       return true;
   }
}
/*************************************************************************
功能：设置测量模式
输入：无 
输出:
返回值：true获取成功，false获取失败
****************************************************************************/

bool CEnergyMeter:: SetMeasureMode(int nIndex)
{
	if (_hDevice == 0 )
	{
		return false;
	}
	_pDevicCom->SetMeasurementMode(_hDevice,m_nChannel,nIndex);

	m_tEnergyMeterParam.m_nCurrentModeIndex = nIndex;

	return true;
}

/*************************************************************************
功能：设置测量波长
输入：无 
输出:
返回值：true获取成功，false获取失败
****************************************************************************/

bool  CEnergyMeter::SetMeasureWavelength(int nIndex)
{
	if (_hDevice == 0 )
	{
		return false;
	}
	_pDevicCom->SetWavelength(_hDevice,m_nChannel,nIndex);

	m_tEnergyMeterParam.m_nCurrentWavelengthIndex = nIndex;

	return true;
}

/*************************************************************************
功能：设置测量范围
输入：无 
输出:
返回值：true获取成功，false获取失败
****************************************************************************/

bool CEnergyMeter::SetMeasureRange(int nIndex)
{
	if (_hDevice == 0 )
	{
		return false;
	}
	_pDevicCom->SetRange(_hDevice,m_nChannel,nIndex);

	m_tEnergyMeterParam.m_nCurrentRangeIndex = nIndex;

	return true;
}



bool CEnergyMeter::SetMeasureAverage(int nIndex)
{
	std::wstring svalue;
    QString sTemp;
	if (_hDevice == 0 )
	{
		return false;
	}

    sTemp =QString("AQ%1").arg(nIndex+1);
    _pDevicCom->Write(_hDevice,sTemp);
	return true;
}

bool CEnergyMeter::GetDeviceParam()
{
    bool bRel = false;

    bRel = OpenDevice();
    if (bRel == false)
    {
        return bRel;
    }

    bRel = GetDeviceInfo();
    if (bRel == false)
    {
        return bRel;
    }

    bRel = GetSensorInfo();
    if (bRel == false)
    {
        return bRel;
    }

    bRel = GetMeasureWavelength();
    if (bRel == false)
    {
        return bRel;
    }

    bRel = GetMeasureAverage();
    if (bRel == false)
    {
        return bRel;
    }

    bRel = GetMeasureRange();
    if (bRel == false)
    {
        return bRel;
    }

    bRel = GetMeasureMode();
    if (bRel == false)
    {
        return bRel;
    }

    return bRel;
}


