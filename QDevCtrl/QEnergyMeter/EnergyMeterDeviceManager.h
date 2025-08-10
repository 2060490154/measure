#ifndef ENERGYMETERDEVICEMANAGER_H
#define ENERGYMETERDEVICEMANAGER_H

#include <QObject>
#include "EnergyMeter.h"
#include "define.h"
class QEnergyMeterDeviceManager : public QObject
{
    Q_OBJECT
public:
    explicit QEnergyMeterDeviceManager(QObject *parent = nullptr);

     ~QEnergyMeterDeviceManager();

signals:


public:
    QList<CEnergyMeter*> m_tEnergyMerterArray;

    CoLMMeasurement*m_pDevicCom;//设备操作控件

    int m_nDevicNum;//设备总数
    QList<tEnergyMeterConfigInfo*> *m_tEnergyConfigInfoList;

public:
    bool ScanAllDevice();//获取当前PC中所有的能量计设备
    bool GetDeviceParam(int nIndex);

    CEnergyMeter* GetSelectEneryMeterDev(int nIndex);

};

#endif // ENERGYMETERDEVICEMANAGER_H
