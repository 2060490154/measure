#ifndef QCUSTOMSLIDERWIDGET_H
#define QCUSTOMSLIDERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>

class QCustomSliderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QCustomSliderWidget(QWidget *parent = nullptr);

signals:
    void signal_SliderRelease(int nPostion);//更新当前数据 放开时

    void signal_SliderMoved(int nPostion);//slider正在移动
public slots:
    void onSliderMoved(int nPostion);//滑动
    void onSliderRelease();//滑块松开


public:
    void setThrosholdAndCurrentValue(int nMaxvalue,int nMinValue,int nCurrentvalue);
public:
    QLabel* m_pDisplayValueLabel;
    QLabel* m_pMaxValueLabel;
    QLabel* m_pMinValueLabel;

    QSlider* m_pSlider;

};

#endif // QCUSTOMSLIDERWIDGET_H
