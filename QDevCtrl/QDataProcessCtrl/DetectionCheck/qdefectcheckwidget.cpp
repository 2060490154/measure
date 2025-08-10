#include "qdefectcheckwidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>
#include <QTime>
#include <QDebug>
#include <QPainter>

QDefectCheckWidget::QDefectCheckWidget(QWidget *parent) : QWidget(parent)
{
    m_pDefectProcess = new QDefectProcess();
    initUI();
    connect(m_pSelectOriginalButton,&QPushButton::clicked,this,&QDefectCheckWidget::onSelectOriginalButtonClick);
    connect(m_pSelectDestImageButton,&QPushButton::clicked,this,&QDefectCheckWidget::onSelectDestImageButtonClick);
    connect(m_pProcessButton,&QPushButton::clicked,this,&QDefectCheckWidget::onProcessImageButtonClick);

    m_pOriginalImageEdit->setText("G:/Exercise/Matlab损伤检测/detection/final_code/picture2/S2-1.jpg");
    m_pDestImageEdit->setText("G:/Exercise/Matlab损伤检测/detection/final_code/picture2/S2-5.jpg");



//    QString sOrigFile = m_pOriginalImageEdit->toPlainText();
//    QString sDestFile = m_pDestImageEdit->toPlainText();

    //load data from QImage
    m_pDefectProcess->m_OriImg = new YImage("G:/Exercise/Matlab损伤检测/detection/final_code/picture2/S2-1.jpg");
    m_pDefectProcess->m_DefImg = new YImage("G:/Exercise/Matlab损伤检测/detection/final_code/picture2/S2-5.jpg");

#if 1
    //Test data: read data from txt and cover the data from the QImage
    m_pDefectProcess->readData();
    m_pDefectProcess->m_OriImg->m_R = m_pDefectProcess->m_temdata[0];
    m_pDefectProcess->m_OriImg->m_G = m_pDefectProcess->m_temdata[1];
    m_pDefectProcess->m_OriImg->m_B = m_pDefectProcess->m_temdata[2];
    m_pDefectProcess->m_DefImg->m_R = m_pDefectProcess->m_temdata[3];
    m_pDefectProcess->m_DefImg->m_G = m_pDefectProcess->m_temdata[4];
    m_pDefectProcess->m_DefImg->m_B = m_pDefectProcess->m_temdata[5];
    //;
#endif
    m_pDefectProcess->m_DefImg->m_wavelength.resize(m_pDefectProcess->m_DefImg->m_height,m_pDefectProcess->m_DefImg->m_width);

}


void QDefectCheckWidget::initUI()
{
    QLabel* pLabel1 = new QLabel("选择损伤前图片:");
    m_pOriginalImagLabel = new QLabel(this);
    m_pDestImagLabel = new QLabel(this);
    m_pProcessImagLabel = new QLabel(this);


    m_pOriginalImageEdit = new QTextEdit(this);
    m_pDestImageEdit = new QTextEdit(this);

    m_pSelectOriginalButton = new QPushButton("...");
    m_pSelectDestImageButton = new QPushButton("...");
    m_pProcessButton = new QPushButton("损伤检测");

    m_pDestImageEdit->setMaximumWidth(600);
    m_pOriginalImageEdit->setMaximumWidth(600);
    pLabel1->setMaximumWidth(150);

    m_pDestImageEdit->setMaximumHeight(30);
    m_pOriginalImageEdit->setMaximumHeight(30);


    m_pSelectOriginalButton->setMaximumWidth(30);
    m_pSelectDestImageButton->setMaximumWidth(30);
    m_pOriginalImagLabel->setMaximumSize(400,400);
    m_pDestImagLabel->setMaximumSize(400,400);
    m_pProcessImagLabel->setMaximumSize(400,400);


    QGridLayout* playout = new QGridLayout(this);

    for(int i = 0; i < 6;i++)
    {
        playout->setColumnStretch(i,1);
        playout->setRowStretch(i,1);
    }

    playout->addWidget(pLabel1,0,0,1,1);
    playout->addWidget(m_pOriginalImageEdit,0,1,1,1);
    playout->addWidget(m_pSelectOriginalButton,0,2,1,1);


    playout->addWidget(new QLabel("选择待处理图片:"),1,0,1,1);
    playout->addWidget(m_pDestImageEdit,1,1,1,1);
    playout->addWidget(m_pSelectDestImageButton,1,2,1,1);

    playout->addWidget(m_pOriginalImagLabel,2,0,1,3);
    playout->addWidget(m_pDestImagLabel,2,3,1,3);
    playout->addWidget(m_pProcessImagLabel,3,0,1,3);

    playout->addWidget(m_pProcessButton,4,0,1,3);

}

/*******************************************************************
**功能：选择原始图像
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectCheckWidget::onSelectOriginalButtonClick()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("原始图像"),"../",tr("jpg (*.jpg)")); //选择路径
    if(filename.length() == 0 )
    {
       return;
    }
    m_pOriginalImageEdit->setText(filename);

    QPixmap Orignal(filename);
    showImage(Orignal,m_pOriginalImagLabel);


}

/*******************************************************************
**功能：选择待处理图像
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectCheckWidget::onSelectDestImageButtonClick()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("待处理图像"),"../",tr("jpg (*.jpg)")); //选择路径
    if(filename.length() == 0 )
    {
       return;
    }
    m_pDestImageEdit->setText(filename);
    QPixmap Orignal(filename);
    showImage(Orignal,m_pDestImagLabel);
}


/*******************************************************************
**功能：图像处理
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectCheckWidget::onProcessImageButtonClick()
{
//    QString sOrigFile = m_pOriginalImageEdit->toPlainText();
//    QString sDestFile = m_pDestImageEdit->toPlainText();

//    QPixmap TempPixmap(sOrigFile);
//    m_pOriginalImagLabel->setPixmap(TempPixmap.scaled(m_pOriginalImagLabel->size(), Qt::KeepAspectRatio));
//    m_pOriginalImagLabel->repaint();
//    QPixmap TempPixmap1(sDestFile);
//    m_pDestImagLabel->setPixmap(TempPixmap1.scaled(m_pDestImagLabel->size(), Qt::KeepAspectRatio));
//    m_pDestImagLabel->repaint();

//      QImage img(sDestFile);

    QTime t_totaltime;
    t_totaltime.start();
    m_pDefectProcess->caclworkflow();
    qDebug()<<"total time: "<<t_totaltime.elapsed();
//    int nlen = m_pDefectProcess->m_tdata.size();
//    for(int i = 0; i < nlen; i++)
//    {
//        if(m_pDefectProcess->m_tdata[i].bIsDefect == true)
//        {
//            img.setPixel(m_pDefectProcess->m_tdata[i].nx,m_pDefectProcess->m_tdata[i].ny,qRgb(255,0,0));
//        }
//        else
//            img.setPixel(m_pDefectProcess->m_tdata[i].nx,m_pDefectProcess->m_tdata[i].ny,qRgb(255,255,255));

//    }
//    TempPixmap = QPixmap::fromImage(img);
//    showImage(TempPixmap,m_pProcessImagLabel);

}

/*******************************************************************
**功能：显示图片  缩放
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectCheckWidget::showImage(QPixmap pPixmap,QLabel* pshowLabel)
{
    QSize tSize = pshowLabel->size();
    pshowLabel->setPixmap(pPixmap.scaled(tSize, Qt::KeepAspectRatio));

  }
