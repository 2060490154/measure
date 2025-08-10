#include "qworkflowparamwidget.h"
#include <QGraphicsDropShadowEffect>
#include <qdatetime.h>
#include <QDir>
#include <QtMath>
#include <QDebug>
#include "quiqss.h"
#include "define.h"
#include "qudpcs.h"
QWorkFlowParamWidget::QWorkFlowParamWidget(int nMeasureType,int nCurrentWaveLength,QWidget *parent) : QWidget(parent)
{
    m_nMeasureType = nMeasureType;
    m_nCurrentWaveLength = nCurrentWaveLength;
    InitUI();
    QUDPcs *UDPcs = new QUDPcs();

    CreateExpNoString();

    onUpdateSetParam();
    connect(m_pRefreshParamBtn,&QPushButton::clicked,this,&QWorkFlowParamWidget::onUpdateSetParam);
    connect(m_pRefreshParamBtn, &QPushButton::clicked, [this,UDPcs]() {
        UDPcs->SEND_PLAN(m_nMeasureType, MeasureNoEdit);
    });
    connect(m_pNewExpNoButton,&QPushButton::clicked,this,&QWorkFlowParamWidget::onNewExpNo);
    void(QComboBox::*fp)(int)=&QComboBox::currentIndexChanged;
    connect(m_pDevSizeTypeCombox,fp,this,&QWorkFlowParamWidget::onChangeSizeType);
    connect(m_pMeasureStepEdit,&QTextEdit::textChanged,this,&QWorkFlowParamWidget::onSetDefaultPointCnt);

    this->setAttribute(Qt::WA_QuitOnClose,false);
}

