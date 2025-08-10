/**************************************************
*文件说明:自定义滑块模块 显示最大值 最小值 当前值
*创建人:李刚
*维护记录:
*2019-01-24 create by lg
******************************************************/
#include "qcustomsliderwidget.h"

QCustomSliderWidget::QCustomSliderWidget(QWidget *parent) : QWidget(parent)
{

    m_pDisplayValueLabel = new QLabel(this);
    m_pDisplayValueLabel->hide();
    m_pDisplayValueLabel->setFixedSize(50,30);

    m_pMaxValueLabel = new QLabel(this);
    m_pMinValueLabel = new QLabel(this);
    m_pSlider = new QSlider();
    m_pSlider->setOrientation(Qt::Horizontal);

    QGridLayout* pLayout = new QGridLayout(this);

    pLayout->addWidget(m_pMinValueLabel,0,0,1,1);
    pLayout->addWidget(m_pSlider,0,1,1,3);
    pLayout->addWidget(m_pMaxValueLabel,0,4,1,1);

    connect(m_pSlider,&QSlider::sliderMoved,this,&QCustomSliderWidget::onSliderMoved);
    connect(m_pSlider,&QSlider::sliderReleased,this,&QCustomSliderWidget::onSliderRelease);
}


/******************************************
*功能:设置最大值 最小值数据
*输入:nMaxvalue为最大值 nMinValue为最小值
*输出: 显示最大值 最小值数据
*返回值:无
*创建人:李刚
*修改记录:
* 2019-01-24 create by lg
******************************************/
void QCustomSliderWidget::setThrosholdAndCurrentValue(int nMaxvalue, int nMinValue,int nCurrentValue)
{
    m_pMaxValueLabel->setText(QString("%1").arg(nMaxvalue));
    m_pMinValueLabel->setText(QString("%1").arg(nMinValue));

    m_pSlider->setMinimum(nMinValue);
    m_pSlider->setMaximum(nMaxvalue);
    m_pSlider->setValue(nCurrentValue);
}

/******************************************
*功能:滑块滑动处理
*输入:nPostion位置信息
*输出:标签显示当前位置数据
*返回值:无
*创建人:李刚
*修改记录:
* 2019-01-24 create by lg
******************************************/
void QCustomSliderWidget::onSliderMoved(int nPostion)
{
    m_pDisplayValueLabel->setText(QString::number(nPostion));
    int ax = (m_pSlider->width())*m_pSlider->value()/(float)(m_pSlider->maximum()-m_pSlider->minimum());
    int ay = -10;
    m_pDisplayValueLabel->move(ax+m_pDisplayValueLabel->width(),ay);
    m_pDisplayValueLabel->show();
    emit signal_SliderMoved(nPostion);
}

/******************************************
*功能:滑块松开事件处理
*输入:无
*输出:无
*返回值:无
*创建人:李刚
*修改记录:
* 2019-01-24 create by lg
******************************************/
void QCustomSliderWidget:: onSliderRelease()
{
    m_pDisplayValueLabel->hide();
    emit signal_SliderRelease(m_pSlider->value());
}
