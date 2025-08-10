#include "qshutterctrlwidget.h"
#include <QGridLayout>
#include <QMessageBox>

QShutterCtrlWidget::QShutterCtrlWidget(QDevManagerCtrl* pCtrl,QWidget *parent) : QWidget(parent)
{
    m_pShutterCtrl = pCtrl->m_pShutterCtrl;
    m_pPortCtrl1=pCtrl->m_pPortCtrl1;
    m_pPortCtrl2=pCtrl->m_pPortCtrl2;
    m_pPortCtrl3=pCtrl->m_pPortCtrl3;

    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,4);


    for(int i = 0; i < M_SHUTTER_TOTAL_NUM;i++)
    {
         QShutterCtrlItemWidget* pShutterItemWidget = new QShutterCtrlItemWidget(m_pShutterCtrl,i,this);

        playout->addWidget(pShutterItemWidget,i%2,i/2,1,1);//2行2列排列
    }

    QCOMCtrlItemWidget* pShutterItemWidget = new QCOMCtrlItemWidget(m_pPortCtrl1,0,this);
    playout->addWidget(pShutterItemWidget,0,1,1,1);//2行2列排列
    QCOMCtrlItemWidget* pShutterItemWidget2 = new QCOMCtrlItemWidget(m_pPortCtrl2,1,this);
    playout->addWidget(pShutterItemWidget2,1,1,1,1);//2行2列排列
    QCOMCtrlItemWidget* pShutterItemWidget3 = new QCOMCtrlItemWidget(m_pPortCtrl3,2,this);
    playout->addWidget(pShutterItemWidget3,2,1,1,1);//2行2列排列

}

/*******************************************************************
**功能：子组件
**输入：
**输出：
**返回值：
*******************************************************************/
QShutterCtrlItemWidget::QShutterCtrlItemWidget(QShutterDevCtrl* pCtrl,int nIndex,QWidget *parent) : QGroupBox(parent)
{
    _pShutterCtrl = pCtrl;
    _nIndex = nIndex;

    InitUI();

    connect(m_pOpenButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onOpenShutter);
    connect(m_pCloseButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onCloseShutter);
    connect(m_pRefreshButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onCheckStatus);
    connect(m_pSetLimitDataButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onSetLimitData);
    connect(m_psetProcessStatusButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onSetProcessStatus);

}

