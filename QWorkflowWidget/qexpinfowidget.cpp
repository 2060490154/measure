#include "qexpinfowidget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include "quiqss.h"
#include "qmsword.h"
#include <windows.h>
#include <QDateTime>
#include <QLineEdit>
#include <QApplication>
#include <QDebug>
QExpInfoWidget::QExpInfoWidget(QWidget *parent) : QWidget(parent)
{
    InitUI();
    connect(m_pRefreshBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_RefreshBtnClicked);
    connect(m_pHistoryBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_HistoryBtnClicked);
    connect(m_pAddDevBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_AddDevBtnClicked);
    connect(m_pAddClientBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_AddClientBtnClicked);
    connect(m_pClientInfoComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_ClientComBoxChanged(int)));
    m_fFluxThreshHold = 0.0;

}

QExpInfoWidget::~QExpInfoWidget()
{

}

void QExpInfoWidget::InitUI()
{
    m_ExpTableWidget = new QTableWidget(1,5,this);
    m_DevTableWidget = new QTableWidget(1,4,this);
    m_InfoTableWidget = new QTableWidget(11,3,this);
    InitTabHeader(m_ExpTableWidget);
    InitTabHeader(m_DevTableWidget);
    m_InfoTableWidget->setColumnWidth(0,150);
    m_InfoTableWidget->setColumnWidth(1,400);
    m_InfoTableWidget->verticalHeader()->setHidden(true);//垂直表头 不可见
    m_InfoTableWidget->horizontalHeader()->setHidden(true);//垂直表头 不可见
    m_InfoTableWidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满

    m_DevTableWidget->setItem(0,0,new QTableWidgetItem("名称"));
    m_DevTableWidget->setItem(0,1,new QTableWidgetItem("型号"));
    m_DevTableWidget->setItem(0,2,new QTableWidgetItem("主要技术指标"));

    m_ExpTableWidget->setItem(0,0,new QTableWidgetItem("日期"));
    m_ExpTableWidget->setItem(0,1,new QTableWidgetItem("实验类型"));
    m_ExpTableWidget->setItem(0,2,new QTableWidgetItem("样品名称"));
    m_ExpTableWidget->setItem(0,3,new QTableWidgetItem("样品大小"));

    m_InfoTableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_InfoTableWidget->setItem(0,0,new QTableWidgetItem("用户姓名"));
    m_InfoTableWidget->setItem(1,0,new QTableWidgetItem("用户地址"));
    m_InfoTableWidget->setItem(2,0,new QTableWidgetItem("用户联系方式"));
    m_InfoTableWidget->setItem(3,0,new QTableWidgetItem("样品名称"));
    m_InfoTableWidget->setItem(4,0,new QTableWidgetItem("样品数量"));
    m_InfoTableWidget->setItem(5,0,new QTableWidgetItem("样品参数"));
    m_InfoTableWidget->setItem(6,0,new QTableWidgetItem("检测时间"));
    m_InfoTableWidget->setItem(7,0,new QTableWidgetItem("检测温度"));
    m_InfoTableWidget->setItem(8,0,new QTableWidgetItem("检测相对湿度"));
    m_InfoTableWidget->setItem(9,0,new QTableWidgetItem("检测洁净度"));
    m_InfoTableWidget->setItem(10,0,new QTableWidgetItem("质量管理体系建立依据"));

    m_pClientInfoComBox = new QComboBox(this);
    m_pAddClientBtn = new QPushButton(tr("新增用户"),this);
    m_pAddClientBtn->setMaximumSize(QSize(150,40));
    m_InfoTableWidget->setCellWidget(0,1,m_pClientInfoComBox);
    m_InfoTableWidget->setCellWidget(0,2,m_pAddClientBtn);

    m_InfoTableWidget->setItem(4,1,new QTableWidgetItem("1"));
    m_InfoTableWidget->setItem(4,2,new QTableWidgetItem("件"));
    m_InfoTableWidget->setItem(5,1,new QTableWidgetItem("损伤阈值"));
    m_InfoTableWidget->setItem(7,1,new QTableWidgetItem("23"));
    m_InfoTableWidget->setItem(7,2,new QTableWidgetItem("摄氏度"));
    m_InfoTableWidget->setItem(8,1,new QTableWidgetItem("45"));
    m_InfoTableWidget->setItem(8,2,new QTableWidgetItem("%"));
    m_InfoTableWidget->setItem(9,1,new QTableWidgetItem("ISO 6级"));
    m_InfoTableWidget->setItem(10,1,new QTableWidgetItem("CNAS-CL01"));

    m_ExpLabel = new QLabel(this);
    setLabelTitle(m_ExpLabel,"实验列表",":/png/dev.png","rgb(68,69,73)");
    m_DevLabel = new QLabel(this);
    setLabelTitle(m_DevLabel,"设备列表",":/png/dev.png","rgb(68,69,73)");
    m_InfoLabel = new QLabel(this);
    setLabelTitle(m_InfoLabel,"其他信息",":/png/dev.png","rgb(68,69,73)");
    QGridLayout *t_MainLayout = new QGridLayout(this);

    m_pGenerateBtn = new QPushButton(tr("生成报告"),this);
    m_pGenerateBtn->setMaximumSize(QSize(150,40));
    QUIQss::setBtnQss(m_pGenerateBtn, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    connect(m_pGenerateBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_GenerateBtn_clicked);

    m_pRefreshBtn = new QPushButton(tr("刷新"),this);
    m_pRefreshBtn->setMaximumSize(QSize(150,40));
    QUIQss::setBtnQss(m_pRefreshBtn, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");

    m_pHistoryBtn = new QPushButton(tr("历史实验信息"),this);
    m_pHistoryBtn->setMaximumSize(QSize(150,40));
    QUIQss::setBtnQss(m_pHistoryBtn, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");

    m_pAddDevBtn = new QPushButton(tr("添加设备信息"),this);
    m_pAddDevBtn->setMaximumSize(QSize(150,40));
    QUIQss::setBtnQss(m_pAddDevBtn, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");


    t_MainLayout->setVerticalSpacing(10);
    t_MainLayout->setHorizontalSpacing(2);
    t_MainLayout->addWidget(m_ExpLabel,0,0,1,3);
    t_MainLayout->addWidget(m_ExpTableWidget,1,0,1,3);
    t_MainLayout->addWidget(m_pRefreshBtn,2,0,1,1);
    t_MainLayout->addWidget(m_pHistoryBtn,2,2,1,1);
    t_MainLayout->addWidget(m_DevLabel,3,0,1,3);
    t_MainLayout->addWidget(m_DevTableWidget,4,0,1,3);
    t_MainLayout->addWidget(m_pAddDevBtn,5,1,1,1);

    t_MainLayout->addWidget(m_InfoLabel,0,3,1,4);
    t_MainLayout->addWidget(m_InfoTableWidget,1,3,4,4);
    t_MainLayout->addWidget(m_pGenerateBtn,5,3,1,1);
    t_MainLayout->setAlignment(Qt::AlignVCenter);


}



//设置标题
void QExpInfoWidget::setLabelTitle(QLabel *plabel, QString sTitle, QString sIConPath, QString sRgb)
{
    plabel->setStyleSheet("color:rgb(255,255,255);background-color:"+sRgb+";");

    if(sTitle !="" && sIConPath !="")
    {
        plabel->setText("\
                        <table width=\"100%\">\
                        <tbody>\
                        <tr>\
                        <td><img src=\""+sIConPath+"\" ></td>\
                        <td align=\"left\" valign=\"middle\"> "+sTitle+"</td>\
                        </tr>\
                        </tbody>\
                        </table>");

    }

     QFont font =  plabel->font();
     font.setFamily("微软雅黑");
     font.setBold(true);
     font.setPixelSize(14);

     plabel->setFont(font);
}



void QExpInfoWidget::InitTabHeader(QTableWidget *ptable)
{
    ptable->clear();
    ptable->verticalHeader()->setHidden(true);//垂直表头 不可见
    ptable->horizontalHeader()->setHidden(true);//垂直表头 不可见
    ptable->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ptable->horizontalHeader()->setStretchLastSection(true);//最后一列充满
    if (ptable->columnCount() == 4)
    {
        ptable->setColumnWidth(0,184);
        ptable->setColumnWidth(1,183);
        ptable->setColumnWidth(2,183);
        ptable->setColumnWidth(3,15);
    }
    else if (ptable->columnCount() == 5)
    {
        ptable->setColumnWidth(0,137);
        ptable->setColumnWidth(1,137);
        ptable->setColumnWidth(2,138);
        ptable->setColumnWidth(3,138);
        ptable->setColumnWidth(4,15);
    }
    ptable->setMinimumWidth(600);
}

void QExpInfoWidget::on_RefreshBtnClicked()
{
    bool bOk = false;
    emit signal_getExpInfoFromdb(m_tExpInfos);

    //清除当前列表
    while(m_ExpTableWidget->rowCount() != 1)
    {
        m_ExpTableWidget->removeRow(m_ExpTableWidget->rowCount() - 1);
    }

    //刷新操作只显示当天实验信息
    int crntYear = QDateTime::currentDateTime().toString("yyyy").toInt(&bOk);
    int crntMonth = QDateTime::currentDateTime().toString("MM").toInt(&bOk);
    int crntDay = QDateTime::currentDateTime().toString("dd").toInt(&bOk);


    m_tExpCheckBoxs.clear();
    m_tExpInfosInRange.clear();
    for (int i=0; i<m_tExpInfos.size(); i++)
    {
        int infoYear = m_tExpInfos[i].DataTime.mid(0,4).toInt(&bOk);
        int infoMonth = m_tExpInfos[i].DataTime.mid(5,2).toInt(&bOk);
        int infoDay = m_tExpInfos[i].DataTime.mid(8,2).toInt(&bOk);

        if (infoDay == crntDay && infoMonth == crntMonth && infoYear == crntYear)
        {
            m_tExpInfosInRange.append(m_tExpInfos[i]);
            int crntRow = m_ExpTableWidget->rowCount();
            m_ExpTableWidget->insertRow(crntRow);
            m_ExpTableWidget->setItem(crntRow,0,new QTableWidgetItem(m_tExpInfos[i].DataTime.mid(0,10) +" "+ m_tExpInfos[i].DataTime.mid(11,8)));
            if (M_MEASURETYPE_1On1 == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("1On1"));
            }
            else if (M_MEASURETYPE_SOn1 == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("SOn1"));
            }
            else if (M_MEASURETYPE_ROn1 == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("ROn1"));
            }
            else if (M_MEASURETYPE_RASTER == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("Raster"));
            }
            m_ExpTableWidget->setItem(crntRow,2,new QTableWidgetItem(m_tExpInfos[i].SampleName));
            m_ExpTableWidget->setItem(crntRow,3,new QTableWidgetItem(m_tExpInfos[i].SampleSize));
            QCheckBox *t_ChkBox = new QCheckBox(this);
            m_tExpCheckBoxs.push_back(t_ChkBox);
            m_ExpTableWidget->setCellWidget(crntRow,4,t_ChkBox);
        }
    }
}


