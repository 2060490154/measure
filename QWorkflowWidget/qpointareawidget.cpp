#include "qpointareawidget.h"
#include <qpainter.h>
#include <qmessagebox.h>
#include <QtMath>
#include <qdebug.h>
QPointAreaWidget::QPointAreaWidget(double dmaxWidth, double dmaxHigh, int nType, double dStep, QWidget *parent) : QWidget(parent)
{
    m_nSizeType = nType;//矩形
    m_dStep = dStep;
    m_dMaxWidth = dmaxWidth;
    m_dMaxHigh = dmaxHigh;

    m_nStartPosx = 20;
    m_nStartPosy = 20;

    InitUI();

    //this->setStyleSheet("border-style: 1px solid  rgb(68,69,73)");

    onUpdatePointUI(m_nSizeType,m_dStep,100,100,0);

}

/*******************************************************************
**功能：初始化界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QPointAreaWidget::InitUI()
{
    m_pPointInfoLabel = new QLabel(this);
    m_pPointInfoLabel->setGeometry(m_nStartPosx+(int)m_dMaxWidth/2+20,m_nStartPosy,(int)m_dMaxWidth/2,(int)m_dMaxHigh);
    m_pMenu = new QMenu(this);//菜单

    QAction* pEnableAction = new QAction(tr("使能该点"), this);
    connect(pEnableAction, SIGNAL(triggered()), this, SLOT(onEnableSelectPoint()));

    QAction* pDisEnableAction = new QAction(tr("标记为跳过点"), this);
    connect(pDisEnableAction, SIGNAL(triggered()), this, SLOT(onDisEnabelSelectPoint()));

    QAction* pEnableAllAction = new QAction(tr("使能所有点"), this);
    connect(pEnableAllAction, SIGNAL(triggered()), this, SLOT(onEnableAllPoint()));

    m_pMenu->addAction(pEnableAction);
    m_pMenu->addAction(pDisEnableAction);
    m_pMenu->addAction(pEnableAllAction);


}

/*******************************************************************
**功能：画图
**输入：
**输出：
**返回值：
*******************************************************************/
void QPointAreaWidget::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    paint.setPen(QPen(Qt::gray,2));

    m_ncirclRedius = m_dStep/2;
    float lfpoint = 0.0;//计算起始点 圆形的时候使用
    if(m_dStep < 2)
    {
        m_ncirclRedius = 1;
    }

    if(m_nSizeType == M_SIZETYPE_RECT)
    {
        paint.drawRect(m_nStartPosx,m_nStartPosy,m_dWidth*m_lfScal,m_dHigh*m_lfScal);
    }

    if(m_nSizeType == M_SIZETYPE_CIRCL)//圆形
    {
        lfpoint = m_dRealHigh/2.0*0.414/1.414;
        paint.drawEllipse(m_nStartPosx,m_nStartPosy,m_dRealHigh*m_lfScal,m_dRealHigh*m_lfScal);

        paint.drawRect(m_nStartPosx+lfpoint*m_lfScal,m_nStartPosy+lfpoint*m_lfScal,m_dWidth*m_lfScal,m_dHigh*m_lfScal);


    }

    for(int i = 0; i < m_tPointList.size();i++ )
    {
        QPointInfo PointInfo = m_tPointList[i];
        if(PointInfo.nStatus == M_STATUS_UNUSE)
        {
            paint.setPen(QPen(Qt::green,2));
            paint.setBrush(Qt::green);
        }
        if(PointInfo.nStatus == M_STATUS_USED)
        {
            paint.setPen(QPen(Qt::red,2));
            paint.setBrush(Qt::red);
        }
        if(PointInfo.nStatus ==M_STATUS_DISABLE)
        {
            paint.setPen(QPen(Qt::gray,2));
            paint.setBrush(Qt::gray);
        }

        if(PointInfo.nStatus ==M_STATUS_DESTROY)
        {
            paint.setPen(QPen(Qt::yellow,2));
            paint.setBrush(Qt::gray);
        }

        if(PointInfo.bSelect)
        {
            paint.setPen(QPen(Qt::blue,2));
            paint.setBrush(Qt::blue);
        }
        paint.drawEllipse(PointInfo.dxPos*m_lfScal+m_nStartPosx-m_ncirclRedius/2+lfpoint*m_lfScal,PointInfo.dyPos*m_lfScal+m_nStartPosy-m_ncirclRedius/2+lfpoint*m_lfScal,m_ncirclRedius,m_ncirclRedius);
    }

    this->showPointInfo();

}

