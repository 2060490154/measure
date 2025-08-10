#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include "qerrcodeprocess.h"
#include "QCommon/qexcel.h"
#include "qquerywidget.h"

extern QSplashScreen* g_psplashScreen;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("固定通量寿命测试仿真系统");

    setWindowState(Qt::WindowMaximized);//设置最大化


    qDebug()<<"\n\n"<<"=========================================固定通量阈值检测系统"+QDateTime::currentDateTime().toString("yyMMddhhmmss")+"=========================================";
    qDebug()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,"=========================================固定通量阈值检测系统"+QDateTime::currentDateTime().toString("yyMMddhhmmss")+"=========================================");

    g_psplashScreen->showMessage(QObject::tr("连接数据库，获取配置信息..."),Qt::AlignBottom | Qt::AlignHCenter, Qt::white);//显示信息

    //数据库初始化
    m_pDbData = new QDBData();
    if(!m_pDbData->connectDataBase())
    {
        QMessageBox::warning(this,"错误"," 连接数据库错误");
        return;
    }
    m_pDbData->getAllDevConfigInfo();

    m_pConfigProcess = new QConfigProcess();
    bool bRel = m_pConfigProcess->getPlatformConfig();
    if(bRel == false)
    {
        QMessageBox messageBox(QMessageBox::Warning,"提示","读取平台配置文件错误!");
        messageBox.setStandardButtons (QMessageBox::Ok);
        messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
        messageBox.exec ();
    }

    bRel = m_pConfigProcess->getDevConfig();
    if(bRel == false)
    {
        QMessageBox messageBox(QMessageBox::Warning,"提示","获取控制设备配置信息错误!");
        messageBox.setStandardButtons (QMessageBox::Ok);
        messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
        messageBox.exec ();
    }
    //Sleep(1000);
    g_psplashScreen->showMessage(QObject::tr("正在搜索硬件设备..."),Qt::AlignBottom | Qt::AlignHCenter, Qt::white);//显示信息

    //多视图
    m_pUserViews = new QStackedWidget(this);
    //导航栏
    m_pNavWidget = new QNavWidget();
    m_pNavWidget->m_pUserViews = m_pUserViews;
    //测量流程和设备控制
    m_pDevManagerCtrl = new QDevManagerCtrl(&m_pConfigProcess->m_tDevConfigParam);//配置文件设备控制
    m_pDevManagerCtrl->m_pAllDevConfigInfo = &m_pDbData->m_tAllDevConfigInfo;//数据库配置信息
    m_pDevManagerWidget = new QDevManagerWidget(m_pDevManagerCtrl);//设备视图
    qDebug() << "1";
    m_p1On1Widget = new QWorkFlowWidget(M_MEASURETYPE_1On1,m_pDevManagerCtrl,m_pConfigProcess,this);
    qDebug() << "2";
    m_pSOn1Widget = new QWorkFlowWidget(M_MEASURETYPE_SOn1,m_pDevManagerCtrl,m_pConfigProcess,this);
    qDebug() << "3";
    m_pROn1Widget = new QWorkFlowWidget(M_MEASURETYPE_ROn1,m_pDevManagerCtrl,m_pConfigProcess,this);
    qDebug() << "4";
    m_pRasterWidget = new QWorkFlowWidget(M_MEASURETYPE_RASTER,m_pDevManagerCtrl,m_pConfigProcess,this);
    qDebug() << "5";
    //数据库写入 信号 槽连接
