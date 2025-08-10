#include "qimageexpandwidget.h"
#include <QGridLayout>


QImageExpandWidget::QImageExpandWidget(QWidget *parent) : QWidget(parent)
{

    this->setMinimumSize(500,500);
    m_pPixmap = NULL;
    m_pImageLabel = new QLabel(this);
    m_pScrollArea = new QScrollArea(this);

    m_pImageLabel->setBackgroundRole(QPalette::Base);
    m_pScrollArea->setWidgetResizable(true);

    installEventFilter(m_pScrollArea);

    m_pScrollArea->setWidget(m_pImageLabel);
    m_pScrollArea->setVisible(true);

    QGridLayout* playout = new QGridLayout(this);
    playout->addWidget(m_pScrollArea);

    this->setAttribute(Qt::WA_QuitOnClose,false);

}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QImageExpandWidget::showImage( QPixmap* pPixmap)
{
    m_pPixmap = pPixmap;
}

/*******************************************************************
**功能：绘图
**输入：
**输出：
**返回值：
*******************************************************************/
void QImageExpandWidget::paintEvent(QPaintEvent *event)
{
    if(m_pPixmap != NULL)
    {
        m_pImageLabel->setPixmap(*m_pPixmap);
    }
}
