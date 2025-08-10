#ifndef RELABEL_H
#define RELABEL_H

#include <QObject>
#include <QPaintEvent>
#include <QPixmap>
#include <QLabel>
#include <QVector>

#define M_DRAW_CIRCLE 0
#define M_DRAW_RECT 1
#define M_TYPE_ORI 3
#define M_TYPE_DEF 4

class ReLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ReLabel(QWidget *parent = 0);
    void SaveRePaint(QString sbackpath, QString sSavePath);
public slots:
    void paintEvent(QPaintEvent* event);
public:
    QVector<double> m_x;
    QVector<double> m_y;
    QVector<double> m_radius;
    QVector<double> m_width;
    QVector<double> m_height;
    QVector<double> m_Area;
    int m_nDrawFlag;
    int m_nTypeFlag;
    void copylabel(ReLabel *labelobj);
    double m_lfminiRatio;
    double m_dMinx;
    double m_dMaxX;
    double m_dMiny;
    double m_dMaxy;

    double m_dXR;
    double m_dYR;
    double m_dXC;
    double m_dYC;
signals:

};

#endif // RELABEL_H
