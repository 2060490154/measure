#ifndef RELABEL_H
#define RELABEL_H

#include <QObject>
#include <QPaintEvent>
#include <QPixmap>
#include <QLabel>
//#include "qdefectprocess.h"
#include <QVector>
class ReLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ReLabel(QWidget *parent = 0);
public slots:
    void paintEvent(QPaintEvent* event);
public:
//    QVector<block> m_blk;
    QVector<double> m_x;
    QVector<double> m_y;
    QVector<double> m_radius;
};

#endif // RELABEL_H