/*******************************************************************
**功能：初始化界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QShutterCtrlItemWidget::InitUI()
{

    this->setTitle(QString("光闸%1").arg(_nIndex+1));
   // this->setMaximumHeight(150);
    QLabel* plabel1= new QLabel(this);
    plabel1->setText("设备IP:");

    QLabel* plabel2= new QLabel(this);
    plabel2->setText(_pShutterCtrl->m_tShutterDev[_nIndex].tHostAddress.toString());

    QLabel* plabel3= new QLabel(this);
    plabel3->setText("当前状态:");

    QLabel* plabel4= new QLabel(this);
    plabel4->setText("当前采集电压:");

    QLabel* plabel5= new QLabel(this);
    plabel5->setText("mv");


    QLabel* plabel6= new QLabel(this);
    plabel6->setText("设置门限值:");

    QLabel* plabel7= new QLabel(this);
    plabel7->setText("mv");

    QLabel* plabel8= new QLabel(this);
    plabel8->setText("设置脉冲计数");

    QLabel* plabel9= new QLabel(this);
    plabel9->setText("设置延时(ms)");

    m_pStatus = new QLabel(this);
    m_pSetLimitEdit = new QTextEdit(this);
    m_pshowCurrentDataLabel = new QLabel(this);

    m_psetPlusCntEdit = new QTextEdit(this);
    m_psetDelayEdit = new QTextEdit(this);

    m_psetPlusCntEdit->setText("1");
    m_psetDelayEdit->setText("0");

    m_pSetLimitEdit->setText("1");
    m_pshowCurrentDataLabel->setText("0");

    m_pCloseButton = new QPushButton("闭合");
    m_pOpenButton = new QPushButton("打开");
    m_pRefreshButton = new QPushButton("状态查询");
    m_pSetLimitDataButton = new QPushButton("设置电压门限");

    m_psetProcessStatusButton = new QPushButton("设置流程模式");

    m_pCloseButton->setMaximumWidth(100);
    m_pCloseButton->setMinimumHeight(30);

    m_pOpenButton->setMaximumWidth(100);
    m_pOpenButton->setMinimumHeight(30);

    m_pSetLimitDataButton->setMaximumWidth(100);
    m_pSetLimitDataButton->setMinimumHeight(30);

    m_pRefreshButton->setMaximumWidth(100);
    m_pRefreshButton->setMinimumHeight(30);

    m_psetProcessStatusButton->setMaximumWidth(100);
    m_psetProcessStatusButton->setMinimumHeight(30);

    m_pSetLimitEdit->setMaximumWidth(200);
    m_pSetLimitEdit->setMaximumHeight(30);
    m_pshowCurrentDataLabel->setMaximumWidth(200);


    m_psetPlusCntEdit->setMaximumWidth(200);
    m_psetPlusCntEdit->setMaximumHeight(30);

    m_psetDelayEdit->setMaximumWidth(200);
    m_psetDelayEdit->setMaximumHeight(30);


    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(plabel1,0,0,1,1);
    playout->addWidget(plabel2,0,1,1,1);
    playout->addWidget(plabel3,1,0,1,1);
    playout->addWidget(m_pStatus,1,1,1,1);

    playout->addWidget(plabel4,2,0,1,1);
    playout->addWidget(m_pshowCurrentDataLabel,2,1,1,1);
    playout->addWidget(plabel5,2,2,1,1);

    playout->addWidget(plabel6,3,0,1,1);
    playout->addWidget(m_pSetLimitEdit,3,1,1,1);
    playout->addWidget(plabel7,3,2,1,1);


    playout->addWidget(plabel6,3,0,1,1);
    playout->addWidget(m_pSetLimitEdit,3,1,1,1);
    playout->addWidget(plabel7,3,2,1,1);

    playout->addWidget(plabel8,4,0,1,1);
    playout->addWidget(m_psetPlusCntEdit,4,1,1,1);

    playout->addWidget(plabel9,5,0,1,1);
    playout->addWidget(m_psetDelayEdit,5,1,1,1);


    playout->addWidget(m_pOpenButton,6,0,1,1);
    playout->addWidget(m_pCloseButton,6,1,1,1);
    playout->addWidget(m_pRefreshButton,7,0,1,1);
    playout->addWidget(m_pSetLimitDataButton,7,1,1,1);
    playout->addWidget(m_psetProcessStatusButton,8,0,1,1);

    showStatus();

}

void QShutterCtrlItemWidget::showStatus()
{
    if(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentStatus == M_SHUTTER_STATUS_NONE)
    {
        m_pStatus->setText("未知状态");
    }

    if(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentStatus == M_SHUTTER_STATUS_OPENED)
    {
        m_pStatus->setText("打开");
    }
    if(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentStatus == M_SHUTTER_STATUS_CLOSED)
    {
        m_pStatus->setText("闭合");
    }


    QString scurrentdata = QString("%1").arg(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentVoltData);
    m_pshowCurrentDataLabel->setText(scurrentdata);


    QString sdata = QString("%1").arg(_pShutterCtrl->m_tShutterDev[_nIndex].nSetLimitData);
    m_pSetLimitEdit->setText(sdata);



}

void QShutterCtrlItemWidget::onOpenShutter()
{
    bool bRel;

    bRel = _pShutterCtrl->SetDevStatus(_nIndex,true);

    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","打开光闸失败");
        return;
    }

    showStatus();
}

void QShutterCtrlItemWidget::onCloseShutter()
{
    bool bRel;

    bRel = _pShutterCtrl->SetDevStatus(_nIndex,false);

    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","关闸吸合失败");
        return;
    }

    showStatus();
}

//查询状态
void QShutterCtrlItemWidget:: onCheckStatus()
{
    bool bRel = _pShutterCtrl->checkDevStatus();
    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","查询状态失败");
        return;
    }

    showStatus();

}

//设置门限值
void QShutterCtrlItemWidget:: onSetLimitData()
{
    bool bOk = false;
    int nData = m_pSetLimitEdit->toPlainText().toInt(&bOk);
    if(nData <= 0)
    {
        QMessageBox::warning(this,"提示","门限设置必须大于0");
        return;
    }


    bool bRel = _pShutterCtrl->SetDevLimit(_nIndex,nData);
    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","设置门限值失败");
        return;
    }

    showStatus();

}

//设置流程
void QShutterCtrlItemWidget::onSetProcessStatus()
{
    bool bOk = false;
    int nPlusCnt = m_psetPlusCntEdit->toPlainText().toInt(&bOk);
    int nDelayTime = m_psetDelayEdit->toPlainText().toInt(&bOk);

    if(nPlusCnt <= 0 )
    {
        QMessageBox::warning(this,"提示","脉冲数必须大于0");
        return;
    }

    bool bRel = _pShutterCtrl->setProcessStatus(_nIndex,nPlusCnt,nDelayTime);

    if(!bRel )
    {
        QMessageBox::warning(this,"错误","执行超时");
        return;
    }

}

/*******************************************************************
**功能：串口功能函数
**输入：
**输出：
**返回值：
*******************************************************************/