void QWorkFlowParamWidget::InitUI()
{
    //QLabel* pRaius = new QLabel("实验编号");
    m_pWidthLabel = new QLabel("长:");
    m_pHightLabel = new QLabel("宽:");
    m_pThickLabel = new QLabel("厚:");
    m_pLabel = new QLabel("mm");
    m_pLabel2 = new QLabel("mm");

    m_pNewExpNoButton = new QPushButton("新建编号");
    m_pRefreshParamBtn = new QPushButton("发布扫描计划");
    m_pMeasureNoEdit = new QTextEdit();
    m_pMeasureStepEdit = new QTextEdit("10");
    m_pSampleName = new QTextEdit("##");
    m_pRectWidthEdit = new QTextEdit("100");
    m_pRectHightEdit = new QTextEdit("100");
    m_pRectThickEdit = new QTextEdit("2");
    m_pRadiusEdit = new QTextEdit();
    m_pEnergyIncreamEdit = new QTextEdit("0.0");
    m_pMinimumEnergyEdit = new QTextEdit("0.0");
    m_pMaximumEnergyEdit = new QTextEdit("0.0");
    m_pEnergySectionNumEdit = new QTextEdit("1");
    m_p1On1CntEdit = new QTextEdit("10");
    m_pMinEnergyDeg = new QTextEdit();
    m_pMaxEnergyDeg = new QTextEdit();




    m_pWaveLengthCombox = new QComboBox();//波长类型
    m_pWaveLengthCombox->addItem("1064nm");
    m_pWaveLengthCombox->addItem("532nm");
    m_pWaveLengthCombox->addItem("355nm");
    if(m_nCurrentWaveLength < 3)
    {
        m_pWaveLengthCombox->setCurrentIndex(m_nCurrentWaveLength);
    }
    else
        m_pWaveLengthCombox->setCurrentIndex(0);

    m_pManualArea = new QLineEdit("0");


    m_pDevSizeTypeCombox = new QComboBox();//元器件尺寸类型
    m_pDevSizeTypeCombox->addItem("矩形");
    m_pDevSizeTypeCombox->addItem("圆形");
    m_pDevSizeTypeCombox->setCurrentIndex(0);

    m_pRadiusEdit->setHidden(true);


    //设置大小
    m_pEnergyIncreamEdit->setMaximumSize(QSize(500,30));
    m_pMinimumEnergyEdit->setMaximumSize(QSize(500,30));
    m_pMaximumEnergyEdit->setMaximumSize(QSize(500,30));
    m_pEnergySectionNumEdit->setMaximumSize(QSize(500,30));
    m_pMeasureNoEdit->setMaximumSize(QSize(500,30));
    m_pMeasureStepEdit->setMaximumSize(QSize(300,30));
    m_pSampleName->setMaximumSize(QSize(100,30));
    m_pRectWidthEdit->setMaximumSize(QSize(100,30));
    m_pRectHightEdit->setMaximumSize(QSize(100,30));
    m_pRectThickEdit->setMaximumSize(QSize(100,30));
    m_pMinEnergyDeg->setMaximumSize(QSize(100,30));
    m_pMaxEnergyDeg->setMaximumSize(QSize(100,30));
    m_pRadiusEdit->setMaximumSize(QSize(100,30));
    m_pDevSizeTypeCombox->setMaximumSize(QSize(50,30));
    m_pWaveLengthCombox->setMaximumSize(QSize(50,30));
    m_pManualArea->setMaximumSize(QSize(100,30));
    m_pRefreshParamBtn->setMinimumSize(QSize(100,30));
    m_pNewExpNoButton->setMinimumSize(QSize(100,30));

    m_p1On1CntEdit->setMaximumSize(QSize(500,30));

    //布局
    QVBoxLayout* pVBox = new QVBoxLayout(this);

    QHBoxLayout* pHBox = new QHBoxLayout();
    pHBox->addWidget(new QLabel("实验编号"));
    pHBox->addWidget(m_pMeasureNoEdit);
    pHBox->addSpacerItem(new QSpacerItem(40,30));
    pHBox->addWidget(m_pNewExpNoButton);
    pVBox->addLayout(pHBox);

    //元件名称和元件编号
    QHBoxLayout* pHBox7 = new QHBoxLayout();
    pHBox7->addWidget(new QLabel("元件名称"));
    pHBox7->addWidget(m_pSampleName);
    pHBox7->addSpacerItem(new QSpacerItem(400,30));
    pVBox->addLayout(pHBox7);


    QHBoxLayout* pHBox1 = new QHBoxLayout();
    pHBox1->addWidget(new QLabel("元件尺寸 "));
    pHBox1->addWidget(m_pDevSizeTypeCombox);
    pHBox1->addWidget(m_pWidthLabel);
    pHBox1->addWidget(m_pRectWidthEdit);
    pHBox1->addWidget(new QLabel("mm"));
    pHBox1->addWidget(m_pHightLabel);
    pHBox1->addWidget(m_pRectHightEdit);
    pHBox1->addWidget(m_pLabel);

    pHBox1->addWidget(m_pThickLabel);
    pHBox1->addWidget(m_pRectThickEdit);
    pHBox1->addWidget(m_pLabel2);
    pVBox->addLayout(pHBox1);


    QHBoxLayout* pHBox3 = new QHBoxLayout();
    pHBox3->addWidget(new QLabel("检测步长:"));
    pHBox3->addWidget(m_pMeasureStepEdit);
    pHBox3->addWidget(new QLabel("mm"));
    pHBox3->addWidget(new QLabel("最小能量百分比:"));
    pHBox3->addWidget(m_pMinEnergyDeg);
    pHBox3->addWidget(new QLabel("%"));
    pHBox3->addWidget(new QLabel("最大能量百分比:"));
    pHBox3->addWidget(m_pMaxEnergyDeg);
    pHBox3->addWidget(new QLabel("%"));
    getCurrentDeg(m_nCurrentWaveLength);
    pVBox->addLayout(pHBox3);

    QHBoxLayout* pHBox4 = new QHBoxLayout();
    pHBox4->addWidget(new QLabel("波长选择:"));
    pHBox4->addWidget(m_pWaveLengthCombox);
    pHBox4->addSpacerItem(new QSpacerItem(10,30));
    pHBox4->addWidget(new QLabel("参考光斑面积:"));
    pHBox4->addWidget(m_pManualArea);
    pHBox4->addWidget(new QLabel("平方厘米"));
    pHBox4->addSpacerItem(new QSpacerItem(100,30));
    pVBox->addLayout(pHBox4);



    if(m_nMeasureType == M_MEASURETYPE_1On1)
    {
        QHBoxLayout* pHBox5 = new QHBoxLayout();
        //        pHBox5->addWidget(new QLabel("能量台阶数:"));
        //        pHBox5->addWidget(m_pEnergyIncreamEdit);
        pHBox5->addWidget(new QLabel(""));
        pHBox5->addWidget(new QLabel("最小能量百分比:"));
        pHBox5->addWidget(m_pMinimumEnergyEdit);
        //        pHBox5->addWidget(new QLabel("%"));
        pHBox5->addWidget(new QLabel("能量台阶百分比:"));
        pHBox5->addWidget(m_pMaximumEnergyEdit);
        //        pHBox5->addWidget(new QLabel("%"));
        pVBox->addLayout(pHBox5);

        QHBoxLayout* pHBox6 = new QHBoxLayout();
        pHBox6->addWidget(new QLabel("每个能量对应点数:"));
        pHBox6->addWidget(m_p1On1CntEdit);
        pHBox6->addSpacerItem(new QSpacerItem(150,30));
        pHBox6->addWidget(m_pRefreshParamBtn);
        pVBox->addLayout(pHBox6);
    }
    else    if(m_nMeasureType == M_MEASURETYPE_SOn1)
    {
        QHBoxLayout* pHBox5 = new QHBoxLayout();
        pHBox5->addWidget(new QLabel("最低能量:"));
        pHBox5->addWidget(m_pMinimumEnergyEdit);
        pHBox5->addWidget(new QLabel("mJ"));
        pHBox5->addWidget(new QLabel("能量台阶:"));
        pHBox5->addWidget(m_pEnergyIncreamEdit);
        pHBox5->addWidget(new QLabel("mJ"));
        pVBox->addLayout(pHBox5);

        QHBoxLayout* pHBox6 = new QHBoxLayout();
        pHBox6->addWidget(new QLabel("单点发次设置:"));
        pHBox6->addWidget(m_p1On1CntEdit);
        pHBox6->addSpacerItem(new QSpacerItem(150,30));
        pHBox6->addWidget(m_pRefreshParamBtn);
        pVBox->addLayout(pHBox6);
    }
    else    if(m_nMeasureType == M_MEASURETYPE_ROn1)
    {
        QHBoxLayout* pHBox5 = new QHBoxLayout();
        pHBox5->addWidget(new QLabel("最小能量百分比:"));
        pHBox5->addWidget(m_pMinimumEnergyEdit);
        //        pHBox5->addWidget(new QLabel("%"));
        pHBox5->addWidget(new QLabel("能量台阶百分比:"));
        pHBox5->addWidget(m_pMaximumEnergyEdit);
        //        pHBox5->addWidget(new QLabel("%"));
        pHBox5->addWidget(new QLabel("能量台阶数:"));
        pHBox5->addWidget(m_pEnergySectionNumEdit);
        pHBox5->addSpacerItem(new QSpacerItem(20,30));
        pHBox5->addWidget(m_pRefreshParamBtn);
        pVBox->addLayout(pHBox5);
    }
    else if (m_nMeasureType == M_MEASURETYPE_RASTER)
    {
        QHBoxLayout* pHBox5 = new QHBoxLayout();
        pHBox5->addWidget(new QLabel("最低能量百分比:"));
        pHBox5->addWidget(m_pMinimumEnergyEdit);
        pHBox5->addWidget(new QLabel("%"));
        pHBox5->addWidget(new QLabel("最高能量百分比:"));
        pHBox5->addWidget(m_pMaximumEnergyEdit);
        pHBox5->addWidget(new QLabel("%"));
        pHBox5->addWidget(new QLabel("能量段数:"));
        pHBox5->addWidget(m_pEnergySectionNumEdit);
        pVBox->addLayout(pHBox5);

        QHBoxLayout* pHBox6 = new QHBoxLayout();
        pHBox6->addWidget(new QLabel("每能量段点数:"));
        pHBox6->addWidget(m_p1On1CntEdit);
        pHBox6->addSpacerItem(new QSpacerItem(150,30));
        pHBox6->addWidget(m_pRefreshParamBtn);
        pVBox->addLayout(pHBox6);
    }



    this->setMaximumSize(QSize(600,300));

}



