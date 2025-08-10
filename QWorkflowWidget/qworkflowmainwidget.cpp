#include "qworkflowmainwidget.h"
#include <QGraphicsDropShadowEffect>
#include <QTime>
#include <QDebug>
#include <QApplication>
#include <QSpacerItem>
#include <QDir>
#include "quiqss.h"
#include "qexpresultwidget.h"
#include <QSqlError>
QWorkFlowWidget::QWorkFlowWidget(int nMeasureType,QDevManagerCtrl* pCtrl, QConfigProcess* pConfigProcess,QWidget *parent) : QWidget(parent)
{
    //获取配置信息
    m_pConfigProcess = pConfigProcess;
    m_pResultDlg = NULL;
    m_nLastPointNum = 0;
    m_bInitDev = false;
    m_nMeasureType = nMeasureType;
    _pDevManagerCtrl = pCtrl;
    m_pPointAreaWidget = NULL;
    m_pWorkFlowThread = new QWorkFlowThread(_pDevManagerCtrl);
    m_pWorkFlowThread->m_pEnergyAdjCoeff = &m_pConfigProcess->m_tPlatformConfig.m_st_Coeff[0];//能量调节系数
    m_dAreaRecord = 0.0;
    m_dAreaSquareRecord = 0.0;
    m_dAreaAver = 0.0;
    m_dAreaRMS = 0.0;
    m_dEnergyRatio = 0.0;
    m_dEnergyRatioRMS = 0.0;
    InitUI();
    //消息
    connect(m_pWorkflowParamWidget,&QWorkFlowParamWidget::updatePointUI,m_pPointAreaWidget,&QPointAreaWidget::onUpdatePointUI);
    connect(m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateExpParas,m_pDevMonitorWidget,&QDevMonitorWidget::getNewExpNo);
    connect(m_pPointAreaWidget,&QPointAreaWidget::updateWorkflow,this,&QWorkFlowWidget::onUpdateWorkflow);
    connect(m_pStandbyStartTestBtn,&QPushButton::clicked,this,&QWorkFlowWidget::OnStartStandbyTest);
    connect(m_pStartTestBtn,&QPushButton::clicked,this,&QWorkFlowWidget::OnStartTest);
    connect(m_pWorkFlowThread,&QThread::finished,this,&QWorkFlowWidget::onThreadFinish);
    connect(m_pSuspendBtn,&QPushButton::clicked,this,&QWorkFlowWidget::OnSuspendTest);
    connect(m_pManualYesBtn,&QPushButton::clicked,this,&QWorkFlowWidget::onManualYesBtnClicked);
    connect(m_pManualNoBtn,&QPushButton::clicked,this,&QWorkFlowWidget::onManualNoBtnClicked);
    connect(m_pManualTestBtn,&QPushButton::clicked,this,&QWorkFlowWidget::onManualTest);
    connect(m_pAverArea,&QPushButton::clicked,this,&QWorkFlowWidget::onAverArea);
    connect(m_pMeasureEnergyMeterRatio,&QPushButton::clicked,this,&QWorkFlowWidget::onMeasureEnergyRatio);
    connect(m_pSampleStageMove2Zero,&QPushButton::clicked,this,&QWorkFlowWidget::onSampleStage2Zero);
    connect(_pDevManagerCtrl,&QDevManagerCtrl::singal_showCalcFluxStatus,this,&QWorkFlowWidget::onShowFluxStatus);
    connect(_pDevManagerCtrl,&QDevManagerCtrl::signal_sendArea,this,&QWorkFlowWidget::onAreaRecord);
    connect(_pDevManagerCtrl,&QDevManagerCtrl::singal_showDefectStatus,this,&QWorkFlowWidget::onShowDefectStatus);

    //新光闸测试
    //yhy0320
    connect(m_pPulsewidthtest,&QPushButton::clicked,this,&QWorkFlowWidget::onNewPulsewidthtest);
//    connect(m_pNewShutterBtn,&QPushButton::clicked,this,&QWorkFlowWidget::onNewShutterTest);
    connect(_pDevManagerCtrl,&QDevManagerCtrl::signal_NewShutterTestResult,this,&QWorkFlowWidget::onNewShutterLog);
    //初始化显示流程
    onUpdateWorkflow();
}

