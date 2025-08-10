/***************************************************************************
**                                                                                               **
**QMotorCtrlWidget为电机控制组件，包含多个QMotorCtrlItemWidget，每个
** QMotorCtrlItemWidget对应一个电机
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
#include "qmotorctrlwidget.h"
#include "qmessagebox.h"
#include <QTime>
#include <QDebug>
QMotorCtrlWidget::QMotorCtrlWidget(QList<tMotorConfigInfo>* pConfigList,QWidget *parent) : QWidget(parent)
{
    m_pMotorConfigInfoList = pConfigList;


    InitUI();

}

void QMotorCtrlWidget::InitUI()
{
    QGridLayout* playout = new QGridLayout(this);
    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,1);
    for(int i = 0; i < m_pMotorConfigInfoList->size() ; i++)
    {
        m_pMotorWidget[i] = new QMotorCtrlItemWidget(&m_pMotorConfigInfoList->at(i),NULL);
        playout->addWidget(m_pMotorWidget[i],i/4,i%4,1,1);
    }

}

//获得设备操作指针
void QMotorCtrlWidget::getDevHandle(QMotorDevManager* pMotorDevManager)
{
    for(int i = 0; i <  m_pMotorConfigInfoList->size() ; i++)
    {
        tMotorConfigInfo tTempconfig = m_pMotorConfigInfoList->at(i);
        m_pMotorWidget[i]->m_pMotorCtrl = pMotorDevManager->m_pMotorCtrl[tTempconfig.nMotorCtrlIndex] ;//每个电机控制器 控制4个电机
        m_pMotorWidget[i]->m_pRunThread = new QMotorRunThread( m_pMotorWidget[i]->m_pMotorCtrl );//创建执行线程
        connect(m_pMotorWidget[i]->m_pMotorCtrl, &QMotorCtrl::UpdateStatus, m_pMotorWidget[i], &QMotorCtrlItemWidget::onUpdateMotorInfo);//关联显示更新

        m_pMotorWidget[i]->m_pMotorCtrl->m_pMotorTypeCoeff[tTempconfig.nMotorIndex].m_nMotorCoeff = tTempconfig.nMotorCoeff;
        m_pMotorWidget[i]->m_pMotorCtrl->m_pMotorTypeCoeff[tTempconfig.nMotorIndex].m_nMotorType = tTempconfig.nMortorType;

    }
}

//单个电机的控制模块
QMotorCtrlItemWidget::QMotorCtrlItemWidget(const tMotorConfigInfo* pMotorConfigInfo,QMotorCtrl* pMotorCtrl,QWidget *parent) : QGroupBox(parent)
{
    m_pMotorConfigInfo = pMotorConfigInfo;
    m_nMotorCtrlIndex = m_pMotorConfigInfo->nMotorCtrlIndex;//控制箱编号
    m_nMotorIndex = m_pMotorConfigInfo->nMotorIndex;//计算电机在控制箱中的编号

    m_pMotorCtrl = pMotorCtrl;



    InitUI();

    showLimitStatus(0,0);
    if(m_pMotorCtrl != NULL)
    {
        connect(m_pMotorCtrl,&QMotorCtrl::UpdateStatus,this,&QMotorCtrlItemWidget::onUpdateMotorInfo);
    }

}

//UI初始化
void QMotorCtrlItemWidget::InitUI()
{
  if(m_pMotorConfigInfo->tPostionConfig.size() == 0)
  {
      creatNoPostionUI();
      connect(m_pMoveLeftBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onMoveLeft);
      connect(m_pMoveRightBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onMoveRight);
      connect(m_pImmidStopBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onStopMove);
      connect(m_pReturnZeroBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::OnMove2Zero);
  }
  else
  {
      creatPostionUI();

      foreach (tMotorPostionInfo item, m_pMotorConfigInfo->tPostionConfig)
      {
          m_pPostionCombox->addItem(item.sPostionName);
      }
      connect(m_pStartMoveBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onStartMove);
      connect(m_pImmidStopBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onStopMove);

  }

}

//创建不带档位信息的UI
void QMotorCtrlItemWidget::creatNoPostionUI()
{
    QString str;
    this->setTitle(m_pMotorConfigInfo->sMotorName);

    m_pLogList = new QListWidget();
    m_pLogList->setMinimumHeight(100);

    QLabel* pLabel1 = new QLabel("运动位置");
    QLabel* pLabel2 = new QLabel("mm");
    QLabel* pLabel3 = new QLabel("mm");

    if(m_pMotorConfigInfo->nMortorType == 1)
    {
        pLabel2->setText("度");
        pLabel3->setText("度");
    }


    m_pPostionEdit = new QTextEdit();
    m_pMoveLeftBtn = new QPushButton("");
    m_pMoveRightBtn = new QPushButton("");
    m_pImmidStopBtn = new QPushButton("");
    m_pReturnZeroBtn = new QPushButton("");
    m_pLeftLimitStatusLabel = new QLabel();
    m_pRightLimitStatusLabel = new QLabel();
    m_pCurrentPosLabel = new QLabel("0");


    //设置大小
    m_pPostionEdit->setMaximumSize(QSize(400,30));
    m_pMoveLeftBtn->setMinimumSize(QSize(50,50));
    m_pMoveLeftBtn->setMaximumSize(QSize(50,50));
    m_pMoveRightBtn->setMaximumSize(QSize(50,50));
    m_pMoveRightBtn->setMinimumSize(QSize(50,50));
    m_pImmidStopBtn->setMaximumSize(QSize(50,50));
    m_pImmidStopBtn->setMinimumSize(QSize(50,50));
    m_pReturnZeroBtn->setMaximumSize(QSize(50,50));
    m_pReturnZeroBtn->setMinimumSize(QSize(50,50));

    //设置按钮图标
    m_pMoveLeftBtn -> setIcon(QIcon(":/png/left.png")); //将图片设置到按钮上
    m_pMoveLeftBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pMoveLeftBtn->setFlat(true);
    m_pMoveLeftBtn->setToolTip("向左运动");

    m_pMoveRightBtn -> setIcon(QIcon(":/png/right.png")); //将图片设置到按钮上
    m_pMoveRightBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pMoveRightBtn->setFlat(true);
    m_pMoveRightBtn->setToolTip("向右运动");

    m_pImmidStopBtn -> setIcon(QIcon(":/png/stop.png")); //将图片设置到按钮上
    m_pImmidStopBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pImmidStopBtn->setFlat(true);
    m_pImmidStopBtn->setToolTip("立即停止运动");

    m_pReturnZeroBtn -> setIcon(QIcon(":/png/zero.png")); //将图片设置到按钮上
    m_pReturnZeroBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pReturnZeroBtn->setFlat(true);
    m_pReturnZeroBtn->setToolTip("归零");


    m_pLeftLimitStatusLabel->setToolTip("左限位");
    m_pRightLimitStatusLabel->setToolTip("右限位");

    QGridLayout* playout = new QGridLayout(this);

    //playout->setSpacing(0);
    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,1);
    playout->setRowStretch(3,1);



    playout->addWidget(pLabel1,0,0,1,1);
    playout->addWidget(m_pPostionEdit,0,1,1,4);
    playout->addWidget(pLabel3,0,5,1,1);


    playout->addWidget(new QLabel("限位状态"),1,0,1,1);
    playout->addWidget(m_pLeftLimitStatusLabel,1,2,1,1);
    playout->addWidget(m_pRightLimitStatusLabel,1,3,1,1);

    playout->addWidget(new QLabel("当前位置"),2,0,1,1);
    playout->addWidget(m_pCurrentPosLabel,2,1,1,1);
    playout->addWidget(pLabel2,2,2,1,1);

    playout->addWidget(m_pMoveLeftBtn,3,1,1,1);
    playout->addWidget(m_pImmidStopBtn,3,2,1,1);
    playout->addWidget(m_pMoveRightBtn,3,3,1,1);
    playout->addWidget(m_pReturnZeroBtn,3,4,1,1);

     playout->addWidget(m_pLogList,4,0,1,6);

}

/*******************************************************************
**功能：创建待档位的电机控制界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::creatPostionUI()
{

    QString str;
    this->setTitle(m_pMotorConfigInfo->sMotorName);

    m_pLogList = new QListWidget();
    m_pLogList->setMinimumHeight(100);

    QLabel* pLabel1 = new QLabel("运动位置");

    m_pPostionCombox = new QComboBox(this);
    m_pStartMoveBtn = new QPushButton("");
    m_pLeftLimitStatusLabel = new QLabel();
    m_pRightLimitStatusLabel = new QLabel();
    m_pCurrentPosLabel = new QLabel("0");
    m_pImmidStopBtn = new QPushButton("");


    //设置大小
    m_pPostionCombox->setMaximumSize(QSize(400,50));
    m_pPostionCombox->setMinimumHeight(30);
    m_pStartMoveBtn->setMinimumSize(QSize(50,50));

    m_pImmidStopBtn->setMaximumSize(QSize(50,50));
    m_pImmidStopBtn->setMinimumSize(QSize(50,50));

    //设置按钮图标
    m_pStartMoveBtn -> setIcon(QIcon(":/png/start.png")); //将图片设置到按钮上
    m_pStartMoveBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pStartMoveBtn->setFlat(true);
    m_pStartMoveBtn->setToolTip("运动");

    m_pImmidStopBtn -> setIcon(QIcon(":/png/stop.png")); //将图片设置到按钮上
    m_pImmidStopBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pImmidStopBtn->setFlat(true);
    m_pImmidStopBtn->setToolTip("立即停止运动");


    m_pLeftLimitStatusLabel->setToolTip("左限位");
    m_pRightLimitStatusLabel->setToolTip("右限位");

    QGridLayout* playout = new QGridLayout(this);

    //playout->setSpacing(0);
    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,1);
    playout->setRowStretch(3,1);



    playout->addWidget(pLabel1,0,0,1,1);
    playout->addWidget(m_pPostionCombox,0,1,1,4);


    playout->addWidget(new QLabel("限位状态"),1,0,1,1);
    playout->addWidget(m_pLeftLimitStatusLabel,1,2,1,1);
    playout->addWidget(m_pRightLimitStatusLabel,1,3,1,1);

    playout->addWidget(new QLabel("当前位置"),2,0,1,1);
    playout->addWidget(m_pCurrentPosLabel,2,1,1,1);


    playout->addWidget(m_pStartMoveBtn,3,1,1,1);
    playout->addWidget(m_pImmidStopBtn,3,3,1,1);



     playout->addWidget(m_pLogList,4,0,1,6);

}

/*******************************************************************
**功能：显示限位信息
**输入：nLeftStatus--左限位状态 nRightStatus--右限位状态
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::showLimitStatus(int nLeftStatus,int nRightStatus)
{
    QString sLimitStatus[2] = {":/png/connect.png",":/png/disconnect.png"};//处于左限位状态  0--未处于 1--处于限位状态


    m_pLeftLimitStatusLabel->setText("\
                    <table width=\"100%\">\
                    <tbody>\
                    <tr>\
                    <td><img src=\""+sLimitStatus[nLeftStatus]+"\"></td>\
                    </tr>\
                    </tbody>\
                    </table>");

    m_pRightLimitStatusLabel->setText("\
                    <table width=\"100%\">\
                    <tbody>\
                    <tr>\
                    <td><img src=\""+sLimitStatus[nRightStatus]+"\" ></td>\
                    </tr>\
                    </tbody>\
                    </table>");

}

/*******************************************************************
**功能：更新状态显示信息
**输入：控制箱编号(0-2)  电机编号(0-3)  状态数据
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::onUpdateMotorInfo(int nMotorDevIndex,int nMotorIndex,QVariant varStatus)
{
    motorStatusdata tTempmotorStatus = varStatus.value<motorStatusdata>();
    motorStatusdata* pStatus  = &tTempmotorStatus;

    QString str;
    if(nMotorIndex != m_nMotorIndex || nMotorDevIndex != m_nMotorCtrlIndex )
    {
        return ;
    }

     showLimitStatus(pStatus->m_nLeftLimitStatus,pStatus->m_nRightLimitStatus);//更新限位信息
     if(m_pCurrentPosLabel != NULL && m_pMotorConfigInfo->tPostionConfig.size() == 0)//不带档位 显示步数信息
     {
         if (m_pMotorConfigInfo->nMortorType == 0)
         {
             m_pCurrentPosLabel->setText(QString("%1").arg((float)(pStatus->m_nCurrentLPV*(-1))/(float)m_pMotorConfigInfo->nMotorCoeff,0,'f',2));//需要转换
         }
         else
         {
             m_pCurrentPosLabel->setText(QString("%1").arg((float)(pStatus->m_nCurrentLPV)/(float)m_pMotorConfigInfo->nMotorCoeff,0,'f',2));//需要转换
         }
         m_pCurrentPosLabel->setStyleSheet("background-color:rgb(255,255,255)");
     }
     else     if(m_pMotorCtrl-> m_tMotorStatusdata[m_nMotorIndex].m_nRunState == M_STEP_STATUS_RUNNING)//正在运动
     {
        m_pCurrentPosLabel->setStyleSheet("background-color:rgb(255,255,0)");
     }
     else     if(m_pMotorCtrl-> m_tMotorStatusdata[m_nMotorIndex].m_nRunState == M_STEP_STATUS_FAILD)//运动失败
     {
        m_pCurrentPosLabel->setStyleSheet("background-color:rgb(255,0,0)");
     }
     else    if(m_pMotorCtrl-> m_tMotorStatusdata[m_nMotorIndex].m_nRunState == M_STEP_STATUS_SUCCESS)//运动成功
     {
        m_pCurrentPosLabel->setStyleSheet("background-color:rgb(0,255,0)");
     }

}
/*******************************************************************
**功能：向左运动
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::onMoveLeft()
{
    bool bOK = false;
    if(!m_pMotorCtrl->OpenDev())
    {
        setlogInfo("未连接到电机设备，无法执行该操作");
        return;
    }

    float nPos = m_pPostionEdit->toPlainText().toFloat(&bOK);//
   // m_pMotorCtrl->m_pMotorTypeCoeff[m_nMotorIndex].m_nMotorCoeff = m_pMotorConfigInfo->nMotorCoeff;
    if(bOK == true)
    {
        bool bRel =  m_pMotorCtrl->MoveToPosition(m_nMotorIndex,nPos);
        if(bRel == false)
        {
            setlogInfo("执行向左运动失败");
            return;
        }
    }
    setlogInfo("执行向左运动");
}

/*******************************************************************
**功能：向右运动
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::onMoveRight()
{
    bool bOK = false;
    if(!m_pMotorCtrl->OpenDev())
    {
        setlogInfo("未连接到电机设备，无法执行该操作");
        return;
    }

    float nPos = m_pPostionEdit->toPlainText().toFloat(&bOK);//*m_pMotorConfigInfo->nMotorCoeff;
   // m_pMotorCtrl->m_pMotorTypeCoeff[m_nMotorIndex].m_nMotorCoeff = m_pMotorConfigInfo->nMotorCoeff;
    if(bOK == true)
    {
       bool bRel =  m_pMotorCtrl->MoveToPosition(m_nMotorIndex,nPos*(-1));
       if(bRel == false)
       {
           setlogInfo("执行向右运动失败");
           return;
       }
    }
    setlogInfo("执行向右运动");
}

/*******************************************************************
**功能：开始运动 运动到指定档位
**输入：档位选择
**输出：当前档位
**返回值：无
*******************************************************************/
void QMotorCtrlItemWidget::onStartMove()
{

    if(!m_pMotorCtrl->OpenDev())
    {
       setlogInfo("未连接到电机设备，无法执行该操作");
        return;
    }

    int nCurrentSel = m_pPostionCombox->currentIndex();

    tMotorPostionInfo pTemInfo = m_pMotorConfigInfo->tPostionConfig[nCurrentSel];
    int nPos = pTemInfo.lfPostionNum*m_pMotorConfigInfo->nMotorCoeff;


    setlogInfo("开始运动...");

    if(m_pRunThread->isRunning())
    {
        setlogInfo("电机正在运动，请等待运动结束！");
        return;
    }

    m_pCurrentPosLabel->setText(pTemInfo.sPostionName);

    //启动线程 耗时较长 需要放到线程中
    m_pRunThread->m_nMotorIndex = m_nMotorIndex;
    m_pRunThread->m_nPosition = nPos;
    m_pRunThread->start();


}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::OnMove2Zero()
{
    if(!m_pMotorCtrl->OpenDev())
    {
        setlogInfo("未连接到电机设备，无法执行该操作");
        return;
    }
#if 1
     int nPos = 0;
    // m_pMotorCtrl->m_pMotorTypeCoeff[m_nMotorIndex].m_nMotorCoeff = m_pMotorConfigInfo->nMotorCoeff;
    if(m_pMotorConfigInfo->nMortorType == 0)
    {
         nPos = -160;//平移台最长不超过160mm
    }
    if(m_pMotorConfigInfo->nMortorType == 1)
    {
        nPos = 361;//轮盘最多转1圈
    }
    bool bRel =  m_pMotorCtrl->MoveToPosition(m_nMotorIndex,nPos);
#else
    bool bRel = m_pMotorCtrl->SearchHome(m_nMotorIndex);
#endif


    //第一次归零时显示朝零点方向移动的距离，当再次电机归零按钮时将逻辑计数清零
//    if(m_pMotorCtrl->m_tMotorStatusdata[m_nMotorIndex].m_nLeftLimitStatus == 1 || m_pMotorCtrl->m_tMotorStatusdata[m_nMotorIndex].m_nRightLimitStatus == 1)
//    {
//        m_pMotorCtrl->clearLP(m_nMotorIndex);
//    }

   if(bRel == false)
   {
       setlogInfo("执行归零操作失败");
       return;
   }
    setlogInfo("执行归零操作");

}
/*******************************************************************
**功能：停止当前电机的运动
**输入：无
**输出：无
**返回值：无
*******************************************************************/
void QMotorCtrlItemWidget::onStopMove()
{
    if(!m_pMotorCtrl->OpenDev())
    {
       setlogInfo("未连接到电机设备，无法执行该操作");
        return;
    }

    bool bRel = m_pMotorCtrl->ImmediateStop(m_nMotorIndex);

    if(bRel == true)
    {
        setlogInfo("停止电机运动成功!");
    }
    else
    {
         setlogInfo("停止电机运动失败!");
    }

}

/*******************************************************************
**功能：打印提示信息
**输入：slog 提示信息内容
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::setlogInfo(QString slog)
{
    QTime time = QTime::currentTime();

    QString str = time.toString("hh:mm:ss") +"    "+ slog;


    m_pLogList->addItem(str);
    m_pLogList->setCurrentRow(m_pLogList->count()-1);

}

