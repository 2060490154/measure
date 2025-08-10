#include "qImageinfowidget.h"
#include <QDebug>
#include <windows.h>
#include <QApplication>

QRunTimeImageWidget::QRunTimeImageWidget(QWidget *parent) : QWidget(parent)
{
    m_pBeforeImageWidget = new QImageLabel("测试前显微图像",this);
    m_pAfterImageWidget = new QImageLabel("测试后显微图像",this);
    m_pKxccdImageWidget = new QImageLabel("近场图像",this);

    m_pBeforeImageWidget->setMaximumHeight(300);
    m_pBeforeImageWidget->setMaximumWidth(300);

    m_pBeforeImageWidget->setMinimumHeight(250);
    m_pBeforeImageWidget->setMinimumWidth(250);

//    m_pBeforeImageWidget->setFixedHeight(300);
//    m_pBeforeImageWidget->setFixedWidth(300);
    m_pAfterImageWidget->setMaximumHeight(300);
    m_pAfterImageWidget->setMaximumWidth(300);

    m_pAfterImageWidget->setMinimumHeight(250);
    m_pAfterImageWidget->setMinimumWidth(250);

//    m_pAfterImageWidget->setFixedHeight(300);
//    m_pAfterImageWidget->setFixedWidth(300);

    m_pKxccdImageWidget->setMaximumHeight(300);
    m_pKxccdImageWidget->setMaximumWidth(300);
    m_pKxccdImageWidget->setMinimumHeight(250);
    m_pKxccdImageWidget->setMinimumWidth(250);

//    m_pKxccdImageWidget->setFixedHeight(300);
//    m_pKxccdImageWidget->setFixedWidth(300);


    QGridLayout* playout = new QGridLayout(this);
    playout->addWidget(m_pBeforeImageWidget,0,0,1,1);
    playout->addWidget(m_pAfterImageWidget,0,1,1,1);
    playout->addWidget(m_pKxccdImageWidget,0,2,1,1);

//    QHBoxLayout* pVBox = new QHBoxLayout(this);
//    pVBox->addWidget(m_pBeforeImageWidget);
//    pVBox->addWidget(m_pAfterImageWidget);
//    pVBox->addWidget(m_pKxccdImageWidget);


    for (int i = 0; i < 256; i++)
    {
        m_vcolorTable.append(qRgb(i, i, i));
    }


}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QRunTimeImageWidget::onShowIDSCapImage(QVariant varTaskExpInfo,int nImageType,QString sPath)
{
    tExpInfo tTaskExpInfo = varTaskExpInfo.value<tExpInfo>();
    if(!QFile(sPath).exists())
    {
        qDebug()<<nImageType<<"Not Found IDSIMG";
        return;
    }

    QPixmap TempPixmap;
    TempPixmap.load(sPath);
	
    if(nImageType == 0)
    {
        m_pBeforeImageWidget->m_pImagelabel->clear();
        m_pAfterImageWidget->m_pImagelabel->clear();
        m_pBeforeImageWidget->showImage(TempPixmap);
        m_pBeforeImageWidget->m_pImageInfolabel->setText(QString("测试前显微图像(第%1点，第%2发次)").arg(tTaskExpInfo.m_nPointNo+1).arg(tTaskExpInfo.m_nTimes+1));
    }

    if(nImageType == 1)
    {
        m_pAfterImageWidget->m_pImagelabel->clear();
        m_pAfterImageWidget->showImage(TempPixmap);
        m_pAfterImageWidget->m_pImageInfolabel->setText(QString("测试后显微图像(第%1点，第%2发次)").arg(tTaskExpInfo.m_nPointNo+1).arg(tTaskExpInfo.m_nTimes+1));
    }
    this->update();
}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QRunTimeImageWidget::onShowKxccdCapImage(char* pbuff,int nwidth,int nhigh,QVariant varTaskExpInfo)
{
    m_ImageBuff = QImage((uchar*)pbuff, nwidth, nhigh, QImage::Format_Indexed8);  //封装QImage
    m_ImageBuff.setColorTable(m_vcolorTable); //设置颜色表
    m_ImageBuff.mirrored(false,true);

    QPixmap TempPixmap = QPixmap::fromImage(m_ImageBuff);
    m_pKxccdImageWidget->showImage(TempPixmap);


//    QString m_str = QString("E:\\qtWorkSpace\\MingwPro\\FluxMeasureApp\\FluxMeasureApp\\CCDIMG\\%1.jpg").arg((m_count+1)%17);
//    QPixmap m_map;
//    m_map.load(m_str,"JPG");
//    m_count++;
//    m_pKxccdImageWidget->showImage(m_map);
    this->update();
}
