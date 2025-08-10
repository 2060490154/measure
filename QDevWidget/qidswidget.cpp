/***************************************************************************
**                                                                                               **
**QIDSWidget为显微控制组件
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
#include "qidswidget.h"
#include <qgraphicseffect.h>
#include <qfile.h>
#include <QGridLayout>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <QTime>
#include <QDateTime>
#include <qimagereader.h>
#include <qfiledialog.h>
#include "quiqss.h"
#include <QDebug>
#include "qudpcs.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QtSql>

QIDSWidget::QIDSWidget(QWidget *parent) : QWidget(parent)
{
    m_pIDSDevCtrl = NULL;
    m_nRcvFrameCnt = 0;
    m_lfScal = 1.0;
    Save_filename = "C:/Users/caep/Desktop/image_one" ;//yhy0319

    m_sCurrentGray="x=0,y=0,gray=0";


    for (int i = 0; i < 256; i++)
    {
        m_vcolorTable.append(qRgb(i, i, i));
    }

    InitUI();

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

//初始化UI
void QIDSWidget::InitUI()
{

    InitDock();

    m_pImageLabel = new  QLabel("显微监视图像",this);
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
    layout->setMargin(0);


    this->setLayout(layout);

}


/***************************************************
 * 创建左侧DOCK
 * *************************************************/
void QIDSWidget::InitDock()
{

    //初始化 dock 界面
    m_pdockWidget = new QDockWidget();
    m_pdockWidget->setWindowTitle(tr("显微监视操作"));
    m_pdockWidget->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
    //m_pdockWidget->setStyleSheet("border: 1px solid #3C80B1;");

    //状态显示
    m_pStatusLabel = new QLabel("状态显示",this);

    m_plogList = new QListWidget();



    //按钮
    m_pConnectButton = new QPushButton();
    m_pConnectButton->setText(("连接"));
    connect(m_pConnectButton,SIGNAL(clicked()),this,SLOT(onConnectDev()));
    m_pConnectButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    m_pDisConnectButton = new QPushButton();
    m_pDisConnectButton->setText(("断开"));
    connect(m_pDisConnectButton,SIGNAL(clicked()),this,SLOT(onDisConnectDev()));
    m_pDisConnectButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pchoosePathButton = new QPushButton();
    m_pchoosePathButton->setText(("选择保存路径"));
    connect(m_pchoosePathButton,SIGNAL(clicked()),this,SLOT(choosePathButton()));
    m_pchoosePathButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);//yhy0319

    m_pOpenImageButton = new QPushButton();
    m_pOpenImageButton->setText(("打开图像"));
    connect(m_pOpenImageButton,SIGNAL(clicked()),this,SLOT(onOpenImageFile()));
    m_pOpenImageButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_pSaveImageButton = new QPushButton();
    m_pSaveImageButton->setText(("保存图像"));
    connect(m_pSaveImageButton,SIGNAL(clicked()),this,SLOT(onSaveAsImageFile()));
    m_pSaveImageButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //设置按钮图片

    SetButtonICon(m_pOpenImageButton,":/png/openfile.png");
    SetButtonICon(m_pSaveImageButton,":/png/saveas.png");

    //曝光时间设置
    m_pSetExposureTimeBtn = new QPushButton("设置");
    m_pSetExposureTimeBtn->setMinimumSize(40,17);
    connect(m_pSetExposureTimeBtn,&QPushButton::clicked,this,&QIDSWidget::onSetExposureTime);
    m_pExposureTimeEdit = new QLineEdit("20");
    QHBoxLayout *tem_ExposeHLayout = new QHBoxLayout();
    m_ppathLineEdit = new QLineEdit(Save_filename);
    m_ppathLineEdit->setReadOnly(true); //yhy0319
    tem_ExposeHLayout->addWidget(new QLabel("曝光时间: "));
    tem_ExposeHLayout->addWidget(m_pExposureTimeEdit);
    tem_ExposeHLayout->addWidget(m_pSetExposureTimeBtn);
    tem_ExposeHLayout->setMargin(0);
    tem_ExposeHLayout->setSpacing(3);


    //Dockwidget布局
    QGridLayout* dockWidgetlayout = new QGridLayout();

    for(int i = 0; i < 8; i++)
    {
        dockWidgetlayout->setColumnStretch(i,1);//设置列比例
        dockWidgetlayout->setRowStretch(i,4);
    }
    dockWidgetlayout->setRowStretch(3,2);

    dockWidgetlayout->addWidget(CreateOperationGroupBox(),0,0,1,8);
    dockWidgetlayout->addWidget(CreateWorkModeGroupBox(),1,0,1,8);
    dockWidgetlayout->addLayout(tem_ExposeHLayout,2,0,1,8);

     dockWidgetlayout->addWidget(CreateImageProcessGroup(),3,0,1,8);


    dockWidgetlayout->addWidget(m_pStatusLabel,4,0,1,8);
    dockWidgetlayout->addWidget(m_plogList,5,0,5,8);


    QWidget* dockWidget = new QWidget(this);
    dockWidget->setLayout(dockWidgetlayout);
    m_pdockWidget->setWidget(dockWidget);
}



