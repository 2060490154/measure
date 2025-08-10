/***************************************************************************
**                                                                                               **
**QKxccdctrlWidget为科学CCD控制组件
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
#include "qkxccdctrlwidget.h"
#include <qgraphicseffect.h>
#include <qfile.h>
#include <QGridLayout>
#include <qcheckbox.h>
#include <QTime>
#include <qimagereader.h>
#include <qfiledialog.h>
#include <QRegExp>
#include <QInputDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "quiqss.h"
using namespace cv;
using namespace std;

#define M_SHOW_STATUS 0 //当前灰度显示的位置 本工程不能显示在状态栏

QKxccdctrlWidget::QKxccdctrlWidget(QKxccdCtrl* pCtrl,QWidget *parent) : QWidget(parent)
{
    m_pKxccdCtrl = pCtrl;
    connect(m_pKxccdCtrl,&QKxccdCtrl::signal_showKxccdImag,this,&QKxccdctrlWidget::onShowImage);

    m_pSearchDevWidget = NULL;
    m_pMeasureWidget = new QMeasureWidget(this);
    connect(m_pMeasureWidget,&QMeasureWidget::singnal_changeType,this,&QKxccdctrlWidget::onChangeLineType);//更新绘图类型 信号槽

    m_sIPAdd = m_pKxccdCtrl->m_tDevSocket.m_sDevIP;
    m_nPort = m_pKxccdCtrl->m_tDevSocket.m_nInitPort;

    m_nRcvFrameCnt = 0;
    m_lfScal = 1.0;

    m_sCurrentGray="x=0,y=0,gray=0";

    m_nWindowPos = 0;
    m_nWindowWidth = 4096;

    m_bPaint = false;

    m_tStartPoint=QPoint(0,0);




    for (int i = 0; i < 256; i++)
    {
        m_vcolorTable.append(qRgb(i, i, i));
    }

    initUI();

    QUIQss::setBtnQss(m_pConnectButton,"#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    QUIQss::setBtnQss(m_pDisConnectButton,"#E74C3C", "#FFFFFF", "#EC7064", "#FFF5E7", "#DC2D1A", "#F5A996");





#if 0
    QImageReader reader("c:\\test.bmp");
    reader.setAutoTransform(true);
    QImage OrigImageBuffer = reader.read();

    QPixmap pixmap = QPixmap::fromImage(OrigImageBuffer);
    m_pImageLabel->setAlignment(Qt::AlignLeft);
    m_pImageLabel->setPixmap(pixmap);
#endif
}

 QKxccdctrlWidget::~QKxccdctrlWidget()
{
     if(m_pMeasureWidget != NULL)
     {
         m_pMeasureWidget->close();
         delete m_pMeasureWidget;
     }
     m_pKxccdCtrl->closeDev();
}
//初始化UI
void QKxccdctrlWidget::initUI()
{

    initDock();

    m_pShowGrayValueLabel = new QLabel("当前灰度:"+m_sCurrentGray,this);
    m_pShowGrayValueLabel->setAlignment(Qt::AlignTop);
    m_pImageLabel = new  QLabel("图像显示",this);
    m_pImageLabel->setBackgroundRole(QPalette::Base);
    m_pImageLabel->setStyleSheet("border: 1px solid #3C80B1;");
    m_pImageLabel->setAlignment(Qt::AlignCenter);

    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setWidgetResizable(true);

    installEventFilter(m_pScrollArea);

    m_pScrollArea->setWidget(m_pImageLabel);
    m_pScrollArea->setVisible(true);


    //窗口整体布局
    QGridLayout* layout = new QGridLayout(this);

    layout->setColumnStretch(1,1);//设置列比例
    layout->setColumnStretch(0,4);
    layout->addWidget(m_pdockWidget,0,1,8,1);
    layout->addWidget(m_pScrollArea,0,0,8,1);
#if M_SHOW_STATUS
    layout->addWidget(m_pShowGrayValueLabel,8,0,1,1);
#endif
    layout->setMargin(0);

}


/***************************************************
 * 创建左侧DOCK
 * *************************************************/
void QKxccdctrlWidget::initDock()
{

    //初始化 dock 界面
    m_pdockWidget = new QDockWidget(this);
    m_pdockWidget->setWindowTitle(tr("科学CCD操作"));
    m_pdockWidget->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);

    //状态显示
    m_pStatusLabel = new QLabel("状态显示",this);

    m_plogList = new QListWidget(this);

    //IP地址
    QLabel* pIPLabel = new QLabel(this);
    pIPLabel->setText("地址");
    m_pIPAddressEdit = new QLineEdit(this);
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator *pValidator = new QRegExpValidator(regExp, this);
    m_pIPAddressEdit->setValidator(pValidator);
    m_pIPAddressEdit->setInputMask("000.000.000.000;");
    m_pIPAddressEdit->setText(m_sIPAdd);



    //Dockwidget布局
    QWidget* dockWidget = new QWidget(this);
    m_pdockWidget->setWidget(dockWidget);
    QGridLayout* dockWidgetlayout = new QGridLayout(dockWidget);

    for(int i = 0; i < 8; i++)
    {
        dockWidgetlayout->setColumnStretch(i,1);//设置列比例
        dockWidgetlayout->setRowStretch(i,4);
    }
    dockWidgetlayout->setRowStretch(4,8);

    dockWidgetlayout->addWidget(new QLabel("地址:"),0,0,1,2);
    dockWidgetlayout->addWidget(m_pIPAddressEdit,0,2,1,6);
    dockWidgetlayout->addWidget(createOperationGroupBox(),1,0,1,8);
    dockWidgetlayout->addWidget(createWorkModeGroupBox(),2,0,1,8);
    dockWidgetlayout->addWidget(createSetParamGroup(),3,0,1,8);

    dockWidgetlayout->addWidget(createImageProcessGroup(),4,0,1,8);

    dockWidgetlayout->addWidget(m_pStatusLabel,5,0,1,8);
    dockWidgetlayout->addWidget(m_plogList,6,0,3,8);


}



