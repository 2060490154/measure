#ifndef QSYSTEMPARAMWIDGET_H
#define QSYSTEMPARAMWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include "define.h"
#include "QConfigProcess.h"

class QSystemParamWidget:	public QWidget
{
    Q_OBJECT
public:
    explicit QSystemParamWidget(QConfigProcess* pCtrl);


public:
    void InitUI();

    QGroupBox* createCoeffAdjGroupBox(QString sName, QTextEdit* pKEdit,QTextEdit* pBEdit,QPushButton* pUpdateButton);
    QGroupBox* createDevCoeffGroupBox(QString sName, QTextEdit* pCeoffEdit,QPushButton* pUpdateButton);
    QGroupBox* createDetectMethodGroupBox(QString sName, QRadioButton* pRBtn1,QRadioButton* pRBtn2);


public slots:
    void OnUpdateBaseWaveCoeffData();
    void OnUpdateDoubleWaveCoeffData();
    void OnUpdateTribWaveCoeffData();
    void OnUpdateEnergyDevCoeffData();

public:
    QTextEdit* m_pBaseWave_K_TextEdit;//基频K
    QTextEdit* m_pDoubleWave_K_TextEdit;//二倍频
    QTextEdit* m_pTribWave_K_TextEdit;//三倍频

    QTextEdit* m_pBaseWave_B_TextEdit;//基频B
    QTextEdit* m_pDoubleWave_B_TextEdit;//二倍频
    QTextEdit* m_pTribWave_B_TextEdit;//三倍频

    QPushButton* m_pBaseWaveUpdatePushButton;
    QPushButton* m_pDoubleWaveUpdatePushButton;
    QPushButton* m_pTribWaveUpdatePushButton;

    //能量计设备系数
    QTextEdit* m_pEnergyDev_TextEdit[M_ENERGYMETER_DEV_NUM];//能量计1
    QPushButton* m_pEnergyDevUpdatePushButton[M_ENERGYMETER_DEV_NUM];

    //系数
    QConfigProcess* m_pConfigProcess;

    //损伤检测方法选择
    QRadioButton *m_pDetectMingRBtn;
    QRadioButton *m_pDetectAnRBtn;

};

#endif // QSYSTEMPARAMWIDGET_H
