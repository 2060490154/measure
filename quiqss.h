#ifndef QUIQSS_H
#define QUIQSS_H
#include <qlabel.h>
#include <QPushButton>
#include <qprogressbar.h>
#include <qslider.h>
#include <qlineedit.h>

class QUIQss
{
public:
    QUIQss();
public:
    static void setBtnQss(QPushButton *btn,
                   QString normalColor, QString normalTextColor,
                   QString hoverColor, QString hoverTextColor,
                   QString pressedColor, QString pressedTextColor);
    static void setTxtQss(QLineEdit *txt, QString normalColor, QString focusColor);
    static void setBarQss(QProgressBar *bar, QString normalColor, QString chunkColor);
    static void setSliderQss(QSlider *slider, QString normalColor, QString grooveColor, QString handleColor);
};

#endif // QUIQSS_H