/***************************************************
 * 创建工作模式选择groupbox
 * *************************************************/
QGroupBox* QKxccdctrlWidget::createWorkModeGroupBox()
{
    QGroupBox *groupBox = new QGroupBox(tr("工作方式"));

     m_pContinueActiveCheckBox = new QCheckBox(tr("内触发"));
     m_pHardwareSynActiveCheckBox = new QCheckBox(tr("连续外触发"));
     m_pHardwareSynNativeCheckBox = new QCheckBox(tr("帧存采集"));


    connect(m_pContinueActiveCheckBox,SIGNAL(clicked()),this,SLOT(onSetInContinueMode()));
    connect(m_pHardwareSynActiveCheckBox,SIGNAL(clicked()),this,SLOT(onSetOutContinueMode()));
    connect(m_pHardwareSynNativeCheckBox,SIGNAL(clicked()),this,SLOT(onSetRecapMode()));



    QVBoxLayout *vbox = new QVBoxLayout(groupBox);
    vbox->addWidget(m_pContinueActiveCheckBox);
    vbox->addWidget(m_pHardwareSynActiveCheckBox);
    vbox->addWidget(m_pHardwareSynNativeCheckBox);

    vbox->setSpacing(5);

    return groupBox;
}

/***************************************************
 * 创建操作groupbox
 * *************************************************/
QGroupBox* QKxccdctrlWidget::createOperationGroupBox()
{
    m_pConnectButton = new QPushButton(this);
    m_pConnectButton->setText(("连接"));
    connect(m_pConnectButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onConnectDev);
    m_pConnectButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    m_pDisConnectButton = new QPushButton(this);
    m_pDisConnectButton->setText(("断开"));
    connect(m_pDisConnectButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onDisConnectDev);
    m_pDisConnectButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QGroupBox *groupBox = new QGroupBox(tr("操作"));

    QHBoxLayout* pHBox = new QHBoxLayout(groupBox);
    QVBoxLayout* pVBox = new QVBoxLayout;
    pVBox->addWidget(m_pConnectButton);
    pVBox->addWidget(m_pDisConnectButton);
    pHBox->addLayout(pVBox,1);

    return groupBox;
}


/***************************************************
 * 创建图像操作groupbox
 * *************************************************/
QGroupBox* QKxccdctrlWidget::createImageProcessGroup()
{
    //按钮
    m_pOpenImageButton = new QPushButton(this);
    m_pOpenImageButton->setText(("打开图像"));
    connect(m_pOpenImageButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onOpenImageFile);
    m_pOpenImageButton->setMinimumHeight(25);
    m_pOpenImageButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pSaveImageButton = new QPushButton(this);
    m_pSaveImageButton->setText(("保存图像"));
    m_pSaveImageButton->setMinimumHeight(25);
    connect(m_pSaveImageButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onSaveAsImageFile);
    m_pSaveImageButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pRecapImageButton = new QPushButton(this);
    m_pRecapImageButton->setText(("重传"));
    connect(m_pRecapImageButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onRecapImag);
    m_pRecapImageButton->setMinimumHeight(25);
    m_pRecapImageButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_pRecapImageButton->setEnabled(false);

    m_pModifyIPAddressButton = new QPushButton(this);
    m_pModifyIPAddressButton->setText(("修改IP地址"));
    m_pModifyIPAddressButton->setMinimumHeight(25);
    connect(m_pModifyIPAddressButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onModfiyIPAddress);
    m_pModifyIPAddressButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    m_pGetDevParamButton = new QPushButton(this);
    m_pGetDevParamButton->setText(("查询参数"));
    m_pGetDevParamButton->setMinimumHeight(25);
    connect(m_pGetDevParamButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onGetDevParam);
    m_pGetDevParamButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pSaveDevParamButton = new QPushButton(this);
    m_pSaveDevParamButton->setText(("保存参数"));
    m_pSaveDevParamButton->setMinimumHeight(25);
    connect(m_pSaveDevParamButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onSaveDevParam);
    m_pSaveDevParamButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pSearchDevButton = new QPushButton(this);
    m_pSearchDevButton->setText(("搜索相机"));
    m_pSearchDevButton->setMinimumHeight(25);
    connect(m_pSearchDevButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onSearchDev);
    m_pSearchDevButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pMeausreAreaButton = new QPushButton(this);
    m_pMeausreAreaButton->setText(("面积测量"));
    m_pMeausreAreaButton->setMinimumHeight(25);
    connect(m_pMeausreAreaButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onAreaMeasure);
    m_pMeausreAreaButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pMeausreLineButton = new QPushButton(this);
    m_pMeausreLineButton->setText(("线灰度测量"));
    m_pMeausreLineButton->setMinimumHeight(25);
    connect(m_pMeausreLineButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onLineMeasure);
    m_pMeausreLineButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    m_pAutoMeausreAreaButton = new QPushButton("计算光斑面积",this);
    m_pAutoMeausreAreaButton->setMinimumHeight(25);
    connect(m_pAutoMeausreAreaButton,&QPushButton::clicked,this,&QKxccdctrlWidget::onCalcuArea);
    m_pAutoMeausreAreaButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);



    //设置按钮图片
    //setButtonICon(m_pOpenImageButton,":/png/openfile.png");
    //setButtonICon(m_pSaveImageButton,":/png/saveas.png");


    QGroupBox *groupBox = new QGroupBox(tr(""));
    QGridLayout* pLayout = new QGridLayout(groupBox);

    pLayout->addWidget(m_pRecapImageButton,0,0,1,1);
    pLayout->addWidget(m_pModifyIPAddressButton,0,1,1,1);

    pLayout->addWidget(m_pOpenImageButton,1,0,1,1);
    pLayout->addWidget(m_pSaveImageButton,1,1,1,1);

    pLayout->addWidget(m_pGetDevParamButton,2,0,1,1);
    pLayout->addWidget(m_pSaveDevParamButton,2,1,1,1);

    pLayout->addWidget(m_pMeausreAreaButton,3,0,1,1);
    pLayout->addWidget(m_pMeausreLineButton,3,1,1,1);

    pLayout->addWidget(m_pAutoMeausreAreaButton,4,0,1,1);
    pLayout->addWidget(m_pSearchDevButton,4,1,1,1);

    pLayout->setSpacing(5);
    return groupBox;
}