//更新尺寸类型
void QWorkFlowParamWidget::onChangeSizeType(int nIndex)
{
    if(nIndex == 0)//矩形
    {
        m_pWidthLabel->setText("长:");
        m_pRectHightEdit->show();
        m_pHightLabel->show();
        m_pLabel->show();

        m_pRectThickEdit->show();
        m_pThickLabel->show();
        m_pLabel2->show();
    }

    if(nIndex == 1)//圆形
    {
        m_pWidthLabel->setText("半径:");
        m_pRectHightEdit->hide();
        m_pHightLabel->hide();
        m_pLabel->hide();
    }
}

//更新设置参数
void QWorkFlowParamWidget::onUpdateSetParam()
{
    bool bOk = false;

    //2020yi
    if(m_pMeasureStepEdit->toPlainText().toDouble(&bOk) <= 0)
    {
        QMessageBox::information(this,"提示","步长设置不正确，不能为0或字符");
        return;
    }


    m_tworkflowParam.m_sExpNumber = m_pMeasureNoEdit->toPlainText();
    m_tworkflowParam.m_sSampleName = m_pSampleName->toPlainText();
    m_tworkflowParam.m_nSizeType = m_pDevSizeTypeCombox->currentIndex()+1;
    //2020yi
    //    m_tworkflowParam.m_nStep = m_pMeasureStepEdit->toPlainText().toInt(&bOk);
    m_tworkflowParam.m_dStep = m_pMeasureStepEdit->toPlainText().toDouble(&bOk);
    m_tworkflowParam.m_nWaveLengthType = m_pWaveLengthCombox->currentIndex();
    m_tworkflowParam.m_lfEnergyIncream = m_pEnergyIncreamEdit->toPlainText().toFloat(&bOk);
    m_tworkflowParam.m_lfMinimumEnergy = m_pMinimumEnergyEdit->toPlainText().toFloat(&bOk);
    m_tworkflowParam.m_lfMaximumEnergy = m_pMaximumEnergyEdit->toPlainText().toFloat(&bOk);
    m_tworkflowParam.m_nEnergySectionNum = m_pEnergySectionNumEdit->toPlainText().toInt(&bOk);
    m_tworkflowParam.m_nCnt = m_p1On1CntEdit->toPlainText().toInt(&bOk);
    m_tworkflowParam.m_dManualArea = m_pManualArea->text().toDouble(&bOk);
    m_tworkflowParam.m_dMinDeg = m_pMinEnergyDeg->toPlainText().toDouble(&bOk);
    m_tworkflowParam.m_dMaxDeg = m_pMaxEnergyDeg->toPlainText().toDouble(&bOk);

    //2020dong
    if(m_tworkflowParam.m_nSizeType == M_SIZETYPE_RECT)//矩形
    {
        m_tworkflowParam.m_dWidth = m_pRectWidthEdit->toPlainText().toDouble(&bOk);
        m_tworkflowParam.m_dHigh = m_pRectHightEdit->toPlainText().toDouble(&bOk);
        m_tworkflowParam.m_dThick = m_pRectThickEdit->toPlainText().toDouble(&bOk);
    }

    if(m_tworkflowParam.m_nSizeType == M_SIZETYPE_CIRCL)//圆形
    {
        m_tworkflowParam.m_dRaidus = m_pRectWidthEdit->toPlainText().toDouble(&bOk);
        m_tworkflowParam.m_dThick = m_pRectThickEdit->toPlainText().toDouble(&bOk);
    }

    emit updatePointUI(m_tworkflowParam.m_nSizeType,m_tworkflowParam.m_dStep,m_tworkflowParam.m_dWidth,m_tworkflowParam.m_dHigh,m_tworkflowParam.m_dRaidus);
    emit updateWorkFlow();
    emit updateExpParas(m_tworkflowParam.m_sExpNumber,m_tworkflowParam.m_sSampleName);
    emit updateDetectMethod();
}

