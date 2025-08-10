#ifndef QRUNTASKTHREAD_H
#define QRUNTASKTHREAD_H
#include <QThread>
#include "define.h"
#include "qdevmanagerctrl.h"
class QRunTaskThread : public QThread
{
public:
    QRunTaskThread(QDevManagerCtrl* pCtrl);

    QWorkTask* m_pTaskInfo;

public:
    void run();

public:
    QDevManagerCtrl* _pDevManagerCtrl;
};

#endif // QRUNTASKTHREAD_H