/***************************************************
 * 创建参数设置 groupbox
 * *************************************************/
QGroupBox* QKxccdctrlWidget::createSetParamGroup()
{
    //参数设置
    m_pParamSelectComBox = new QComboBox(this);
    m_pParamValueSliderWidget = new QCustomSliderWidget(this);
    m_pWindowWidthSliderWidget = new QCustomSliderWidget(this);
    m_pWindowPosSliderWidget = new QCustomSliderWidget(this);

    for(int i = 0;i < m_pKxccdCtrl->m_st_ParamInfoList.size();i++)
    {
        m_pParamSelectComBox->addItem(m_pKxccdCtrl->m_st_ParamInfoList.at(i).sParamName);
    }

    m_pParamSelectComBox->setCurrentIndex(0);
    m_pParamValueSliderWidget->setThrosholdAndCurrentValue(m_pKxccdCtrl->m_st_ParamInfoList.at(0).nMaxValue,m_pKxccdCtrl->m_st_ParamInfoList.at(0).nMinValue,m_pKxccdCtrl->m_st_ParamInfoList.at(0).nCurrentValue);

    m_pWindowPosSliderWidget->setThrosholdAndCurrentValue(4096,0,m_nWindowPos);
    m_pWindowWidthSliderWidget->setThrosholdAndCurrentValue(4096,0,m_nWindowWidth);

    connect(m_pWindowPosSliderWidget,&QCustomSliderWidget::signal_SliderMoved,this,&QKxccdctrlWidget::onUpdateWindowPos);
    connect(m_pWindowWidthSliderWidget,&QCustomSliderWidget::signal_SliderMoved,this,&QKxccdctrlWidget::onUpdateWindowWidth);
    connect(m_pParamValueSliderWidget,&QCustomSliderWidget::signal_SliderRelease,this,&QKxccdctrlWidget::onUpdateParamvalue);

    void(QComboBox::*fp)(int)=&QComboBox::currentIndexChanged;
    connect(m_pParamSelectComBox,fp,this,&QKxccdctrlWidget::onParamSelectChange);

    //显示位数
    m_pCheckBox[0] = new QRadioButton(this);
    m_pCheckBox[0]->setText("8Bit");
    m_pCheckBox[1] = new QRadioButton(this);
    m_pCheckBox[1]->setText("12Bit");
    m_pCheckBox[2] = new QRadioButton(this);
    m_pCheckBox[2]->setText("16Bit");
    m_pCheckBox[1]->setChecked(true);//默认为12bit
    connect(m_pCheckBox[0],SIGNAL(clicked()),this,SLOT(onSet8Bit()));
    connect(m_pCheckBox[1],SIGNAL(clicked()),this,SLOT(onSet12Bit()));
    connect(m_pCheckBox[2],SIGNAL(clicked()),this,SLOT(onSet16Bit()));

    QGroupBox *groupBox = new QGroupBox(tr("参数设置"));

    QGridLayout* pLayout = new QGridLayout(groupBox);

    pLayout->setColumnStretch(0,1);
    pLayout->setColumnStretch(1,4);

    pLayout->addWidget(m_pParamSelectComBox,0,0,1,1);
    pLayout->addWidget(m_pParamValueSliderWidget,0,1,1,3);

    pLayout->addWidget(new QLabel("窗位"),1,0,1,1);
    pLayout->addWidget(m_pWindowPosSliderWidget,1,1,1,3);

    pLayout->addWidget(new QLabel("窗宽"),2,0,1,1);
    pLayout->addWidget(m_pWindowWidthSliderWidget,2,1,1,3);

    pLayout->addWidget(new QLabel("显示位数"),3,0,1,1);
    pLayout->addWidget(m_pCheckBox[0],3,1,1,1);
    pLayout->addWidget(m_pCheckBox[1],3,2,1,1);
    pLayout->addWidget(m_pCheckBox[2],3,3,1,1);

    pLayout->setSpacing(5);
    return groupBox;
}
//设置复选框  互斥选择
void QKxccdctrlWidget::setWorkModeCheckBoxStatus(QCheckBox* pBox,bool bCheck)
{
    m_pContinueActiveCheckBox->setChecked(false);
    m_pHardwareSynActiveCheckBox->setChecked(false);
    m_pHardwareSynNativeCheckBox->setChecked(false);

    pBox->setChecked(bCheck);
}

//设置复选框  使能状态
void QKxccdctrlWidget::setWorkModeCheckBoxEnable(bool benable)
{
    m_pContinueActiveCheckBox->setEnabled(benable);
    m_pHardwareSynActiveCheckBox->setEnabled(benable);
    m_pHardwareSynNativeCheckBox->setEnabled(benable);
}

//连接设备
void QKxccdctrlWidget::onConnectDev()
{
    m_sIPAdd = m_pIPAddressEdit->text();
    m_pKxccdCtrl->setDevIPAndPort(m_sIPAdd,m_nPort);
    if(!m_pKxccdCtrl->openDev())
    {
        QMessageBox messageBox(QMessageBox::Warning,"提示","连接设备失败");
        messageBox.setStandardButtons (QMessageBox::Ok);
        messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
        messageBox.exec ();
        return;
    }
    m_pConnectButton->setEnabled(false);
    m_pDisConnectButton->setEnabled(true);
    m_nRcvFrameCnt = 0;
    showDevStatus();
}

