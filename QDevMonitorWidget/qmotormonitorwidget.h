#ifndef QMotorMonitorWidget_H
#define QMotorMonitorWidget_H

#include <QWidget>
#include <qlabel.h>
#include <qlist.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "define.h"

class QMotorMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMotorMonitorWidget(QList<tMotorConfigInfo>* pMotorConfigList,QWidget *parent = 0);

signals:

public slots:

    void onShowMotorPos(int nMotorCtrlIndex,int nMotorIndex,QVariant Status);//显示当前位置

public:
    void InitUI();


private:
    QLabel* m_pTipLabel;
    QList<QLabel*>m_tTextEditArray;
    QList<tMotorConfigInfo>* _pMotorConfigList;

};

#endif // QMotorMonitorWidget_H