//    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_capIDSImageBuff,m_pDbData,&QDBData::onWirteIdsImageToDb);//写入图片信息
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_wirteOsciDataToDb,m_pDbData,&QDBData::onWirteOsciDataToDb);//写入示波器数据
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_capKxccdImageBuff,m_pDbData,&QDBData::onWirteKxccdImageToDb);//写入科学CCD数据
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_updateEnergyData,m_pDbData,&QDBData::onWriteEnergyMeterDataToDb);//写入能量计数据
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_updateEnergyData,m_p1On1Widget,&QWorkFlowWidget::onEnergyDataRecord);//能量计数据保存，用于求平均分光比
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_updateEnergyData,m_pROn1Widget,&QWorkFlowWidget::onEnergyDataRecord);//能量计数据保存，用于求平均分光比
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::singal_showDefectStatus,m_pDbData,&QDBData::onWriteDefectStatus);//更新IDS数据库有无损伤信息
    connect(m_pROn1Widget->m_pWorkFlowThread,&QWorkFlowThread::signal_UserChangeResult,m_pDbData,&QDBData::onWriteDefectStatus);//用户更改IDS数据库有无损伤信息
    connect(m_p1On1Widget->m_pWorkFlowThread,&QWorkFlowThread::signal_UserChangeResult,m_pDbData,&QDBData::onWriteDefectStatus);//用户更改IDS数据库有无损伤信息
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::singal_showCalcFluxStatus,m_pDbData,&QDBData::onWriteFluxData);//更新能量数据库通量信息
    connect(m_p1On1Widget,&QWorkFlowWidget::signal_writeExpInfoToDb,m_pDbData,&QDBData::onWriteExpInfoToDb);
    connect(m_pROn1Widget,&QWorkFlowWidget::signal_writeExpInfoToDb,m_pDbData,&QDBData::onWriteExpInfoToDb);
    connect(m_pSOn1Widget,&QWorkFlowWidget::signal_writeExpInfoToDb,m_pDbData,&QDBData::onWriteExpInfoToDb);
    connect(m_pRasterWidget,&QWorkFlowWidget::signal_writeExpInfoToDb,m_pDbData,&QDBData::onWriteExpInfoToDb);
    //ytry //add the energy meter data into the db.
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_showCheckStatus,this,&MainWindow::ShowCheckStatusBox);
    connect(m_p1On1Widget,&QWorkFlowWidget::signal_TestFinished,m_pDbData,&QDBData::onTestFinished);
    connect(m_pROn1Widget,&QWorkFlowWidget::signal_TestFinished,m_pDbData,&QDBData::onTestFinished);
    connect(m_pSOn1Widget,&QWorkFlowWidget::signal_TestFinished,m_pDbData,&QDBData::onTestFinished);
    connect(m_pRasterWidget,&QWorkFlowWidget::signal_TestFinished,m_pDbData,&QDBData::onTestFinished);



    //系统状态控制视图
    //m_pSystemMove2SafeWidget = new QStystemMove2SafeWidget();
    m_pSystemParamWidget = new QSystemParamWidget(m_pConfigProcess);


    //实验信息汇总视图
    m_pExpInfoWidget = new QExpInfoWidget();


    //ydc21427
    //data query
    m_pQueryWidget = new QQueryWidget();

    connect(m_pExpInfoWidget,&QExpInfoWidget::signal_getExpInfoFromdb,m_pDbData,&QDBData::onGetExpInfo4Show);   //从数据库获取实验信息，展示在报表生成界面
    connect(m_pExpInfoWidget,&QExpInfoWidget::signal_SaveNewDevInfo2db,m_pDbData,&QDBData::onSaveNewDevInfo2Db);    //  将新添加的设备信息加入数据库中
    connect(m_pExpInfoWidget,&QExpInfoWidget::signal_SaveNewClientInfo2db,m_pDbData,&QDBData::onSaveNewClientInfo2Db);
    connect(m_pExpInfoWidget,&QExpInfoWidget::signal_GetDevInFromdb,m_pDbData,&QDBData::onGetDevInfo4Show); //  从数据库中获取设备信息
    connect(m_pExpInfoWidget,&QExpInfoWidget::signal_GetClientInfoFromDb,m_pDbData,&QDBData::onGetClientInfo);
    connect(m_p1On1Widget->m_pDevMonitorWidget->m_pExpResultWidget,&QExpResultWidget::signal_FluxThreshHold,m_pExpInfoWidget,&QExpInfoWidget::on_GetFluxThreshHold);
    connect(m_pROn1Widget->m_pDevMonitorWidget->m_pExpResultWidget,&QExpResultWidget::signal_FluxThreshHold,m_pExpInfoWidget,&QExpInfoWidget::on_GetFluxThreshHold);
    connect(m_pSOn1Widget->m_pDevMonitorWidget->m_pExpResultWidget,&QExpResultWidget::signal_FluxThreshHold,m_pExpInfoWidget,&QExpInfoWidget::on_GetFluxThreshHold);
    connect(m_pRasterWidget->m_pDevMonitorWidget->m_pExpResultWidget,&QExpResultWidget::signal_FluxThreshHold,m_pExpInfoWidget,&QExpInfoWidget::on_GetFluxThreshHold);

    connect(m_p1On1Widget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateExpParas,m_pExpInfoWidget,&QExpInfoWidget::on_GetExpNo);
    connect(m_pROn1Widget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateExpParas,m_pExpInfoWidget,&QExpInfoWidget::on_GetExpNo);
    connect(m_pSOn1Widget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateExpParas,m_pExpInfoWidget,&QExpInfoWidget::on_GetExpNo);
    connect(m_pRasterWidget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateExpParas,m_pExpInfoWidget,&QExpInfoWidget::on_GetExpNo);
    connect(m_pQueryWidget,&QQueryWidget::signal_getExpInfoFromdb,m_pDbData,&QDBData::onGetExpInfo4Show);
    connect(m_pQueryWidget,&QQueryWidget::signal_getExpDataFromdb,m_pDbData,&QDBData::onGetExpData4Show);
    connect(m_pQueryWidget,&QQueryWidget::signal_getDefectInfoFromdb,m_pDbData,&QDBData::onGetDefectInfo4Show);
    connect(m_pDbData, &QDBData::signal_queryfinished,m_pQueryWidget,&QQueryWidget::onQueryDbFinish);

    //更新损伤方法
    connect(m_p1On1Widget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateDetectMethod,this,&MainWindow::on_updateDetectMethod);
    connect(m_pROn1Widget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateDetectMethod,this,&MainWindow::on_updateDetectMethod);
    connect(m_pSOn1Widget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateDetectMethod,this,&MainWindow::on_updateDetectMethod);
    connect(m_pRasterWidget->m_pWorkflowParamWidget,&QWorkFlowParamWidget::updateDetectMethod,this,&MainWindow::on_updateDetectMethod);

    m_pExpInfoWidget->on_ShowDevInfo();
    m_pExpInfoWidget->on_ShowClientInfo();

    //人工中断（Ron1）
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_UserEnterAction,m_pROn1Widget,&QWorkFlowWidget::onManualEnterAction);
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_UserConfirmDef,m_pROn1Widget,&QWorkFlowWidget::onManualYesBtnClicked);
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_UserDenyDef,m_pROn1Widget,&QWorkFlowWidget::onManualNoBtnClicked);

    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_UserEnterAction,m_p1On1Widget,&QWorkFlowWidget::onManualEnterAction);
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_UserConfirmDef,m_p1On1Widget,&QWorkFlowWidget::onManualYesBtnClicked);
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_UserDenyDef,m_p1On1Widget,&QWorkFlowWidget::onManualNoBtnClicked);

    //try
    connect(m_pDevManagerCtrl,&QDevManagerCtrl::signal_UserShowDefect,this,&MainWindow::ontryShowWindow);
    InitUI();



    //应用样式
    QFile file(":/qss/ui.qss");
    file.open(QFile::ReadOnly);
    QString stylesheet = file.readAll();
    this->setStyleSheet(stylesheet);


    //设置背景黑色
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background,QColor(188,212,212));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    //TEST
#if 0
    double tem_value = 3.0;
    QString tem_sval = QString::number(tem_value);
    qDebug()<<tem_sval<<tem_sval.contains('.');
    if (!tem_sval.contains('.'))
    {
        tem_sval += ".0";
    }
    qDebug()<<tem_sval<<tem_sval.contains('.');

