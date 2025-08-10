#include "qsystemparamwidget.h"
#include <QApplication>


QSystemParamWidget::QSystemParamWidget(QConfigProcess* pCtrl)
{
    m_pConfigProcess = pCtrl;

    InitUI();

    //显示当前系数
    m_pBaseWave_K_TextEdit->setText(QString("%1").arg(m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[0].m_lfK));
    m_pBaseWave_B_TextEdit->setText(QString("%1").arg(m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[0].m_lfB));

    m_pDoubleWave_K_TextEdit->setText(QString("%1").arg(m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[1].m_lfK));
    m_pDoubleWave_B_TextEdit->setText(QString("%1").arg(m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[1].m_lfB));

    m_pTribWave_K_TextEdit->setText(QString("%1").arg(m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[2].m_lfK));
    m_pTribWave_B_TextEdit->setText(QString("%1").arg(m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[2].m_lfB));


    connect(m_pBaseWaveUpdatePushButton,&QPushButton::clicked,this,&QSystemParamWidget::OnUpdateBaseWaveCoeffData);
    connect(m_pDoubleWaveUpdatePushButton,&QPushButton::clicked,this,&QSystemParamWidget::OnUpdateDoubleWaveCoeffData);
    connect(m_pTribWaveUpdatePushButton,&QPushButton::clicked,this,&QSystemParamWidget::OnUpdateTribWaveCoeffData);

    for(int i = 0; i < M_ENERGYMETER_DEV_NUM;i++)
    {
         connect(m_pEnergyDevUpdatePushButton[i],&QPushButton::clicked,this,&QSystemParamWidget::OnUpdateEnergyDevCoeffData);
    }
}