void QExpInfoWidget::on_HistoryBtnClicked()
{
    m_pDateDialog = new QDialog(this);
    m_pDateDialog->setWindowTitle(tr("选择日期"));
    QGridLayout *t_DialogGrid = new QGridLayout;
    QLabel *t_DateStart = new QLabel(tr("起始日期："));
    QLabel *t_DateEnd = new QLabel(tr("终止日期："));
    m_pDateStartEdit = new QDateTimeEdit;
    m_pDateEndEdit = new QDateTimeEdit;
    m_pOkBtn = new QPushButton(tr("确定"));
    m_pOkBtn->setMinimumSize(QSize(55,30));
    m_pDateStartEdit->setCalendarPopup(true);
    m_pDateEndEdit->setCalendarPopup(true);
    m_pDateStartEdit->setDisplayFormat("yyyy-MM-dd");
    m_pDateEndEdit->setDisplayFormat("yyyy-MM-dd");
    m_pDateStartEdit->setDateTime(QDateTime::currentDateTime());
    m_pDateEndEdit->setDateTime(QDateTime::currentDateTime());

    t_DialogGrid->addWidget(t_DateStart,0,0,1,2);
    t_DialogGrid->addWidget(m_pDateStartEdit,0,2,1,2);

    t_DialogGrid->addWidget(t_DateEnd,1,0,1,2);
    t_DialogGrid->addWidget(m_pDateEndEdit,1,2,1,2);
    t_DialogGrid->addWidget(m_pOkBtn,2,1,1,2);

    connect(m_pOkBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_HistoryOkBtn);
    m_pDateDialog->setLayout(t_DialogGrid);
    m_pDateDialog->setModal(true);
    m_pDateDialog->show();
}


