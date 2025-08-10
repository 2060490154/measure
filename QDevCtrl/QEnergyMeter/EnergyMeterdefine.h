#ifndef _ENERGY_METER_DEFINE_
#define _ENERGY_METER_DEFINE_

#include <qstring.h>
#include <qlist.h>

typedef struct
{
	//设备信息
    QString m_sDeviceName;
    QString m_sRomVersion;
    QString m_sSerialNumber;

	//探头信息
    QString m_sHeadSN;
    QString m_sHeadType;//探头类型
    QString m_sHeadName;//探头名称


	//波长信息
    QStringList m_tWavelengthArray;
	int m_nCurrentWavelengthIndex;

	//测量范围
    QStringList m_tRangeArray;
	int m_nCurrentRangeIndex;

	//测量模式
    QStringList m_tMeasureModeArray;
	int m_nCurrentModeIndex;

	//平均值
    QStringList m_tMeasureAverageArray;
    int m_nCurrentAvgIndex;

	//测量值
    QString m_sMeasureValue;


}tEnergyMeterParam;




#endif