//断开设备
void QKxccdctrlWidget::onDisConnectDev()
{
    m_pKxccdCtrl->closeDev();
    m_pConnectButton->setEnabled(true);
    m_pDisConnectButton->setEnabled(false);
    showDevStatus();
}

/*******************************************************************
**功能：连续内同步
**输入：
**输出：
**返回值：
*******************************************************************/
void QKxccdctrlWidget::onSetInContinueMode()
{
    setWorkModeCheckBoxEnable(false);
    m_pRecapImageButton->setEnabled(false);

    m_sIPAdd = m_pIPAddressEdit->text();
    m_pKxccdCtrl->setDevIPAndPort(m_sIPAdd,m_nPort);

    if(m_pKxccdCtrl == NULL|| !m_pKxccdCtrl->openDev())
    {
        setWorkModeCheckBoxStatus(m_pContinueActiveCheckBox,false);
        showLogInfo("打开相机失败!");
        setWorkModeCheckBoxEnable(true);
        return;
    }

    if(m_pContinueActiveCheckBox->isChecked())
    {
        //设置内同步
        bool bRel1 = m_pKxccdCtrl->setWorkMode(M_CAPTYPE_SYN_IN_ACTIVE);
        bool bRel2 = m_pKxccdCtrl->startCap();
        if(bRel1 == false || bRel2 == false)
        {
            showLogInfo("设置内触发模式失败!");
            setWorkModeCheckBoxStatus(m_pContinueActiveCheckBox,false);
        }
        else
        {
            m_nRcvFrameCnt = 0;
            showLogInfo("设置内触发模式成功!");
            setWorkModeCheckBoxStatus(m_pContinueActiveCheckBox,true);
        }
    }
    else
    {
        if (!m_pKxccdCtrl->stopCap())
        {
            showLogInfo("取消内触发模式失败!");
        }
    }
    setWorkModeCheckBoxEnable(true);
}

/*******************************************************************
**功能：设置外同步连续触发
**输入：
**输出：
**返回值：
*******************************************************************/
void QKxccdctrlWidget::onSetOutContinueMode()
{
    setWorkModeCheckBoxEnable(false);
    m_pRecapImageButton->setEnabled(false);

    m_sIPAdd = m_pIPAddressEdit->text();
    m_pKxccdCtrl->setDevIPAndPort(m_sIPAdd,m_nPort);

    if(m_pKxccdCtrl == NULL|| !m_pKxccdCtrl->openDev())
    {
        setWorkModeCheckBoxStatus(m_pHardwareSynActiveCheckBox,false);
        showLogInfo("打开相机失败!");
        setWorkModeCheckBoxEnable(true);
        return;
    }

    if(m_pHardwareSynActiveCheckBox->isChecked())
    {
        //设置外同步
        bool bRel1 = m_pKxccdCtrl->setWorkMode(M_CAPTYPE_SYN_OUT_ACTIVE);
        bool bRel2 = m_pKxccdCtrl->startCap();
        if(bRel1 == false || bRel2 == false)
        {
            showLogInfo("设置连续外触发模式失败!");
            setWorkModeCheckBoxStatus(m_pHardwareSynActiveCheckBox,false);
        }
        else
        {
            m_nRcvFrameCnt = 0;
            showLogInfo("设置连续外触发模式成功!");
            setWorkModeCheckBoxStatus(m_pHardwareSynActiveCheckBox,true);
        }
    }
    else
    {
        if (!m_pKxccdCtrl->stopCap())
        {
            showLogInfo("取消内触发模式失败!");
        }
    }
    setWorkModeCheckBoxEnable(true);
}

/*******************************************************************
**功能：帧存模式
**输入：
**输出：
**返回值：
*******************************************************************/
void QKxccdctrlWidget::onSetRecapMode()
{
    setWorkModeCheckBoxEnable(false);
    m_pRecapImageButton->setEnabled(false);

    m_sIPAdd = m_pIPAddressEdit->text();
    m_pKxccdCtrl->setDevIPAndPort(m_sIPAdd,m_nPort);

    if(m_pKxccdCtrl == NULL|| !m_pKxccdCtrl->openDev())
    {
        setWorkModeCheckBoxStatus(m_pHardwareSynNativeCheckBox,false);
        showLogInfo("打开相机失败!");
        setWorkModeCheckBoxEnable(true);
        return;
    }

     if(m_pHardwareSynNativeCheckBox->isChecked())
     {
         //设置帧存
         if (!m_pKxccdCtrl->setWorkMode(M_CAPTYPE_SYN_OUT_NATIVE))
         {
             showLogInfo("设置帧存模式失败!");
             setWorkModeCheckBoxStatus(m_pHardwareSynNativeCheckBox,false);
         }
         else
         {
             showLogInfo("设置帧存模式成功!");
             m_nRcvFrameCnt = 0;
             m_pRecapImageButton->setEnabled(true);
             setWorkModeCheckBoxStatus(m_pHardwareSynNativeCheckBox,true);
         }

     }
     setWorkModeCheckBoxEnable(true);
}

//显示图像
void QKxccdctrlWidget::onShowImage(uchar* pbuff,int bufWidth,int bufHight,bool bCnt)
{
    m_ImageBuff = QImage(pbuff, bufWidth, bufHight, QImage::Format_Indexed8);  //封装QImage
    m_ImageBuff.setColorTable(m_vcolorTable); //设置颜色表
    if(bCnt == true)
    {
        m_nRcvFrameCnt++;
    }

    this->update();
}