void QExpInfoWidget::on_HistoryOkBtn()
{
    bool bOk = false;
    emit signal_getExpInfoFromdb(m_tExpInfos);
//    Sleep(50);
    while(m_ExpTableWidget->rowCount() != 1)
    {
        m_ExpTableWidget->removeRow(m_ExpTableWidget->rowCount() - 1);
    }

    QString startdate = m_pDateStartEdit->text();
    QString enddate = m_pDateEndEdit->text();
    int searchYearStart = startdate.mid(0,4).toInt(&bOk);
    int searchMonthStart = startdate.mid(5,2).toInt(&bOk);
    int searchDayStart = startdate.mid(8,2).toInt(&bOk);
    int searchYearEnd = enddate.mid(0,4).toInt(&bOk);
    int searchMonthEnd = enddate.mid(5,2).toInt(&bOk);
    int searchDayEnd = enddate.mid(8,2).toInt(&bOk);

    m_tExpCheckBoxs.clear();
    m_tExpInfosInRange.clear();
    for (int i=0; i<m_tExpInfos.size(); i++)
    {
        int infoYear = m_tExpInfos[i].DataTime.mid(0,4).toInt(&bOk);
        int infoMonth = m_tExpInfos[i].DataTime.mid(5,2).toInt(&bOk);
        int infoDay = m_tExpInfos[i].DataTime.mid(8,2).toInt(&bOk);

        if (IsDateValid(searchYearStart,searchMonthStart,searchDayStart,searchYearEnd,searchMonthEnd,searchDayEnd,infoYear,infoMonth,infoDay))
        {
            m_tExpInfosInRange.append(m_tExpInfos[i]);
            int crntRow = m_ExpTableWidget->rowCount();
            m_ExpTableWidget->insertRow(crntRow);
            m_ExpTableWidget->setItem(crntRow,0,new QTableWidgetItem(m_tExpInfos[i].DataTime.mid(0,10) +" "+ m_tExpInfos[i].DataTime.mid(11,8)));
            if (M_MEASURETYPE_1On1 == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("1On1"));
            }
            else if (M_MEASURETYPE_SOn1 == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("SOn1"));
            }
            else if (M_MEASURETYPE_ROn1 == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("ROn1"));
            }
            else if (M_MEASURETYPE_RASTER == m_tExpInfos[i].ExpType)
            {
                m_ExpTableWidget->setItem(crntRow,1,new QTableWidgetItem("Raster"));
            }
            m_ExpTableWidget->setItem(crntRow,2,new QTableWidgetItem(m_tExpInfos[i].SampleName));
            m_ExpTableWidget->setItem(crntRow,3,new QTableWidgetItem(m_tExpInfos[i].SampleSize));
            QCheckBox *t_ChkBox = new QCheckBox(this);
            m_tExpCheckBoxs.push_back(t_ChkBox);
            m_ExpTableWidget->setCellWidget(crntRow,4,t_ChkBox);
        }
    }
    m_pDateDialog->deleteLater();
}

