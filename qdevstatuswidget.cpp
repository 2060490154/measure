#include "qdevstatuswidget.h"
#include <QGridLayout>
#include "define.h"

QDevStatusWidget::QDevStatusWidget(QDevManagerCtrl* pCtrl,QWidget *parent) : QWidget(parent)
{

    _pCtrl = pCtrl;
    m_pTimer = new QTimer();

    connect(m_pTimer, &QTimer::timeout, this, &QDevStatusWidget::onUpdateDevStatus);

    m_pTimer->start(2000);//2秒刷新一次


    m_pDevStatusLabel = new QLabel(this);
    m_pDevStatusLabel->setStyleSheet("color:rgb(255,255,255);background-color:rgb(165, 188, 188);");

    m_pStatusLabel = new QLabel(this);
    m_pStatusLabel->setStyleSheet("color:rgb(255,255,255);background-color:rgb(165, 188, 188);");

    QGridLayout* playout = new QGridLayout(this);
    playout->addWidget(m_pDevStatusLabel,0,0,1,1);
    playout->addWidget(m_pStatusLabel,0,1,1,1);
    playout->setSpacing(0);
    playout->setMargin(0);

    this->setMaximumHeight(45);
}

//实时更新设备状态
void QDevStatusWidget::onUpdateDevStatus()
{
    _pCtrl->getDevStatus();
    m_pdevStatus = & _pCtrl->m_tdevStatus;
    showDevStatus();

}

void QDevStatusWidget:: showDevStatus()
{

    QString strhead,strtrail,strbody,sIConPath;

    strhead =  " <table width=\"100%\"><tbody><tr>";
    strtrail = "</tr></tbody> </table>";

    //body
    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"设备状态:"+"</td>";

    if(m_pdevStatus->bEngerMeter == false)
    {
        sIConPath = ":/png/disconnect.png";
    }
    else
        sIConPath = ":/png/connect.png";

    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"能量计"+"</td>\
                                        <td valign=\"middle\"><img src=\""+sIConPath+"\" ></td>";

    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"|"+"</td>";
    if(m_pdevStatus->bIDSCamer == false)
    {
        sIConPath = ":/png/disconnect.png";
    }
    else
        sIConPath = ":/png/connect.png";

    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"显微监视"+"</td>\
                                        <td valign=\"middle\"><img src=\""+sIConPath+"\" ></td>";
    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"|"+"</td>";
    if(m_pdevStatus->bMotor == false)
    {
        sIConPath = ":/png/disconnect.png";
    }
    else
        sIConPath = ":/png/connect.png";

    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"电机"+"</td>\
                                        <td valign=\"middle\"><img src=\""+sIConPath+"\" ></td>";
    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"|"+"</td>";
    if(m_pdevStatus->bOscii == false)
    {
        sIConPath = ":/png/disconnect.png";
    }
    else
        sIConPath = ":/png/connect.png";

    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"示波器"+"</td>\
                                        <td valign=\"middle\"><img src=\""+sIConPath+"\" ></td>";
    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"|"+"</td>";
    if(m_pdevStatus->bKxccd == false)
    {
        sIConPath = ":/png/disconnect.png";
    }
    else
        sIConPath = ":/png/connect.png";

    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"科学CCD"+"</td>\
                                        <td valign=\"middle\"><img src=\""+sIConPath+"\" ></td>";
    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"|"+"</td>";
    if(m_pdevStatus->bshutterStatus == false)
    {
        sIConPath = ":/png/disconnect.png";
    }
    else
        sIConPath = ":/png/connect.png";

    strbody = strbody + "<td align=\"left\" valign=\"middle\"> "+"光闸"+"</td>\
                                        <td valign=\"middle\"><img src=\""+sIConPath+"\" ></td>";

    m_pDevStatusLabel->setText(strhead + strbody +strtrail);


     QFont font =  m_pDevStatusLabel->font();
     font.setFamily("微软雅黑");
     font.setBold(true);
     font.setPixelSize(14);

     m_pDevStatusLabel->setFont(font);

}

//显示工作状态
void QDevStatusWidget::onShowWorkStatus(int nworkStatus,bool bwork)
{
    QString strhead,strtrail,strbody;
    QString sworkstatus,sStartwork;

    strhead =  " <table width=\"100%\"><tbody><tr>";
    strtrail = "</tr></tbody> </table>";


    if(nworkStatus == M_MEASURETYPE_1On1)
    {
        sworkstatus = "工作方式：1 On 1";
    }

    if(nworkStatus == M_MEASURETYPE_SOn1)
    {
         sworkstatus = "工作方式：S On 1";
    }

    if(nworkStatus == M_MEASURETYPE_ROn1)
    {
        sworkstatus = "工作方式：R On 1";
    }

    if(nworkStatus == M_MEASURETYPE_RASTER)
    {
        sworkstatus = "工作方式：Raster Scan";
    }

    sStartwork = bwork==true?"工作状态：正在执行..":"工作状态：未执行";

     strbody = strbody + "<td align=\"right\" valign=\"middle\"> "+sworkstatus+"</td>";
     strbody = strbody + "<td align=\"right\" valign=\"middle\"> "+sStartwork+"</td>";

     m_pStatusLabel->setText(strhead + strbody +strtrail);

     QFont font =  m_pStatusLabel->font();
     font.setFamily("微软雅黑");
     font.setBold(true);
     font.setPixelSize(14);

     m_pStatusLabel->setFont(font);
}