void QCOMCtrlItemWidget::log(const QString &str)
{
    textbrower->append(str);
}

QCOMCtrlItemWidget::QCOMCtrlItemWidget(QAttenuatorDevCtrl* pCtrl,int nIndex,QWidget *parent) : QGroupBox(parent)
{
    _pPortCtrl = pCtrl;
    _nIndex = nIndex;

    InitUI();

    connect(m_pOpenButton,&QPushButton::clicked,this,&QCOMCtrlItemWidget::onOpenCOM);
    connect(m_pCloseButton,&QPushButton::clicked,this,&QCOMCtrlItemWidget::onRelaseCOM);
//    connect(m_pRefreshButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onCheckStatus);
//    connect(m_pSetLimitDataButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onSetLimitData);
//    connect(m_psetProcessStatusButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onSetProcessStatus);

    connect(_pPortCtrl,&QAttenuatorDevCtrl::portsreadsignal,this,&QCOMCtrlItemWidget::DisplayRcvTextAfterSendBtn);
    connect(m_pSendMsgButton,&QPushButton::clicked,this,&QCOMCtrlItemWidget::SlotClickSendMsgButton);
    connect(m_pSendPWRMsgButton,&QPushButton::clicked,this,&QCOMCtrlItemWidget::SlotClickSendPWRMsgButton);
    connect(m_pSendCommandMsgButton,&QPushButton::clicked,this,&QCOMCtrlItemWidget::SlotClickSendCommandMsgButton);
    connect(m_pRefreshButton,&QPushButton::clicked,this,&QCOMCtrlItemWidget::SlotClickSetHomeButton);
    showStatus();

}

