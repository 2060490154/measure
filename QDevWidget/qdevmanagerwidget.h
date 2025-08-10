#ifndef QDEVMANAGERWIDGET_H
#define QDEVMANAGERWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "qenergymeterwidget.h"
#include "qidswidget.h"
#include "qmotorctrlwidget.h"
#include "qoscictrlwidget.h"
#include "qkxccdwidget/qkxccdctrlwidget.h"
#include "qdevmanagerctrl.h"
#include "qshutterctrlwidget.h"
#include "qdbdata.h"
#include "qruntaskthread.h"


class QDevManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDevManagerWidget(QDevManagerCtrl* pCtrl,QWidget *parent = 0);

public:
    void InitUI();
signals:

public slots:

public:

    //设备界面
    QEnergyMeterWidget* m_pEnergMeterWidget;
    QOsciCtrlWidget* m_pOsciiWidget;
    QKxccdctrlWidget* m_pKxccdCtrlWidget;//科学CCD
    QIDSWidget* m_pMicroWidget;//显微探视
    QMotorCtrlWidget* m_pMotorCtrlWidget;
    QShutterCtrlWidget* m_pShutterCtrlWidget;


    //执行线程
    QRunTaskThread* m_pRunTaskThread;

private:
    //设备控制类
    QDevManagerCtrl* _pDevManagerCtrl;
};

#endif // QDEVMANAGERWIDGET_H
