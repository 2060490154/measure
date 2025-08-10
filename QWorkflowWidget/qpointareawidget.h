#ifndef QPOINTAREAWIDGET_H
#define QPOINTAREAWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QLabel>
#include "define.h"



class QPointAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QPointAreaWidget(double dmaxWidth, double dmaxHigh, int nType, double dStep, QWidget *parent = 0);

    void InitUI();
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *event);//鼠标选中
    void showPointInfo();



signals:

    void updateWorkflow();
public slots:
    void onEnableSelectPoint();
    void onDisEnabelSelectPoint();
    void onEnableAllPoint();
//     void onUpdatePointUI(int nSizeType,int nStep,int nRealWidth,int nRealHigh,int nRadius);
    //2020yi
    void onUpdatePointUI(int nSizeType, double dStep, double dRealWidth, double dRealHigh, double dRadius);

public:

    QLabel* m_pPointInfoLabel;

    //2020dong
    double m_dMaxHigh;
    double m_dMaxWidth;

    //实际尺寸大小
    double m_dRealWidth;
    double m_dRealHigh;

    //能够打的尺寸大小
    //2020yi
    double m_dWidth;
    double m_dHigh;

    int m_nSizeType;
//    int m_nStep;
    //2020yi
    double m_dStep;
    float m_lfScal;//坐标放大比例
    QList<QPointInfo> m_tPointList;

    QMenu* m_pMenu;



    //画图相关
    int m_nStartPosx;
    int m_nStartPosy;
    int m_ncirclRedius;//圆点大小
};

#endif // QPOINTAREAWIDGET_H
