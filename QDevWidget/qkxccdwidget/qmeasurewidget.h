#ifndef QMEASUREWIDGET_H
#define QMEASUREWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include "QKxccdCtrl/kxccddefine.h"

class QMeasureWidget:public QWidget
{

     Q_OBJECT
public:
    QMeasureWidget(QWidget* pParent);


public slots:
    void SetLineType();


signals:
    void singnal_changeType(int);

public:
    void InitUI(int nType);
    void PlotLine(int nType);
    void showMeasureRel();

private:
    void closeEvent(QCloseEvent* event);


private:
        QCustomPlot* m_pCustomPlot;//显示波形图
        QRadioButton* m_pLineXRadioButton;
        QRadioButton* m_pLineYRadioButton;
        QLabel*       m_pMeasureDataLabel;

public:
        //测量时参数信息
        MeasureData m_tMeasureData;

};

#endif // QMEASUREWIDGET_H
