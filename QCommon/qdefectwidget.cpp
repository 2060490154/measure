#include "qdefectwidget.h"
#include <QApplication>
#include <QDesktopWidget>

QDefectWidget::QDefectWidget(QWidget *parent) : QWidget(parent)
{
    m_pOriImgLab = new ReLabel(this);
    m_pDefImgLab = new ReLabel(this);
    m_pNoBtn = new QPushButton("未损伤",this);
    m_pYesBtn = new QPushButton("损伤",this);

    m_basesize = QApplication::desktop()->height()*3/4;
    m_pOriImgLab->setGeometry(0,0,m_basesize,m_basesize);
    m_pDefImgLab->setGeometry(m_basesize + 20,0,m_basesize,m_basesize);
    m_pYesBtn->setGeometry(m_basesize - 150,m_basesize+20,100,25);
    m_pNoBtn->setGeometry(m_basesize + 50,m_basesize+20,100,25);

    m_pDefImgLab->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_pOriImgLab->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    connect(m_pYesBtn,&QPushButton::clicked,this,&QDefectWidget::onYesBtnClicked);
    connect(m_pNoBtn,&QPushButton::clicked,this,&QDefectWidget::onNoBtnClicked);
}


/******************************
*功能:添加原始图像
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDefectWidget::setOriImg(QImage OriImg)
{
    m_pOriImgLab->setPixmap(QPixmap::fromImage(OriImg).scaled(QSize(m_basesize,m_basesize),Qt::KeepAspectRatio));
}


/******************************
*功能:添加损伤后图像
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDefectWidget::setDefImg(QImage DefImg)
{
    m_pDefImgLab->setPixmap(QPixmap::fromImage(DefImg).scaled(QSize(m_basesize,m_basesize),Qt::KeepAspectRatio));
}


void QDefectWidget::onYesBtnClicked()
{
    //保存损伤图片
    m_pDefImgLab->SaveRePaint(m_sDefImgPath,m_sSavePath);
    emit signal_YesBtnClicked(m_tCurrentTaskExpInfo);
}

void QDefectWidget::onNoBtnClicked()
{
    emit signal_NoBtnClicked(m_tCurrentTaskExpInfo);
}

void QDefectWidget::SaveDefImg()
{}
