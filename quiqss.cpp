#include "quiqss.h"
/***********************
 *
 * 界面扁平化设计
 * create:2018-04-04
 *
 *
 * ************************/


QUIQss::QUIQss()
{

}

void QUIQss::setBtnQss(QPushButton *btn,
                        QString normalColor, QString normalTextColor,
                        QString hoverColor, QString hoverTextColor,
                        QString pressedColor, QString pressedTextColor)
{
    QStringList qss;
    qss.append(QString("QPushButton{border-style:none;padding:10px;border-radius:5px;color:%1;background:%2;}").arg(normalTextColor).arg(normalColor));
    qss.append(QString("QPushButton:hover{color:%1;background:%2;}").arg(hoverTextColor).arg(hoverColor));
    qss.append(QString("QPushButton:pressed{color:%1;background:%2;}").arg(pressedTextColor).arg(pressedColor));
    btn->setStyleSheet(qss.join(""));
}

void QUIQss::setTxtQss(QLineEdit *txt, QString normalColor, QString focusColor)
{
    QStringList qss;
    qss.append(QString("QLineEdit{border-style:none;padding:6px;border-radius:5px;border:2px solid %1;}").arg(normalColor));
    qss.append(QString("QLineEdit:focus{border:2px solid %1;}").arg(focusColor));
    txt->setStyleSheet(qss.join(""));
}

void QUIQss::setBarQss(QProgressBar *bar, QString normalColor, QString chunkColor)
{
    int barHeight = 8;
    int barRadius = 8;

    QStringList qss;
    qss.append(QString("QProgressBar{font:9pt;height:%2px;background:%1;border-radius:%3px;text-align:center;border:1px solid %1;}").arg(normalColor).arg(barHeight).arg(barRadius));
    qss.append(QString("QProgressBar:chunk{border-radius:%2px;background-color:%1;}").arg(chunkColor).arg(barRadius));
    bar->setStyleSheet(qss.join(""));
}

void QUIQss::setSliderQss(QSlider *slider, QString normalColor, QString grooveColor, QString handleColor)
{
    int sliderHeight = 8;
    int sliderRadius = 4;
    int handleWidth = 13;
    int handleRadius = 6;
    int handleOffset = 3;

    QStringList qss;
    qss.append(QString("QSlider::groove:horizontal,QSlider::add-page:horizontal{height:%2px;border-radius:%3px;background:%1;}").arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::sub-page:horizontal{height:%2px;border-radius:%3px;background:%1;}").arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    qss.append(QString("QSlider::handle:horizontal{width:%2px;margin-top:-%3px;margin-bottom:-%3px;border-radius:%4px;"
                       "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 #FFFFFF,stop:0.8 %1);}")
               .arg(handleColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));
    slider->setStyleSheet(qss.join(""));
}