//添加log信息
void QKxccdctrlWidget::showLogInfo(QString slog)
{
    QTime time = QTime::currentTime();

    QString str = time.toString("hh:mm:ss") +"    "+ slog;


    m_plogList->addItem(str);
    m_plogList->setCurrentRow(m_plogList->count()-1);
}

//初始化状态显示
void QKxccdctrlWidget::showDevStatus()
{
    QString sStatus;
    if(m_pKxccdCtrl == NULL)
    {
       return;
    }

    if(m_pKxccdCtrl->isDevOpen())
    {
       m_pConnectButton->setEnabled(false);
       m_pDisConnectButton->setEnabled(true);
       sStatus = "连接状态:已连接";
    }
    else
    {
        m_pDisConnectButton->setEnabled(false);
        m_pConnectButton->setEnabled(true);
        sStatus = "连接状态:未连接";
    }
    if(m_pKxccdCtrl->m_nDevWorkMode == M_CAPTYPE_SYN_IN_ACTIVE)
    {
       sStatus = sStatus + "\n" +"工作模式:连续内同步\n";
    }
    else if(m_pKxccdCtrl->m_nDevWorkMode == M_CAPTYPE_SYN_OUT_ACTIVE)
    {
       sStatus = sStatus + "\n" +"工作模式:连续外触发\n";
    }
    else  if(m_pKxccdCtrl->m_nDevWorkMode == M_CAPTYPE_SYN_OUT_NATIVE)
    {
       sStatus = sStatus + "\n" +"工作模式:帧存模式\n";
    }
    else
    {
       sStatus = sStatus + "\n" +"工作模式:无\n";
    }

    sStatus = sStatus +"帧计数:"+QString("%1").arg(m_nRcvFrameCnt);

    sStatus = sStatus + "\n"+"缩放比例:"+QString("%1").arg(m_lfScal,0,'f',4);

    #if !M_SHOW_STATUS
        sStatus = sStatus + "\n"+"当前灰度:"+m_sCurrentGray;
    #endif
        m_pStatusLabel->setText(sStatus);
    #if M_SHOW_STATUS
        m_pShowGrayValueLabel->setText("当前灰度:"+m_sCurrentGray);
    #endif
    this->update();
}