bool QExpInfoWidget::IsDateValid(int sy, int sm, int sd, int ey, int em, int ed, int iy, int im, int id)
{
    //判断日期是否在选择范围内
    QDateTime t_sDate(QDate(sy,sm,sd));
    QDateTime t_eDate(QDate(ey,em,ed));
    QDateTime t_iDate(QDate(iy,im,id));
    QDateTime t_oriDate(QDate(qMin(sy,ey),1,1));
    if (t_oriDate.daysTo(t_iDate) >= t_oriDate.daysTo(t_sDate) && t_oriDate.daysTo(t_iDate) <= t_oriDate.daysTo(t_eDate))
    {
        return true;
    }
    else
    {
        return false;
    }


}
void QExpInfoWidget::on_AddDevBtnClicked()
{
    m_pAddDevDialog = new QDialog(this);
    m_pAddDevDialog->setWindowTitle(tr("添加设备信息"));
    QGridLayout *t_DialogGrid = new QGridLayout;
    QLabel *t_DevName = new QLabel(tr("设备名称："));
    QLabel *t_DevType = new QLabel(tr("设备型号："));
    QLabel *t_DevNo = new QLabel(tr("设备编号："));
    QLabel *t_DevCertiNo = new QLabel(tr("溯源证书/校准号："));
    QLabel *t_DevSpeci = new QLabel(tr("主要技术指标："));


    m_DevNameEdit = new QLineEdit;
    m_DevTypeEdit = new QLineEdit;
    m_DevNoEdit = new QLineEdit;
    m_DevCertiNoEdit = new QLineEdit;
    m_DevSpeciEdit = new QLineEdit;

    m_pSubmitBtn = new QPushButton(tr("提交"));
    m_pSubmitBtn->setMinimumSize(QSize(55,30));

    t_DialogGrid->addWidget(t_DevName,0,0,1,2);
    t_DialogGrid->addWidget(m_DevNameEdit,0,2,1,2);

    t_DialogGrid->addWidget(t_DevType,1,0,1,2);
    t_DialogGrid->addWidget(m_DevTypeEdit,1,2,1,2);

    t_DialogGrid->addWidget(t_DevNo,2,0,1,2);
    t_DialogGrid->addWidget(m_DevNoEdit,2,2,1,2);

    t_DialogGrid->addWidget(t_DevCertiNo,3,0,1,2);
    t_DialogGrid->addWidget(m_DevCertiNoEdit,3,2,1,2);

    t_DialogGrid->addWidget(t_DevSpeci,4,0,1,2);
    t_DialogGrid->addWidget(m_DevSpeciEdit,4,2,1,2);



    t_DialogGrid->addWidget(m_pSubmitBtn,5,1,1,2);

    connect(m_pSubmitBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_AddDevSubmitBtn);
    m_pAddDevDialog->setLayout(t_DialogGrid);
    m_pAddDevDialog->setModal(true);
    m_pAddDevDialog->show();
}

