#include "qimagelabel.h"
#include <QImageReader>

QImageLabel::QImageLabel(QString sImageInfo,QWidget *parent) : QWidget(parent)
{
    m_pImageExpandWidget = NULL;

    m_pImagelabel = new QLabel();
    m_pImageInfolabel = new QLabel(sImageInfo);

    m_pImagelabel->setBackgroundRole(QPalette::Base);
    m_pImagelabel->setStyleSheet("border: 1px solid #3C80B1;");
    m_pImagelabel->setAlignment(Qt::AlignCenter);
//    m_pImagelabel->setMaximumHeight(500);
//    m_pImagelabel->setMaximumWidth(500);
//    m_pImagelabel->setMinimumHeight(200);
//    m_pImagelabel->setMinimumWidth(200);

//    m_pImagelabel->setFixedHeight(250);
//    m_pImagelabel->setFixedWidth(250);
    m_pImageInfolabel->setAlignment(Qt::AlignCenter);


    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0,5);
    playout->setRowStretch(1,1);
    playout->addWidget(m_pImagelabel,0,0,1,1);
    playout->addWidget(m_pImageInfolabel,1,0,1,1);//图片信息

}
QImageLabel::~QImageLabel()
{
    if(m_pImageExpandWidget != NULL)
    {
        m_pImageExpandWidget->close();
        delete m_pImageExpandWidget;
        m_pImageExpandWidget = NULL;
    }
}
/*******************************************************************
**功能：显示图像
**输入：
**输出：
**返回值：
*******************************************************************/
void QImageLabel::showImage(QPixmap pPixmap)
{
    QSize tSize = m_pImagelabel->size();
    m_OrignalPixMap = pPixmap;
    m_pImagelabel->setPixmap(pPixmap.scaled(tSize, Qt::KeepAspectRatio));

  }

/*******************************************************************
**功能：双击放大图像
**输入：
**输出：
**返回值：
*******************************************************************/
void QImageLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!m_OrignalPixMap.isNull())
    {
        if(m_pImageExpandWidget != NULL)
        {
            m_pImageExpandWidget->close();
            delete m_pImageExpandWidget;
            m_pImageExpandWidget = NULL;
        }
        m_pImageExpandWidget = new QImageExpandWidget();
        m_pImageExpandWidget->showImage(&m_OrignalPixMap);
        m_pImageExpandWidget->setWindowTitle("放大图像显示");
        m_pImageExpandWidget->show();

    }

}
