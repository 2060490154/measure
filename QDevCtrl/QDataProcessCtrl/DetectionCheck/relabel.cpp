#include "relabel.h"
#include <QPainter>
#include <QDebug>
ReLabel::ReLabel(QWidget *parent) : QLabel(parent)
{

}

void ReLabel::paintEvent(QPaintEvent *event)
{
    double m_miniRatio = (double)700/2048;
    qDebug()<<m_miniRatio<<"ratio";
    QLabel::paintEvent(event);
    QPainter m_tpt(this);
    m_tpt.setPen(QPen(Qt::black));
    m_tpt.drawRect(0,0,2448*m_miniRatio,2048*m_miniRatio);
    int t_size = m_x.size();
    for (int i=0; i<t_size; i++)
    {
        m_tpt.drawEllipse(QPointF((float)m_x[i],(float)m_y[i]),m_radius[i],m_radius[i]);
    }

}
