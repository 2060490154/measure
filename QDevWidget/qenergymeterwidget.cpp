/***************************************************************************
**                                                                                               **
**QEnergyMeterWidget为能量计控制组件，包含了多个QEnergyMeterItem
** Wid** get组件
** 每个QEnergyMeterItemWidget对应一个能量计
** 提供的功能函数为：
**
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qenergymeterwidget.h"
#include <qgraphicseffect.h>
#include <qfile.h>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

QEnergyMeterWidget::QEnergyMeterWidget(QEnergyMeterDeviceManager* pManagerCtrl,QList<tEnergyMeterConfigInfo*>* pConfigList,QWidget *parent) : QWidget(parent)
{
    m_pEnergyMeterDeviceManagerCtrl = pManagerCtrl;
    m_pEnergyMeterConfigList = pConfigList;

    //创建能量计界面
    QGridLayout* pEnergyLayout = new QGridLayout(this);
    m_plabel = new QLabel("<div><dive>未搜索到能量计设备，请确认设备已连接并开启电源!</div><a style=\"color:blue\" href = \"engeryfind\">刷新设备</a></div>",this);
    m_plabel->setAlignment(Qt::AlignCenter);
    pEnergyLayout->addWidget(m_plabel,0,0,1,1);
    connect(m_plabel,&QLabel::linkActivated,this,&QEnergyMeterWidget::onrefreshEnergyDev);
    if(m_pEnergyMeterDeviceManagerCtrl->m_nDevicNum != 0)
    {
          InitEnergyMeterWidget();
    }
}


//初始化能量计界面
void QEnergyMeterWidget::InitEnergyMeterWidget()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    Sleep(100);

    QGridLayout* pEnergyLayout = (QGridLayout*)this->layout();
#if 1
    m_pEnergyMeterDeviceManagerCtrl->ScanAllDevice();
    if(m_pEnergyMeterDeviceManagerCtrl->m_nDevicNum >m_pEnergyMeterConfigList->size() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this,"错误提示","能量计数据库配置信息有误，配置条目小于实际能量计个数，请修改数据库文件");
        return;
    }
    for(int i = 0; i < m_pEnergyMeterDeviceManagerCtrl->m_nDevicNum;i++)
    {
        tEnergyMeterConfigInfo* pItem = m_pEnergyMeterConfigList->at(i);
        QEnergyMeterItemWidget* pEnergyItemWidget = new QEnergyMeterItemWidget(i,pItem);
        pEnergyItemWidget->m_pEnergyMeterDev = m_pEnergyMeterDeviceManagerCtrl->GetSelectEneryMeterDev(pItem->nDevIndex);//界面绑定设备

        if(pEnergyItemWidget->m_pEnergyMeterDev->GetDeviceParam())
        {
            pEnergyItemWidget->showParamInfo();
        }
        else
        {
            pEnergyItemWidget->SetlogInfo("获取参数信息失败");
        }

        pEnergyLayout->addWidget(pEnergyItemWidget,i%2,i/2,1,1);//2行2列排列
    }

    if(m_pEnergyMeterDeviceManagerCtrl->m_nDevicNum != 0)
    {
        m_plabel->hide();
    }

#else//测试代码
    for(int i = 0; i < M_ENERGYMETER_DEV_NUM;i++)
    {
        tEnergyMeterConfigInfo* pItem = m_pEnergyMeterConfigList->at(i);

        QEnergyMeterItemWidget* pEnergyItemWidget = new QEnergyMeterItemWidget(i,pItem);
        pEnergyItemWidget->m_pEnergyMeterDev = m_pEnergyMeterDeviceManagerCtrl->GetSelectEneryMeterDev(pItem->nDevIndex);//界面绑定设备

        pEnergyItemWidget->SetlogInfo("获取参数信息失败");

       pEnergyLayout->addWidget(pEnergyItemWidget,i%2,i/2,1,1);//2行2列排列
       m_plabel->hide();
    }
#endif

    QApplication::restoreOverrideCursor();

}

//刷新能量计设备
void QEnergyMeterWidget::onrefreshEnergyDev(QString str)
{

    m_pEnergyMeterDeviceManagerCtrl->ScanAllDevice();//获取设备个数
    InitEnergyMeterWidget();

}


//单个能量计控制窗口
QEnergyMeterItemWidget::QEnergyMeterItemWidget(int nDevIndex,tEnergyMeterConfigInfo* pItemConfig,QWidget *parent) : QWidget(parent)
{
    m_pItemConfig = pItemConfig;//配置信息

    //应用样式
    QFile file(":/qss/ui.qss");
    file.open(QFile::ReadOnly);
    QString stylesheet = file.readAll();
    this->setStyleSheet(stylesheet);


    m_pEnergyMeterDev = NULL;
    m_nDevIndex = nDevIndex;
    InitUI();

    //m_pCoeffValuEdit->setText(QString("%1").arg(m_pItemConfig->lfCoeff,0,'f',4));

    connect(m_pGetValueBtn,&QPushButton::clicked,this,&QEnergyMeterItemWidget::onGetValue);
    connect(m_pMeasureMode,static_cast<void (QComboBox:: *)(int)>(&QComboBox::currentIndexChanged),this,&QEnergyMeterItemWidget::onSetMeasureMode);
    connect(m_pMeasureRange,static_cast<void (QComboBox:: *)(int)>(&QComboBox::currentIndexChanged),this,&QEnergyMeterItemWidget::onsetMeasureRange);
    connect(m_pMeasureAvgComBox,static_cast<void (QComboBox:: *)(int)>(&QComboBox::currentIndexChanged),this,&QEnergyMeterItemWidget::onsetMeasureAvg);
    connect(m_pMeasureWaveLength,static_cast<void (QComboBox:: *)(int)>(&QComboBox::currentIndexChanged),this,&QEnergyMeterItemWidget::onsetMeasureWaveLength);
}

//初始化UI
void QEnergyMeterItemWidget::InitUI()
{
    m_pMeasureMode = new QComboBox();
    m_pMeasureWaveLength = new QComboBox();
    m_pMeasureRange = new QComboBox();
    m_pMeasureAvgComBox = new QComboBox();
    m_pMeasureValueEdit = new QTextEdit();
    m_pAdJustValueEdit = new QTextEdit();
    //m_pCoeffValuEdit = new QTextEdit();
    m_pGetValueBtn = new QPushButton("查询数据");
    m_pCoeffBtn = new QPushButton("更新系数");
    m_pLogList = new QListWidget();

    m_pMeasureMode->setMinimumWidth(150);
    m_pMeasureMode->setMinimumHeight(30);

    m_pMeasureRange->setMinimumWidth(150);
    m_pMeasureRange->setMinimumHeight(30);

    m_pMeasureWaveLength->setMinimumWidth(150);
    m_pMeasureWaveLength->setMinimumHeight(30);

    m_pMeasureAvgComBox->setMinimumWidth(150);
    m_pMeasureAvgComBox->setMinimumHeight(30);

    //m_pCoeffValuEdit->setMaximumHeight(40);
    m_pMeasureValueEdit->setMaximumHeight(40);
    m_pAdJustValueEdit->setMaximumHeight(40);
    m_pGetValueBtn->setMinimumHeight(40);
    m_pGetValueBtn->setMaximumWidth(200);
    m_pCoeffBtn->setMinimumHeight(40);
    m_pMeasureValueEdit->setMaximumWidth(200);
    m_pAdJustValueEdit->setMaximumWidth(200);

    m_pMeasureValueEdit->setEnabled(false);
    m_pAdJustValueEdit->setEnabled(false);

    m_pLogList->setStyleSheet("background-color:rgb(255,255,255);");

    QLabel* plabel1 = new QLabel("测量值");
    QLabel* plabel2 = new QLabel("校准值");

    QGroupBox* pGroupBox = InitSetGroup();


    //布局
    QGroupBox* pAllGroupBox = new QGroupBox(m_pItemConfig->sChannelName);
    QGridLayout* pLayout = new QGridLayout(pAllGroupBox);
    pLayout->addWidget(pGroupBox,0,0,3,4);

   // pLayout->addWidget(new QLabel("校准系数"),0,4,1,1);
   // pLayout->addWidget(m_pCoeffValuEdit,0,5,1,1);

    pLayout->addWidget(plabel1,0,4,1,1);
    pLayout->addWidget(m_pMeasureValueEdit,0,5,1,1);
    pLayout->addWidget(plabel2,1,4,1,1);
    pLayout->addWidget(m_pAdJustValueEdit,1,5,1,1);
    pLayout->addWidget(m_pGetValueBtn,2,5,1,1);
   // pLayout->addWidget(plabel2,1,1,1,1);
  //  pLayout->addWidget(m_pAdJustValueEdit,1,2,1,1);
 //   pLayout->addWidget(m_pCoeffBtn,1,3,1,1);

    pLayout->addWidget(m_pLogList,3,0,1,8);

    QGridLayout* playout1 = new QGridLayout(this);
    playout1->addWidget(pAllGroupBox,0,0,1,1);


}
//创建groupbox
QGroupBox* QEnergyMeterItemWidget::InitSetGroup()
{
    QLabel* plabel1 = new QLabel("测量模式");
    QLabel* plabel2 = new QLabel("测量波长");
    QLabel* plabel3 = new QLabel("测量范围");
    QLabel* plabel4 = new QLabel("平均    ");
    QGroupBox *groupBox = new QGroupBox(tr("参数设置"));
    QVBoxLayout* pVBox = new QVBoxLayout(groupBox);

    QHBoxLayout* pHBox = new QHBoxLayout();
    pHBox->addWidget(plabel1);
    pHBox->addWidget(m_pMeasureMode);
    pVBox->addLayout(pHBox);

    QHBoxLayout* pHBox1 = new QHBoxLayout();
    pHBox1->addWidget(plabel2);
    pHBox1->addWidget(m_pMeasureWaveLength);
    pVBox->addLayout(pHBox1);

    QHBoxLayout* pHBox2 = new QHBoxLayout();
    pHBox2->addWidget(plabel3);
    pHBox2->addWidget(m_pMeasureRange);
    pVBox->addLayout(pHBox2);

    QHBoxLayout* pHBox3 = new QHBoxLayout();
    pHBox3->addWidget(plabel4);
    pHBox3->addWidget(m_pMeasureAvgComBox);
    pVBox->addLayout(pHBox3);

    return groupBox;

}



//查询测量数据
void QEnergyMeterItemWidget::onGetValue()
{
    float lfvalue;
    bool bOk = false;
    if(m_pEnergyMeterDev == NULL)
    {
        SetlogInfo("设备未连接，无法查询");
        return;
    }

    m_lfCoeff = m_pItemConfig->lfCoeff;

    if( m_pEnergyMeterDev->GetMeasureData())
    {
        m_pMeasureValueEdit->setText(m_pEnergyMeterDev->m_tEnergyMeterParam.m_sMeasureValue);
        lfvalue = m_pEnergyMeterDev->m_tEnergyMeterParam.m_sMeasureValue.toFloat(&bOk);
        lfvalue = lfvalue * m_lfCoeff;

        m_pAdJustValueEdit->setText(QString("%1").arg(lfvalue,0,'f',2));

        SetlogInfo("查询测量值成功");
    }
    else
    {
         m_pMeasureValueEdit->setText("");
         SetlogInfo("查询测量值失败");
    }

}

//打印操作信息
void QEnergyMeterItemWidget::SetlogInfo(QString slog)
{
    QTime time = QTime::currentTime();

    QString str = time.toString("hh:mm:ss") +"    "+ slog;


    m_pLogList->addItem(str);
    m_pLogList->setCurrentRow(m_pLogList->count()-1);

}

//显示参数信息
void QEnergyMeterItemWidget::showParamInfo()
{
    QString sTemp;

    //测量模式
    for(int i = 0; i < m_pEnergyMeterDev->m_tEnergyMeterParam.m_tMeasureModeArray.size();i++)
    {
        m_pMeasureMode->addItem(m_pEnergyMeterDev->m_tEnergyMeterParam.m_tMeasureModeArray[i]);
    }
    m_pMeasureMode->setCurrentIndex(1);
    m_pEnergyMeterDev->m_tEnergyMeterParam.m_nCurrentModeIndex = 1;

    //波长
    for(int i = 0; i < m_pEnergyMeterDev->m_tEnergyMeterParam.m_tWavelengthArray.size();i++)
    {
        m_pMeasureWaveLength->addItem(m_pEnergyMeterDev->m_tEnergyMeterParam.m_tWavelengthArray[i]);
    }
    m_pMeasureWaveLength->setCurrentIndex(3);
    m_pEnergyMeterDev->m_tEnergyMeterParam.m_nCurrentWavelengthIndex = 3;

    //测量范围
    for(int i = 0; i < m_pEnergyMeterDev->m_tEnergyMeterParam.m_tRangeArray.size();i++)
    {
        m_pMeasureRange->addItem(m_pEnergyMeterDev->m_tEnergyMeterParam.m_tRangeArray[i]);
    }

    if(m_nDevIndex == 1)//主能量计参数设置 默认为2.0J by lg 20190611
    {
        m_pMeasureRange->setCurrentIndex(1);
        m_pEnergyMeterDev->m_tEnergyMeterParam.m_nCurrentRangeIndex = 1;

    }
    else
    {
        m_pMeasureRange->setCurrentIndex(2);
        m_pEnergyMeterDev->m_tEnergyMeterParam.m_nCurrentRangeIndex = 2;

    }


    //平均值
    for(int i = 0; i < m_pEnergyMeterDev->m_tEnergyMeterParam.m_tMeasureAverageArray.size();i++)
    {
        m_pMeasureAvgComBox->addItem(m_pEnergyMeterDev->m_tEnergyMeterParam.m_tMeasureAverageArray[i]);
    }
    m_pMeasureAvgComBox->setCurrentIndex(0);
    m_pEnergyMeterDev->m_tEnergyMeterParam.m_nCurrentAvgIndex = 0;

}

//设置测量模式
void QEnergyMeterItemWidget::onSetMeasureMode()
{

    int nCurrentIndex = m_pMeasureMode->currentIndex();
    QString sTemp = m_pMeasureMode->currentText();
    if( m_pEnergyMeterDev->SetMeasureMode(nCurrentIndex))
    {
        SetlogInfo("设置测量模式为:"+sTemp+"成功");
    }
    else
    {
        SetlogInfo("设置测量模式为:"+sTemp+"失败");
    }

}

//设置测量范围
void QEnergyMeterItemWidget::onsetMeasureRange()
{

    int nCurrentIndex = m_pMeasureRange->currentIndex();
    QString sTemp = m_pMeasureRange->currentText();
    if( m_pEnergyMeterDev->SetMeasureRange(nCurrentIndex))
    {
        SetlogInfo("设置测量范围为:"+sTemp+"成功");
    }
    else
    {
        SetlogInfo("设置测量范围为:"+sTemp+"失败");
    }

}

//设置测量波长
void QEnergyMeterItemWidget::onsetMeasureWaveLength()
{

    int nCurrentIndex = m_pMeasureWaveLength->currentIndex();
    QString sTemp = m_pMeasureWaveLength->currentText();
    if( m_pEnergyMeterDev->SetMeasureWavelength(nCurrentIndex))
    {
        SetlogInfo("设置测量波长为:"+sTemp+"成功");
    }
    else
    {
        SetlogInfo("设置测量波长为:"+sTemp+"失败");
    }

}
//设置测量平均值
void QEnergyMeterItemWidget::onsetMeasureAvg()
{

    int nCurrentIndex = m_pMeasureAvgComBox->currentIndex();
    QString sTemp = m_pMeasureAvgComBox->currentText();
    if( m_pEnergyMeterDev->SetMeasureAverage(nCurrentIndex))
    {
        SetlogInfo("设置测量平均值为:"+sTemp+"成功");
    }
    else
    {
        SetlogInfo("设置测量平均值为:"+sTemp+"失败");
    }

}