/******************************************
*功能：初始化界面
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
void QSystemParamWidget::InitUI()
{

    m_pBaseWave_K_TextEdit = new QTextEdit();
    m_pBaseWave_B_TextEdit = new QTextEdit();
    m_pBaseWaveUpdatePushButton = new QPushButton();

    m_pDoubleWaveUpdatePushButton = new QPushButton();
    m_pDoubleWave_B_TextEdit = new QTextEdit();
    m_pDoubleWave_K_TextEdit = new QTextEdit();

    m_pTribWaveUpdatePushButton = new QPushButton();
    m_pTribWave_B_TextEdit = new QTextEdit();
    m_pTribWave_K_TextEdit = new QTextEdit();

    m_pEnergyDev_TextEdit[0] = new QTextEdit();
    m_pEnergyDev_TextEdit[1] = new QTextEdit();
    m_pEnergyDevUpdatePushButton[0] = new QPushButton();
    m_pEnergyDevUpdatePushButton[1] = new QPushButton();


    //损伤检测方法
    m_pDetectMingRBtn = new QRadioButton("明场检测");
    m_pDetectAnRBtn = new QRadioButton("暗场检测");
    m_pDetectMingRBtn->setChecked(true);

    QList<QGroupBox*> l_devGroupBoxList;

    //基频系数
    QGroupBox* pBaseWaveGroupBox = createCoeffAdjGroupBox("基频能量调节系数",m_pBaseWave_K_TextEdit,m_pBaseWave_B_TextEdit,m_pBaseWaveUpdatePushButton);

    //二倍频系数
    QGroupBox* pDoubleWaveGroupBox = createCoeffAdjGroupBox("二倍频能量调节系数",m_pDoubleWave_K_TextEdit,m_pDoubleWave_B_TextEdit,m_pDoubleWaveUpdatePushButton);

    //三倍频系数
    QGroupBox* pTribWaveGroupBox = createCoeffAdjGroupBox("三倍频能量调节系数",m_pTribWave_K_TextEdit,m_pTribWave_B_TextEdit,m_pTribWaveUpdatePushButton);

    l_devGroupBoxList.append(pBaseWaveGroupBox);
    l_devGroupBoxList.append(pDoubleWaveGroupBox);
    l_devGroupBoxList.append(pTribWaveGroupBox);
    //能量计系数

    int nSize = m_pConfigProcess->m_tDevConfigParam.m_tEnergyConfigInfoList.size();
    if(nSize <= M_ENERGYMETER_DEV_NUM)
    {
        for(int i = 0; i < nSize;i++)
        {
            tEnergyMeterConfigInfo* l_tInfo = m_pConfigProcess->m_tDevConfigParam.m_tEnergyConfigInfoList[i];

            QGroupBox* pDevGroupBox = createDevCoeffGroupBox(l_tInfo->sChannelName,m_pEnergyDev_TextEdit[i],m_pEnergyDevUpdatePushButton[i]);
            l_devGroupBoxList.append(pDevGroupBox);

            m_pEnergyDev_TextEdit[i]->setText(QString("%1").arg(l_tInfo->lfCoeff));
        }

    }

    //损伤方法选择
    QGroupBox* pDetectMethod = createDetectMethodGroupBox("损伤检测方法选择",m_pDetectMingRBtn,m_pDetectAnRBtn);
    l_devGroupBoxList.append(pDetectMethod);


    QGridLayout* pGridLayOut = new QGridLayout(this);

    pGridLayOut->setRowStretch(0,1);
    pGridLayOut->setRowStretch(1,1);
    pGridLayOut->setRowStretch(2,3);

    pGridLayOut->setColumnStretch(0,1);
    pGridLayOut->setColumnStretch(1,1);
    pGridLayOut->setColumnStretch(2,1);
    pGridLayOut->setColumnStretch(3,3);


    for(int i = 0; i < l_devGroupBoxList.size();i++)
    {
        pGridLayOut->addWidget(l_devGroupBoxList.at(i),i%2,i/2,1,1);
    }

}

/******************************************
*功能：创建能量调节系数groupbox
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
QGroupBox* QSystemParamWidget:: createCoeffAdjGroupBox(QString sName, QTextEdit* pKEdit,QTextEdit* pBEdit,QPushButton* pUpdateButton)
{
    //基频系数
    QGroupBox* pGroupBox = new QGroupBox(this);
    pGroupBox->setTitle(sName);
    pGroupBox->setMaximumHeight(200);
    pGroupBox->setMaximumWidth(300);

//    pKEdit = new QTextEdit(this);
//    pBEdit = new QTextEdit(this);
//    pUpdateButton = new QPushButton(this);
    pUpdateButton->setText("更新系数");
    pUpdateButton->setMinimumHeight(30);
    pKEdit->setMaximumHeight(30);
    pBEdit->setMaximumHeight(30);
    pKEdit->setMaximumWidth(200);
    pBEdit->setMaximumWidth(200);
    pUpdateButton->setMaximumWidth(100);

    QHBoxLayout* pHBox1 = new QHBoxLayout();
    pHBox1->addWidget(new QLabel("K:"));
    pHBox1->addWidget(pKEdit);

    QHBoxLayout* pHBox2 = new QHBoxLayout();
    pHBox2->addWidget(new QLabel("B:"));
    pHBox2->addWidget(pBEdit);

    QVBoxLayout* pVbox = new QVBoxLayout(pGroupBox);
    pVbox->addLayout(pHBox1);
    pVbox->addLayout(pHBox2);
    pVbox->addWidget(pUpdateButton);

    return pGroupBox;
}


/******************************************
*功能：创建能量计系数groupbox
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
QGroupBox* QSystemParamWidget:: createDevCoeffGroupBox(QString sName, QTextEdit* pCeoffEdit,QPushButton* pUpdateButton)
{
    //基频系数
    QGroupBox* pGroupBox = new QGroupBox(this);
    pGroupBox->setTitle(sName);
    pGroupBox->setMaximumHeight(200);
    pGroupBox->setMaximumWidth(300);

    pUpdateButton->setText("更新系数");
    pUpdateButton->setMinimumHeight(30);
    pCeoffEdit->setMaximumHeight(30);
    pCeoffEdit->setMaximumWidth(200);
    pUpdateButton->setMaximumWidth(100);

    QHBoxLayout* pHBox1 = new QHBoxLayout();
    pHBox1->addWidget(new QLabel("系数:"));
    pHBox1->addWidget(pCeoffEdit);

    QVBoxLayout* pVbox = new QVBoxLayout(pGroupBox);
    pVbox->addLayout(pHBox1);
    pVbox->addWidget(pUpdateButton);

    return pGroupBox;
}


/******************************************
*功能：创建检测方法groupbox
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
QGroupBox* QSystemParamWidget:: createDetectMethodGroupBox(QString sName, QRadioButton* pRBtn1,QRadioButton* pRBtn2)
{
    //基频系数
    QGroupBox* pGroupBox = new QGroupBox(this);
    pGroupBox->setTitle(sName);
    pGroupBox->setMaximumHeight(200);
    pGroupBox->setMaximumWidth(300);

    pRBtn1->setMaximumHeight(30);
    pRBtn2->setMaximumHeight(30);
    pRBtn1->setMaximumWidth(200);
    pRBtn2->setMaximumWidth(200);

    QVBoxLayout* pVbox = new QVBoxLayout(pGroupBox);
    pVbox->addWidget(pRBtn1);
    pVbox->addWidget(pRBtn2);

    return pGroupBox;
}


/******************************************
*功能：更新系数数据
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
void QSystemParamWidget::OnUpdateBaseWaveCoeffData()
{
    bool bOk = false;
    float lfK = m_pBaseWave_K_TextEdit->toPlainText().toFloat(&bOk);
    float lfB = m_pBaseWave_B_TextEdit->toPlainText().toFloat(&bOk);

    m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[0].m_lfB = lfB;
    m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[0].m_lfK = lfK;

    m_pConfigProcess->updateEnergyAdjCoeff();


}

/******************************************
*功能：更新系数数据
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
void QSystemParamWidget::OnUpdateDoubleWaveCoeffData()
{
    bool bOk = false;
    float lfK = m_pDoubleWave_K_TextEdit->toPlainText().toFloat(&bOk);
    float lfB = m_pDoubleWave_B_TextEdit->toPlainText().toFloat(&bOk);

    m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[1].m_lfB = lfB;
    m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[1].m_lfK = lfK;

    m_pConfigProcess->updateEnergyAdjCoeff();
}
/******************************************
*功能：更新能量调节系数数据
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
void QSystemParamWidget::OnUpdateTribWaveCoeffData()
{
    bool bOk = false;
    float lfK = m_pTribWave_K_TextEdit->toPlainText().toFloat(&bOk);
    float lfB = m_pTribWave_B_TextEdit->toPlainText().toFloat(&bOk);

    m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[2].m_lfB = lfB;
    m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[2].m_lfK = lfK;

    m_pConfigProcess->updateEnergyAdjCoeff();
}


/******************************************
*功能：更新能量计设备系数数据
*输入：
*输出:
*返回值：
*创建人:
*修改记录:
******************************************/
 void QSystemParamWidget::OnUpdateEnergyDevCoeffData()
 {
     bool bOk = false;
     int nSize = m_pConfigProcess->m_tDevConfigParam.m_tEnergyConfigInfoList.size();
     if(nSize <= M_ENERGYMETER_DEV_NUM)
     {
         for(int i = 0; i < nSize;i++)
         {
             m_pConfigProcess->m_tDevConfigParam.m_tEnergyConfigInfoList[i]->lfCoeff = m_pEnergyDev_TextEdit[i]->toPlainText().toFloat(&bOk);
         }

     }

     m_pConfigProcess->updateEnergyDevCoeff();
 }