void QExpInfoWidget::on_AddClientBtnClicked()
{
    m_pAddClientDialog = new QDialog(this);
    m_pAddClientDialog->setMinimumSize(QSize(250,150));
    m_pAddClientDialog->setWindowTitle(tr("添加客户信息"));
    QGridLayout *t_DialogGrid = new QGridLayout;
    QLabel *t_ClientName = new QLabel(tr("客户名称："));
    QLabel *t_ClientAddress = new QLabel(tr("客户地址："));
    QLabel *t_ClientPhone = new QLabel(tr("客户电话："));

    m_pClientNameEdit = new QLineEdit;
    m_pClientAddressEdit = new QLineEdit;
    m_pClientPhoneEdit = new QLineEdit;
    m_pClientSubmitBtn = new QPushButton(tr("提交"),this);
    t_DialogGrid->addWidget(t_ClientName,0,0,1,2);
    t_DialogGrid->addWidget(m_pClientNameEdit,0,1,1,2);
    t_DialogGrid->addWidget(t_ClientAddress,1,0,1,2);
    t_DialogGrid->addWidget(m_pClientAddressEdit,1,1,1,2);
    t_DialogGrid->addWidget(t_ClientPhone,2,0,1,2);
    t_DialogGrid->addWidget(m_pClientPhoneEdit,2,1,1,2);
    t_DialogGrid->addWidget(m_pClientSubmitBtn,3,1,1,2);
    connect(m_pClientSubmitBtn,&QPushButton::clicked,this,&QExpInfoWidget::on_AddClientSubmitBtn);
    m_pAddClientDialog->setLayout(t_DialogGrid);
    m_pAddClientDialog->setModal(true);
    m_pAddClientDialog->show();
}

void QExpInfoWidget::on_AddClientSubmitBtn()
{
    ClientInfo t_ClientInfos;
    t_ClientInfos.CName = m_pClientNameEdit->text();
    t_ClientInfos.CAddress = m_pClientAddressEdit->text();
    t_ClientInfos.CPhone = m_pClientPhoneEdit->text();
    emit signal_SaveNewClientInfo2db(m_tClientInfos.size(),t_ClientInfos);
//    Sleep(100);
    on_ShowClientInfo();
    m_pAddClientDialog->deleteLater();
}