void QCOMCtrlItemWidget::InitUI()
{
    if(_nIndex == 0){
        this->setTitle(QString("能量衰减器1064-COM5"));
    }
    if(_nIndex == 1){
        this->setTitle(QString("能量衰减器532-COM6"));
    }
    if(_nIndex == 2){
        this->setTitle(QString("能量衰减器355-COM7"));
    }

   // this->setMaximumHeight(150);
    plabel1= new QLabel;
    plabel1->setText("设备COM:");

    plabel2= new QLabel;
    if(_nIndex == 0){
        plabel2->setText("COM5");
    }
    else if(_nIndex == 1){
        plabel2->setText("COM6");
    }
    else if(_nIndex == 2){
        plabel2->setText("COM7");
    }
//    plabel2->setText(_pPortCtrl->portnameList.at(_nIndex+1));//lbz
//    plabel2->setText(_pShutterCtrl->m_tShutterDev[_nIndex].tHostAddress.toString());

    plabel3= new QLabel;      //应该有用
    plabel3->setText("当前状态:");
    plabel4 = new QLabel;
    plabel4->setText("调整能量:");
    plabel6= new QLabel;      //有用
    plabel6->setText("设置角度");
    plabel8= new QLabel;
    plabel8->setText("发送指令");


    m_pStatus = new QLabel("关闭");
    MsgTextEdit = new QTextEdit;
    PWRLineEdit = new QLineEdit;
    CommandLineEdit = new QLineEdit;
    textbrower = new QTextBrowser;

    MsgTextEdit->setText("0");
    m_pCloseButton = new QPushButton("关闭");
    m_pOpenButton = new QPushButton("打开");
    m_pRefreshButton = new QPushButton("设备归零");

    m_pSendMsgButton = new QPushButton("发送");
    m_pSendPWRMsgButton = new QPushButton("发送");
    m_pSendCommandMsgButton = new QPushButton("发送");

    plabel1->setMaximumWidth(100);
    plabel3->setMaximumWidth(100);
    plabel4->setMaximumWidth(100);
    plabel6->setMaximumWidth(100);
    plabel8->setMaximumWidth(100);
    m_pCloseButton->setMaximumWidth(100);
    m_pCloseButton->setMinimumHeight(25);

    m_pOpenButton->setMaximumWidth(100);
    m_pOpenButton->setMinimumHeight(25);

    m_pRefreshButton->setMaximumWidth(100);
    m_pRefreshButton->setMinimumHeight(25);

    m_pSendMsgButton->setMaximumWidth(80);
    m_pSendMsgButton->setMinimumHeight(25);
    m_pSendPWRMsgButton->setMaximumWidth(80);
    m_pSendPWRMsgButton->setMinimumHeight(25);
    m_pSendCommandMsgButton->setMaximumWidth(80);
    m_pSendCommandMsgButton->setMinimumHeight(25);

    MsgTextEdit->setMaximumWidth(200);
    MsgTextEdit->setMaximumHeight(25);
    PWRLineEdit->setMaximumWidth(200);
    PWRLineEdit->setMaximumHeight(25);
    CommandLineEdit->setMaximumWidth(200);
    CommandLineEdit->setMaximumHeight(25);

    textbrower->setMaximumHeight(80);

    QGridLayout* playout = new QGridLayout(this);
    playout->addWidget(plabel1,0,0,1,1);
    playout->addWidget(plabel2,0,1,1,1);
    playout->addWidget(plabel3,1,0,1,1);
    playout->addWidget(m_pStatus,1,1,1,1);
    playout->addWidget(plabel4,2,0,1,1);
    playout->addWidget(PWRLineEdit,2,1,1,1);        //调整能量编辑框
    playout->addWidget(m_pSendPWRMsgButton,2,2,1,1);//调整能量发送按键
    playout->addWidget(plabel6,3,0,1,1);
    playout->addWidget(MsgTextEdit,3,1,1,1);        //编辑发送内容框
    playout->addWidget(m_pSendMsgButton,3,2,1,1);   //发送按钮
    playout->addWidget(plabel8,4,0,1,1);            //发送指令label
    playout->addWidget(CommandLineEdit,4,1,1,1);    //发送指令编辑框
    playout->addWidget(m_pSendCommandMsgButton,4,2,1,1);
    playout->addWidget(m_pOpenButton,5,0,1,1);
    playout->addWidget(m_pCloseButton,5,1,1,1);
    playout->addWidget(m_pRefreshButton,6,0,1,1);
    playout->addWidget(textbrower,7,0,1,3);

//    showStatus();

}



void QCOMCtrlItemWidget::showStatus()
{   //demo
    if(_pPortCtrl->m_tCOMDev[_nIndex].nCurrentStatus == M_COM_STATUS_NONE)
    {
        m_pStatus->setText("未知状态");
    }
    else if(_pPortCtrl->m_tCOMDev[_nIndex].nCurrentStatus == M_COM_STATUS_Free)
    {
        m_pStatus->setText("关闭");
    }
    else if(_pPortCtrl->m_tCOMDev[_nIndex].nCurrentStatus == M_COM_STATUS_OPENED)
    {
        m_pStatus->setText("打开");
    }
//    if(_pPortCtrl->serialPort->isOpen()){
//        m_pStatus->setText("打开");
//    }


//    if(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentStatus == M_SHUTTER_STATUS_NONE)
//    {
//        m_pStatus->setText("未知状态");
//    }

//    _pPortCtrl->

//    if(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentStatus == M_SHUTTER_STATUS_OPENED)
//    {
//        m_pStatus->setText("打开");
//    }
//    if(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentStatus == M_SHUTTER_STATUS_CLOSED)
//    {
//        m_pStatus->setText("闭合");
//    }


//    QString scurrentdata = QString("%1").arg(_pShutterCtrl->m_tShutterDev[_nIndex].nCurrentVoltData);
//    m_pshowCurrentDataLabel->setText(scurrentdata);


//    QString sdata = QString("%1").arg(_pShutterCtrl->m_tShutterDev[_nIndex].nSetLimitData);
//    m_pSetLimitEdit->setText(sdata);
}