void QWorkFlowParamWidget::onNewExpNo()
{
    CreateExpNoString();
}

void QWorkFlowParamWidget::onSetDefaultPointCnt()
{
    //    if (m_pMeasureStepEdit->toPlainText() != "" && m_pMeasureStepEdit->toPlainText() != "0")
    //2020yi
    if (m_pMeasureStepEdit->toPlainText() != "" && m_pMeasureStepEdit->toPlainText() != "0" && m_pMeasureStepEdit->toPlainText().mid(m_pMeasureStepEdit->toPlainText().size()-1,1) != ".")
    {
        m_p1On1CntEdit->setText(QString::number(qCeil(m_pRectWidthEdit->toPlainText().toDouble() / m_pMeasureStepEdit->toPlainText().toDouble())));
    }
}

/*******************************************************************
**功能：生成试验编号
**输入：
**输出：
**返回值：
*******************************************************************/
void QWorkFlowParamWidget:: CreateExpNoString()
{
    QString  sCurrentTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    if(m_nMeasureType == M_MEASURETYPE_1On1)
    {
        m_pMeasureNoEdit->setText("1On1_"+sCurrentTime);
        MeasureNoEdit = "1On1_";
        MeasureNoEdit = MeasureNoEdit.append(sCurrentTime);
    }
    if(m_nMeasureType == M_MEASURETYPE_SOn1)
    {
        m_pMeasureNoEdit->setText("SOn1_"+sCurrentTime);
        MeasureNoEdit = "SOn1_";
        MeasureNoEdit = MeasureNoEdit.append(sCurrentTime);
    }
    if(m_nMeasureType == M_MEASURETYPE_ROn1)
    {
        m_pMeasureNoEdit->setText("ROn1_"+sCurrentTime);
        MeasureNoEdit = "ROn1_";
        MeasureNoEdit = MeasureNoEdit.append(sCurrentTime);
    }
    if(m_nMeasureType == M_MEASURETYPE_RASTER)
    {
        m_pMeasureNoEdit->setText("Raster_"+sCurrentTime);
        MeasureNoEdit = "Life_";
        MeasureNoEdit = MeasureNoEdit.append(sCurrentTime);
    }


}



/******************************
*功能:根据默认波长确定能量波片的系数（最大最小角度）
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
double QWorkFlowParamWidget::getCurrentDeg(int nWaveLength)
{
    if (nWaveLength == 0)
    {
        m_pMinEnergyDeg->setText("80");
        m_pMaxEnergyDeg->setText("35");
    }
    else if (nWaveLength == 1)
    {
        m_pMinEnergyDeg->setText("20");
        m_pMaxEnergyDeg->setText("70");
    }
    else if (nWaveLength == 2)
    {
        m_pMinEnergyDeg->setText("1");
        m_pMaxEnergyDeg->setText("99");
    }
}

