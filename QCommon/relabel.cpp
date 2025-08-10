#include "relabel.h"
#include <QPainter>
#include <qmath.h>
#include <QDebug>
ReLabel::ReLabel(QWidget *parent) : QLabel(parent)
{
    m_nDrawFlag = 0;
}

/******************************
*功能:重绘损伤点
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void ReLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter m_tpt;
    m_tpt.begin(this);

    switch (m_nDrawFlag)
    {
    case M_DRAW_CIRCLE:
    {
        m_tpt.setPen(QPen(Qt::red));
        int t_size = m_x.size();
        m_dMaxX = 0;
        m_dMaxy = 0;
        m_dMinx = m_x[0];
        m_dMiny = m_y[0];
        for (int i=0; i<m_x.size(); i++)
        {
            m_tpt.drawEllipse(QPointF(m_x[i]*m_lfminiRatio,m_y[i]*m_lfminiRatio),m_radius[i]*m_lfminiRatio*2,m_radius[i]*m_lfminiRatio*2);
        }
    }
        break;
    case M_DRAW_RECT:
    {
        QPen tem_pen(Qt::red);
        if (m_nTypeFlag == M_TYPE_ORI)
        {
            tem_pen.setWidth(1);
        }
        else
        {
            tem_pen.setWidth(1);
        }
        m_tpt.setPen(tem_pen);
        double tem_minRect = 1000;
        double tem_maxRect = 0;
        int tem_nMindex;
        int tem_nMaxdex;
        for (int i=0; i<m_width.size(); i++)
        {
            if (m_width[i]*m_height[i] > tem_maxRect)
            {
                tem_nMaxdex = i;
                tem_maxRect = m_width[i]*m_height[i];
            }

            if (m_width[i]*m_height[i] < tem_minRect)
            {
                tem_nMindex = i;
                tem_minRect = m_width[i]*m_height[i];
            }


            //200109当损伤微小时扩大显示矩形
            QRect tem_defRect;
            if (m_width[i]<100 || m_height[i]<100)
            {
                double tem_dLeft = m_x[i] - 5*m_width[i]/2;
                double tem_dTop = m_y[i] - 5*m_height[i]/2;
                tem_defRect.setRect(tem_dLeft*m_lfminiRatio,tem_dTop*m_lfminiRatio,6*m_width[i]*m_lfminiRatio,6*m_height[i]*m_lfminiRatio);
            }
            else
            {
                double tem_dLeft = m_x[i] - m_width[i]/2;
                double tem_dTop = m_y[i] - m_height[i]/2;
                tem_defRect.setRect(tem_dLeft*m_lfminiRatio,tem_dTop*m_lfminiRatio,m_width[i]*m_lfminiRatio,m_height[i]*m_lfminiRatio);
            }


            m_tpt.drawRect(tem_defRect);
        }

        if (m_nTypeFlag == M_TYPE_DEF)
        {
            //标注最大和最小缺陷的详细信息
            m_tpt.setPen(QPen(Qt::black));
            QFont tem_textfont("Times",16);
            m_tpt.setFont(tem_textfont);
            //最大缺陷
            QRect tem_maxtextRect((m_x[tem_nMaxdex]+m_width[tem_nMaxdex]/2)*m_lfminiRatio+5,(m_y[tem_nMaxdex]-m_height[tem_nMaxdex]/2)*m_lfminiRatio,650*m_lfminiRatio,480*m_lfminiRatio);
            m_tpt.drawRect(tem_maxtextRect);
            QRect tem_maxtext((m_x[tem_nMaxdex]+m_width[tem_nMaxdex]/2)*m_lfminiRatio+10,(m_y[tem_nMaxdex]-m_height[tem_nMaxdex]/2)*m_lfminiRatio,650*m_lfminiRatio,480*m_lfminiRatio);
            //200108yi
            m_tpt.drawText(tem_maxtext,Qt::AlignLeft,QString("center-X:%1\ncenter-Y:%2\nWidth:%3um\nHeight:%4um\nDiameter:%5um").arg(m_x[tem_nMaxdex]).arg(m_y[tem_nMaxdex]).arg(m_width[tem_nMaxdex]*2).arg(m_height[tem_nMaxdex]*2).arg(qSqrt(m_Area[tem_nMaxdex]*16/3.14159)));

//            if (tem_nMindex != tem_nMaxdex)
//            {
//                QRect tem_mintextRect((m_x[tem_nMindex]+m_width[tem_nMindex]/2)*m_lfminiRatio+5,(m_y[tem_nMindex]-m_height[tem_nMindex])*m_lfminiRatio+5,300*m_lfminiRatio,160*m_lfminiRatio);
//                m_tpt.drawRect(tem_mintextRect);
//                QRect tem_mintext((m_x[tem_nMindex]+m_width[tem_nMindex]/2)*m_lfminiRatio+10,(m_y[tem_nMindex]-m_height[tem_nMindex])*m_lfminiRatio+5,300*m_lfminiRatio,160*m_lfminiRatio);
//                m_tpt.drawText(tem_mintext,Qt::AlignLeft,QString("center-X:%1\ncenter-Y:%2\nWidth:%3um\nHeight:%4um").arg(m_x[tem_nMindex]).arg(m_y[tem_nMindex]).arg(m_width[tem_nMindex]*2).arg(m_height[tem_nMindex]*2));
//            }
        }
    }
        break;
    default:
    {}
    }
}

/******************************
*功能:复制损伤坐标与尺寸
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void ReLabel::copylabel(ReLabel *labelobj)
{
    m_x = labelobj->m_x;
    m_y = labelobj->m_y;
    m_radius = labelobj->m_radius;
    m_lfminiRatio = labelobj->m_lfminiRatio;
}


/******************************
*功能:保存损伤图片
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void ReLabel::SaveRePaint(QString sbackpath, QString sSavePath)
{
    qDebug()<<"def path: "<<sbackpath;
    qDebug()<<"save path: "<<sSavePath;
    QPixmap tem_repaint(sbackpath);
    QPainter m_tpt(&tem_repaint);
    QPen tem_pen(Qt::red);
    tem_pen.setWidth(3);
    m_tpt.setPen(tem_pen);

    double tem_minRect = 1000;
    double tem_maxRect = 0;
    int tem_nMindex;
    int tem_nMaxdex;
    for (int i=0; i<m_width.size(); i++)
    {
        if (m_width[i]*m_height[i] > tem_maxRect)
        {
            tem_nMaxdex = i;
            tem_maxRect = m_width[i]*m_height[i];
        }

        if (m_width[i]*m_height[i] < tem_minRect)
        {
            tem_nMindex = i;
            tem_minRect = m_width[i]*m_height[i];
        }

        double tem_dLeft = m_x[i] - m_width[i]/2;
        double tem_dTop = m_y[i] - m_height[i]/2;
        QRect tem_defRect(tem_dLeft,tem_dTop,m_width[i],m_height[i]);
        m_tpt.drawRect(tem_defRect);
    }

    //标注最大和最小缺陷的详细信息
    m_tpt.setPen(QPen(Qt::black));
    QFont tem_textfont("Times",60);
    m_tpt.setFont(tem_textfont);
    //最大缺陷
    QRect tem_maxtextRect((m_x[tem_nMaxdex]+m_width[tem_nMaxdex]/2)+5,(m_y[tem_nMaxdex]-m_height[tem_nMaxdex]/2),530,400);
    m_tpt.drawRect(tem_maxtextRect);
    QRect tem_maxtext((m_x[tem_nMaxdex]+m_width[tem_nMaxdex]/2)+10,(m_y[tem_nMaxdex]-m_height[tem_nMaxdex]/2),530,400);
    m_tpt.drawText(tem_maxtext,Qt::AlignLeft,QString("center-X:%1\ncenter-Y:%2\nWidth:%3um\nHeight:%4um").arg(m_x[tem_nMaxdex]).arg(m_y[tem_nMaxdex]).arg(m_width[tem_nMaxdex]*2).arg(m_height[tem_nMaxdex]*2));
    tem_repaint.save(sSavePath);
}