/***************************************************
 * 创建工作模式选择groupbox
 * *************************************************/
QGroupBox* QIDSWidget::CreateWorkModeGroupBox()
{
    QGroupBox *groupBox = new QGroupBox(tr("工作方式"));

     m_pContinueActiveCheckBox = new QCheckBox(tr("连续内触发"));
     m_pHardwareSynActiveCheckBox = new QCheckBox(tr("外触发"));


    connect(m_pContinueActiveCheckBox,SIGNAL(clicked()),this,SLOT(onContinueActiveMode()));
    connect(m_pHardwareSynActiveCheckBox,SIGNAL(clicked()),this,SLOT(onHardwareActiveMode()));



    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(m_pContinueActiveCheckBox);
    vbox->addWidget(m_pHardwareSynActiveCheckBox);

    vbox->setSpacing(0);

    groupBox->setLayout(vbox);

    return groupBox;
}

/***************************************************
 * 创建操作groupbox
 * *************************************************/
QGroupBox* QIDSWidget::CreateOperationGroupBox()
{
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
QGroupBox* QIDSWidget::CreateImageProcessGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr(""));

    QVBoxLayout *vBox = new QVBoxLayout(groupBox);

    QHBoxLayout *pathHBox = new QHBoxLayout();

    pathHBox->addWidget(m_ppathLineEdit);
    pathHBox->addWidget(m_pchoosePathButton);

    vBox->addLayout(pathHBox);

    QHBoxLayout *buttonHBox = new QHBoxLayout();

    buttonHBox->addWidget(m_pOpenImageButton);
    buttonHBox->addWidget(m_pSaveImageButton);

    vBox->addLayout(buttonHBox);

//    vBox->setSpacing(10);
//    vBox->setContentsMargins(10, 10, 10, 10);
    return groupBox;
}//yhy0319
//设置复选框  互斥选择
void QIDSWidget::SetCheckBoxStatus(QCheckBox* pBox,bool bCheck)
{
    m_pContinueActiveCheckBox->setChecked(false);
    m_pHardwareSynActiveCheckBox->setChecked(false);

    pBox->setChecked(bCheck);
}