void QExpInfoWidget::on_AddDevSubmitBtn()
{
    DevInfo4Show t_DevInfo;
    t_DevInfo.DevName = m_DevNameEdit->text();
    t_DevInfo.DevType = m_DevTypeEdit->text();
    t_DevInfo.DevNo = m_DevNoEdit->text();
    t_DevInfo.DevCertiNo = m_DevCertiNoEdit->text();
    t_DevInfo.DevComment = m_DevSpeciEdit->text();
    emit signal_SaveNewDevInfo2db(t_DevInfo);
    Sleep(100);
    on_ShowDevInfo();
    m_pAddDevDialog->deleteLater();
}


void QExpInfoWidget::on_ShowDevInfo()
{
    emit signal_GetDevInFromdb(m_tDevInfos);
    Sleep(100);
    while (m_DevTableWidget->rowCount() != 1)
    {
        m_DevTableWidget->removeRow(m_DevTableWidget->rowCount() - 1);
    }
    m_tDevCheckBoxs.clear();
    m_tDevInfosInRange.clear();
    for (int i=0; i<m_tDevInfos.size(); i++)
    {
        m_tDevInfosInRange.append(m_tDevInfos[i]);
        int crntRow = m_DevTableWidget->rowCount();
        m_DevTableWidget->insertRow(crntRow);
        m_DevTableWidget->setItem(crntRow,0,new QTableWidgetItem(m_tDevInfos[i].DevName));
        m_DevTableWidget->setItem(crntRow,1,new QTableWidgetItem(m_tDevInfos[i].DevType));
        m_DevTableWidget->setItem(crntRow,2,new QTableWidgetItem(m_tDevInfos[i].DevComment));
        QCheckBox *t_ChkBox = new QCheckBox(this);
        m_tDevCheckBoxs.push_back(t_ChkBox);
        m_DevTableWidget->setCellWidget(crntRow,3,t_ChkBox);
    }
}

void QExpInfoWidget::on_ShowClientInfo()
{
    emit signal_GetClientInfoFromDb(m_tClientInfos);
    m_pClientInfoComBox->clear();
    for(int i=0; i<m_tClientInfos.size(); i++)
    {
        m_pClientInfoComBox->addItem(m_tClientInfos[i].CName);
    }
    if (m_tClientInfos.size() != 0)
    {
        m_pClientInfoComBox->setCurrentIndex(0);
        m_InfoTableWidget->setItem(1,1,new QTableWidgetItem(m_tClientInfos[0].CAddress));
        m_InfoTableWidget->setItem(2,1,new QTableWidgetItem(m_tClientInfos[0].CPhone));
    }
}