/******************************************
*功能:绘图
*输入:
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::paintEvent(QPaintEvent * event)
{
    QPen pen; //设置红色画笔
    pen.setColor(QColor(255,0,0));

    if(!m_ImageBuff.isNull())
    {
        m_ImageTempBuff = m_ImageBuff.scaled(m_ImageBuff.width()*m_lfScal,m_ImageBuff.height()*m_lfScal);
        m_ImageTempBuff = m_ImageTempBuff.mirrored(false,true);
        m_Pixmap = QPixmap::fromImage(m_ImageTempBuff);
        m_pImageLabel->setAlignment(Qt::AlignLeft);//靠左显示

        if(m_bPaint)
        {
            paintLine();
        }
        m_pImageLabel->setPixmap(m_Pixmap);


    }
     showDevStatus();
}

/******************************************
* 功能:将坐标信息限定在图像大小范围内
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::setPointValid(QPoint* tPoint)
{
    if(tPoint->x() < 0 )
    {
        tPoint->setX(0);
    }
    if(tPoint->x() >= m_Pixmap.width())
    {
        tPoint->setX(m_Pixmap.width()-1);
    }

    if(tPoint->y() < 0)
    {
        tPoint->setY(0);
    }

    if(tPoint->y() >= m_Pixmap.height())
    {
        tPoint->setY(m_Pixmap.height()-1);
    }
}

/******************************************
* 功能:绘制矩形 直线
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::paintLine()
{
    if(m_Pixmap.isNull())
    {
        return;
    }

    QPen pen; //设置红色画笔
    QPainter painter(&m_Pixmap);

    pen.setColor(QColor(255,0,0));
    painter.setPen(pen); //添加画笔

    setPointValid(&m_tStartPoint);
    setPointValid(&m_tEndPoint);

    switch(m_nLineType)
    {
        case E_PAINT_LINE_X:
        {
            if(m_tStartPoint.y() >= m_Pixmap.height()-1)
            {
                m_tStartPoint.setY(m_Pixmap.height()-1);
            }
            QPoint tStart(0,m_tStartPoint.y());
            QPoint tEnd(m_Pixmap.width()-1,m_tStartPoint.y());
            painter.drawLine(tStart,tEnd);

            //计算参数
            int datalen = m_pKxccdCtrl->m_higImg.getAreaData(tStart/m_lfScal,tEnd/m_lfScal);
            m_pKxccdCtrl->m_higImg.calMeasureValue(datalen);
            //画曲线
            m_pMeasureWidget->m_tMeasureData = m_pKxccdCtrl->m_higImg.m_tMeasureData;
            m_pMeasureWidget->PlotLine(E_PAINT_LINE_X);

        }
        break;
       case E_PAINT_LINE_Y:
        {
            if(m_tStartPoint.x() >= m_Pixmap.width()-1)
            {
                m_tStartPoint.setX(m_Pixmap.width()-1);
            }
            QPoint tStart(m_tStartPoint.x(),0);
            QPoint tEnd(m_tStartPoint.x(),m_Pixmap.height()-1);
            painter.drawLine(tStart,tEnd);

            //计算参数
            int datalen = m_pKxccdCtrl->m_higImg.getAreaData(tStart/m_lfScal,tEnd/m_lfScal);
            m_pKxccdCtrl->m_higImg.calMeasureValue(datalen);
            //画曲线
            m_pMeasureWidget->m_tMeasureData = m_pKxccdCtrl->m_higImg.m_tMeasureData;
            m_pMeasureWidget->PlotLine(E_PAINT_LINE_Y);

        }
        break;
        case E_PAINT_RECT:
        {
            int datalen = 0;
            painter.drawRect(m_tStartPoint.x(),m_tStartPoint.y(),m_tEndPoint.x()-m_tStartPoint.x(),m_tEndPoint.y()-m_tStartPoint.y());

            //计算参数
            datalen = m_pKxccdCtrl->m_higImg.getAreaData(m_tStartPoint/m_lfScal,m_tEndPoint/m_lfScal);
            m_pKxccdCtrl->m_higImg.calMeasureValue(datalen);
            //画曲线
            m_pMeasureWidget->m_tMeasureData = m_pKxccdCtrl->m_higImg.m_tMeasureData;
            m_pMeasureWidget->PlotLine(E_PAINT_RECT);
        }
        break;

    }
}
/******************************************
* 功能:按钮样式设置
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::setButtonICon(QPushButton* pButton,QString sPngName)
{
    QPixmap pixmap(sPngName);
    pButton->setIcon(pixmap);
    pButton->setIconSize(pixmap.size());
}

/******************************************
* 功能:打开图片 图片格式hig
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::onOpenImageFile()
{
    if(m_pContinueActiveCheckBox->isChecked() || m_pHardwareSynActiveCheckBox->isChecked() || m_pHardwareSynNativeCheckBox->isChecked())
    {
        return;
    }

    QString sFilePath = QFileDialog::getOpenFileName(this, tr("打开图片"), ".", tr("Image Files(*.hig *.HIG)"));

    if(sFilePath.length() != 0)
    {
        m_pKxccdCtrl->m_higImg.loadFile(sFilePath);       
        m_pKxccdCtrl->updateWindowInfo(m_nWindowPos,m_nWindowWidth);
    }

}


/******************************************
*功能:保存图片
*输入:
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onSaveAsImageFile()
{
    if(m_pImageLabel->pixmap() == NULL)
    {
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this,tr("保存图片"),"",tr("Images (*.bmp *.hig)")); //选择路径
    if(filename.length() != 0 )
    {
         m_pKxccdCtrl->saveImgeToFile(filename);
    }

}

/******************************************
* 功能:鼠标移动操作
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::mouseMoveEvent(QMouseEvent *event)
{
    QRect ScrollRect =  QRect(m_pScrollArea->pos(),m_pScrollArea->size());
    QPoint currentPos =  event->pos();//客户区 左上角为(0,0)点  判断是否在图片显示区域
    if(!ScrollRect.contains(currentPos))
    {
        return;
    }
    if( m_ImageBuff.isNull())
    {
        return;
    }

    int pos_x = currentPos.x() + m_pScrollArea->horizontalScrollBar()->value();
    int pos_y = currentPos.y() + m_pScrollArea->verticalScrollBar()->value();

    QPoint tPos(pos_x,pos_y);

    pos_x = pos_x/m_lfScal;
    pos_y = pos_y/m_lfScal;

   int nGray =m_pKxccdCtrl->m_higImg.getPixelValue(pos_x,pos_y);
   // int nGray = qRed(m_ImageBuff.pixel(pos_x,pos_y));



    m_sCurrentGray = QString("x=%1 y=%2 gray=%3").arg(pos_x).arg(pos_y).arg(nGray);

    showDevStatus();


    switch(m_nLineType)
    {
        case E_PAINT_LINE_X:
        {
            if(m_bTracking == true)//认为在线上
            {
                m_tStartPoint = tPos;
                m_bPaint = true;
                this->update();
            }

        }
        break;
        case E_PAINT_LINE_Y:
        {
            if(m_bTracking == true)//认为在线上
            {
                m_tStartPoint = tPos;
                m_bPaint = true;
                this->update();
            }

        }
        break;
        case E_PAINT_RECT:
        {

            if(m_bDrawOrg == true)
            {
                m_tStartPoint = tPos;
            }
            else if(m_bDrawEnd == true)
            {
                m_tEndPoint = tPos;
            }
            else if(m_bTracking == true)
            {
                QPoint tStep = tPos - m_ptDown;
                m_ptDown = tPos;
                m_tStartPoint = m_tStartPoint + tStep;
                m_tEndPoint = m_tEndPoint + tStep;
            }
            else
            {
                 m_tEndPoint = tPos;
            }

            m_bPaint = true;
            this->update();
        }
        break;

    }


}

/******************************************
* 功能:鼠标按下事件
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::mousePressEvent(QMouseEvent *event)
{

    QRect ScrollRect =  QRect(m_pScrollArea->pos(),m_pScrollArea->size());
    QPoint currentPos =  event->pos();//客户区 左上角为(0,0)点  判断是否在图片显示区域

    if(event->button() != Qt::LeftButton || !ScrollRect.contains(currentPos))
    {
        this->setCursor(Qt::ArrowCursor);
        return ;
    }


    int pos_x = currentPos.x() + m_pScrollArea->horizontalScrollBar()->value();
    int pos_y = currentPos.y() + m_pScrollArea->verticalScrollBar()->value();

    QPoint tPos(pos_x,pos_y);

    switch(m_nLineType)
    {
        case E_PAINT_LINE_X:
        {
            if(m_tStartPoint.y() == 0)
            {
                m_tStartPoint = tPos;
            }

            if(tPos.y()<=m_tStartPoint.y()+10 && tPos.y() >= m_tStartPoint.y()-10)//认为在线上
            {
                m_bTracking = true;
                this->setCursor(Qt::SizeVerCursor);
            }
            else
            {
                m_bTracking = false;
                this->setCursor(Qt::ArrowCursor);
            }

        }
        break;
        case E_PAINT_LINE_Y:
        {
            if(m_tStartPoint.x() == 0)
            {
                m_tStartPoint = tPos;
            }

            if(tPos.x()<=m_tStartPoint.x()+10 && tPos.x() >= m_tStartPoint.x()-10)//认为在线上
            {
                m_bTracking = true;
                this->setCursor(Qt::SizeHorCursor);
            }
            else
            {
                m_bTracking = false;
                this->setCursor(Qt::ArrowCursor);
            }

        }
        break;
        case E_PAINT_RECT:
        {
            QRect rect(m_tStartPoint,m_tEndPoint);

            //是否在起始点
            if(tPos.x()<=m_tStartPoint.x()+10 && tPos.x() >= m_tStartPoint.x()-10 &&tPos.y()<=m_tStartPoint.y()+10 && tPos.y() >= m_tStartPoint.y()-10 )
            {
                m_bDrawEnd = false;
                m_bTracking = false;
                m_bDrawOrg = true;
                this->setCursor(Qt::SizeFDiagCursor);
            }//是否在终止点
            else if(tPos.x()<=m_tEndPoint.x()+10 && tPos.x() >= m_tEndPoint.x()-10 && tPos.y()<=m_tEndPoint.y()+10 && tPos.y() >= m_tEndPoint.y()-10)
            {
                m_bDrawOrg = false;
                m_bTracking = false;
                m_bDrawEnd = true;
                this->setCursor(Qt::SizeFDiagCursor);
            }//是否在矩形内
            else if(rect.contains(tPos))
            {
                m_bDrawOrg = false;
                m_bDrawEnd = false;
                m_bTracking = true;
                m_ptDown = tPos;
                this->setCursor(Qt::SizeAllCursor);
            }
            else
            {
                m_tStartPoint = tPos;
                m_tEndPoint  = m_tStartPoint;


                this->setCursor(Qt::SizeFDiagCursor);

                m_bDrawOrg = false;
                m_bDrawEnd = false;
                m_bTracking = false;
                this->setCursor(Qt::ArrowCursor);
            }

        }
        break;

    }

}

/******************************************
* 功能:滚轮移动操作
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::wheelEvent(QWheelEvent *event)
{
    if( m_ImageBuff.isNull())
    {
        return;
    }

    QRect ScrollRect =  QRect(m_pScrollArea->pos(),m_pScrollArea->size());
    QPoint currentPos =  event->pos();//客户区 左上角为(0,0)点  判断是否在图片显示区域
    if(!ScrollRect.contains(currentPos))
    {
        return;
    }

    int ndelta =  event->delta();

    if(ndelta > 0 && m_lfScal <= 1.75)
    {
        m_lfScal = m_lfScal+0.25;
    }
    if(ndelta < 0 && m_lfScal >=0.5)
    {
        m_lfScal = m_lfScal-0.25;
    }


    this->update();
}

/******************************************
* 功能:重传功能
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void QKxccdctrlWidget:: onRecapImag()
 {
     if(m_pKxccdCtrl == NULL|| !m_pKxccdCtrl->isDevOpen())
     {
         showLogInfo("相机未打开!");
         return;
     }

     if(!m_pKxccdCtrl->startReCap())
     {
         showLogInfo("重传失败!");
     }
     else
         showLogInfo("重传成功!");
 }


 /******************************************
 * 功能:修改IP信息
 * 输入:
 * 输出:
 * 返回值:
 * 维护记录:
 ******************************************/
 void QKxccdctrlWidget::onModfiyIPAddress()
 {
     QInputDialog* l_pInputDlg = new QInputDialog(this);

     l_pInputDlg->setAutoFillBackground(true);
     //pInputDlg->setStyleSheet("background-color:rgb(53,63,73);color:rgb(255,255,255);");
     l_pInputDlg->setOkButtonText("确定");
     l_pInputDlg->setCancelButtonText("取消");
     l_pInputDlg->setLabelText("IP地址:");
     l_pInputDlg->setTextValue(m_pIPAddressEdit->text());
     l_pInputDlg->setWindowTitle("修改IP地址");
     l_pInputDlg->setGeometry(100,100,400,400);
     l_pInputDlg->move((QApplication::desktop()->width()-l_pInputDlg->width())/2,(QApplication::desktop()->height()-l_pInputDlg->height())/2);

     if(l_pInputDlg->exec() == QDialog::Accepted)
     {
         QString sIP = l_pInputDlg->textValue();
         bool bRel = m_pKxccdCtrl->modifyIPAddress(sIP);
         if(bRel == true)
         {
            showLogInfo("修改IP地址成功,新IP地址为:"+sIP);
            m_pIPAddressEdit->setText(sIP);
         }
         else
         {
             showLogInfo("修改IP地址失败");
         }


         //临时监控功能 by lg  20190611 监控IP地址被修改问题
         QFile tem_log(QApplication::applicationDirPath() + "/Log/ccdModifyIp.txt");
         if (!(tem_log.open(QIODevice::Append)))
         {
             qDebug()<<"open failed";
         }
         else
         {
             QString slog = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +" 修改后的IP："+sIP;
             QTextStream in(&tem_log);
             in<<slog<<"\t\n";
             tem_log.close();
         }

     }

 }