#endif
#if 0
    QExcel *m_pExcel = new QExcel();
    m_pExcel->initSheet();
    m_pExcel->setSheetName("实验数据");
    m_pExcel->insertContent(1,3,"test53.0");
    m_pExcel->insertContent(2,2,"3.0");
    m_pExcel->saveSheet("C:/TESTTEST.xlsx");
    m_pExcel->closeSheet();

    delete m_pExcel;
#endif
#if 0
    QString strOriImg = QApplication::applicationDirPath() + QString("/IDSIMAGE/0.jpg");
    QString strDefImg = QApplication::applicationDirPath() + QString("/IDSIMAGE/1.jpg");
    QDataProcessCtrl *pDataprocess = NULL;
    pDataprocess = new QDataProcessCtrl();

    if (pDataprocess->caclworkflow(strOriImg,strDefImg))
    {
        delete pDataprocess;
    }
    else
    {
        delete pDataprocess;
    }
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pSystemParamWidget;
    delete m_pDevManagerCtrl;
}

void MainWindow::InitUI()
{
    //try
    m_pWidgetShow = new QWidget();
    m_pBtnShow = new QPushButton(m_pWidgetShow);
    m_pBtnShow->setText("BTN");
    m_pWidgetShow->setGeometry(200,200,200,200);
    m_pBtnShow->setGeometry(10,10,100,25);
    m_pWidgetShow->hide();



    //导航栏

    QStringList SubItem1;
    SubItem1.push_back("1 On 1测试");
    SubItem1.push_back("S On 1测试");
    SubItem1.push_back("R On 1测试");
    SubItem1.push_back("Raster Scan测试");
    m_pNavWidget->SetNavItem("流程控制",SubItem1,":/png/chart.png");//第一组

    QStringList SubItem2;
    SubItem2.push_back("科学CCD");
    SubItem2.push_back("显微检测");
    SubItem2.push_back("能量计");
    SubItem2.push_back("示波器");
    SubItem2.push_back("电机控制");
    SubItem2.push_back("光闸与能量衰减器");
    m_pNavWidget->SetNavItem("设备控制",SubItem2,":/png/dev.png");//第二组

	QStringList SubItem3;
    SubItem3.push_back("系统参数配置");
   // SubItem3.push_back("运动至安全状态");
    m_pNavWidget->SetNavItem("系统参数配置", SubItem3, ":/png/dev.png");//第三组

    QStringList SubItem4;
    SubItem4.push_back("生成检测报告");
    SubItem4.push_back("DataQuery");
    m_pNavWidget->SetNavItem("实验信息汇总", SubItem4, ":/png/dev.png");//第三组
    m_pNavWidget->ShowNavListItem();



    m_pUserViews->addWidget(m_p1On1Widget);    //1On1
    m_pUserViews->addWidget(m_pSOn1Widget);    //SOn1
    m_pUserViews->addWidget(m_pROn1Widget);    //ROn1
    m_pUserViews->addWidget(m_pRasterWidget);    //Raster
    m_pUserViews->addWidget(m_pDevManagerWidget->m_pKxccdCtrlWidget);//科学CCD
    m_pUserViews->addWidget(m_pDevManagerWidget->m_pMicroWidget);//显微探视
    m_pUserViews->addWidget(m_pDevManagerWidget->m_pEnergMeterWidget);//能量计
    m_pUserViews->addWidget(m_pDevManagerWidget->m_pOsciiWidget);//示波器
    m_pUserViews->addWidget(m_pDevManagerWidget->m_pMotorCtrlWidget);//电机
    m_pUserViews->addWidget(m_pDevManagerWidget->m_pShutterCtrlWidget);//电机
    m_pUserViews->addWidget(m_pSystemParamWidget);//系统参数配置界面
    //m_pUserViews->addWidget(m_pSystemMove2SafeWidget);//系统状态控制 运动到安全位置
    m_pUserViews->addWidget(m_pExpInfoWidget);  //实验信息汇总
    m_pUserViews->addWidget(m_pQueryWidget);
    m_pUserViews->setCurrentIndex(0);
    //状态栏
    m_pDevStatusWidget  = new QDevStatusWidget(m_pDevManagerCtrl,this);
    //m_pDevStatusWidget->showDevStatus();

    //页面布局
    QWidget* pwidget = new QWidget(this);
    QGridLayout* playout = new QGridLayout(pwidget);
    playout->setColumnStretch(0,1);
    playout->setColumnStretch(1,5);
    playout->setRowStretch(0,25);
    playout->setRowStretch(1,1);
    playout->addWidget(m_pNavWidget,0,0,2,1);
    playout->addWidget(m_pUserViews,0,1,1,1);
    playout->addWidget(m_pDevStatusWidget,1,1,1,1);

    playout->setSpacing(0);
    playout->setMargin(0);

    this->setCentralWidget(pwidget);

}


