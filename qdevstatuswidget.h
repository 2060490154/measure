#ifndef QDEVSTATUSWIDGET_H
#define QDEVSTATUSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include "qdevmanagerctrl.h"

class QDevStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDevStatusWidget(QDevManagerCtrl* pCtrl,QWidget *parent = 0);

 public:
    void showDevStatus();


signals:

public slots:
    void onShowWorkStatus(int nworkStatus,bool bwork);
    void onUpdateDevStatus();
public:
    QLabel* m_pDevStatusLabel;
    QLabel* m_pStatusLabel;

     struct _devStatus* m_pdevStatus;
     QDevManagerCtrl* _pCtrl;

     QTimer* m_pTimer;
};

#endif // QDEVSTATUSWIDGET_H