void QExpInfoWidget::on_GenerateBtn_clicked()
{
    //遍历出用户勾选的实验条目和设备条目
    QVector<int> ExpCheckChosed;
    QVector<int> DevCheckChosed;
    for (int i=0; i<m_tExpCheckBoxs.size(); i++)
    {
        if (m_tExpCheckBoxs[i]->isChecked())
        {
            ExpCheckChosed.append(i);
        }
    }
    for (int i=0; i<m_tDevCheckBoxs.size(); i++)
    {
        if (m_tDevCheckBoxs[i]->isChecked())
        {
            DevCheckChosed.append(i);
        }
    }



    QMSWord t_word;
    t_word.open(QApplication::applicationDirPath() + "/Doc/QG.dot",false);

    //添加检测报告基本信息
    t_word.setMarkContent("Number","2019-000-001-01"); //预留12个字符的位置，不够需要补全
    t_word.setMarkContent("ClientName",m_pClientInfoComBox->currentText());
    t_word.setMarkContent("ClientAddress",m_InfoTableWidget->item(1,1)->text());
    t_word.setMarkContent("ClientTel",m_InfoTableWidget->item(2,1)->text());
    t_word.setMarkContent("SampleName",m_ExpTableWidget->item(ExpCheckChosed.last()+1,2)->text());
    t_word.setMarkContent("SampleNumber",QString("%1件").arg(ExpCheckChosed.size()));
    t_word.setMarkContent("SamplePara","损伤阈值");
    QString MeasureDate = m_ExpTableWidget->item(ExpCheckChosed.last()+1,0)->text();
    t_word.setMarkContent("MeasureYear",MeasureDate.mid(0,4));
    t_word.setMarkContent("MeasureMonth",MeasureDate.mid(5,2));
    t_word.setMarkContent("MeasureDay",MeasureDate.mid(8,2));
    t_word.setMarkContent("Dependency",m_InfoTableWidget->item(10,1)->text());

    //根据用户勾选添加设备信息
    t_word.addTableRow("DeclareTable",DevCheckChosed.size(),0);
    for (int i=0; i<DevCheckChosed.size(); i++)
    {
        t_word.setCellText(2,i+2,1,m_tDevInfosInRange[DevCheckChosed[i]].DevName);
        t_word.setCellText(2,i+2,2,m_tDevInfosInRange[DevCheckChosed[i]].DevType);
        t_word.setCellText(2,i+2,3,m_tDevInfosInRange[DevCheckChosed[i]].DevNo);
        t_word.setCellText(2,i+2,4,m_tDevInfosInRange[DevCheckChosed[i]].DevCertiNo);
        t_word.setCellText(2,i+2,5,m_tDevInfosInRange[DevCheckChosed[i]].DevComment);
    }

    //添加检测环境信息
    t_word.setMarkContent("MeasureTemperature",m_InfoTableWidget->item(7,1)->text() + "℃");
    t_word.setMarkContent("MeasureHumidity",m_InfoTableWidget->item(8,1)->text() + "%");
    t_word.setMarkContent("MeasureCleanliness",m_InfoTableWidget->item(9,1)->text());

    //添加检测结果信息
    t_word.addTableRow("ResultTable",ExpCheckChosed.size(),0);
    for (int i=0; i<ExpCheckChosed.size(); i++)
    {
        t_word.setCellText(3,i+3,1,QString::number(i+1));
        t_word.setCellText(3,i+3,2,m_tExpInfosInRange[ExpCheckChosed[i]].SampleName);
        int tem_nHpos = m_tExpInfosInRange[ExpCheckChosed[i]].SampleSize.indexOf("h");
        int tem_nWpos = m_tExpInfosInRange[ExpCheckChosed[i]].SampleSize.indexOf("w");
        int tem_nLength = m_tExpInfosInRange[ExpCheckChosed[i]].SampleSize.size();
        QString tem_WSize = m_tExpInfosInRange[ExpCheckChosed[i]].SampleSize.mid(tem_nWpos+2,tem_nLength-1);
        QString tem_HSize = m_tExpInfosInRange[ExpCheckChosed[i]].SampleSize.mid(tem_nHpos+2,tem_nWpos-1);
        t_word.setCellText(3,i+3,3,"宽："+tem_WSize+",高："+tem_HSize);
        t_word.setCellText(3,i+3,4,QString("#%1").arg(i+1));
        t_word.setCellText(3,i+3,5,QString::number(m_fFluxThreshHold));
    }
    t_word.setCellText(3,3+ExpCheckChosed.size(),2,"1. 波长：1064nm; \n2. 偏振态：水平偏振; \n3. 脉宽：");  //备注栏

    //添加图片信息——此处插入图像需要根据实际实验从数据库中读取，暂时从本地取图
    t_word.insertPic("DefImg",QApplication::applicationDirPath() + "/IDSIMAGE/1h.jpg");
    t_word.insertPic("DefImg",QApplication::applicationDirPath() + "/IDSIMAGE/1q.jpg");
    t_word.insertPic("CCDImg",QApplication::applicationDirPath() + "/CCDIMG/ccdimg.bmp");
    t_word.insertPic("PlotImg",QApplication::applicationDirPath() + "/PLOTIMG/"+m_sExpNo+".png");

    QString str = m_sExpNo+".doc";
    t_word.saveAs(m_sSavePath + "/Doc/"+str);
    t_word.close();
}

void QExpInfoWidget::on_ClientComBoxChanged(int index)
{
    if (m_tClientInfos.size() > 0 && index >= 0)
    {
        m_InfoTableWidget->setItem(1,1,new QTableWidgetItem(m_tClientInfos[index].CAddress));
        m_InfoTableWidget->setItem(2,1,new QTableWidgetItem(m_tClientInfos[index].CPhone));
    }
}

void QExpInfoWidget::on_GetFluxThreshHold(double thresh)
{
    m_fFluxThreshHold = thresh;
}

void QExpInfoWidget::on_GetExpNo(QString sExpNo,QString sSampleName)
{
    m_sExpNo = sExpNo;
    m_sSavePath = QString(DATA_SAVE_PATH) + "/" + sSampleName + "/" + sExpNo;
}
