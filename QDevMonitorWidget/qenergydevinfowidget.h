#ifndef QENERGYDEVINFOWIDGET_H
#define QENERGYDEVINFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <qtextedit.h>
#include <QHBoxLayout>
#include<QVBoxLayout>
#include "define.h"

class QEnergyDevInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QEnergyDevInfoWidget(QList<tEnergyMeterConfigInfo*>* pConfigList,QWidget *parent = 0);

signals:

public slots:
    void onShowEnergyData(int nIndex, QString sValue, QVariant tTaskExpInfo);

public:
    void CreateUI();

public:
    int m_nDevicNum;


private:
    QLabel* m_pTipLabel;
    QList<QLabel*>m_tTextEditArray;
    QList<tEnergyMeterConfigInfo*>*m_pEnergyMeterConfig;
};

#endif // QENERGYDEVINFOWIDGET_H
