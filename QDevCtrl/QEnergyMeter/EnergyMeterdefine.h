#ifndef _ENERGY_METER_DEFINE_
#define _ENERGY_METER_DEFINE_

#include <qstring.h>
#include <qlist.h>

typedef struct
{
	//�豸��Ϣ
    QString m_sDeviceName;
    QString m_sRomVersion;
    QString m_sSerialNumber;

	//̽ͷ��Ϣ
    QString m_sHeadSN;
    QString m_sHeadType;//̽ͷ����
    QString m_sHeadName;//̽ͷ����


	//������Ϣ
    QStringList m_tWavelengthArray;
	int m_nCurrentWavelengthIndex;

	//������Χ
    QStringList m_tRangeArray;
	int m_nCurrentRangeIndex;

	//����ģʽ
    QStringList m_tMeasureModeArray;
	int m_nCurrentModeIndex;

	//ƽ��ֵ
    QStringList m_tMeasureAverageArray;
    int m_nCurrentAvgIndex;

	//����ֵ
    QString m_sMeasureValue;


}tEnergyMeterParam;




#endif
