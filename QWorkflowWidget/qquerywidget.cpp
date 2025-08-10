#include "qquerywidget.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
//#include <QWorkFlow/qpointareawidget.h>
//#include <QWorkFlow/qimageshowwidget.h>
//#include <QWorkFlow/qexpresultwidget.h>
//#include "QWorkFlow/qimageshow.h"
#include <QDateTime>
#include <QCoreApplication>
#include <QGroupBox>
#include <QFileDialog>
#include <QDateTimeEdit>
#include "qcustomplot.h"
#include "QCommon/qexcel.h"
#include <QDebug>
#include <QMenu>

QQueryWidget::QQueryWidget(QWidget *parent) : QWidget(parent)
{
    m_pExcel = NULL;
    InitUI();
    connectAction();
    m_tColorList.push_back(QColor(123,162,63));
    m_tColorList.push_back(QColor(145,180,147));
    m_tColorList.push_back(QColor(0,92,175));
    m_tColorList.push_back(QColor(180,129,187));

    RightClick = new QMenu(m_pExpDataTable);
    deleteAction = new QAction(QString::fromLocal8Bit("Delete"),this);
    RightClick->addAction(deleteAction);
    connect(m_pExpDataTable,&QTableWidget::customContextMenuRequested,this,&QQueryWidget::RightClickSlot);
    connect(RightClick,&QMenu::triggered,this,&QQueryWidget::RightClickDelete);
}


