#ifndef QMOTORDEVMANAGER_H
#define QMOTORDEVMANAGER_H

#include <QObject>
#include "qmotorctrl.h"
#include "define.h"



class QMotorDevManager
{
public:
    QMotorDevManager();
    ~QMotorDevManager();

public:
    bool  getAllMotorDevStatus();//是否正常打开


public:
    QMotorCtrl* m_pMotorCtrl[M_MOTOR_CTRLDEV_NUM];
};

#endif // QMOTORDEVMANAGER_H