/******************************************
*功能:窗位移动
*输入:position 当前值
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onUpdateWindowPos(int position)
{
    m_nWindowPos = position;
    m_pKxccdCtrl->updateWindowInfo(m_nWindowPos,m_nWindowWidth);
}

/******************************************
*功能:窗宽移动
*输入:position 当前值
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onUpdateWindowWidth(int position)
{
    m_nWindowWidth = position;
    m_pKxccdCtrl->updateWindowInfo(m_nWindowPos,m_nWindowWidth);
}

/******************************************
*功能:更新参数设置
*输入:position 当前值
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onUpdateParamvalue(int position)
{
    int nCurrentIndex = m_pParamSelectComBox->currentIndex();

    bool bRel = m_pKxccdCtrl->setDevParam(nCurrentIndex,position);
    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","设置参数失败!");
    }
}

/******************************************
*功能:参数选择变化
*输入:当前选择编号
*输出:更新最大值 最小值数据
*返回值:无
*修改记录:
******************************************/
void QKxccdctrlWidget::onParamSelectChange(int nIndex)
{
    m_pParamValueSliderWidget->setThrosholdAndCurrentValue(m_pKxccdCtrl->m_st_ParamInfoList[nIndex].nMaxValue, m_pKxccdCtrl->m_st_ParamInfoList[nIndex].nMinValue,m_pKxccdCtrl->m_st_ParamInfoList[nIndex].nCurrentValue);
}