//鼠标选中点
void QPointAreaWidget::mousePressEvent(QMouseEvent *event)
{
    float lfpoint = m_dRealHigh/2.0*0.414/1.414;

    if(m_nSizeType == M_SIZETYPE_RECT)
    {
        lfpoint = 0.0;
    }

    int nxpoint = (event->pos().x()-m_nStartPosx)/m_lfScal-lfpoint;//映射为矩形框坐标
    int nypoint = (event->pos().y()-m_nStartPosy)/m_lfScal-lfpoint;



    int x  = (event->pos().x()-m_nStartPosx+m_ncirclRedius/2)/m_lfScal-lfpoint;//映射为点坐标
    int y = (event->pos().y()-m_nStartPosy+m_ncirclRedius/2)/m_lfScal-lfpoint;

    if(event->button()==Qt::LeftButton)
    {

        for(int i = 0; i < m_tPointList.size();i++ )
        {
            QPointInfo PointInfo = m_tPointList[i];

            if(x <= PointInfo.dxPos+ m_ncirclRedius && x >=PointInfo.dxPos- m_ncirclRedius && y <= PointInfo.dyPos+ m_ncirclRedius && y >=PointInfo.dyPos- m_ncirclRedius )
            {
                m_tPointList[i].bSelect = !m_tPointList[i].bSelect;
                break;
            }
        }

        //是否在该区域内
        if( nxpoint > m_dWidth || nxpoint < 0 || nypoint < 0 || nypoint > m_dHigh)
        {
            for(int i = 0; i < m_tPointList.size();i++ )
            {
                if(m_tPointList[i].bSelect)
                {
                    m_tPointList[i].bSelect = false;
                }
            }
        }
    }

    if(event->button()==Qt::RightButton)
    {
        if( nxpoint <= m_dWidth && nxpoint >= 0 && nypoint >= 0 && nypoint <= m_dHigh)
        {
            m_pMenu->move(cursor().pos());
            m_pMenu->show();

        }

    }

    this->update();
}

//创建显示的点
void QPointAreaWidget::onUpdatePointUI(int nSizeType, double dStep, double dRealWidth, double dRealHigh, double dRadius)
{
    //更新参数
    m_nSizeType = nSizeType;
    m_dStep = dStep;

    if(m_nSizeType == M_SIZETYPE_RECT)//矩形
    {
        m_dRealWidth = dRealWidth;
        m_dRealHigh = dRealHigh;
    }

    if(m_nSizeType == M_SIZETYPE_CIRCL)//圆形
    {
        m_dRealWidth = dRadius;
        m_dRealHigh = m_dRealWidth;
    }

    float lfscal1 = m_dMaxHigh/m_dRealHigh;
    float lfScal2 = m_dMaxWidth/m_dRealWidth*0.5;

    m_lfScal = qMin(lfscal1,lfScal2);
    //2020yi
    double dMaxx = m_dRealWidth;
    double dMaxy = m_dRealHigh;

    //矩形的话  实际尺寸==打把尺寸
    if(m_nSizeType == M_SIZETYPE_RECT)
    {
        //2020yi
        m_dWidth = m_dRealWidth;
        m_dHigh = m_dRealHigh;
    }


    //圆形  实际尺寸！= 打把尺寸  为圆的内接正方形
    if(m_nSizeType == M_SIZETYPE_CIRCL)
    {
        //2020yi
        m_dWidth = m_dRealHigh/1.414;
        m_dHigh = m_dRealWidth/1.414;
        dStep = 0;//圆形的时候 不需要考虑边界问题
    }

    //2020yi
    int nXPtNum;
    int nYPtNum;
    if (qCeil(m_dWidth/m_dStep) == qFloor(m_dWidth/m_dStep))
    {
        nXPtNum = m_dWidth/m_dStep;
        nYPtNum = m_dHigh/m_dStep;
    }
    else
    {
        nXPtNum = qFloor(m_dWidth/m_dStep);
        nYPtNum = qFloor(m_dHigh/m_dStep);
    }

    m_tPointList.clear();
#if 0
    for(int i = nStep ; i <= nMaxx; i = i + m_nStep)
    {
        for(int j = nStep; j <= nMaxy;j = j + m_nStep)
        {
            QPointInfo PointInfo;
            PointInfo.nStatus = 0;
            PointInfo.nxPos = i;
            PointInfo.nyPos = j;
            PointInfo.bSelect = false;

            //圆形时的四角不能使用
            if(m_nSizeType == M_SIZETYPE_RECT)
            {
                if((i != nMaxx) && (j != nMaxy))
                {
                    m_tPointList.append(PointInfo);
                }
            }
            else if(((m_nSizeType == M_SIZETYPE_CIRCL)&&(i == 0 && j ==nMaxy)) || (i ==0 && j == 0) || (j == 0 && i == nMaxx) || (i==nMaxx && j == nMaxy))
            {

            }
            else if(m_nSizeType == M_SIZETYPE_CIRCL)
            {
                m_tPointList.append(PointInfo);
            }

        }
    }
#else
    //2020yi
    for (int j=0; j<nYPtNum; j++)
    {
        for (int i=0; i<nXPtNum; i++)
        {
            QPointInfo PointInfo;
            PointInfo.nStatus = 0;
            PointInfo.dxPos = i*m_dStep;
            PointInfo.dyPos = j*m_dStep;
            PointInfo.bSelect = false;

            //2020yi
            m_tPointList.append(PointInfo);
        }
    }
#endif
if(m_nSizeType == M_SIZETYPE_RECT)
    {
        QList<QPointInfo> PointListTrans;
        //2020yi
        for (int j=0; j<nYPtNum; j++)
        {
            if (j%2 == 0)   //偶数
            {
                for (int i=0; i<nXPtNum; i++)
                {
                    PointListTrans.append(m_tPointList[j*nXPtNum + i]);
                }
            }

            if (j%2 != 0)   //奇数
            {
                for (int i=nXPtNum-1; i>=0; i--)
                {
                    PointListTrans.append(m_tPointList[j*nXPtNum + i]);
                }
            }
        }
        m_tPointList.clear();
        m_tPointList = PointListTrans;
    }
    emit updateWorkflow();
    this->update();
}