QWorkFlowWidget::~QWorkFlowWidget()
{
    if(m_pWorkFlowThread->isRunning())
    {
        m_pWorkFlowThread->quit();
        m_pWorkFlowThread->wait();
    }
    delete m_pResultDlg;
    delete m_pWorkFlowThread;
}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QWorkFlowWidget::InitUI()
{
    //设备数据显示
    m_pDevMonitorWidget = new QDevMonitorWidget(_pDevManagerCtrl);


    //测量参数设置
    m_pParamSetTab = new QTabWidget();
    m_pWorkflowParamWidget = new QWorkFlowParamWidget(m_nMeasureType,m_pConfigProcess->m_tPlatformConfig.m_nCurrentWaveLength,this);
    m_pPointAreaWidget = new QPointAreaWidget(600,300,1,10);//点阵显示界面
    m_pDevMonitorWidget->m_sExpNo = m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber;

    m_pParamSetTab->setMinimumHeight(350);
    m_pParamSetTab->setMaximumWidth(600);
    m_pParamSetTab->addTab(m_pWorkflowParamWidget,"参数设置");
    m_pParamSetTab->addTab(m_pPointAreaWidget,"扫描点设置");
    m_pParamSetTab->setCurrentIndex(0);


    //测试流程
    m_pStandbyStartTestBtn = new QPushButton(M_STR_START_STANDBY);
    m_pStartTestBtn = new QPushButton(M_STR_START_MEASURE);
    m_pSuspendBtn = new QPushButton(M_STR_SUSPEND_MEASURE);
    m_pManualYesBtn = new QPushButton("确认损伤");
    m_pManualNoBtn = new QPushButton("未损伤");

    QGridLayout* playout = new QGridLayout(this);
    m_pStandbyWorkflowTab = new QWorkflowTab();
    m_pStandbyWorkflowTab->setLabelTitle("准备流程",":/png/dev.png","rgb(68,69,73)");
    //自动测试流程
    QWidget* pAutoWorkflowWidget = new QWidget(this);
    m_pMeasureWorkflowTab = new QWorkflowTab();
    m_pMeasureWorkflowTab->setLabelTitle("自动测试流程",":/png/dev.png","rgb(68,69,73)");
    QGridLayout* pAutoWidgetLayout = new QGridLayout(pAutoWorkflowWidget);
    pAutoWidgetLayout->addWidget(m_pMeasureWorkflowTab,0,0,4,4);
    pAutoWidgetLayout->addWidget(m_pStartTestBtn,4,0,1,1,Qt::AlignHCenter);
    pAutoWidgetLayout->addWidget(m_pSuspendBtn,4,1,1,1,Qt::AlignHCenter);
    pAutoWidgetLayout->addWidget(m_pManualYesBtn,4,2,1,1,Qt::AlignHCenter);
    pAutoWidgetLayout->addWidget(m_pManualNoBtn,4,3,1,1,Qt::AlignHCenter);
    m_pManualYesBtn->hide();
    m_pManualNoBtn->hide();

    //手动测试流程
    QStringList tTableHeader;
    tTableHeader.push_back("点编号");
    tTableHeader.push_back("设定能量");
    tTableHeader.push_back("通量");
    tTableHeader.push_back("是否有损伤");
    m_pManualWorkflowListWidget = new QTableWidget();
    m_pManualWorkflowListWidget->setColumnCount(tTableHeader.count());
    m_pManualWorkflowListWidget->setHorizontalHeaderLabels(tTableHeader);
    m_pManualWorkflowListWidget->verticalHeader()->setHidden(true);//垂直表头 不可见
    m_pManualWorkflowListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    m_pManualWorkflowListWidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满

    QLabel* pEnerglabel = new QLabel("设定能量");
    pEnerglabel->setMaximumWidth(90);
    m_pManualTestEngeryEdit = new QTextEdit();
    m_pManualTestEngeryEdit->setMaximumHeight(30);
    m_pManualTestEngeryEdit->setText("0");

    m_pCurrentPointEdit = new QTextEdit();
    m_pCurrentPointEdit->setMaximumHeight(30);
    m_pCurrentPointEdit->setText("0");

    m_pManualTestBtn = new QPushButton("通量测试",this);
    m_pManualLogListWidget = new QListWidget();
    m_pManualTestBtn->setMinimumHeight(30);
    m_pManualTestBtn->setMinimumWidth(80);

    m_pAverArea = new QPushButton("测试平均光斑面积",this);
//    m_pAverCntEdit = new QLineEdit("10");//yhy0320
    m_pAverArea->setMinimumHeight(30);
    m_pAverArea->setMinimumWidth(80);


//    m_pNewShutterBtn = new QPushButton("新光闸测试",this);
//    m_pNewShutCntEdit = new QLineEdit("200");
//    m_pNewShutterBtn->setMinimumSize(80,30);//yhy0320

    m_pSampleStageMove2Zero = new QPushButton("样品台归零",this);
    m_pSampleStageMove2Zero->setMinimumHeight(30);
    m_pSampleStageMove2Zero->setMinimumWidth(80);

    m_pMeasureEnergyMeterRatio = new QPushButton("分光比测试",this);
//    m_pEnergyAverCnt = new QLineEdit("10");//yhy0320
    m_pMeasureEnergyMeterRatio->setMinimumHeight(30);
    m_pMeasureEnergyMeterRatio->setMinimumWidth(80);

    m_pPulsewidthtest = new QPushButton("脉宽测试",this);
    m_pPulsewidthtest->setMinimumHeight(30);
    m_pPulsewidthtest->setMinimumWidth(80);//yhy0320

    m_pOneclicktest = new QPushButton("一键测试",this);
    m_pNumber = new QLineEdit("10");
    m_pOneclicktest->setMinimumHeight(30);
    m_pOneclicktest->setMinimumWidth(80);//yhy0320

    QWidget* pManalWorkflowWidget = new QWidget(this);
    QGridLayout* pManualWidgetLayout = new QGridLayout(pManalWorkflowWidget);
    pManualWidgetLayout->addWidget(pEnerglabel,0,0,1,1);
    pManualWidgetLayout->addWidget(m_pManualTestEngeryEdit,0,1,1,2);
    pManualWidgetLayout->addWidget(new QLabel("mJ"),0,3,1,1);
    pManualWidgetLayout->addWidget(new QLabel("设置点编号"),0,4,1,1);
    pManualWidgetLayout->addWidget(m_pCurrentPointEdit,0,5,1,2);
    pManualWidgetLayout->addWidget(m_pManualWorkflowListWidget,1,0,1,9);
    pManualWidgetLayout->addWidget(m_pManualTestBtn,2,0,1,1);
    pManualWidgetLayout->addWidget(m_pNumber,2,1,1,1);
    pManualWidgetLayout->addWidget(m_pAverArea,2,2,1,1);

//    //新光闸测试//yhy0320
//    pManualWidgetLayout->addWidget(m_pNewShutCntEdit,2,4,1,1);
//    pManualWidgetLayout->addWidget(m_pNewShutterBtn,2,5,1,1);

    pManualWidgetLayout->addWidget(m_pSampleStageMove2Zero,2,3,1,1);
//    pManualWidgetLayout->addWidget(m_pEnergyAverCnt,2,5,1,1);
    pManualWidgetLayout->addWidget(m_pMeasureEnergyMeterRatio,2,4,1,1);
    pManualWidgetLayout->addWidget(m_pPulsewidthtest,2,5,1,1);
    pManualWidgetLayout->addWidget(m_pOneclicktest,2,6,1,1);
    pManualWidgetLayout->addWidget(m_pManualLogListWidget,5,0,3,9);
    m_pStandbyStartTestBtn->setMaximumSize(QSize(150,40));
    m_pStartTestBtn->setMaximumSize(QSize(100,40));
    m_pSuspendBtn->setMaximumSize(QSize(100,40));
    m_pManualYesBtn->setMaximumSize(QSize(100,40));
    m_pManualNoBtn->setMaximumSize(QSize(100,40));
	QUIQss::setBtnQss(m_pStandbyStartTestBtn, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    QUIQss::setBtnQss(m_pStartTestBtn,"#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    QUIQss::setBtnQss(m_pSuspendBtn,"#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    QUIQss::setBtnQss(m_pManualYesBtn,"#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    QUIQss::setBtnQss(m_pManualNoBtn,"#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    //自动流程和手动流程
    m_pWorkflowTab = new QTabWidget();

    m_pWorkflowTab->addTab(pManalWorkflowWidget,"手动流程");
    m_pWorkflowTab->addTab(pAutoWorkflowWidget,"自动流程");
    m_pWorkflowTab->setCurrentIndex(0);


    playout->setRowStretch(0,1);
    playout->setRowStretch(1,2);
    playout->setRowStretch(2,1);


    playout->setColumnStretch(0,1);
    playout->setColumnStretch(1,2);


    playout->addWidget(m_pParamSetTab,0,0,1,1);
    playout->addWidget(m_pDevMonitorWidget,0,1,1,1);


    playout->addWidget(m_pStandbyWorkflowTab,1,0,1,1);
    playout->addWidget(m_pWorkflowTab,1,1,2,2);
    playout->addWidget(m_pStandbyStartTestBtn,2,0,1,1,Qt::AlignHCenter);

    m_pSuspendBtn->hide();

}

/*******************************************************************
**功能：创建或更新测试流程
**输入：
**输出：
**返回值：
*******************************************************************/
void QWorkFlowWidget::onUpdateWorkflow()
{
    m_pWorkFlowThread->m_tPointList.clear();
    for(int i = 0; i < m_pPointAreaWidget->m_tPointList.size();i++)
    {
        m_pWorkFlowThread->m_tPointList.append(m_pPointAreaWidget->m_tPointList[i]);
    }
	m_pWorkFlowThread->CreatStandbyWorkFlow(m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
    if(m_nMeasureType == M_MEASURETYPE_1On1)
    {
        m_pWorkFlowThread->create1On1WorkFlow(m_pWorkflowParamWidget->m_tworkflowParam.m_nCnt,m_pWorkflowParamWidget->m_tworkflowParam.m_lfMinimumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_lfMaximumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_lfEnergyIncream,m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType,m_pWorkflowParamWidget->m_tworkflowParam.m_dMinDeg,m_pWorkflowParamWidget->m_tworkflowParam.m_dMaxDeg);
    }

    if(m_nMeasureType == M_MEASURETYPE_SOn1)
    {
        m_pWorkFlowThread->CreateSOn1WorkFlow(m_pWorkflowParamWidget->m_tworkflowParam.m_nCnt,m_pWorkflowParamWidget->m_tworkflowParam.m_lfEnergyIncream,m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType,m_pWorkflowParamWidget->m_tworkflowParam.m_dWidth / m_pWorkflowParamWidget->m_tworkflowParam.m_dStep - 1);

    }

    if(m_nMeasureType == M_MEASURETYPE_ROn1)
    {
        m_pWorkFlowThread->CreateROn1WorkFlow(m_pWorkflowParamWidget->m_tworkflowParam.m_lfMinimumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_lfMaximumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_nEnergySectionNum,m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType,m_pWorkflowParamWidget->m_tworkflowParam.m_dMinDeg,m_pWorkflowParamWidget->m_tworkflowParam.m_dMaxDeg);
    }
    if(m_nMeasureType == M_MEASURETYPE_RASTER)
    {
        m_pWorkFlowThread->m_nRasterCycleCnt = 0;   //第一次调用Raster流程，将循环计数置零
        m_pWorkFlowThread->createRasterWorkFlow(m_pWorkflowParamWidget->m_tworkflowParam.m_nCnt,m_pWorkflowParamWidget->m_tworkflowParam.m_lfMinimumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_lfMaximumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_nEnergySectionNum,m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
    }

    m_pStandbyWorkflowTab->m_pWorkFlowList = &m_pWorkFlowThread->m_pStandbyFlow;
    m_pStandbyWorkflowTab->setWorkFlowList(M_MEASURETYPE_STANDBY);

    m_pMeasureWorkflowTab->m_pWorkFlowList  = &m_pWorkFlowThread->m_pWorkFlowList;
    m_pMeasureWorkflowTab->setWorkFlowList(m_nMeasureType);

    _pDevManagerCtrl->m_dManualArea = m_pWorkflowParamWidget->m_tworkflowParam.m_dManualArea;
    _pDevManagerCtrl->m_nStopPoint = -1;
    _pDevManagerCtrl->m_nStopTimes = -1;
    m_pDevMonitorWidget->m_pExpResultWidget->m_pExpResultPlot->clearGraphs();
}
/******************************
*功能:更新Raster流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onUpdateRaster()
{
    if(m_nMeasureType == M_MEASURETYPE_RASTER)
    {
        m_pWorkFlowThread->createRasterWorkFlow(m_pWorkflowParamWidget->m_tworkflowParam.m_nCnt,m_pWorkflowParamWidget->m_tworkflowParam.m_lfMinimumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_lfMaximumEnergy,m_pWorkflowParamWidget->m_tworkflowParam.m_nEnergySectionNum,m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
    }

    m_pMeasureWorkflowTab->m_pWorkFlowList  = &m_pWorkFlowThread->m_pWorkFlowList;
    m_pMeasureWorkflowTab->setWorkFlowList(m_nMeasureType);
}




//开始正式测试流程
void QWorkFlowWidget::OnStartTest()
{
    //确定是否输入了面积
    if (m_pWorkflowParamWidget->m_pManualArea->text().toDouble() == 0)
    {
        QMessageBox messageBox(QMessageBox::Warning,"提示","请在参数界面输入面积！");
        messageBox.setStandardButtons (QMessageBox::Ok);
        messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
        messageBox.exec ();
        return;
    }
    //确定本次实验的存储位置
    //元件名称文件夹
    QDir tem_NewSampleDir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName);
    if (!(tem_NewSampleDir.exists()))
    {
        tem_NewSampleDir.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName);
    }
    //实验编号文件夹
    QDir tem_NewSampleDir2(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" +m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber);
    if (!(tem_NewSampleDir2.exists()))
    {
        tem_NewSampleDir2.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber);
    }
    //数据文件夹
    tem_NewSampleDir2.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber + "/IDSIMAGE");
    tem_NewSampleDir2.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber + "/CCDIMG") ;
    tem_NewSampleDir2.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber + "/OutPut") ;
    tem_NewSampleDir2.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber + "/IDSIMAGE/DATA") ;//修改保存路径 by lg 20190611
    tem_NewSampleDir2.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber + "/PLOTIMG") ;
    tem_NewSampleDir2.mkdir(DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber + "/Doc") ;

    _pDevManagerCtrl->m_sSavePath = DATA_SAVE_PATH + m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName + "/" + m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber;

     QString str = m_pStartTestBtn->text();

     if(str == M_STR_START_MEASURE)
     {
         if(!checkWavelengthChange())//波长不一致  则需要提示 执行准备流程
         {
             QMessageBox messagebox(QMessageBox::Warning,"提示","计划使用的波长与当前实际波长不一致，请执行准备流程");
             messagebox.setStandardButtons (QMessageBox::Ok);
             messagebox.setButtonText (QMessageBox::Ok,QString("确 定"));
             messagebox.exec();
             return;

         }
         m_pSuspendBtn->show();
         m_pWorkFlowThread->m_nWorkFlowTYpe =2;
         m_pWorkFlowThread->m_nMeasureType = m_nMeasureType;
         m_pWorkFlowThread->start();
         QUIQss::setBtnQss(m_pStartTestBtn, "#E74C3C", "#FFFFFF", "#EC7064", "#FFF5E7", "#DC2D1A", "#F5A996");
         m_pStartTestBtn->setText(M_STR_STOP_MEASURE);

         //保存实验数据至数据库中
         QString sSize;
         if(m_pWorkflowParamWidget->m_tworkflowParam.m_nSizeType == M_SIZETYPE_CIRCL)
         {
             sSize = QString("半径:%1").arg(m_pWorkflowParamWidget->m_tworkflowParam.m_dRaidus);
         }
         else
              sSize = QString("h:%1,w:%2").arg(m_pWorkflowParamWidget->m_tworkflowParam.m_dHigh).arg(m_pWorkflowParamWidget->m_tworkflowParam.m_dWidth);

        emit signal_writeExpInfoToDb(m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,sSize,m_nMeasureType, m_pWorkflowParamWidget->m_tworkflowParam.m_nSizeType, \
                                     m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName, m_pWorkflowParamWidget->m_tworkflowParam.m_nCnt, m_pWorkflowParamWidget->m_tworkflowParam.m_lfMinimumEnergy, \
                                     m_pWorkflowParamWidget->m_tworkflowParam.m_lfEnergyIncream,m_pWorkflowParamWidget->m_tworkflowParam.m_nEnergySectionNum,m_pWorkflowParamWidget->m_tworkflowParam.m_nCnt);
     }
     else
     {
         if(m_pWorkFlowThread->isRunning())
         {
             m_pWorkFlowThread->m_bImmediateStop = true;
         }
     }

}


/******************************
*功能:raster重复流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::OnRepeatTest()
{
    QString str = m_pStartTestBtn->text();

    if(str == M_STR_START_MEASURE)
    {
        m_pSuspendBtn->show();
        m_pWorkFlowThread->m_nWorkFlowTYpe =2;
        m_pWorkFlowThread->m_nMeasureType = m_nMeasureType;
        m_pWorkFlowThread->start();
        QUIQss::setBtnQss(m_pStartTestBtn, "#E74C3C", "#FFFFFF", "#EC7064", "#FFF5E7", "#DC2D1A", "#F5A996");
        m_pStartTestBtn->setText(M_STR_STOP_MEASURE);
    }
    else
    {
        if(m_pWorkFlowThread->isRunning())
        {
            m_pWorkFlowThread->m_bImmediateStop = true;
        }
    }
}





void QWorkFlowWidget::OnSuspendTest()
{
    QString str = m_pSuspendBtn->text();

    if(str == M_STR_SUSPEND_MEASURE)
    {
        m_pWorkFlowThread->m_bSuspend = true;
        QUIQss::setBtnQss(m_pSuspendBtn, "#E74C3C", "#FFFFFF", "#EC7064", "#FFF5E7", "#DC2D1A", "#F5A996");
        m_pSuspendBtn->setText(M_STR_CONTINUE_MEASURE);
    }
    else
    {
        m_pWorkFlowThread->m_bSuspend = false;
        QUIQss::setBtnQss(m_pSuspendBtn,"#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
        m_pSuspendBtn->setText(M_STR_SUSPEND_MEASURE);
    }
}

void QWorkFlowWidget::OnStartStandbyTest()
{
    QString str = m_pStandbyStartTestBtn->text();
    if(str ==M_STR_START_STANDBY)
    {
        //m_bInitDev = false;
        m_nLastPointNum = 0;
        m_pWorkFlowThread->m_nWorkFlowTYpe =1;
        m_pWorkFlowThread->start();
        QUIQss::setBtnQss(m_pStandbyStartTestBtn, "#E74C3C", "#FFFFFF", "#EC7064", "#FFF5E7", "#DC2D1A", "#F5A996");
        m_pStandbyStartTestBtn->setText(M_STR_STOP_STANDBY);

        m_pStandbyWorkflowTab->setLabelTitle("准备流程",":/png/dev.png","rgb(0, 200, 0)");
    }
    else
    {
        if(m_pWorkFlowThread->isRunning())
        {
            m_pWorkFlowThread->m_bImmediateStop = true;
        }
    }
}

void QWorkFlowWidget::onThreadFinish()
{
    bool bSuccess = false;
    if(m_pWorkFlowThread->m_nWorkFlowTYpe == 1)
    {
        for(int i = 0; i < m_pWorkFlowThread->m_pStandbyFlow.size();i++)
        {
            for(int j = 0; j < m_pWorkFlowThread->m_pStandbyFlow.at(i)->m_pMoveWorkFlow.size();j++)
            {
                if(m_pWorkFlowThread->m_pStandbyFlow.at(i)->m_pMoveWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
                {
                    bSuccess = true;
                }
                else
                {
                    bSuccess = false;
                    break;
                }
            }
        }
        if(bSuccess == true)
        {
            m_pConfigProcess->m_tPlatformConfig.m_nCurrentWaveLength = m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType;
            m_pWorkflowParamWidget->getCurrentDeg(m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
            bool bRel = m_pConfigProcess->updateWaveLengthStatus();
            if(bRel == false)
            {
                QMessageBox messageBox(QMessageBox::Warning,"提示","写入平台配置文件失败!");
                messageBox.setStandardButtons (QMessageBox::Ok);
                messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
                messageBox.exec ();
            }

            //m_bInitDev = true;
        }
        m_pStandbyWorkflowTab->setLabelTitle("准备流程",":/png/dev.png","rgb(29,139,149)");
    }
    if(m_pWorkFlowThread->m_nWorkFlowTYpe == 3)
    {
        manualTestFinished();
    }
    if(m_pWorkFlowThread->m_nWorkFlowTYpe == 4)
    {
        AverAreaTestFinished();
    }
    if(m_pWorkFlowThread->m_nWorkFlowTYpe == 5)
    {
        AverEnergyMeterTestFinished();
    }
    if(m_pWorkFlowThread->m_nWorkFlowTYpe == 6)
    {
        AverPulsewidthTestFinished();//yhy0320
    }

    if (m_pWorkFlowThread->m_nWorkFlowTYpe == 2)
    {
        m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.clear();
        m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.clear();
        m_pDevMonitorWidget->m_pExpResultWidget->m_dEnergyAver.clear();
        m_pDevMonitorWidget->m_pExpResultWidget->m_dEnergyRMS.clear();
        emit signal_TestFinished(m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName,m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData,m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData,\
                                 m_pDevMonitorWidget->m_pExpResultWidget->m_dEnergyAver, m_pDevMonitorWidget->m_pExpResultWidget->m_dEnergyRMS, m_pWorkflowParamWidget->m_nMeasureType,\
                                 m_pDevMonitorWidget->m_pExpResultWidget->m_dAverFlux,m_pDevMonitorWidget->m_pExpResultWidget->m_dRMSFlux,_pDevManagerCtrl->m_sSavePath,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
        showResultInDialog();
        outputResultFile();
    }

	QUIQss::setBtnQss(m_pStandbyStartTestBtn, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    m_pStandbyStartTestBtn->setText(M_STR_START_STANDBY);

    QUIQss::setBtnQss(m_pStartTestBtn,"#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    m_pStartTestBtn->setText(M_STR_START_MEASURE);
    m_pSuspendBtn->hide();
    m_pManualYesBtn->hide();
    m_pManualNoBtn->hide();

    if (m_pWorkFlowThread->m_nWorkFlowTYpe == 2 && m_pWorkFlowThread->m_nMeasureType == M_MEASURETYPE_RASTER)
    {
        if (m_pWorkFlowThread->m_bCycleStatus == true)
        {
            onUpdateRaster();
            OnRepeatTest();
        }
    }
}

/******************************
*功能:输出结果数据文件
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::outputResultFile()
{
    QDir tem_NewSampleDir(_pDevManagerCtrl->m_sSavePath + QString("/OutPut"));
    if (!(tem_NewSampleDir.exists()))
    {
        tem_NewSampleDir.mkdir(_pDevManagerCtrl->m_sSavePath + QString("/OutPut"));
    }
    QFile file(_pDevManagerCtrl->m_sSavePath + QString("/OutPut/%1_%2.txt").arg(m_pWorkflowParamWidget->m_tworkflowParam.m_sSampleName).arg(m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber));
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        QMessageBox::warning(this,"file write","can't open",QMessageBox::Yes);

    }
    QTextStream in(&file);
    //实验编号
    in<<"======"<<m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber<<"======"<<"\n";

    //光斑面积平均和RMS
    in<<"Area Average: "<<m_dAreaAver<<"\n";
    in<<"Area RMS: "<<m_dAreaRMS<<"\n";

    //能量平均和RMS（每能量段）
    int tem_EnergyCnt = m_pDevMonitorWidget->m_pExpResultWidget->m_dEnergyAver.size();
    in<<"\nEnergySection No.      Energy Average      Energy RMS"<<"\n";
    for (int i=0; i<tem_EnergyCnt; i++)
    {
        in<<i+1<<"      "<<m_pDevMonitorWidget->m_pExpResultWidget->m_dEnergyAver.at(i)<<"      "<<m_pDevMonitorWidget->m_pExpResultWidget->m_dEnergyRMS.at(i)<<"\n";
    }

    //平均通量和损伤概率
    int tem_PointCnt = m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.size();
    in<<"\nAverage Flux      Defect"<<"\n";
    for (int i=0; i<tem_PointCnt; i++)
    {
        in<<m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.at(i)<<"      "<<m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.at(i)<<"\n";
    }
    file.close();
    //;
}

bool QWorkFlowWidget::checkWavelengthChange()
{
    int nSelectWavelength = m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType;
    if(nSelectWavelength != m_pConfigProcess->m_tPlatformConfig.m_nCurrentWaveLength)
    {
        return false;
    }

    return true;
}

/******************************
*功能:展示结果页面
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::showResultInDialog()
{
    if (!(m_pResultDlg == NULL))
    {
        delete m_pResultDlg;
        m_pResultDlg = NULL;
    }
    m_pResultDlg = new QDialog(this);
    m_pResultDlg->setMinimumSize(QSize(300,400));
    m_stLineEditList.clear();
    int tem_nDataSize = m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.size();
    QVBoxLayout *tem_vlayout = new QVBoxLayout();
    for (int i=0; i<tem_nDataSize; i++)
    {
        QLineEdit *tem_pLineEdit = new QLineEdit(m_pResultDlg);
        m_stLineEditList.append(tem_pLineEdit);
        if (m_pWorkflowParamWidget->m_nMeasureType == M_MEASURETYPE_1On1)
        {
            tem_pLineEdit->setText(QString("平均通量：%1  损伤概率：%2").arg(m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.at(i)).arg(m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.at(i)));
        }
        else if (m_pWorkflowParamWidget->m_nMeasureType == M_MEASURETYPE_ROn1)
        {
            tem_pLineEdit->setText(QString("临界通量：%1  点号：%2").arg(m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.at(i)).arg(m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.at(i)));
        }
        tem_vlayout->addWidget(tem_pLineEdit);
    }
    m_pAlterBtn = new QPushButton("确认修改数据");
    connect(m_pAlterBtn,&QPushButton::clicked,this,&QWorkFlowWidget::onAlterData);
    tem_vlayout->addWidget(m_pAlterBtn);
    m_pResultDlg->setLayout(tem_vlayout);
    m_pResultDlg->setModal(true);
    m_pResultDlg->show();

}

void QWorkFlowWidget::onAlterData()
{
    QList<int> temp_indexlist;
//    QVector<float> tem_pFluxData;
//    QVector<float> tem_pDefeData;
    for (int i=0; i<m_stLineEditList.size(); i++)
    {
        if (m_stLineEditList.at(i)->text() == "")
        {
            temp_indexlist.append(i);
//            tem_pFluxData.append(m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.at(i));
//            tem_pDefeData.append(m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.at(i));
        }
    }

    for(int i = m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.size()-1;i >= 0; i--)
    {
        if(temp_indexlist.indexOf(i) != -1)
        {
            m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.removeAt(i);
            m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.removeAt(i);
        }
    }
//    m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.clear();
//    m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.clear();
//    for (int i=0; i<tem_pFluxData.size(); i++)
//    {
//        m_pDevMonitorWidget->m_pExpResultWidget->m_pFluxData.append(tem_pFluxData.at(i));
//        m_pDevMonitorWidget->m_pExpResultWidget->m_pDefeData.append(tem_pDefeData.at(i));
//    }
    m_pDevMonitorWidget->m_pExpResultWidget->onShowResult(m_pWorkflowParamWidget->m_nMeasureType,_pDevManagerCtrl->m_sSavePath);
    m_pResultDlg->close();
}


/******************************
*功能:测量分光比
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onMeasureEnergyRatio()
{
    int tem_nCnt = m_pNumber->text().toInt();//yhy0320
    m_tEnergyDevData[0].clear();
    m_tEnergyDevData[1].clear();

    //创建测试流程
    showLog("正在生成测试流程..");

    m_pWorkFlowThread->createAverEnergyRatioWorkFlow(tem_nCnt,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
    showLog("生成测试流程完毕!");
    //执行测试流程
    m_pWorkFlowThread->m_nWorkFlowTYpe =5;
    m_pWorkFlowThread->start();
    m_pAverArea->setEnabled(false);
    m_pSampleStageMove2Zero->setEnabled(false);
    m_pStandbyStartTestBtn->setEnabled(false);
    m_pStartTestBtn->setEnabled(false);
    showLog("开始执行分光比测试流程,请稍等..");
}

//手动通量测试
void QWorkFlowWidget::onManualTest()
{

    bool bOk = false;
    double dStep = m_pWorkflowParamWidget->m_tworkflowParam.m_dStep;
    int nNum = m_pCurrentPointEdit->toPlainText().toInt(&bOk);
    double dMoveStep = (nNum - m_nLastPointNum)*dStep;
    double lfEnergy = m_pManualTestEngeryEdit->toPlainText().toDouble(&bOk);

    //创建测试流程
    showLog("正在生成测试流程..");

    int nRowCnt = m_pManualWorkflowListWidget->rowCount();
    m_pManualWorkflowListWidget->insertRow(nRowCnt);
    QTableWidgetItem* pItem = new QTableWidgetItem(QString("%1").arg(nNum));
    m_pManualWorkflowListWidget->setItem(nRowCnt,0,pItem);
    QTableWidgetItem* pItem1 = new QTableWidgetItem(QString("%1").arg(lfEnergy));
    m_pManualWorkflowListWidget->setItem(nRowCnt,1,pItem1);

    m_pWorkFlowThread->create1On1ManualWorkFlow(lfEnergy,dMoveStep,m_pWorkflowParamWidget->m_tworkflowParam.m_sExpNumber,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
    showLog("生成测试流程完毕!");
    //执行测试流程
    m_pWorkFlowThread->m_nWorkFlowTYpe =3;
    m_pWorkFlowThread->start();

    m_pManualTestBtn->setEnabled(false);
    m_pAverArea->setEnabled(false);
    m_pSampleStageMove2Zero->setEnabled(false);
    m_pStandbyStartTestBtn->setEnabled(false);
    m_pStartTestBtn->setEnabled(false);
    showLog("开始执行测试流程,请稍等..");

}
/******************************
*功能:测试脉宽
*输入:
*输出:
*返回值:
*修改记录:yhy0320
*******************************/
void QWorkFlowWidget::onNewPulsewidthtest()
{
    int tem_nCnt = m_pNumber->text().toInt();
    showLog("正在生成脉宽测试流程..");
    m_pWorkFlowThread->createPulsewidthWorkFlow(tem_nCnt,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
    showLog("生成脉宽测试流程完毕!");

    m_pWorkFlowThread->m_nWorkFlowTYpe = 6;
    m_pWorkFlowThread->start();
    m_pAverArea->setEnabled(false);
    m_pSampleStageMove2Zero->setEnabled(false);
    m_pStandbyStartTestBtn->setEnabled(false);
    m_pStartTestBtn->setEnabled(false);
    showLog("开始执行脉宽测试流程,请稍等..");
}

/******************************
*功能:一键测试
*输入:
*输出:
*返回值:
*修改记录:yhy0320
*******************************/
void QWorkFlowWidget::onNewOneclicktest()
{

}

/******************************
*功能:测试平均光斑面积
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onAverArea()
{
    int tem_nCnt = m_pNumber->text().toInt();//yhy0320
    m_dAreaRecord = 0.0;
    m_dAreaRecord_e2 = 0.0;
    m_dAreaSquareRecord = 0.0;
    m_tAreaVector.clear();
    m_tAreaVector_e2.clear();

    //创建测试流程
    showLog("正在生成测试流程..");

    m_pWorkFlowThread->createAverAreaManualWorkFlow(tem_nCnt,m_pWorkflowParamWidget->m_tworkflowParam.m_nWaveLengthType);
    showLog("生成测试流程完毕!");
    //执行测试流程
    m_pWorkFlowThread->m_nWorkFlowTYpe =4;
    m_pWorkFlowThread->start();
    m_pAverArea->setEnabled(false);
    m_pSampleStageMove2Zero->setEnabled(false);
    m_pStandbyStartTestBtn->setEnabled(false);
    m_pStartTestBtn->setEnabled(false);
    showLog("开始执行平均面积测试流程,请稍等..");
}


/******************************
*功能:新光闸测试
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onNewShutterTest()
{
    int tem_cnt = m_pNewShutCntEdit->text().toInt();
    //创建测试流程
    showLog("正在生成新光闸测试流程..");

    m_pWorkFlowThread->createNewShutterTestWorkFlow(tem_cnt);
    showLog("生成新光闸测试流程完毕!");
    m_pWorkFlowThread->m_nWorkFlowTYpe =3;
    m_pWorkFlowThread->start();
    showLog("开始执行测试流程,请稍等..");
}


/******************************
*功能:新光闸测试记录
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onNewShutterLog(int nCntNum, int result)
{
    if (result == 1)
    {
        //showLog(QString("第%1发测试：成功。").arg(nCntNum));
    }
    else if (result == 2)
    {
        showLog(QString("光闸第%1发测试：成功----能量计。").arg(nCntNum));
    }
    else if (result == 3)
    {
        showLog(QString("光闸第%1发测试：成功----多次发送。").arg(nCntNum));
    }
    else
    {
        showLog(QString("光闸第%1发测试：失败====================").arg(nCntNum));
    }
}


/******************************
*功能:样品台归零
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onSampleStage2Zero()
{
    //创建测试流程
    showLog("正在生成测试流程..");

    m_pWorkFlowThread->createSampleStage2ZeroWorkFlow();
    showLog("生成测试流程完毕!");
    //执行测试流程
    m_pWorkFlowThread->m_nWorkFlowTYpe =3;
    m_pWorkFlowThread->start();
    m_pAverArea->setEnabled(false);
    m_pSampleStageMove2Zero->setEnabled(false);
    m_pStandbyStartTestBtn->setEnabled(false);
    m_pStartTestBtn->setEnabled(false);
    showLog("开始执行样品台归零流程,请稍等..");
}
/******************************
*功能:累加多次采集的光斑面积
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onAreaRecord(double dArea,double dArea_e2)
{
    if (!(dArea == 0 && dArea_e2 == 0))
    {
        m_dAreaRecord = m_dAreaRecord + dArea;
        m_tAreaVector.push_back(dArea);
        m_dAreaRecord_e2 = m_dAreaRecord_e2 + dArea_e2;
        m_tAreaVector_e2.push_back(dArea_e2);
    }
    showLog(QString("第%1点: ").arg(m_tAreaVector.count()));
    showLog(QString("方法一：") + QString::number(dArea) + "cm^2");
    showLog(QString("方法二：") + QString::number(dArea_e2) + "cm^2");
}

/******************************
*功能:记录能量计数据
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onEnergyDataRecord(int nIndex, QString sEnergyData, QVariant tTaskExpInfo)
{
    m_tEnergyDevData[nIndex].push_back(sEnergyData.toDouble());
}

/******************************
*功能:测量脉宽结束
*输入:
*输出:
*返回值:
*修改记录:yhy0320
*******************************/
void QWorkFlowWidget::AverPulsewidthTestFinished()
{
    QList<float> Pulsewidths;
    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
    QString dsn = QString::fromLocal8Bit("QtLocalDSN");
    db.setDatabaseName(dsn);
    db.setHostName("local");  //数据库主机名
    db.setPort(1433);
    db.setUserName("sa");
    db.setPassword("123456");
    float Pulsewidth = 0;
    if(!db.open())
    {
        qDebug("=== %s",qPrintable(db.lastError().text()));
        QMessageBox msgBox;
        msgBox.setWindowTitle("連接失败");
        msgBox.setText("!db.open()");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

    }else
    {
        int tem_nCnt = m_pNumber->text().toInt();
        QString sSql = QString("select top %1 * from T_OSCI_DATA order by DateTime desc").arg(tem_nCnt);
        QSqlQuery sqlQuery(db);
        bool bOk = sqlQuery.exec(sSql);

        //
        if (bOk==true)
        {

        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("数据查询失敗");
            msgBox.setText(sqlQuery.lastError().text());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        while(sqlQuery.next())
        {
            Pulsewidths.append(sqlQuery.value(6).toFloat());
        }
        float sum_Pulsewidth=0;
        QListIterator<float> i(Pulsewidths);
        for(;i.hasNext();)
        {
            sum_Pulsewidth = sum_Pulsewidth + i.next();
        }
        Pulsewidth = sum_Pulsewidth/(float)tem_nCnt;

        QString sSql1 = QString("insert into T_PULSEWIDTH_DATA (PulseWidth,time) values(%1,?)").arg(Pulsewidth);
        QSqlQuery sqlQuery1(db);
        sqlQuery1.prepare(sSql1);
        QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        sqlQuery1.addBindValue(CurrentTime);
        bool bOk1 = sqlQuery1.exec();

        //
        if (bOk1==true)
        {

        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("数据写入失敗");
            msgBox.setText(sqlQuery.lastError().text());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        //wzj0324
         db.close();

    }
    bool bSuccess = false;
    for(int i = 0; i < m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.size();i++)
    {
        //运动
        for(int j = 0; j < m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pMoveWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pMoveWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行运动操作成功！");
        }
        else
        {
//            showLog("执行运动操作失败！");
            break;
        }

        //设置
        for(int j = 0; j < m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pSetWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pSetWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行设置操作成功！");
        }
        else
        {
            showLog("执行设置操作失败！");
            break;
        }
        //采集
        for(int j = 0; j < m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pCapWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pCapWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行采集操作成功！");
        }
        else
        {
            showLog("执行采集操作失败！");
            break;
        }
        //数据处理
        for(int j = 0; j < m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pDataProcessWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pPulsewidthWorkFlowlist.at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行数据处理操作成功！");
        }
        else
        {
            showLog("执行数据处理操作失败！");
            break;
        }
    }
    showLog(QString("测量脉宽为： %1 ns").arg(Pulsewidth));
    m_pManualTestBtn->setEnabled(true);
    m_pAverArea->setEnabled(true);
    m_pSampleStageMove2Zero->setEnabled(true);
    m_pStandbyStartTestBtn->setEnabled(true);
    m_pStartTestBtn->setEnabled(true);
    m_pMeasureEnergyMeterRatio->setEnabled(true);

}

/******************************
*功能:平均光斑面积结束
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::AverAreaTestFinished()
{
    m_dAreaAver = m_dAreaRecord/(m_tAreaVector.count());
    m_dAreaAver_e2 = m_dAreaRecord_e2/(m_tAreaVector.count());
    for (int i=0; i<m_tAreaVector.size(); i++)
    {
        m_dAreaSquareRecord += qPow((m_tAreaVector.at(i) - m_dAreaAver),2);
    }
    for (int i=0; i<m_tAreaVector_e2.size(); i++)
    {
        m_dAreaSquareRecord_e2 += qPow((m_tAreaVector_e2.at(i) - m_dAreaAver_e2),2);
    }
    m_dAreaRMS = qSqrt(m_dAreaSquareRecord/(m_tAreaVector.count()));
    m_dAreaRMS_e2 = qSqrt(m_dAreaSquareRecord_e2/(m_tAreaVector.count()));
    m_pWorkflowParamWidget->m_pManualArea->setText(QString("%1").arg(m_dAreaAver));
    bool bSuccess = false;
    for(int i = 0; i < m_pWorkFlowThread->m_pAverAreaWorkFlowlist.size();i++)
    {
        //运动
        for(int j = 0; j < m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pMoveWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pMoveWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行运动操作成功！");
        }
        else
        {
            showLog("执行运动操作失败！");
            break;
        }

        //设置
        for(int j = 0; j < m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pSetWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pSetWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行设置操作成功！");
        }
        else
        {
            showLog("执行设置操作失败！");
            break;
        }
        //采集
        for(int j = 0; j < m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pCapWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pCapWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行采集操作成功！");
        }
        else
        {
            showLog("执行采集操作失败！");
            break;
        }
        //数据处理
        for(int j = 0; j < m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pDataProcessWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverAreaWorkFlowlist.at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行数据处理操作成功！");
        }
        else
        {
            showLog("执行数据处理操作失败！");
            break;
        }
    }
//    showLog(QString(" %1").arg(m_tAreaVector.count()));
    showLog(QString("平均光斑面积为： %1cm^2").arg(m_dAreaAver));
    showLog(QString("光斑面积RMS为： %1cm^2").arg(m_dAreaRMS));
    showLog(QString("(等效光斑法)平均光斑面积为： %1cm^2").arg(m_dAreaAver_e2));
    showLog(QString("(等效光斑法)光斑面积RMS为： %1cm^2").arg(m_dAreaRMS_e2));
    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
    QString dsn = QString::fromLocal8Bit("QtLocalDSN");
    db.setDatabaseName(dsn);
    db.setHostName("local");  //数据库主机名
    db.setPort(1433);
    db.setUserName("sa");
    db.setPassword("123456");
    float Pulsewidth = 0;
    if(!db.open())
    {
        qDebug("=== %s",qPrintable(db.lastError().text()));
        QMessageBox msgBox;
        msgBox.setWindowTitle("連接失败");
        msgBox.setText("!db.open()");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

    }else
    {
        int tem_nCnt = m_pNumber->text().toInt();
        QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString sSql = QString("insert into T_AREA_DATA (AreaAver,AreaRMS,AreaAver_e2,AreaRMS_e2,time) values(%1,%2,%3,%4,?)").arg(m_dAreaAver).arg(m_dAreaRMS).arg(m_dAreaAver_e2).arg(m_dAreaRMS_e2);
        QSqlQuery sqlQuery(db);
        sqlQuery.prepare(sSql);
        sqlQuery.addBindValue(CurrentTime);
        bool bOk = sqlQuery.exec();

        //
        if (bOk==true)
        {

        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("操作失敗");
            msgBox.setText(sqlQuery.lastError().text());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        //wzj0324
         db.close();

    }

    m_pManualTestBtn->setEnabled(true);
    m_pAverArea->setEnabled(true);
    m_pSampleStageMove2Zero->setEnabled(true);
    m_pStandbyStartTestBtn->setEnabled(true);
    m_pStartTestBtn->setEnabled(true);
    m_pMeasureEnergyMeterRatio->setEnabled(true);
}

/******************************
*功能:平均分光比测试结束
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::AverEnergyMeterTestFinished()
{
    m_dEnergyRatio = 0.0;
    m_dEnergyRatioRMS = 0.0;

    if (!(m_tEnergyDevData[0].size() == m_tEnergyDevData[1].size()))
    {
        showLog("主能量计和监视能量计采集次数不一致，平均分光比测试失败！");
        return;
    }
    else
    {
        int nNumCnt = 0;
        for (int i=0; i<m_tEnergyDevData[0].size(); i++)
        {

            if (m_tEnergyDevData[0].at(i) == 0 || m_tEnergyDevData[1].at(i) == 0)
            {
                continue;
            }
            else
            {
                m_dEnergyRatio += m_tEnergyDevData[1].at(i)/m_tEnergyDevData[0].at(i);
                nNumCnt++;
            }

        }
        if(nNumCnt != 0)
        {
            //2020dong
            m_dEnergyRatio = m_dEnergyRatio/nNumCnt;
        }


        nNumCnt = 0;
        //RMS
        for (int i=0 ;i<m_tEnergyDevData[0].size(); i++)
        {
            if (m_tEnergyDevData[0].at(i) == 0 || m_tEnergyDevData[1].at(i) == 0)
            {
                continue;
            }
            else
            {
                m_dEnergyRatioRMS += qPow((m_tEnergyDevData[1].at(i)/m_tEnergyDevData[0].at(i) - m_dEnergyRatio),2);
                nNumCnt++;
            }

            //;
        }
        if(nNumCnt != 0)
        {
            //2020dong
            m_dEnergyRatioRMS = qSqrt(m_dEnergyRatioRMS/nNumCnt);
        }

    }
    bool bSuccess = false;
    for(int i = 0; i < m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.size();i++)
    {
        //运动
        for(int j = 0; j < m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pMoveWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pMoveWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行运动操作成功！");
        }
        else
        {
            showLog("执行运动操作失败！");
            break;
        }

        //设置
        for(int j = 0; j < m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pSetWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pSetWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行设置操作成功！");
        }
        else
        {
            showLog("执行设置操作失败！");
            break;
        }
        //采集
        for(int j = 0; j < m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pCapWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pCapWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行采集操作成功！");
        }
        else
        {
            showLog("执行采集操作失败！");
            break;
        }
        //数据处理
        for(int j = 0; j < m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pDataProcessWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pAverEnergyMeterWorkFlowlist.at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行数据处理操作成功！");
        }
        else
        {
            showLog("执行数据处理操作失败！");
            break;
        }
    }
    showLog(QString("平均分光比为： %1").arg(m_dEnergyRatio));
    showLog(QString("分光比RMS为： %1").arg(m_dEnergyRatioRMS));

    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
    QString dsn = QString::fromLocal8Bit("QtLocalDSN");
    db.setDatabaseName(dsn);
    db.setHostName("local");  //数据库主机名
    db.setPort(1433);
    db.setUserName("sa");
    db.setPassword("123456");
    float Pulsewidth = 0;
    if(!db.open())
    {
        qDebug("=== %s",qPrintable(db.lastError().text()));
        QMessageBox msgBox;
        msgBox.setWindowTitle("連接失败");
        msgBox.setText("!db.open()");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

    }else
    {
        int tem_nCnt = m_pNumber->text().toInt();
        QString  CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString sSql = QString("insert into T_ENERGYRATIO_DATA (EnergyRatio,EnergyRatioRMS,time) values(%1,%2,?)").arg(m_dEnergyRatio).arg(m_dEnergyRatioRMS);
        QSqlQuery sqlQuery(db);
        sqlQuery.prepare(sSql);
        sqlQuery.addBindValue(CurrentTime);
        bool bOk = sqlQuery.exec();

        //
        if (bOk==true)
        {

        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("数据存储失敗");
            msgBox.setText(sqlQuery.lastError().text());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        //wzj0324
         db.close();

    }

    m_pManualTestBtn->setEnabled(true);
    m_pAverArea->setEnabled(true);
    m_pSampleStageMove2Zero->setEnabled(true);
    m_pStandbyStartTestBtn->setEnabled(true);
    m_pStartTestBtn->setEnabled(true);
    m_pMeasureEnergyMeterRatio->setEnabled(true);
}

//手动测试结束
void QWorkFlowWidget::manualTestFinished()
{
    bool bSuccess = false;
    for(int i = 0; i < m_pWorkFlowThread->m_pManualWorkFlowlist.size();i++)
    {
        for(int j = 0; j < m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pMoveWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pMoveWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行运动操作成功！");
        }
        else
        {
            showLog("执行运动操作失败！");
            break;
        }

        for(int j = 0; j < m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pSetWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pSetWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行设置操作成功！");
        }
        else
        {
            showLog("执行设置操作失败！");
            break;
        }

        for(int j = 0; j < m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pCapWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pCapWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行采集操作成功！");
        }
        else
        {
            showLog("执行采集操作失败！");
            break;
        }

        for(int j = 0; j < m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pDataProcessWorkFlow.size();j++)
        {
            if(m_pWorkFlowThread->m_pManualWorkFlowlist.at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus == M_STEP_STATUS_SUCCESS)
            {
                bSuccess = true;
            }
            else
            {
                bSuccess = false;
                break;
            }
        }
        if(bSuccess == true)
        {
            showLog("执行数据处理操作成功！");
        }
        else
        {
            showLog("执行数据处理操作失败！");
            break;
        }
    }


    m_pManualTestBtn->setEnabled(true);
    m_pAverArea->setEnabled(true);
    m_pSampleStageMove2Zero->setEnabled(true);
    m_pStandbyStartTestBtn->setEnabled(true);
    m_pStartTestBtn->setEnabled(true);
    m_pMeasureEnergyMeterRatio->setEnabled(true);
}

/*******************************************************************
**功能：显示日志信息
**输入：
**输出：
**返回值：
*******************************************************************/
void QWorkFlowWidget::showLog(QString slog)
{
    QTime time = QTime::currentTime();

    QString str = time.toString("hh:mm:ss") +"    "+ slog;


    m_pManualLogListWidget->addItem(str);
    m_pManualLogListWidget->setCurrentRow(m_pManualLogListWidget->count()-1);
    m_pManualLogListWidget->repaint();
}

//显示通量数据
void QWorkFlowWidget::onShowFluxStatus(QVariant tTaskExpInfo,float lfdata)
{
    tExpInfo TaskExpInfo = tTaskExpInfo.value<tExpInfo>();
    if(m_pWorkFlowThread->m_nWorkFlowTYpe != 3 || TaskExpInfo.m_nMeasureType != m_nMeasureType)
    {
        return;
    }
    int nRowCnt = m_pManualWorkflowListWidget->rowCount();

    QTableWidgetItem* pItem = new QTableWidgetItem(QString("%1").arg(lfdata));
    m_pManualWorkflowListWidget->setItem(nRowCnt-1,2,pItem);
}

//显示有无损伤
void QWorkFlowWidget::onShowDefectStatus(QVariant tTaskExpInfo,bool bStatus)
{
    tExpInfo TaskExpInfo = tTaskExpInfo.value<tExpInfo>();
    if(m_pWorkFlowThread->m_nWorkFlowTYpe != 3 || TaskExpInfo.m_nMeasureType != m_nMeasureType)
    {
        return;
    }
    int nRowCnt = m_pManualWorkflowListWidget->rowCount();
    QTableWidgetItem* pItem = new QTableWidgetItem();
    if(bStatus == true)
    {
        pItem->setText("有");
    }
    else
    {
       pItem->setText("无");
    }

    m_pManualWorkflowListWidget->setItem(nRowCnt-1,3,pItem);
}

/******************************
*功能:人工介入
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onManualEnterAction()
{
    m_pWorkFlowThread->m_bManualAlter = true;
    m_pManualYesBtn->show();
    m_pManualNoBtn->show();
}


/******************************
*功能:用户不更改当前点状态
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onManualYesBtnClicked()
{
    m_pWorkFlowThread->m_bManualAlter = false;
    m_pManualYesBtn->hide();
    m_pManualNoBtn->hide();
    //update database
}

/******************************
*功能:用户更改当前点状态为未损伤
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowWidget::onManualNoBtnClicked(QVariant tTaskExpInfo)
{
    //人工判断没有损伤，将数据处理状态进行更改
    _pDevManagerCtrl->m_nStopPoint = -1;
    _pDevManagerCtrl->m_nStopTimes = -1;
    _pDevManagerCtrl->m_bIsRasterDefect = false;
    _pDevManagerCtrl->m_bManualChangeResult = true;
    m_pWorkFlowThread->m_bManualAlter = false;
    m_pManualYesBtn->hide();
    m_pManualNoBtn->hide();
    //update database
//    emit singal_showDefectStatus(var,true);
}