/******************************************
*功能:设置显示的位数 8bit
*输入:
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onSet8Bit()
{
    updateShowBit(0x01);
    m_pCheckBox[1]->setChecked(false);
    m_pCheckBox[2]->setChecked(false);

}
/******************************************
*功能:设置显示的位数 12bit
*输入:
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onSet12Bit()
{
    updateShowBit(0x02);
    m_pCheckBox[0]->setChecked(false);
    m_pCheckBox[2]->setChecked(false);
}

/******************************************
*功能:设置显示的位数 16bit
*输入:
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onSet16Bit()
{
    updateShowBit(0x03);
    m_pCheckBox[0]->setChecked(false);
    m_pCheckBox[1]->setChecked(false);
}

/******************************************
*功能:根据选择的显示位数 更新界面和相关参数
*输入:
*输出:
*返回值:
*修改记录:
******************************************/

void QKxccdctrlWidget::updateShowBit(int nBitType)
{
    int nBitLen = 12;
    if (nBitType == 0x1)
    {
        nBitLen = 10;
    }
    else if (nBitType == 0x2)
    {
        nBitLen = 12;
    }
    else if (nBitType == 0x3)
    {
        nBitLen =16;
    }

    m_nWindowPos = 0;
    m_nWindowWidth = 1<<nBitLen;
    m_pWindowPosSliderWidget->setThrosholdAndCurrentValue(1<<nBitLen,0,m_nWindowPos);
    m_pWindowWidthSliderWidget->setThrosholdAndCurrentValue(1<<nBitLen,0,m_nWindowWidth);
    m_pKxccdCtrl->updateWindowInfo(m_nWindowPos,m_nWindowWidth);
}

/******************************************
*功能:参数查询
*输入:
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onGetDevParam()
{
    if(!m_pKxccdCtrl->getDevParam())
    {
         QMessageBox::warning(this,"提示","查询参数失败!");
    }
    else
    {
        //更新滑块位置
        int nIndex = m_pParamSelectComBox->currentIndex();
        m_pParamValueSliderWidget->setThrosholdAndCurrentValue(m_pKxccdCtrl->m_st_ParamInfoList.at(nIndex).nMaxValue,m_pKxccdCtrl->m_st_ParamInfoList.at(nIndex).nMinValue,m_pKxccdCtrl->m_st_ParamInfoList.at(nIndex).nCurrentValue);
    }
}

/******************************************
*功能:保存参数
*输入:
*输出:
*返回值:
*修改记录:
******************************************/
void QKxccdctrlWidget::onSaveDevParam()
{
    if(!m_pKxccdCtrl->saveDevParam())
    {
         QMessageBox::warning(this,"提示","保存参数失败!");
    }
}

/******************************************
* 功能:搜索相机功能
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::onSearchDev()
{
    if(m_pSearchDevWidget != NULL)
    {
        m_pSearchDevWidget->close();
        delete m_pSearchDevWidget;
    }

    m_pSearchDevWidget = new QSearchDevWidget(m_pKxccdCtrl);

    m_pSearchDevWidget->show();

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::onChangeLineType(int nType)
{
    m_nLineType = nType;
    if(m_nLineType == E_PAINT_NON)
    {
        this->setCursor(Qt::ArrowCursor);
    }

    this->update();
}
/******************************************
* 功能:面积测量
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::onAreaMeasure()
{
    m_nLineType = E_PAINT_RECT;
    m_tStartPoint = QPoint(0,0);
    m_tEndPoint = QPoint(0,0);

    m_pMeasureWidget->InitUI(m_nLineType);
    m_pMeasureWidget->setWindowTitle("面积测量");
    m_pMeasureWidget->show();

}

/******************************************
* 功能:线测量
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QKxccdctrlWidget::onLineMeasure()
{
    m_nLineType = E_PAINT_LINE_X;
    m_tStartPoint = QPoint(0,0);
    m_tEndPoint = QPoint(0,0);

    m_pMeasureWidget->InitUI(m_nLineType);
    m_pMeasureWidget->setWindowTitle("线灰度测量");
    m_pMeasureWidget->show();
}

/******************************
*功能:计算光斑面积
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
 void QKxccdctrlWidget::onCalcuArea()
 {
     double dArea = 0;

     if(m_Pixmap.isNull())
     {
         return;
     }

     m_Pixmap.save("E:\\ccd.bmp");
     Mat image = imread("E:\\ccd.bmp");
     Mat imageCV = image;

     //高斯去噪
     Mat ImageGuss;
     cvtColor(imageCV,ImageGuss,COLOR_RGB2GRAY);
     Mat ImageBlur;
     GaussianBlur(ImageGuss,ImageBlur,Size(15,15),0,0);


     //binarilization
     Mat ImageBinary;
     threshold(ImageBlur,ImageBinary,0,255,THRESH_OTSU);


     //查找轮廓
     vector<vector<Point>> contours;
     vector<Vec4i> hireachy;

     findContours(ImageBinary,contours,hireachy,RETR_LIST,CHAIN_APPROX_SIMPLE,Point());
     for (size_t t = 0; t < contours.size();t++)
     {
         double area = contourArea(contours[t]);
         if (area > dArea){dArea = area;}
     }

     QFile file("E:\\ccd.bmp");
     if (file.exists())
     {
         file.remove();
     }

     QMessageBox::information(this,"光斑面积",QString("光斑面积为:%1mm^2").arg(dArea*169/1000000));
 }