/******************************
*功能:初始化界面
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::InitUI()
{
    InitExpInfoWidget();
    InitDataTable();

    QVBoxLayout *tem_InfoParaVLayout = new QVBoxLayout();
    tem_InfoParaVLayout->setMargin(0);
    tem_InfoParaVLayout->setSpacing(5);
    tem_InfoParaVLayout->addWidget(m_pExpInfoWidget);
    tem_InfoParaVLayout->setStretch(0,1);

    QGridLayout *tem_DataGridlayout = new QGridLayout();
    tem_DataGridlayout->addWidget(m_pExpDataTable,0,0,1,6);
    tem_DataGridlayout->addWidget(m_pExportAllDataBTn,1,0,1,1);
    tem_DataGridlayout->addWidget(m_pExportSelectedDataBtn,1,1,1,1);
    tem_DataGridlayout->addWidget(m_pAddNumEdit,1,2,1,1);
    tem_DataGridlayout->addWidget(m_pAddNumBtn,1,3,1,1);
    tem_DataGridlayout->addWidget(m_pReOrderBtn,1,4,1,1);
    tem_DataGridlayout->setRowStretch(0,10);
    tem_DataGridlayout->setRowStretch(1,1);
    tem_DataGridlayout->setMargin(0);
    tem_DataGridlayout->setSpacing(5);
    tem_DataGridlayout->setAlignment(m_pExportAllDataBTn,Qt::AlignHCenter);
    tem_DataGridlayout->setAlignment(m_pExportSelectedDataBtn,Qt::AlignHCenter);


    QGridLayout *tem_mainLayout = new QGridLayout();
    tem_mainLayout->addWidget(m_pExpInfoWidget,0,0,1,1);
    tem_mainLayout->addLayout(tem_DataGridlayout,1,0,1,1);
    tem_mainLayout->setRowStretch(0,1);
    tem_mainLayout->setRowStretch(1,5);

    this->setLayout(tem_mainLayout);
}


/******************************
*功能:初始化实验信息table
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::InitExpInfoWidget()
{
    m_pExpInfoWidget = new QWidget();
    m_pExpInfoTable = new QTableWidget(1,4);
    m_pRefreshBtn = new QPushButton("刷新");
    m_pHistoryBtn = new QPushButton("历史信息");
    m_pQueryBtn = new QPushButton("查询");
    m_pRefreshBtn->setFixedSize(100,25);
    m_pHistoryBtn->setFixedSize(100,25);
    m_pQueryBtn->setFixedSize(100,25);
    m_pExpInfoLab = new QLabel();
    setTableTitleLabel(m_pExpInfoLab,"实验信息选择","","rgb(0,137,167)");

    QStringList *tem_ExpInfoHList = new QStringList();
    tem_ExpInfoHList->push_back("实验编号");
    tem_ExpInfoHList->push_back("样品名称");
    tem_ExpInfoHList->push_back("实验类型");
    tem_ExpInfoHList->push_back("时间");

    m_pExpInfoTable->setHorizontalHeaderLabels(*tem_ExpInfoHList);
    m_pExpInfoTable->verticalHeader()->setHidden(true);
    m_pExpInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout *tem_ExpInfoVlayout1 = new QVBoxLayout();
    tem_ExpInfoVlayout1->setMargin(0);
    tem_ExpInfoVlayout1->setSpacing(0);
    tem_ExpInfoVlayout1->addWidget(m_pExpInfoLab);
    tem_ExpInfoVlayout1->addWidget(m_pExpInfoTable);
    tem_ExpInfoVlayout1->setStretch(0,1);
    tem_ExpInfoVlayout1->setStretch(1,10);

    QHBoxLayout *tem_ExpInfoHLayout1 = new QHBoxLayout();
    tem_ExpInfoHLayout1->setMargin(0);
    tem_ExpInfoHLayout1->addWidget(m_pRefreshBtn);
    tem_ExpInfoHLayout1->addWidget(m_pHistoryBtn);
    tem_ExpInfoHLayout1->addWidget(m_pQueryBtn);

    QVBoxLayout *tem_ExpInfoVlayout2 = new QVBoxLayout();
    tem_ExpInfoVlayout2->setMargin(0);
    tem_ExpInfoVlayout2->setSpacing(5);
    tem_ExpInfoVlayout2->addLayout(tem_ExpInfoVlayout1);
    tem_ExpInfoVlayout2->addLayout(tem_ExpInfoHLayout1);

    m_pExpInfoWidget->setLayout(tem_ExpInfoVlayout2);

#if 0
    m_pExpInfoTable->setItem(0,0,new QTableWidgetItem("1On1_20190504151452"));
    m_pExpInfoTable->setItem(0,1,new QTableWidgetItem("测试品1"));
    m_pExpInfoTable->setItem(0,2,new QTableWidgetItem("1On1"));
    m_pExpInfoTable->setItem(0,3,new QTableWidgetItem("1064"));
    m_pExpInfoTable->setItem(0,4,new QTableWidgetItem("2019-05-04 15:15:28.000"));
    //;
#endif

}

/******************************
*功能:初始化参数table
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::InitParasTable()
{
    m_pExpParasTable = new QTableWidget(15,1);
    m_pParasLab = new QLabel();
    setTableTitleLabel(m_pParasLab,"实验参数","","rgb(0,137,167)");

    QStringList *tem_ParasVList = new QStringList();
    *tem_ParasVList<<"实验编号"<<"实验类型"<<"实验波长(nm)"<<"样品名称"<<"样品形状"<<"样品长度(mm)"<<"样品宽度(mm)"\
                    <<"样品直径(mm)"<<"实验总轮数"<<"实验步长(mm)"<<"实验总点数"<<"实验总能量段数"<<"每能量段点数"<<"每点发次数"<<"时间";
    m_pExpParasTable->setVerticalHeaderLabels(*tem_ParasVList);
    m_pExpParasTable->horizontalHeader()->setHidden(true);

    QVBoxLayout *tem_ParasVLayout = new QVBoxLayout();
    tem_ParasVLayout->setMargin(0);
    tem_ParasVLayout->setSpacing(0);
    tem_ParasVLayout->addWidget(m_pParasLab);
    tem_ParasVLayout->addWidget(m_pExpParasTable);

    m_pParasWidget = new QWidget();
    m_pParasWidget->setLayout(tem_ParasVLayout);

#if 0
    m_pExpParasTable->setItem(0,0,new QTableWidgetItem("123"));
    m_pExpParasTable->setItem(1,0,new QTableWidgetItem("1On1"));
    m_pExpParasTable->setItem(2,0,new QTableWidgetItem("1064"));
    m_pExpParasTable->setItem(3,0,new QTableWidgetItem("213"));
    m_pExpParasTable->setItem(4,0,new QTableWidgetItem("2143"));
    m_pExpParasTable->setItem(5,0,new QTableWidgetItem("100"));
    m_pExpParasTable->setItem(6,0,new QTableWidgetItem("100"));
    m_pExpParasTable->setItem(7,0,new QTableWidgetItem("1231"));
    m_pExpParasTable->setItem(8,0,new QTableWidgetItem("1"));
    m_pExpParasTable->setItem(9,0,new QTableWidgetItem("10"));
    m_pExpParasTable->setItem(10,0,new QTableWidgetItem("90"));
    m_pExpParasTable->setItem(11,0,new QTableWidgetItem("9"));
    m_pExpParasTable->setItem(12,0,new QTableWidgetItem("10"));
    m_pExpParasTable->setItem(13,0,new QTableWidgetItem("1"));
    m_pExpParasTable->setItem(14,0,new QTableWidgetItem("2019-05-04 15:15:28.000"));

    //;
#endif
}

/******************************
*功能:初始化数据table
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::InitDataTable()
{
    m_pExpDataTable = new QTableWidget(1,5);
    QStringList *tem_DataHList = new QStringList();
    *tem_DataHList<<"点次"<<"发次"<<"能量(mJ)"<<"通量(J/cm^2)"<<"是否损伤";
    m_pExpDataTable->setHorizontalHeaderLabels(*tem_DataHList);
    m_pExpDataTable->setContextMenuPolicy(Qt::CustomContextMenu);
//    m_pExpDataTable->verticalHeader()->setHidden(true);
//    m_pExpDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pExportAllDataBTn = new QPushButton("导出所有数据");
    m_pExportAllDataBTn->setFixedSize(100,25);
    m_pExportSelectedDataBtn = new QPushButton("导出选中的数据");
    m_pExportSelectedDataBtn->setFixedSize(100,25);
    m_pAddNumBtn = new QPushButton("调整点号");
    m_pAddNumEdit = new QLineEdit();
    m_pReOrderBtn = new QPushButton("刷新");
    m_pAddNumBtn->setFixedSize(100,25);
    m_pAddNumEdit->setFixedSize(100,25);
    m_pReOrderBtn->setFixedSize(100,25);

#if 0
    m_pExpDataTable->setItem(0,0,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(0,1,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(0,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(0,3,new QTableWidgetItem("30.5"));
    m_pExpDataTable->setItem(0,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(0,5,new QTableWidgetItem("1.74"));
    m_pExpDataTable->setItem(0,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(1,0,new QTableWidgetItem("2"));
    m_pExpDataTable->setItem(1,1,new QTableWidgetItem("2"));
    m_pExpDataTable->setItem(1,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(1,3,new QTableWidgetItem("29.8"));
    m_pExpDataTable->setItem(1,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(1,5,new QTableWidgetItem("1.70"));
    m_pExpDataTable->setItem(1,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(2,0,new QTableWidgetItem("3"));
    m_pExpDataTable->setItem(2,1,new QTableWidgetItem("3"));
    m_pExpDataTable->setItem(2,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(2,3,new QTableWidgetItem("30.2"));
    m_pExpDataTable->setItem(2,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(2,5,new QTableWidgetItem("1.73"));
    m_pExpDataTable->setItem(2,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(3,0,new QTableWidgetItem("4"));
    m_pExpDataTable->setItem(3,1,new QTableWidgetItem("4"));
    m_pExpDataTable->setItem(3,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(3,3,new QTableWidgetItem("30.2"));
    m_pExpDataTable->setItem(3,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(3,5,new QTableWidgetItem("1.73"));
    m_pExpDataTable->setItem(3,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(4,0,new QTableWidgetItem("5"));
    m_pExpDataTable->setItem(4,1,new QTableWidgetItem("5"));
    m_pExpDataTable->setItem(4,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(4,3,new QTableWidgetItem("30.4"));
    m_pExpDataTable->setItem(4,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(4,5,new QTableWidgetItem("1.74"));
    m_pExpDataTable->setItem(4,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(5,0,new QTableWidgetItem("6"));
    m_pExpDataTable->setItem(5,1,new QTableWidgetItem("6"));
    m_pExpDataTable->setItem(5,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(5,3,new QTableWidgetItem("30.1"));
    m_pExpDataTable->setItem(5,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(5,5,new QTableWidgetItem("1.72"));
    m_pExpDataTable->setItem(5,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(6,0,new QTableWidgetItem("7"));
    m_pExpDataTable->setItem(6,1,new QTableWidgetItem("7"));
    m_pExpDataTable->setItem(6,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(6,3,new QTableWidgetItem("29.8"));
    m_pExpDataTable->setItem(6,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(6,5,new QTableWidgetItem("1.70"));
    m_pExpDataTable->setItem(6,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(7,0,new QTableWidgetItem("8"));
    m_pExpDataTable->setItem(7,1,new QTableWidgetItem("8"));
    m_pExpDataTable->setItem(7,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(7,3,new QTableWidgetItem("30.2"));
    m_pExpDataTable->setItem(7,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(7,5,new QTableWidgetItem("1.73"));
    m_pExpDataTable->setItem(7,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(8,0,new QTableWidgetItem("9"));
    m_pExpDataTable->setItem(8,1,new QTableWidgetItem("9"));
    m_pExpDataTable->setItem(8,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(8,3,new QTableWidgetItem("30.4"));
    m_pExpDataTable->setItem(8,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(8,5,new QTableWidgetItem("1.74"));
    m_pExpDataTable->setItem(8,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(9,0,new QTableWidgetItem("10"));
    m_pExpDataTable->setItem(9,1,new QTableWidgetItem("10"));
    m_pExpDataTable->setItem(9,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(9,3,new QTableWidgetItem("31.4"));
    m_pExpDataTable->setItem(9,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(9,5,new QTableWidgetItem("1.74"));
    m_pExpDataTable->setItem(9,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(10,0,new QTableWidgetItem("11"));
    m_pExpDataTable->setItem(10,1,new QTableWidgetItem("11"));
    m_pExpDataTable->setItem(10,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(10,3,new QTableWidgetItem("65.4"));
    m_pExpDataTable->setItem(10,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(10,5,new QTableWidgetItem("3.73"));
    m_pExpDataTable->setItem(10,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(11,0,new QTableWidgetItem("12"));
    m_pExpDataTable->setItem(11,1,new QTableWidgetItem("12"));
    m_pExpDataTable->setItem(11,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(11,3,new QTableWidgetItem("64.2"));
    m_pExpDataTable->setItem(11,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(11,5,new QTableWidgetItem("3.67"));
    m_pExpDataTable->setItem(11,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(12,0,new QTableWidgetItem("13"));
    m_pExpDataTable->setItem(12,1,new QTableWidgetItem("13"));
    m_pExpDataTable->setItem(12,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(12,3,new QTableWidgetItem("65.5"));
    m_pExpDataTable->setItem(12,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(12,5,new QTableWidgetItem("3.74"));
    m_pExpDataTable->setItem(12,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(13,0,new QTableWidgetItem("14"));
    m_pExpDataTable->setItem(13,1,new QTableWidgetItem("14"));
    m_pExpDataTable->setItem(13,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(13,3,new QTableWidgetItem("65.8"));
    m_pExpDataTable->setItem(13,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(13,5,new QTableWidgetItem("3.76"));
    m_pExpDataTable->setItem(13,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(14,0,new QTableWidgetItem("15"));
    m_pExpDataTable->setItem(14,1,new QTableWidgetItem("15"));
    m_pExpDataTable->setItem(14,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(14,3,new QTableWidgetItem("65.5"));
    m_pExpDataTable->setItem(14,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(14,5,new QTableWidgetItem("3.74"));
    m_pExpDataTable->setItem(14,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(15,0,new QTableWidgetItem("16"));
    m_pExpDataTable->setItem(15,1,new QTableWidgetItem("16"));
    m_pExpDataTable->setItem(15,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(15,3,new QTableWidgetItem("65.4"));
    m_pExpDataTable->setItem(15,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(15,5,new QTableWidgetItem("3.73"));
    m_pExpDataTable->setItem(15,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(16,0,new QTableWidgetItem("17"));
    m_pExpDataTable->setItem(16,1,new QTableWidgetItem("17"));
    m_pExpDataTable->setItem(16,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(16,3,new QTableWidgetItem("65.8"));
    m_pExpDataTable->setItem(16,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(16,5,new QTableWidgetItem("3.76"));
    m_pExpDataTable->setItem(16,6,new QTableWidgetItem("否"));

    m_pExpDataTable->setItem(17,0,new QTableWidgetItem("18"));
    m_pExpDataTable->setItem(17,1,new QTableWidgetItem("18"));
    m_pExpDataTable->setItem(17,2,new QTableWidgetItem("1"));
    m_pExpDataTable->setItem(17,3,new QTableWidgetItem("65.6"));
    m_pExpDataTable->setItem(17,4,new QTableWidgetItem("0.0175"));
    m_pExpDataTable->setItem(17,5,new QTableWidgetItem("3.75"));
    m_pExpDataTable->setItem(17,6,new QTableWidgetItem("否"));
    //;
#endif
}

/******************************
*功能:初始化结果table
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::InitResultTable()
{
    m_pExpResultTable = new QTableWidget(1,2);
    QStringList *tem_ResultHList = new QStringList();
    tem_ResultHList->push_back("平均通量(J/cm^2)");
    tem_ResultHList->push_back("损伤概率");
    m_pExpResultTable->setHorizontalHeaderLabels(*tem_ResultHList);
    m_pExpResultTable->verticalHeader()->setHidden(true);

    m_pResultLab = new QLabel();
    setTableTitleLabel(m_pResultLab,"数据统计结果","","rgb(0,137,167)");

    QVBoxLayout *tem_ResultVLayout = new QVBoxLayout();
    tem_ResultVLayout->setMargin(0);
    tem_ResultVLayout->setSpacing(0);
    tem_ResultVLayout->addWidget(m_pResultLab);
    tem_ResultVLayout->addWidget(m_pExpResultTable);

    m_pResultWidget = new QWidget();
    m_pResultWidget->setLayout(tem_ResultVLayout);

#if 0
    m_pExpResultTable->setItem(0,0,new QTableWidgetItem("1.73"));
    m_pExpResultTable->setItem(0,1,new QTableWidgetItem("0"));

    m_pExpResultTable->setItem(1,0,new QTableWidgetItem("3.74"));
    m_pExpResultTable->setItem(1,1,new QTableWidgetItem("0"));

    m_pExpResultTable->setItem(2,0,new QTableWidgetItem("6.12"));
    m_pExpResultTable->setItem(2,1,new QTableWidgetItem("0.1"));

    m_pExpResultTable->setItem(3,0,new QTableWidgetItem("8.22"));
    m_pExpResultTable->setItem(3,1,new QTableWidgetItem("0.3"));

    m_pExpResultTable->setItem(4,0,new QTableWidgetItem("10.5"));
    m_pExpResultTable->setItem(4,1,new QTableWidgetItem("0.5"));

    m_pExpResultTable->setItem(5,0,new QTableWidgetItem("12.3"));
    m_pExpResultTable->setItem(5,1,new QTableWidgetItem("0.6"));

    m_pExpResultTable->setItem(6,0,new QTableWidgetItem("14.7"));
    m_pExpResultTable->setItem(6,1,new QTableWidgetItem("0.8"));

    m_pExpResultTable->setItem(7,0,new QTableWidgetItem("16.1"));
    m_pExpResultTable->setItem(7,1,new QTableWidgetItem("1"));

    m_pExpResultTable->setItem(8,0,new QTableWidgetItem("18.3"));
    m_pExpResultTable->setItem(8,1,new QTableWidgetItem("1"));

    //;
#endif
}

/******************************
*功能:初始化点阵界面
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::InitShowArea()
{
//    m_pPointArea = new QPointAreaWidget(200,200,0,10);
//    m_pPointArea->m_tPointStatus.clear();
//    m_pPointArea->m_pPointInfoGroupBox->hide();

//    m_pLegendLab = new QLabel();
//    m_pSaveIDSImgBtn = new QPushButton("保存显微图像");
//    m_pSaveIDSImgBtn->setFixedSize(100,25);
//    m_pSavePlotImgBtn = new QPushButton("保存结果图");
//    m_pSavePlotImgBtn->setFixedSize(100,25);
//    //实验结果widget
//    m_pExpResultWidget = new QExpResultWidget();
//    m_pExpResultWidget->setMinimumWidth(250);
//    //IDS前图
//    m_pIDSImageBeforeWidget = new QImageShowWidget();
//    m_pIDSImageBeforeWidget->m_pImageWidget->m_sImageInfo = "测试前显微图像";

//    //IDS后图
//    m_pIDSImageAfterWidget = new QImageShowWidget();
//    m_pIDSImageAfterWidget->m_pImageWidget->m_sImageInfo = "测试后显微图像";
}


/******************************
*功能:连接信号槽
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::connectAction()
{
    connect(m_pRefreshBtn,&QPushButton::clicked,this,&QQueryWidget::onRefreshBtnClicked);
    connect(m_pHistoryBtn,&QPushButton::clicked,this,&QQueryWidget::onHistoryBtnClicked);
    connect(m_pQueryBtn,&QPushButton::clicked,this,&QQueryWidget::onQueryBtnClicked);
//    connect(m_pExpInfoTable,&QTableWidget::cellDoubleClicked,this,&QQueryWidget::onExpInfoTableDoubleClicked);
//    connect(m_pSavePlotImgBtn,&QPushButton::clicked,this,&QQueryWidget::onSavePlotImg);
//    connect(m_pExportAllDataBTn,&QPushButton::clicked,this,&QQueryWidget::onExportAllDataBtnClicked);
    connect(m_pExportSelectedDataBtn,&QPushButton::clicked,this,&QQueryWidget::onExportSelectedDataBtnClicked);
    connect(m_pAddNumBtn,&QPushButton::clicked,this,&QQueryWidget::onAddNum);
    connect(m_pReOrderBtn,&QPushButton::clicked,this,&QQueryWidget::onReorder);
}


/******************************
*功能:设置title
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::setTableTitleLabel(QLabel *pLabel, QString sTitle, QString sIconPath, QString sRgb)
{
    pLabel->setStyleSheet("color:rgb(255,255,255);background-color:"+sRgb+";");
    //当标题内容和图标路径不为空时，设置标签内容
    if (sTitle != "" && sIconPath != "")
    {
        pLabel->setText("<table width = \"100%\" >\
                                <tbody>\
                                <tr>\
                                <td><img src = \""+sIconPath+"\"></td>\
                                <td align = \"left\", valign = \"middle\"> "+sTitle+" </td>\
                                </tr></tbody></table>");

    }
    else if (sTitle != "" && sIconPath == "")
    {
        pLabel->setText(sTitle);
        pLabel->setAlignment(Qt::AlignHCenter);
    }
    QFont tem_TableFont = pLabel->font();
    tem_TableFont.setFamily("Microsoft YaHei");
    tem_TableFont.setBold(true);
    tem_TableFont.setPixelSize(14);
    pLabel->setFont(tem_TableFont);
    //;
}


/******************************
*功能:刷新按钮响应函数
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onRefreshBtnClicked()
{
    bool bOk = false;
    m_tExpInfos.clear();
    emit signal_getExpInfoFromdb(m_tExpInfos);

    //清除当前列表
    for (int i=m_pExpInfoTable->rowCount()-1; i>=0; i--)
    {
        m_pExpInfoTable->removeRow(i);
    }

    //刷新操作只显示当天实验信息
    int crntYear = QDateTime::currentDateTime().toString("yyyy").toInt(&bOk);
    int crntMonth = QDateTime::currentDateTime().toString("MM").toInt(&bOk);
    int crntDay = QDateTime::currentDateTime().toString("dd").toInt(&bOk);


    for (int i=0; i<m_tExpInfos.size(); i++)
    {
        int infoYear = m_tExpInfos[i].DataTime.mid(0,4).toInt(&bOk);
        int infoMonth = m_tExpInfos[i].DataTime.mid(5,2).toInt(&bOk);
        int infoDay = m_tExpInfos[i].DataTime.mid(8,2).toInt(&bOk);

        if (infoDay == crntDay && infoMonth == crntMonth && infoYear == crntYear)
        {
            int crntRow = m_pExpInfoTable->rowCount();
            m_pExpInfoTable->insertRow(crntRow);
            m_pExpInfoTable->setItem(crntRow,0,new QTableWidgetItem(m_tExpInfos[i].sExpNo));
            m_pExpInfoTable->setItem(crntRow,1,new QTableWidgetItem(m_tExpInfos[i].SampleName));
            m_pExpInfoTable->setItem(crntRow,2,new QTableWidgetItem(transToExpTypeStr(m_tExpInfos[i].ExpType)));
            m_pExpInfoTable->setItem(crntRow,3,new QTableWidgetItem(m_tExpInfos[i].DataTime.mid(0,10) +" "+ m_tExpInfos[i].DataTime.mid(11,8)));
        }
    }
}



/******************************
*功能:查看历史信息
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onHistoryBtnClicked()
{
    m_pDateDialog = new QDialog(this);
    m_pDateDialog->setWindowTitle(tr("选择历史实验时间范围"));
    QGridLayout *t_DialogGrid = new QGridLayout;
    QLabel *t_DateStart = new QLabel(tr("起始时间"));
    QLabel *t_DateEnd = new QLabel(tr("终止时间"));
    m_pDateStartEdit = new QDateTimeEdit;
    m_pDateEndEdit = new QDateTimeEdit;
    m_pOkBtn = new QPushButton(tr("搜索"));
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

    connect(m_pOkBtn,&QPushButton::clicked,this,&QQueryWidget::on_HistoryOkBtn);
    m_pDateDialog->setLayout(t_DialogGrid);
    m_pDateDialog->setModal(true);
    //移动弹出窗口
    QPoint tem_GlobalPt(m_pHistoryBtn->mapToGlobal(QPoint(0,0)));
    m_pDateDialog->move(tem_GlobalPt.x(),tem_GlobalPt.y()+m_pHistoryBtn->height()+5);
    m_pDateDialog->show();
}




/******************************
*功能:查询选中的实验条目
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onQueryBtnClicked()
{
    //清理实验数据表
    for (int i=m_pExpDataTable->rowCount(); i>=0; i--)
    {
        m_pExpDataTable->removeRow(i);
    }

    QItemSelectionModel *_selection = m_pExpInfoTable->selectionModel();
    QModelIndexList _selectedIndex = _selection->selectedIndexes();
//    int _nRowCnt = 0;
    qDebug()<<"TOTAL: "<<_selectedIndex.count();
    for (int i=0; i<_selectedIndex.count(); i++)
    {
        qDebug()<<"SELECTED+++++++ "<<_selectedIndex.at(i).row();
        int index = _selectedIndex.at(i).row();
        QString _sExpNo = m_pExpInfoTable->item(index,0)->text();
        qDebug()<<"SELECTED EXpNo:"<<_sExpNo;
        updateDataTable(i,_sExpNo);
//        if (index < 0 || index > 2)
//        {
//            continue;
//        }
//        qDebug()<<"has item:"<<m_tExpNo.at(index).size();
//        for (int j=0; j<m_tExpNo.at(index).size(); j++)
//        {
//            qDebug()<<j<<"+++";
//            _nRowCnt = m_pqueryTable->rowCount();
//            qDebug()<<"insert row num: "<<_nRowCnt;
//            m_pqueryTable->insertRow(_nRowCnt);
//            m_pqueryTable->setItem(_nRowCnt,0,new QTableWidgetItem(m_tExpNo.at(index).at(j)));
//            m_pqueryTable->setItem(_nRowCnt,1,new QTableWidgetItem(m_tPtNo.at(index).at(j)));
//            m_pqueryTable->setItem(_nRowCnt,2,new QTableWidgetItem(m_tShotNo.at(index).at(j)));
//            m_pqueryTable->setItem(_nRowCnt,3,new QTableWidgetItem(m_tStatus.at(index).at(j)));
//            m_pqueryTable->item(_nRowCnt,0)->setBackgroundColor(m_tColorList.at(i));
//            m_pqueryTable->item(_nRowCnt,1)->setBackgroundColor(m_tColorList.at(i));
//            m_pqueryTable->item(_nRowCnt,2)->setBackgroundColor(m_tColorList.at(i));
//            m_pqueryTable->item(_nRowCnt,3)->setBackgroundColor(m_tColorList.at(i));
//        }

    }
}

/******************************
*功能:确认选择历史实验数据时间范围
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::on_HistoryOkBtn()
{
    //初始化查询参数并查数据库
    bool bOk = false;
    m_bQueryExpInfo = false;
    m_tExpInfos.clear();
    emit signal_getExpInfoFromdb(m_tExpInfos);
    //清除当前列表
    for (int i=m_pExpInfoTable->rowCount()-1; i>=0; i--)
    {
        m_pExpInfoTable->removeRow(i);
    }

    //确定时间范围
    QString startdate = m_pDateStartEdit->text();
    QString enddate = m_pDateEndEdit->text();
    int searchYearStart = startdate.mid(0,4).toInt(&bOk);
    int searchMonthStart = startdate.mid(5,2).toInt(&bOk);
    int searchDayStart = startdate.mid(8,2).toInt(&bOk);
    int searchYearEnd = enddate.mid(0,4).toInt(&bOk);
    int searchMonthEnd = enddate.mid(5,2).toInt(&bOk);
    int searchDayEnd = enddate.mid(8,2).toInt(&bOk);

    for (int i=m_tExpInfos.size() - 1; i>=0; i--)
    {
        int infoYear = m_tExpInfos[i].DataTime.mid(0,4).toInt(&bOk);
        int infoMonth = m_tExpInfos[i].DataTime.mid(5,2).toInt(&bOk);
        int infoDay = m_tExpInfos[i].DataTime.mid(8,2).toInt(&bOk);
        //剔除非选择时间范围内的条目
        if (!IsDateValid(searchYearStart,searchMonthStart,searchDayStart,searchYearEnd,searchMonthEnd,searchDayEnd,infoYear,infoMonth,infoDay))
        {
            m_tExpInfos.remove(i);
        }
    }
    //将时间范围内的条目插入表中，近期的实验排在前面
    for (int i=0; i<m_tExpInfos.size(); i++)
    {
        int crntRow = m_pExpInfoTable->rowCount();
        m_pExpInfoTable->insertRow(crntRow);
        m_pExpInfoTable->setItem(crntRow,0,new QTableWidgetItem(m_tExpInfos[i].sExpNo));
        m_pExpInfoTable->setItem(crntRow,1,new QTableWidgetItem(m_tExpInfos[i].SampleName));
        m_pExpInfoTable->setItem(crntRow,2,new QTableWidgetItem(transToExpTypeStr(m_tExpInfos[i].ExpType)));
        m_pExpInfoTable->setItem(crntRow,3,new QTableWidgetItem(m_tExpInfos[i].DataTime.mid(0,10) +" "+ m_tExpInfos[i].DataTime.mid(11,8)));

    }
    m_pDateDialog->deleteLater();
}


/******************************
*功能:判断历史实验信息条目是否在用户期望查询的时间范围中
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QQueryWidget::IsDateValid(int sy, int sm, int sd, int ey, int em, int ed, int iy, int im, int id)
{
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


/******************************
*功能:双击实验信息表格选中实验条目
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onExpInfoTableDoubleClicked(int nrow, int ncol)
{
//    if (m_tExpInfos.size() <= 0)
//    {
//        return;
//    }
//    else
//    {
//        //重填参数列表
//        updateParasTable(nrow);
//        //清空并重填数据列表、结果列表
//        updateDataTable(nrow);
//        updateResultTable(nrow);
//        //重绘统计图和点阵图
////        updatePointArea(nrow);
//        updatePlot(nrow);
//    }
}


/******************************
*功能:更新参数列表
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::updateParasTable(int nrow)
{
//    m_pExpParasTable->clearContents();
//    m_pExpParasTable->setItem(0,0,new QTableWidgetItem(m_tExpInfos[nrow].m_sExpNo));
//    m_pExpParasTable->setItem(1,0,new QTableWidgetItem(transToExpTypeStr(m_tExpInfos[nrow].m_nExpType+1)));
//    m_pExpParasTable->setItem(2,0,new QTableWidgetItem(transToExpWaveStr(m_tExpInfos[nrow].m_nExpWaveType)));
//    m_pExpParasTable->setItem(3,0,new QTableWidgetItem(m_tExpInfos[nrow].m_sSampleName));
//    m_pExpParasTable->setItem(4,0,new QTableWidgetItem(m_tExpInfos[nrow].m_nSampleShape==0?"矩形":"圆形"));
//    m_pExpParasTable->setItem(5,0,new QTableWidgetItem(m_tExpInfos[nrow].m_dSampleLength==0?"(空)":QString::number(m_tExpInfos[nrow].m_dSampleLength)));
//    m_pExpParasTable->setItem(6,0,new QTableWidgetItem(m_tExpInfos[nrow].m_dSampleWidth==0?"(空)":QString::number(m_tExpInfos[nrow].m_dSampleWidth)));
//    m_pExpParasTable->setItem(7,0,new QTableWidgetItem(m_tExpInfos[nrow].m_dSampleDiameter==0?"(空)":QString::number(m_tExpInfos[nrow].m_dSampleDiameter)));
//    m_pExpParasTable->setItem(8,0,new QTableWidgetItem(QString::number(m_tExpInfos[nrow].m_nExpRoundCnt)));
//    m_pExpParasTable->setItem(9,0,new QTableWidgetItem(QString::number(m_tExpInfos[nrow].m_dExpStep)));
//    m_pExpParasTable->setItem(10,0,new QTableWidgetItem(QString::number(m_tExpInfos[nrow].m_nExpPtCnt)));
//    m_pExpParasTable->setItem(11,0,new QTableWidgetItem(QString::number(m_tExpInfos[nrow].m_nExpEnergySecCnt)));
//    m_pExpParasTable->setItem(12,0,new QTableWidgetItem(m_tExpInfos[nrow].m_nExpPtCntPerSec==0?"(空)":QString::number(m_tExpInfos[nrow].m_nExpPtCntPerSec)));
//    m_pExpParasTable->setItem(13,0,new QTableWidgetItem(m_tExpInfos[nrow].m_nExpShotCntPerPt==0?"(空)":QString::number(m_tExpInfos[nrow].m_nExpShotCntPerPt)));
//    m_pExpParasTable->setItem(14,0,new QTableWidgetItem(m_tExpInfos[nrow].m_sDateTime));
}

/******************************
*功能:更新数据列表
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::updateDataTable(int nIndex, QString sExpNo)
{
    m_bQueryExpDataInfo = false;
    m_bQueryIDSDefectInfo = false;
    m_tExpData.clear();
    m_tDefectInfo.clear();
    int _nColorIndex = nIndex%5;
    //查询能量、面积、通量 (以实验轮数、点数、发次数排序)
    emit signal_getExpDataFromdb(m_tExpData,sExpNo);
    //查询损伤情况 (以实验轮数、点数、发次数排序)
    emit signal_getDefectInfoFromdb(m_tDefectInfo,sExpNo);
    while (m_bQueryExpDataInfo == false || m_bQueryIDSDefectInfo == false)
    {
        Sleep(20);
        if (m_bQueryExpDataInfo == true && m_bQueryIDSDefectInfo == true)
        {
            qDebug()<<"QUERY FINISHED";
            break;
        }
    }
    qDebug()<<"ALter: "<<m_bQueryExpDataInfo<<m_bQueryIDSDefectInfo;
    qDebug()<<"RESULT: "<<m_tExpData.size()<<m_tDefectInfo.size();
    //清除当前列表
//    for (int i=m_pExpDataTable->rowCount()-1; i>=0; i--)
//    {
//        m_pExpDataTable->removeRow(i);
//    }
    //排除查询结果异常
    if (!(m_tExpData.size() == m_tDefectInfo.size()) || m_tExpData.size() == 0)
    {
        return;
    }

    for (int i=0; i<m_tExpData.size(); i++)
    {
        int crntRow = m_pExpDataTable->rowCount();
        m_pExpDataTable->insertRow(crntRow);
        m_pExpDataTable->setItem(crntRow,0,new QTableWidgetItem(QString::number(m_tExpData[i].m_nPointNo+1)));
        m_pExpDataTable->setItem(crntRow,1,new QTableWidgetItem(QString::number(m_tExpData[i].m_nShotNo+1)));
        m_pExpDataTable->setItem(crntRow,2,new QTableWidgetItem(QString::number(m_tExpData[i].m_dEnergy)));
        m_pExpDataTable->setItem(crntRow,3,new QTableWidgetItem(QString::number(m_tExpData[i].m_dFlux)));
        m_pExpDataTable->setItem(crntRow,4,new QTableWidgetItem(QString::number(m_tDefectInfo[i])));
        m_pExpDataTable->item(crntRow,0)->setBackgroundColor(m_tColorList.at(m_tExpData[i].m_nPointNo%4));
        m_pExpDataTable->item(crntRow,1)->setBackgroundColor(m_tColorList.at(m_tExpData[i].m_nPointNo%4));
        m_pExpDataTable->item(crntRow,2)->setBackgroundColor(m_tColorList.at(m_tExpData[i].m_nPointNo%4));
        m_pExpDataTable->item(crntRow,3)->setBackgroundColor(m_tColorList.at(m_tExpData[i].m_nPointNo%4));
        m_pExpDataTable->item(crntRow,4)->setBackgroundColor(m_tColorList.at(m_tExpData[i].m_nPointNo%4));
    }
}

/******************************
*功能:更新点阵界面显示
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::updatePointArea(int nRow)
{
//    m_pPointArea->m_tPointStatus.clear();
//    for (int i=0; i<m_tExpInfos[nRow].m_nExpMannualPtCnt; i++)
//    {
//        m_pPointArea->m_tPointStatus.append(E_POINT_MANNUALUSED);
//    }
//    int tem_nUnDefect = 0;
//    int tem_nDefect = 0;
//    if (m_tDefectInfo.size() == 0)
//    {
//        return;
//    }
//    for (int i=0; i<m_tExpInfos[nRow].m_nExpPtCnt; i++)
//    {
//        if (m_tDefectInfo[i] == 0){tem_nUnDefect++;}
//        if (m_tDefectInfo[i] == 1){tem_nDefect++;}
//        m_pPointArea->m_tPointStatus.append(m_tDefectInfo[i]);
//    }
//    m_pPointArea->onUpdatePointUI(m_tExpInfos[nRow].m_nSampleShape,(int)m_tExpInfos[nRow].m_dExpStep,\
//                                  m_tExpInfos[nRow].m_dSampleLength,m_tExpInfos[nRow].m_dSampleWidth,\
//                                  m_tExpInfos[nRow].m_dSampleDiameter,m_tExpInfos[nRow].m_dKeeperHeight,\
//                                  m_tExpInfos[nRow].m_dBorderMargin,m_tExpInfos[nRow].m_nExpPtCnt+\
//                                  m_tExpInfos[nRow].m_nExpMannualPtCnt,true,-1,-1);
//    updateLegend(m_tExpInfos[nRow].m_nExpMannualPtCnt,tem_nUnDefect,tem_nDefect);
}

/******************************
*功能:更新结果页面
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::updateResultTable(int nRow)
{
//    m_bQueryResultInfo = false;
//    m_tAverFlux.clear();
//    m_tDefectRatio.clear();
//    emit signal_getResultFromdb(m_tAverFlux,m_tDefectRatio,m_tExpInfos[nRow].m_sExpNo);
//    while(m_bQueryResultInfo == false)
//    {
//        if (m_bQueryResultInfo == true)
//        {
//            break;
//        }
//    }
//    if (!(m_tAverFlux.size() == m_tDefectRatio.size()) || m_tDefectRatio.size() == 0)
//    {
//        return;
//    }
//    //清除当前列表
//    for (int i=m_pExpResultTable->rowCount()-1; i>=0; i--)
//    {
//        m_pExpResultTable->removeRow(i);
//    }
//    if (m_tAverFlux.size() != m_tDefectRatio.size())
//    {
//        return;
//    }
//    for(int i=0; i<m_tAverFlux.size(); i++)
//    {
//        int crntRow = m_pExpResultTable->rowCount();
//        m_pExpResultTable->insertRow(crntRow);
//        m_pExpResultTable->setItem(crntRow,0,new QTableWidgetItem(QString::number(m_tAverFlux[i])));
//        m_pExpResultTable->setItem(crntRow,1,new QTableWidgetItem(QString::number(m_tDefectRatio[i])));
//    }
}


/******************************
*功能:将实验类型转换成string
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
QString QQueryWidget::transToExpTypeStr(int nType)
{
    switch (nType)
    {
    case M_MEASURETYPE_1On1:
    {return "1On1";}
        break;
    case M_MEASURETYPE_ROn1:
    {return "ROn1";}
        break;
    case M_MEASURETYPE_SOn1:
    {return "SOn1";}
        break;
    case M_MEASURETYPE_RASTER:
    {return "Raster";}
        break;
    default:
    {return "ErrorType";}
    }
}

/******************************
*功能:将波长类型转换成string
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
QString QQueryWidget::transToExpWaveStr(int nType)
{
    switch (nType)
    {
    case 0:
    {return "1064";}
        break;
    case 1:
    {return "532";}
        break;
    case 2:
    {return "355";}
        break;
    default:
    {return "ErrorType";}
    }
}

/******************************
*功能:查询成功，更改相应标志位
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onQueryDbFinish(int nQueryType)
{
    qDebug()<<"I got it";
    switch (nQueryType)
    {
    case 0:
    {m_bQueryExpDataInfo = true;}
        break;
    case 1:
    {m_bQueryIDSDefectInfo = true;}
        break;
    }
}


/******************************
*功能:更新legend
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::updateLegend(int nMannual, int nUnDefect, int nDefect)
{
    m_pLegendLab->setText(QString("<img src = :/res/png/Legend_Mannual.png/>手动点: %1个<img src = :/res/png/Legend_UnDefect.png/>未损伤点: %2个<br /><img src = :/res/png/Legend_Defect.png/>损伤点: %3个")\
                          .arg(nMannual).arg(nUnDefect).arg(nDefect));
}

/******************************
*功能:更新plot
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::updatePlot(int nrow)
{
//    if (m_tAverFlux.size() == 0)
//    {
//        return;
//    }
//    //copy flux and defect vector
//    m_pExpResultWidget->m_vFluxData = m_tAverFlux;
//    m_pExpResultWidget->m_vCleanFluxData = m_tAverFlux;
//    m_pExpResultWidget->m_vDefeData = m_tDefectRatio;
//    m_pExpResultWidget->m_vCleanDefeData = m_tDefectRatio;
//    //plot
//    m_pExpResultWidget->onShowResult(m_tExpInfos[nrow].m_nExpType+1);
}


/******************************
*功能:保存plot
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onSavePlotImg()
{
//    QString filename = QFileDialog::getSaveFileName(this,tr("保存图片"),"C:\\Users\\zhongqing\\Desktop",tr("Images (*.bmp)")); //选择路径
//    if(filename.length() != 0 )
//    {
//        m_pExpResultWidget->m_pExpResultPlot->saveBmp(filename);
//    }
}


/******************************
*功能:导出所有数据
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onExportAllDataBtnClicked()
{
//    if (!(m_pExcel == NULL)){delete m_pExcel;}

//    m_pExcel = new QExcel();
//    m_pExcel->initSheet();
//    m_pExcel->setSheetName("实验数据");
//    m_pExcel->insertContent(1,1,m_pExpParasTable->item(0,0)->text());
//    m_pExcel->saveSheet();
//    m_pExcel->closeSheet();

//    delete m_pExcel;
//    m_pExcel = NULL;
}


/******************************
*功能:导出选中的数据
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onExportSelectedDataBtnClicked()
{
    //选中的列号
    QItemSelectionModel *_selection = m_pExpDataTable->selectionModel();
    QModelIndexList _selectedIndex = _selection->selectedIndexes();
    QList<int> _selectedColumn;
    for (int i=0; i<_selectedIndex.count(); i++)
    {
        if (!_selectedColumn.contains(_selectedIndex.at(i).column()))
        {
            _selectedColumn.push_back(_selectedIndex.at(i).column());
        }
    }

    int _nRowCnt = m_pExpDataTable->rowCount();
    int _nColCnt = _selectedColumn.size();

    QExcel *m_pExcel = new QExcel();
    m_pExcel->initSheet();
    m_pExcel->setSheetName("实验数据");

    //ydc21427
    //判断能量计数据精度：1064nm使用200mJ档位，保留一位小数；355nm使用20mJ档，保留两位小数
    int _nEneryRangeType = -1;  //默认-1，0表示一位小数，1表示两位小数
    QString tem_Energy;
    for (int i=0; i< _nRowCnt; i++)
    {
        tem_Energy = m_pExpDataTable->item(i,2)->text();

        //如果当前实验数据表中存在两位小数的数据，则认为实验使用了20mJ档位，应保留两位小数
        if (tem_Energy.contains('.') && (tem_Energy.length() - (tem_Energy.indexOf('.')+1) == 2))
        {
            _nEneryRangeType = 1;
            break;
        }
    }
    if(_nEneryRangeType == -1)
    {
        _nEneryRangeType = 0;
    }

    QString _sDataFormat = "";
    if (_nEneryRangeType == 1)
    {
        _sDataFormat = "0.00";
    }
    else
    {
        _sDataFormat = "0.0";
    }

    int _nGroupNo = 0;
    int _nLineNo = 1;
    QString _nBackupPtNo = m_pExpDataTable->item(0,0)->text();
    for (int i=0; i<_nRowCnt; i++)
    {
        if (m_pExpDataTable->item(i,0)->text() != _nBackupPtNo)
        {
            _nBackupPtNo = m_pExpDataTable->item(i,0)->text();
            _nGroupNo++;
            _nLineNo = 1;
        }
        for (int j=0; j<_nColCnt; j++)
        {
            //对能量和通量设置小数点精度
            if (_selectedColumn.at(j) == 2 || _selectedColumn.at(j) == 3)
            {
                m_pExcel->insertContent(_nLineNo,(_nGroupNo*_nColCnt+j+1),m_pExpDataTable->item(i,_selectedColumn.at(j))->text(),_sDataFormat);
            }
            else
            {
                m_pExcel->insertContent(_nLineNo,(_nGroupNo*_nColCnt+j+1),m_pExpDataTable->item(i,_selectedColumn.at(j))->text(),"0");
            }
        }
        _nLineNo++;
    }

    m_pExcel->saveSheet("I:/OutPutData.xlsx");
    m_pExcel->closeSheet();

    delete m_pExcel;


#if 0
    QExcel *m_pExcel = new QExcel();
    m_pExcel->initSheet();
    m_pExcel->setSheetName("实验数据");
    m_pExcel->insertContent((m_tShotNo.at(i)+1),(m_tPtNo.at(i)*2+1),tem_Energy,_sDataFormat);
    m_pExcel->insertContent((m_tShotNo.at(i)+1),(m_tPtNo.at(i)*2+2),QString::number(m_tDefect.at(i)),"0");
    m_pExcel->saveSheet(m_sSavePath + QString("/IDSIMAGE/DATA/%1_%2.xlsx").arg(sSampleName).arg(sExpNo));
    m_pExcel->closeSheet();

    delete m_pExcel;
#endif
}


/******************************
*功能:数据表格中右键弹出菜单
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::RightClickSlot(QPoint pos)
{
    QModelIndex index = m_pExpDataTable->indexAt(pos);    //找到tableview当前位置信息
    iDeletcRow = index.row();    //获取到了当前右键所选的行数

    if(index.isValid())        //如果行数有效，则显示菜单
    {
        RightClick->exec(QCursor::pos());
    }
}


/******************************
*功能:删除所选条目
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::RightClickDelete(QAction *act)
{
    if(act->text() == QString::fromLocal8Bit("Delete"))   //看选中了删除这个菜单
    {
        m_pExpDataTable->removeRow(iDeletcRow);  //删除掉了表格信息
    }
}



/******************************
*功能:重新排列修改后的数据表
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onReorder()
{
    //fetch the content of the table
    int _nRowCnt = m_pExpDataTable->rowCount();

    QList<QString> _PtNo;
    QList<QString> _ShotNo;
    QList<QString> _Energy;
    QList<QString> _Flux;
    QList<QString> _Status;
    for (int i=0; i<_nRowCnt; i++)
    {
        _PtNo.push_back(m_pExpDataTable->item(i,0)->text());
        _ShotNo.push_back(m_pExpDataTable->item(i,1)->text());
        _Energy.push_back(m_pExpDataTable->item(i,2)->text());
        _Flux.push_back(m_pExpDataTable->item(i,3)->text());
        _Status.push_back(m_pExpDataTable->item(i,4)->text());
    }
    //check repeat
    for(int i=0; i<_PtNo.size()-1; i++)
    {
        for (int j=i+1; j<_PtNo.size(); j++)
        {
            if (_PtNo[j] == _PtNo[i] && _ShotNo[j] == _ShotNo[i])
            {
                QMessageBox *_mesbox = new QMessageBox();
                _mesbox->setText("存在点号与发次号均相同的数据条目");
                _mesbox->exec();
                return;
            }
        }
    }

    //reorder 1:PtNo 2:ShotNo
    for(int i=0; i<_PtNo.size()-1; i++)
    {
        for (int j=i+1; j<_PtNo.size(); j++)
        {
            if ((_PtNo[j].toInt() < _PtNo[i].toInt()) || ((_PtNo[j].toInt() == _PtNo[i].toInt()) && (_ShotNo[j].toInt() <= _ShotNo[i].toInt())))
            {
                QString _temPt, _temShot, _temEnergy, _temFlux, _temstatus;

                _temPt = _PtNo[i];
                _temShot = _ShotNo[i];
                _temEnergy = _Energy[i];
                _temFlux = _Flux[i];
                _temstatus = _Status[i];

                _PtNo.replace(i,_PtNo[j]);
                _ShotNo.replace(i,_ShotNo[j]);
                _Energy.replace(i,_Energy[j]);
                _temFlux.replace(i,_temFlux[j]);
                _Status.replace(i,_Status[j]);


                _PtNo.replace(j,_temPt);
                _ShotNo.replace(j,_temShot);
                _Energy.replace(j,_temEnergy);
                _temFlux.replace(j,_temFlux);
                _Status.replace(j,_temstatus);

            }
        }
    }

    //rewrite into table
    for (int i=_nRowCnt-1; i>=0; i--)
    {
        m_pExpDataTable->removeRow(i);
    }
    for (int i=0; i<_PtNo.size(); i++)
    {
        _nRowCnt = m_pExpDataTable->rowCount();
        m_pExpDataTable->insertRow(_nRowCnt);
        m_pExpDataTable->setItem(_nRowCnt,0,new QTableWidgetItem(_PtNo.at(i)));
        m_pExpDataTable->setItem(_nRowCnt,1,new QTableWidgetItem(_ShotNo.at(i)));
        m_pExpDataTable->setItem(_nRowCnt,2,new QTableWidgetItem(_Energy.at(i)));
        m_pExpDataTable->setItem(_nRowCnt,3,new QTableWidgetItem(_Flux.at(i)));
        m_pExpDataTable->setItem(_nRowCnt,4,new QTableWidgetItem(_Status.at(i)));
        m_pExpDataTable->item(_nRowCnt,0)->setBackgroundColor(m_tColorList.at(_PtNo[i].toInt()%4));
        m_pExpDataTable->item(_nRowCnt,1)->setBackgroundColor(m_tColorList.at(_PtNo[i].toInt()%4));
        m_pExpDataTable->item(_nRowCnt,2)->setBackgroundColor(m_tColorList.at(_PtNo[i].toInt()%4));
        m_pExpDataTable->item(_nRowCnt,3)->setBackgroundColor(m_tColorList.at(_PtNo[i].toInt()%4));
        m_pExpDataTable->item(_nRowCnt,4)->setBackgroundColor(m_tColorList.at(_PtNo[i].toInt()%4));
    }
}




/******************************
*功能:调整点号
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QQueryWidget::onAddNum()
{
    int _nAddNum = m_pAddNumEdit->text().toInt();
    if (_nAddNum == 0) {return;}
    //选中的列号
    QItemSelectionModel *_selection = m_pExpDataTable->selectionModel();
    QModelIndexList _selectedIndex = _selection->selectedIndexes();
    for (int i=0; i<_selectedIndex.count(); i++)
    {
        int _nRowIndex = _selectedIndex.at(i).row();
        m_pExpDataTable->item(_nRowIndex,0)->setText(QString::number(m_pExpDataTable->item(_nRowIndex,0)->text().toInt()+_nAddNum));
    }
}
