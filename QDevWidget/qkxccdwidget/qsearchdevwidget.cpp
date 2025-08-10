/**************************************************
*文件说明:相机搜索功能
*
*维护记录:
*2019-01-25 create by lg
******************************************************/
#include "qsearchdevwidget.h"
#include <QGridLayout>

QSearchDevWidget::QSearchDevWidget(QKxccdCtrl* pKxccdCtrl,QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_ShowModal, true);
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowMaximizeButtonHint);

    this->setWindowTitle("搜索相机");

    m_pRefreshButton = new QPushButton(this);
    m_pRefreshButton->setText("刷新");
    connect(m_pRefreshButton,&QPushButton::clicked,this,&QSearchDevWidget::onRefreshFun);

    m_pCloseButton = new QPushButton(this);
    m_pCloseButton->setText("关闭");
    connect(m_pCloseButton,&QPushButton::clicked,this,&QSearchDevWidget::onCloseWindow);

    m_pRefreshButton->setMaximumWidth(100);
    m_pRefreshButton->setMaximumHeight(30);

    m_pCloseButton->setMaximumWidth(100);
    m_pCloseButton->setMaximumHeight(30);

    m_pListWidget = new QListWidget(this);
    m_pListWidget->setMinimumWidth(250);
    m_pListWidget->setMinimumHeight(300);

    m_pshowInfoLabel = new QLabel();

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(m_pListWidget,0,0,6,2);
    playout->addWidget(m_pRefreshButton,6,0,1,1);
    playout->addWidget(m_pCloseButton,6,1,1,1);
    playout->addWidget(m_pshowInfoLabel,8,0,1,2);


    m_nSearchTime = 0;

    _pKxccdCtrl = pKxccdCtrl;
    if(!_pKxccdCtrl->initSearchDevResource())
    {
        QMessageBox::warning(this,"提示","初始化资源失败，请关闭!");
        return;
    }

    m_pSearchTimer = new QTimer();
    connect(m_pSearchTimer,&QTimer::timeout,this,&QSearchDevWidget::onSearchTimer);
    m_pSearchTimer->start(200);
    m_pRefreshButton->setEnabled(false);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
* 2019-01-25  create by lg
******************************************/
void QSearchDevWidget::onRefreshFun()
{
    m_nSearchTime = 0;

    _pKxccdCtrl->m_sSearchDevIPList.clear();
    m_pListWidget->clear();

    m_pRefreshButton->setEnabled(false);

    m_pSearchTimer->start();
}

/******************************************
* 功能:关闭功能
* 输入:
* 输出:
* 返回值:
* 维护记录:
* 2019-01-25  create by lg
******************************************/
void QSearchDevWidget::onCloseWindow()
{
    m_pSearchTimer->stop();
    _pKxccdCtrl->clearSearchDevResource();
    this->close();
}

/******************************************
* 功能:定时器 执行搜索任务
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSearchDevWidget::onSearchTimer()
{
    QString l_sTemp ="正在搜索,请稍等";
    QString l_sWait[3]={".","..","..."};
    QString l_sValue = "";

    _pKxccdCtrl->startSearchDev();

    //显示搜索状态
    l_sValue = l_sTemp + l_sWait[m_nSearchTime%3];
    m_nSearchTime++;

    if (m_nSearchTime >= 100)
    {
        m_pSearchTimer->stop();
        l_sValue = "搜索结束";
        m_pRefreshButton->setEnabled(true);
    }

    m_pshowInfoLabel->setText(l_sValue);

    //更新列表显示
    m_pListWidget->clear();

    foreach (QString item, _pKxccdCtrl->m_sSearchDevIPList)
    {
        m_pListWidget->addItem(item);
    }
}

/******************************************
* 功能:关闭事件
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSearchDevWidget::closeEvent(QCloseEvent *event)
{
    m_pSearchTimer->stop();
    _pKxccdCtrl->clearSearchDevResource();
}
