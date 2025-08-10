#pragma once

// 能量计基本操作
#include "ophirlmmeasurement.h"
#include "EnergyMeterdefine.h"
using namespace OphirLMMeasurementLib;
class CEnergyMeter
{
public:
    CEnergyMeter(CoLMMeasurement* pDevicCom,QString sDevicserialNumber);
	virtual ~CEnergyMeter();

public:
	//设备索引
	int m_nIndex;
	//设备信息
	tEnergyMeterParam m_tEnergyMeterParam;

	//通道号
    int m_nChannel;
public:
    bool OpenDevice();//打开设备
    bool CloseDevice();//关闭
	bool GetDeviceInfo();//获取设备信息
	bool GetSensorInfo();//获取探头信息
	bool GetMeasureWavelength();//读取波长信息
	bool GetMeasureRange();//读取测量范围
	bool GetMeasureMode();//读取测量模式
	bool GetMeasureAverage();//获取当前平均值
	bool GetMeasureData();//获取测量值
    bool GetDeviceParam();//获取当前设备参数信息
    bool GetEnergyFlag();   //获取能量计Flag
    int GetCurrentRange();

	bool SetMeasureMode(int nIndex);
	bool SetMeasureWavelength(int nIndex);
	bool SetMeasureRange(int nIndex);
	bool SetMeasureAverage(int nIndex);

private:
    int _hDevice;//设备句柄
    CoLMMeasurement* _pDevicCom;//设备操作控件
    QString _sDevicserialNumber;//设备串号
    QString m_sEnergyFlag;

};

//typedef CTypedPtrArray <CObArray,CEnergyMeter*>tEnergyMeterArray;


