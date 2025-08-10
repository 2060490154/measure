#ifndef QENERGYMETERWIDGET_H
#define QENERGYMETERWIDGET_H

#include <QWidget>
#include <qcombobox.h>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <qlabel.h>
#include "EnergyMeter.h"
#include "EnergyMeterDeviceManager.h"

class QEnergyMeterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QEnergyMeterWidget(QEnergyMeterDeviceManager* pManagerCtrl,QList<tEnergyMeterConfigInfo*>* pConfigList,QWidget *parent = 0);
    void InitEnergyMeterWidget();//初始化能量计系统

signals:

public slots:
    void onrefreshEnergyDev(QString str);//刷新能量计设备

public:
    QEnergyMeterDeviceManager* m_pEnergyMeterDeviceManagerCtrl;
    QList<tEnergyMeterConfigInfo*>* m_pEnergyMeterConfigList;

public:
     QLabel* m_plabel;

};

class QEnergyMeterItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QEnergyMeterItemWidget(int nDevIndex,tEnergyMeterConfigInfo* pConfig,QWidget *parent = 0);

signals:

public slots:
    void onGetValue();
    void onSetMeasureMode();//设置测量模式
    void onsetMeasureRange();//设置测量范围
    void onsetMeasureWaveLength();
    void onsetMeasureAvg();

public:
    void InitUI();
    QGroupBox* InitSetGroup();
    void SetlogInfo(QString slog);
    void showParamInfo();

public:
    QComboBox* m_pMeasureMode;//测试模式
    QComboBox* m_pMeasureWaveLength;//测量波长
    QComboBox* m_pMeasureRange;//测量范围
    QComboBox* m_pMeasureAvgComBox;//平均

    QTextEdit* m_pMeasureValueEdit;//测试值
    QTextEdit* m_pAdJustValueEdit;//校准值
    //QTextEdit* m_pCoeffValuEdit;//校准系数

    QPushButton* m_pGetValueBtn;//查询数据
    QPushButton* m_pCoeffBtn;//校准系数
    QListWidget* m_pLogList;



    //数据
    int m_nDevIndex;
    CEnergyMeter* m_pEnergyMeterDev;

    tEnergyMeterConfigInfo* m_pItemConfig;

    float m_lfCoeff;//校准系数
};

#endif // QENERGYMETERWIDGET_H