//连接设备
void QIDSWidget::onConnectDev()
{
    if(!m_pIDSDevCtrl->OpenCamera())
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
void QIDSWidget::onDisConnectDev()
{
    m_pIDSDevCtrl->CloseCamera();
    m_pConnectButton->setEnabled(true);
    m_pDisConnectButton->setEnabled(false);
    showDevStatus();
}

//软件连续触发
void QIDSWidget::onContinueActiveMode()
{
    if(m_pIDSDevCtrl == NULL)
    {
        m_pContinueActiveCheckBox->setEnabled(false);
        return;
    }


    m_pContinueActiveCheckBox->setEnabled(false);
    if(m_pContinueActiveCheckBox->isChecked())
    {
        SetCheckBoxStatus(m_pContinueActiveCheckBox,true);


        bool bRel = m_pIDSDevCtrl->SetTriggerMode(M_INNER_VIDEO);
        bool bCap =  m_pIDSDevCtrl->StartCapImage(true);

        if(bRel == true && bCap == true)
        {
            ShowLogInfo("开始连续触发采集!");
            m_nRcvFrameCnt = 0;
        }
        else
        {
            ShowLogInfo("连续触发采集失败!");
            SetCheckBoxStatus(m_pContinueActiveCheckBox,false);
        }

    }
    else
    {
        SetCheckBoxStatus(m_pContinueActiveCheckBox,false);
        m_pIDSDevCtrl->StopCapImage();
        ShowLogInfo("结束连续触发采集!");
    }

    m_pContinueActiveCheckBox->setEnabled(true);

    showDevStatus();

}

//硬件触发
void QIDSWidget::onHardwareActiveMode()
{
    if(m_pIDSDevCtrl == NULL)
    {
        m_pHardwareSynActiveCheckBox->setEnabled(false);
        return;
    }

    m_pHardwareSynActiveCheckBox->setEnabled(false);
    if(m_pHardwareSynActiveCheckBox->isChecked())
    {
        SetCheckBoxStatus(m_pHardwareSynActiveCheckBox,true);

        bool bRel = m_pIDSDevCtrl->SetTriggerMode(M_EXTERN_HI);
        bool bCap =  m_pIDSDevCtrl->StartCapImage(true);

        if(bRel == true && bCap == true)
        {
            ShowLogInfo("开始外触发采集!");
            m_nRcvFrameCnt = 0;
        }
        else
        {
            ShowLogInfo("开始外触发采集失败!");
            SetCheckBoxStatus(m_pHardwareSynActiveCheckBox,false);
        }

    }
    else
    {
        SetCheckBoxStatus(m_pHardwareSynActiveCheckBox,false);
        m_pIDSDevCtrl->StopCapImage();
        ShowLogInfo("结束外触发采集!");
    }

    m_pHardwareSynActiveCheckBox->setEnabled(true);

    showDevStatus();

}


//显示图像
void QIDSWidget::onShowImage(QString sFilePath)
{
    if (!QFile(sFilePath).exists())
    {
        return;
    }


    m_Pixmap.load(sFilePath);

    m_nRcvFrameCnt++;

    m_nOrginalWidth = m_Pixmap.width();
    m_nOrigalHeight = m_Pixmap.height();

    this->update();
}

//t添加log信息
void QIDSWidget::ShowLogInfo(QString slog)
{
    QTime time = QTime::currentTime();

    QString str = time.toString("hh:mm:ss") +"    "+ slog;


    m_plogList->addItem(str);
    m_plogList->setCurrentRow(m_plogList->count()-1);
}

//初始化状态显示
void QIDSWidget::showDevStatus()
{
    QString sStatus;
    if(m_pIDSDevCtrl->m_hCam != 0)
    {
        m_pConnectButton->setEnabled(false);
        sStatus = "连接状态:已连接";
    }
    else
    {
         m_pDisConnectButton->setEnabled(false);
         sStatus = "连接状态:未连接";
    }
    if(m_pIDSDevCtrl->m_nCurrentTriggerMode == M_EXTERN_HI)
    {
        sStatus = sStatus + "\n\n" +"工作模式:外触发单帧模式\n\n"+"采集状态:正在采集";
    }
    else if(m_pIDSDevCtrl->m_nCurrentTriggerMode == M_INNER_VIDEO)
    {
        sStatus = sStatus + "\n\n" +"工作模式:内触发连续模式\n\n"+"采集状态:正在采集";
    }
    else
    {
        sStatus = sStatus + "\n\n" +"工作模式:无\n\n"+"采集状态:未采集";
    }

    sStatus = sStatus + "\n\n"+"帧计数:"+QString("%1").arg(m_nRcvFrameCnt);

    sStatus = sStatus + "\n\n"+"当前灰度:"+m_sCurrentGray;

    sStatus = sStatus + "\n\n"+"缩放比例:"+QString("%1").arg(m_lfScal,0,'g',4);

    m_pStatusLabel->setText(sStatus);
}

/****************************************************
 *
 * 绘图
 * **************************************************/
void QIDSWidget::paintEvent(QPaintEvent * event)
{
    QPen pen; //设置红色画笔
    pen.setColor(QColor(255,0,0));

    if (!m_Pixmap.isNull())
    {
        m_Pixmap = m_Pixmap.scaled(m_nOrginalWidth*m_lfScal,m_nOrigalHeight*m_lfScal);
        m_pImageLabel->setAlignment(Qt::AlignLeft);//靠左显示
        m_pImageLabel->setPixmap(m_Pixmap);
    }
    showDevStatus();
}


//设置设备句柄
void QIDSWidget::getDevHandle(QIDSCamerDevCtrl* pDevCtrl)
{
    m_pIDSDevCtrl = pDevCtrl;
    connect(m_pIDSDevCtrl,&QIDSCamerDevCtrl::signal_showImageBuff,this,&QIDSWidget::onShowImage,Qt::DirectConnection);//显示图像数据,同步调用
}


//设置按钮图标
void QIDSWidget::SetButtonICon(QPushButton* pButton,QString sPngName)
{
    QPixmap pixmap(sPngName);
    pButton->setIcon(pixmap);
    pButton->setIconSize(pixmap.size());
}
//选择保存路径//yhy0319
void QIDSWidget::choosePathButton()
{
    QString selectedPath = QFileDialog::getExistingDirectory(this,"chooseDir","/");
    if (!selectedPath.isEmpty()) {
        Save_filename = selectedPath;
        m_ppathLineEdit->setText(selectedPath);
    }
}
//显示图片
void QIDSWidget::onOpenImageFile()
{
    if(m_pContinueActiveCheckBox->isChecked() || m_pHardwareSynActiveCheckBox->isChecked())
    {
        return;
    }

    QString sFilePath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.jpg)"));

    if(sFilePath.length() != 0)
    {
        m_ImageBuff.load(sFilePath);
        if(!m_ImageBuff.isNull() )
        {
            m_ImageTempBuff = m_ImageBuff.scaled(m_ImageBuff.width()*m_lfScal,m_ImageBuff.height()*m_lfScal);
            m_Pixmap = QPixmap::fromImage(m_ImageTempBuff);
            m_nOrginalWidth = m_Pixmap.width();
            m_nOrigalHeight = m_Pixmap.height();
        }
        this->update();
    }

}
//保存图片//yhy0319
void QIDSWidget::onSaveAsImageFile()
{
    QUDPcs *UDPcs = new QUDPcs();
    UDPcs->GET_one();
    UDPcs->waitForDealMsg();
    bool res = UDPcs->lastDealMsgResult;
    if(res){
        QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
        QString dsn = QString::fromLocal8Bit("QtLocalDSN");
        db.setDatabaseName(dsn);
        db.setHostName("local");  //数据库主机名
        db.setPort(1433);
        db.setUserName("sa");
        db.setPassword("123456");
        QString sFolderName = Save_filename;
        if(!db.open())
        {
            qDebug("=== %s",qPrintable(db.lastError().text()));
            QMessageBox msgBox;

            msgBox.setWindowTitle("連接成功");

            msgBox.setText("!db.open()");
            msgBox.setStandardButtons(QMessageBox::Ok);

            msgBox.exec();

        }else
        {
            QString sSql = QString("select top 1 * from T_SINGLE_IMAGE order by time desc");
            QSqlQuery sqlQuery(db);
            bool bOk = sqlQuery.exec(sSql);
            if (bOk==true)
            {

            }
            else
            {
                QMessageBox msgBox;

                msgBox.setWindowTitle("連接失敗");

                msgBox.setText(sqlQuery.lastError().text());
                msgBox.setStandardButtons(QMessageBox::Ok);

                msgBox.exec();
            }
            while(sqlQuery.next())
            {
                QByteArray l_byteArray;
                l_byteArray = sqlQuery.value(0).toByteArray();
                if(l_byteArray.size()>0)
                {
                    QFile l_file(QString("%1/%2.jpg").arg(sFolderName).arg(sqlQuery.value(1).toString()));
                    l_file.open(QFile::ReadWrite);
                    l_file.write(l_byteArray);
                    l_file.close();
                }
            }
            //wzj0324
             db.close();
        }
    }else{

        QMessageBox msgBox;

        msgBox.setWindowTitle("错误");

        msgBox.setText("保存失败，请重新尝试！");

        msgBox.setStandardButtons(QMessageBox::Ok);

        msgBox.exec();
    }
}