void MainWindow::ShowCheckStatusBox(QWorkTask *pTask)
{
    QMessageBox t_mesbox(this);
    t_mesbox.setText("当前实验点处出现损伤\n继续实验流程还是结束实验流程？");
    t_mesbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int t_ret = t_mesbox.exec();
    if (t_ret == QMessageBox::Ok)
    {
        pTask->m_nStepStatus = M_STEP_STATUS_SUCCESS;
        pTask->m_bNeedUserCheck = false;
    }
    else
    {
        pTask->m_bNeedUserCheck = false;
    }
}


void MainWindow::ontryShowWindow(QVariant tTaskExpInfo)
{
//    m_pWidgetShow->show();
    m_pDevManagerCtrl->m_pDefWidget->m_tCurrentTaskExpInfo = tTaskExpInfo;
    m_pDevManagerCtrl->m_pDefWidget->showMaximized();
}


void MainWindow::on_updateDetectMethod()
{
    if (m_pSystemParamWidget->m_pDetectMingRBtn->isChecked())
    {
        m_pDevManagerCtrl->m_nDetectionMethod = 0;
    }
    if (m_pSystemParamWidget->m_pDetectAnRBtn->isChecked())
    {
        m_pDevManagerCtrl->m_nDetectionMethod = 1;
    }
}