//使能选择点
void QPointAreaWidget::onEnableSelectPoint()
{
    for(int i = 0; i < m_tPointList.size();i++ )
    {
        if(m_tPointList[i].bSelect)
        {
            m_tPointList[i].nStatus = 0;
            m_tPointList[i].bSelect = false;
        }
    }
    emit updateWorkflow();

    this->update();

}
//灰化选择点
void QPointAreaWidget::onDisEnabelSelectPoint()
{
    for(int i = 0; i < m_tPointList.size();i++ )
    {
        if(m_tPointList[i].bSelect)
        {
            m_tPointList[i].nStatus = 2;
            m_tPointList[i].bSelect = false;
        }
    }
    emit updateWorkflow();
    this->update();

}
//使能所有点
void QPointAreaWidget::onEnableAllPoint()
{
    for(int i = 0; i < m_tPointList.size();i++ )
    {
        m_tPointList[i].nStatus = 0;
        m_tPointList[i].bSelect = false;
    }
    emit updateWorkflow();
    this->update();
}

//显示当前点信息
void QPointAreaWidget::showPointInfo()
{
    int nTotal = m_tPointList.size();
    int ndisenabelNum = 0;//跳过点
    int ndestroyNum = 0;//损坏点
    int nnuseNum = 0;//可打点数

    for(int i = 0; i < m_tPointList.size();i++ )
    {
        if(m_tPointList[i].nStatus == M_STATUS_DISABLE)
        {
            ndisenabelNum++;
        }
        if(m_tPointList[i].nStatus == M_STATUS_DESTROY)
        {
            ndestroyNum++;
        }
    }
    nnuseNum = nTotal - ndisenabelNum - ndestroyNum;


    m_pPointInfoLabel->setText("\
                    <table width=\"100%\">\
                    <tbody >\
                    <tr align=\"left\" valign=\"middle\">\
                    <td>"+QString("总点数:%1").arg(nTotal)+"</td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td></td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td>"+QString("可打点数:%1").arg(nnuseNum)+"</td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td></td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td>"+QString("跳过点数:%1").arg(ndisenabelNum)+"</td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td></td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td>"+QString("损坏点数:%1").arg(ndestroyNum)+"</td></tr>\
                    </tbody>\
                    </table>");


}