/******************************
*功能:设置曝光时间
*输入:
*输出:
*返回值:
*修改记录:190822 edited by Yi
*******************************/
void QIDSWidget::onSetExposureTime()
{
    double tem_ExposureTime = m_pExposureTimeEdit->text().toDouble();
    if (tem_ExposureTime < 0 || tem_ExposureTime > 39)
    {
        QMessageBox messageBox(QMessageBox::Warning,"提示","超出范围！\n当前可调曝光时间范围是：0ms ~ 39ms");
        messageBox.setStandardButtons (QMessageBox::Ok);
        messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
        messageBox.exec ();
        return;
    }
    else
    {
        m_pIDSDevCtrl->SetExposureMode(tem_ExposureTime);
    }

}


//鼠标移动操作
void QIDSWidget::mouseMoveEvent(QMouseEvent *event)
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

    pos_x = pos_x/m_lfScal;
    pos_y = pos_y/m_lfScal;

    int nGray = qRed(m_ImageBuff.pixel(pos_x,pos_y));


    m_sCurrentGray = QString("x=%1 y=%2 gray=%3").arg(pos_x).arg(pos_y).arg(nGray);

    showDevStatus();


}

void QIDSWidget::wheelEvent(QWheelEvent *event)
{
    if( m_Pixmap.isNull())
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
