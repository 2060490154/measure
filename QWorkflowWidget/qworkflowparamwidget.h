#ifndef QWORKFLOWPARAMWIDGET_H
#define QWORKFLOWPARAMWIDGET_H

#include <QWidget>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qimagereader.h>
#include <qpushbutton.h>
#include <QGroupBox>
#include <QTextEdit>
#include <QTextLine>
#include <QComboBox>
#include <QMessageBox>
#include "define.h"

class QWorkFlowParamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QWorkFlowParamWidget(int nMeasureType,int nCurrentWaveLength,QWidget *parent = 0);

signals:
//    void updatePointUI(int nSizeType,int nStep,int nRealWidth,int nRealHigh,int nRadius);
    //2020yi
    void updatePointUI(int nSizeType,double nStep,double dRealWidth,double dRealHigh,double dRadius);
    void updateWorkFlow();
    void updateExpParas(QString sExpNo,QString sSampleName);
    void updateDetectMethod();
public slots:
    void onUpdateSetParam();//保存设置参数
    void onChangeSizeType(int nIndex);
    void onNewExpNo();
    void onSetDefaultPointCnt();


public:
    void InitUI();

    void CreateExpNoString();
    double getCurrentDeg(int nWaveLength);

public:
    struct workflowParam m_tworkflowParam;
    QLineEdit *m_pManualArea;
private:
    QComboBox* m_pDevSizeTypeCombox;//

    QComboBox* m_pWaveLengthCombox;//波长选择
    QLabel* m_pWidthLabel;//长/半径显示
    QLabel* m_pHightLabel;//长/半径显示
    QLabel* m_pThickLabel;
    QLabel* m_pLabel;
    QLabel* m_pLabel2;
    QTextEdit* m_pMeasureNoEdit;//实验编号
    QTextEdit* m_pMeasureStepEdit;//实验步长
    QTextEdit* m_pSampleName;   //元件名称
    QTextEdit* m_pRectWidthEdit;//矩形宽
    QTextEdit* m_pRectHightEdit;//矩形高
    QTextEdit* m_pRectThickEdit;//矩形厚
    QTextEdit* m_pRadiusEdit;//圆形半径
    QTextEdit* m_pEnergyIncreamEdit;//能量阶梯
    QTextEdit* m_pMinimumEnergyEdit;//最低能量
    QTextEdit* m_pMaximumEnergyEdit;//最高能量
    QTextEdit* m_pMinEnergyDeg; //波片最小能量度数
    QTextEdit* m_pMaxEnergyDeg; //波片最大能量度数
    QTextEdit* m_pEnergySectionNumEdit;//能量段数
    QTextEdit* m_p1On1CntEdit;//每个能量阶梯对应的点数

    QPushButton* m_pRefreshParamBtn;//更新步长 尺寸

    QPushButton* m_pNewExpNoButton;//新建试验编号




public:
    int m_nMeasureType;//测试类型 1On1 SOn1..
    int m_nCurrentWaveLength;//1064nm 533nm 351nm
    QString MeasureNoEdit;


};

#endif // QWorkFlowParamWidget_H
