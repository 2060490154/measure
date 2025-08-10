#ifndef QMOTORRUNTHREAD_H
#define QMOTORRUNTHREAD_H

#include <QObject>
#include "qmotorctrl.h"

class QMotorRunThread : public QThread
{

public:
    QMotorRunThread(QMotorCtrl* pCtrl);

    int m_nMotorIndex;//0-11
    int m_nPosition;//位置信息

public:
    void run();

public:
    QMotorCtrl* _pMotorCtrl;
};

#endif // QMOTORRUNTHREAD_H