void QCOMCtrlItemWidget::onOpenCOM()
{

    QString ComPortStr = QString("COM%1").arg(_nIndex+5);
    _pPortCtrl->findFreePorts();
    _pPortCtrl->initSerialPort(ComPortStr);

    if(!(_pPortCtrl->serialPort->isOpen()))
    {
        QMessageBox::warning(this,"提示","打开串口失败");
        return;
    }
    else{
        _pPortCtrl->m_tCOMDev[_nIndex].nCurrentStatus = M_COM_STATUS_OPENED;
    }

    showStatus();
}

void QCOMCtrlItemWidget::onRelaseCOM()
{
    QString ComPortStr = QString("COM%1").arg(_nIndex);
    _pPortCtrl->serialPort->setPortName(ComPortStr);
    _pPortCtrl->serialPort->close();
    _pPortCtrl->m_tCOMDev[_nIndex].nCurrentStatus = M_COM_STATUS_Free;
    showStatus();
}
void QCOMCtrlItemWidget::SlotClickSendMsgButton()
{
    _pPortCtrl->sendMsg("LPA>ANG!_"+MsgTextEdit->toPlainText());
}

void QCOMCtrlItemWidget::SlotClickSendPWRMsgButton()
{
    _pPortCtrl->sendMsg("LPA>PWR!_"+PWRLineEdit->text());
}

void QCOMCtrlItemWidget::SlotClickSendCommandMsgButton()
{
    _pPortCtrl->sendMsg(CommandLineEdit->text());
}

void QCOMCtrlItemWidget::SlotClickSetHomeButton()
{
    _pPortCtrl->sendMsg("LPA>HOME!");
}


void QCOMCtrlItemWidget::DisplayRcvTextAfterSendBtn()
{
    if(!(_pPortCtrl->LeastestReceivedMsg.indexOf("UNKNOWN") == -1)){
        log("未知指令");
    }
    else if(!(_pPortCtrl->LeastestReceivedMsg.indexOf("null") == -1)){
        log("转动失败，请归零设备");
    }
    else{
        log("操作成功"+QString(_pPortCtrl->LeastestReceivedMsg));
    }
}

//    if(!(_pPortCtrl->serialPort->isOpen()))
//    {
//        QMessageBox::warning(this,"提示","打开串口失败");
//        return;
//    }
//    else{
//        _pPortCtrl->m_tCOMDev[_nIndex].nCurrentStatus = M_COM_STATUS_OPENED;
//    }

//    showStatus();
//}

////查询状态
//void QCOMCtrlItemWidget:: onCheckStatus()
//{
//    bool bRel = _pShutterCtrl->checkDevStatus();
//    if(bRel == false)
//    {
//        QMessageBox::warning(this,"提示","查询状态失败");
//        return;
//    }

//    showStatus();
//}

////设置门限值
//void QCOMCtrlItemWidget:: onSetLimitData()
//{
//    bool bOk = false;
//    int nData = m_pSetLimitEdit->toPlainText().toInt(&bOk);
//    if(nData <= 0)
//    {
//        QMessageBox::warning(this,"提示","门限设置必须大于0");
//        return;
//    }


//    bool bRel = _pShutterCtrl->SetDevLimit(_nIndex,nData);
//    if(bRel == false)
//    {
//        QMessageBox::warning(this,"提示","设置门限值失败");
//        return;
//    }

//    showStatus();

//}

////设置流程
//void QCOMCtrlItemWidget::onSetProcessStatus()
//{
//    bool bOk = false;
//    int nPlusCnt = m_psetPlusCntEdit->toPlainText().toInt(&bOk);
//    int nDelayTime = m_psetDelayEdit->toPlainText().toInt(&bOk);

//    if(nPlusCnt <= 0 )
//    {
//        QMessageBox::warning(this,"提示","脉冲数必须大于0");
//        return;
//    }

//    {
//        bool bRel = _pShutterCtrl->setProcessStatus(_nIndex,nPlusCnt,nDelayTime);

//        if(!bRel )
//        QMessageBox::warning(this,"错误","执行超时");
//        return;
//    }
//}




