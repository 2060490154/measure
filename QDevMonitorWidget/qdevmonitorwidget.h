#ifndef QDEVMONITORWIDGET_H
#define QDEVMONITORWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include "qenergydevinfowidget.h"
#include "qmotormonitorwidget.h"
#include "qImageinfowidget.h"
#include "qosciidevinfowidget.h"
#include "qdevmanagerctrl.h"
#include "qexpresultwidget.h"

class QDevMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDevMonitorWidget(QDevManagerCtrl* pDevCtrl,QWidget *parent = nullptr);

signals:

public slots:
    void getNewExpNo(QString sExpNo, QString sSampleName);
public:
    void InitUI();

public:

        QTabWidget* m_pDevMonitorTableWidget;//设备状态 Tab切换

        QEnergyDevInfoWidget* m_pEnergyDevInfoWidget;
        QMotorMonitorWidget* m_pMotorDevInfoWidget;
        QRunTimeImageWidget* m_pIDSDevInfoWidget;
        QOSciiDevInfoWidget* m_pOSciiDevInfoWidget;
        QExpResultWidget* m_pExpResultWidget;
public:
        QString m_sExpNo;


private:
        QDevManagerCtrl* _pDevManagerCtrl;

};

#endif // QDEVMONITORWIDGET_H
