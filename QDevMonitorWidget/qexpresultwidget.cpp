#include "qexpresultwidget.h"
#include <QGridLayout>

QExpResultWidget::QExpResultWidget(QString sExpNo, QWidget *parent) : QWidget(parent)
{
    m_sExpNo = sExpNo;
    m_pExpResultPlot = new QCustomPlot(this);
    QGridLayout *playout = new QGridLayout(this);
    playout->addWidget(m_pExpResultPlot,0,0,1,1);

    m_pExpResultPlot->xAxis->setRange(0,5);
    m_pExpResultPlot->yAxis->setRange(0,1);

    //properties
    m_pExpResultPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);
    m_pExpResultPlot->legend->setVisible(false);


}


void QExpResultWidget::onShowResult(int ExpType,QString sPath)
{
    m_sSavePath = sPath;
    m_pExpResultPlot->clearGraphs();
    if (ExpType == M_MEASURETYPE_1On1)
    {
        m_pExpResultPlot->xAxis->setLabel("平均通量(J/cm2)");
        m_pExpResultPlot->yAxis->setLabel("损伤概率");
        //调整坐标轴范围
        float maxflux = 0.0;    //确定m_pFluxData中的最大值
        for (int i=0; i<m_pFluxData.size(); i++)
        {
            if (m_pFluxData[i] > maxflux)
            {
                maxflux = m_pFluxData[i];
            }
        }

        //设置坐标轴范围
        m_pExpResultPlot->xAxis->setRange(0,maxflux*1.5);   //x轴扩大1.5倍
        m_pExpResultPlot->yAxis->setRange(0,1.5);

        //PenColor1--scatter
        m_pExpResultPlot->addGraph();
        QPen scatterpen(Qt::red);
        m_pExpResultPlot->graph(0)->setPen(scatterpen);

        //PenColor2--line
        m_pExpResultPlot->addGraph();
        QPen linepen(Qt::blue);
        m_pExpResultPlot->graph(1)->setPen(linepen);

        //draw the scttering points
        QVector<double> tem_transFlux;
        QVector<double> tem_transDefe;
        if (!(m_pFluxData.size() == m_pDefeData.size()))
        {
            return;
        }
        else
        {
            for (int i=0; i<m_pFluxData.size(); i++)
            {
                tem_transFlux.push_back(m_pFluxData[i]);
                tem_transDefe.push_back(m_pDefeData[i]);
            }
            m_pExpResultPlot->graph(0)->setData(tem_transFlux,tem_transDefe);
            m_pExpResultPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
            m_pExpResultPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
        }


        //calcu the paras of line
        double a = 0.0;
        double b = 0.0;
        LinearFit(a,b,tem_transFlux,tem_transDefe);
        QVector<double> xfit;
        QVector<double> yfit;
        for (int i=0; i<100; i++)
        {
            xfit.append(i);
            yfit.append(a + b*i);
        }
        m_pExpResultPlot->graph(1)->setData(xfit,yfit);
        m_pExpResultPlot->replot();

        //emit the final fluxthreshhold
        if (b == 0)
        {
            emit signal_FluxThreshHold(-1);
        }
        else
        {
            emit signal_FluxThreshHold(-1*a/b);
        }
        //设置损伤阈值标签
        QCPItemText *tem_text = new QCPItemText(m_pExpResultPlot);
        tem_text->setText(QString("损伤阈值:")+QString::number(-1*a/b)+QString("J/cm^2"));
        tem_text->position->setCoords(-1*a/b,0.1);
        tem_text->setVisible(true);
        m_pExpResultPlot->replot();
        m_pExpResultPlot->savePng(m_sSavePath + "/PLOTIMG/"+m_sExpNo+".png",500,300);
    }
    else if (ExpType == M_MEASURETYPE_ROn1)
    {
#if 0
        if (!(m_pFluxData.size() == m_pDefeData.size()))
        {
            return;
        }
        m_pExpResultPlot->xAxis->setLabel("点次");
        m_pExpResultPlot->yAxis->setLabel("临界通量(J/cm2)");

        //柱状图对象
        QCPBars *tem_bar = new QCPBars(m_pExpResultPlot->xAxis,m_pExpResultPlot->yAxis);
        tem_bar->setAntialiased(false); //反锯齿
        tem_bar->setPen(QPen(QColor(111,9,176).lighter(170)));
        tem_bar->setBrush(QBrush(QColor(111,9,176)));
        tem_bar->setWidth(0.2);

        //x Axis
        QVector<double> ticks;  //柱状图编号
        QVector<QString> labels;    //柱状图标签
        for (int i=0; i<m_pFluxData.size(); i++)
        {
            ticks.push_back(i+1);
            labels.push_back(QString::number(m_pDefeData[i]));
        }

        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);
        m_pExpResultPlot->xAxis->setTicker(textTicker);
        m_pExpResultPlot->xAxis->setTickLength(0,m_pFluxData.size()+1);

        float maxpoint = 0.0;
        QVector<float> tem_PointCopy = m_pDefeData;
        qSort(tem_PointCopy.begin(),tem_PointCopy.end());
        maxpoint = tem_PointCopy[tem_PointCopy.size() - 1];   //判断最大点号
        //x Axis range
        m_pExpResultPlot->xAxis->setRange(0,maxpoint+2);
        m_pExpResultPlot->xAxis->setSubTicks(false);

        //y Axis
        float maxflux = 0.0;
        float minflux = 0.0;
        QVector<float> tem_fluxCopy = m_pFluxData;
        qSort(tem_fluxCopy.begin(),tem_fluxCopy.end());
        maxflux = tem_fluxCopy[tem_fluxCopy.size() - 1];   //判断最大临界通量
        //判断最小非零值
        for (int i=0; i<tem_fluxCopy.size(); i++)
        {
            if (tem_fluxCopy[i] != 0)
            {
                minflux = tem_fluxCopy[i];
                break;
            }
        }
        //y Axis range
        m_pExpResultPlot->yAxis->setRange(0,maxflux*1.7);
        //transfer the float to double
        QVector<double> transdata;
        for (int i=0; i<m_pFluxData.size(); i++)
        {
            transdata.push_back(m_pFluxData[i]);
        }
        tem_bar->setData(ticks,transdata);

        //plot the criticle line
        m_pExpResultPlot->addGraph();
        QPen pen(Qt::red);
        m_pExpResultPlot->graph(0)->setPen(pen);
        QVector<double> xdata;
        QVector<double> ydata;
        xdata<<0<<maxpoint+1;
        ydata<<minflux<<minflux;
        m_pExpResultPlot->graph(0)->setData(xdata,ydata);
        //plot the min flux label
        QCPItemText *tem_text = new QCPItemText(m_pExpResultPlot);
        tem_text->setText(QString::number(minflux));
        tem_text->position->setCoords(0.5,minflux*1.2);
        tem_text->setVisible(true);
        m_pExpResultPlot->replot();
#else
        m_pExpResultPlot->xAxis->setLabel("平均通量(J/cm2)");
        m_pExpResultPlot->yAxis->setLabel("损伤概率");
        //调整坐标轴范围
        float maxflux = 0.0;    //确定m_pFluxData中的最大值
        for (int i=0; i<m_pFluxData.size(); i++)
        {
            if (m_pFluxData[i] > maxflux)
            {
                maxflux = m_pFluxData[i];
            }
        }

        //设置坐标轴范围
        m_pExpResultPlot->xAxis->setRange(0,maxflux*1.5);   //x轴扩大1.5倍
        m_pExpResultPlot->yAxis->setRange(0,1.5);

        //PenColor1--scatter
        m_pExpResultPlot->addGraph();
        QPen scatterpen(Qt::red);
        m_pExpResultPlot->graph(0)->setPen(scatterpen);

        //draw the scttering points
        QVector<double> tem_transFlux;
        QVector<double> tem_transDefe;
        if (!(m_pFluxData.size() == m_pDefeData.size()))
        {
            return;
        }
        else
        {
            for (int i=0; i<m_pFluxData.size(); i++)
            {
                tem_transFlux.push_back(m_pFluxData[i]);
                tem_transDefe.push_back(m_pDefeData[i]);
            }
            m_pExpResultPlot->graph(0)->setData(tem_transFlux,tem_transDefe);
            m_pExpResultPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
            m_pExpResultPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
        }
#endif
        double tem_nMinFLux = m_pFluxData.at(0);
        for (int i=0; i<m_pFluxData.size(); i++)
        {
            if (m_pFluxData.at(i) < tem_nMinFLux) {tem_nMinFLux = m_pFluxData.at(i);}
        }
        //emit the final fluxthreshhold
        emit signal_FluxThreshHold(tem_nMinFLux);
        m_pExpResultPlot->savePng(QApplication::applicationDirPath() + "/PLOTIMG/"+m_sExpNo+".png",500,300);
    }

}


/******************************
*功能:线性拟合
*输入:
*输出:
*返回值:
*修改记录:190419：在while循环中的if语句原有该条件“ && ydataClean.at(ydataClean.size() - 2) == 1 ”,
*               表示最后保留一个概率为1的点，现在改为最后所有概率为1的点都剔除
*******************************/
void QExpResultWidget::LinearFit(double &a, double &b, QVector<double> xdata, QVector<double> ydata)
{
    //剔除ydata后部始终为1的点
    QVector<double> ydataClean = ydata;
    QVector<double> xdataClean = xdata;

    while (!(ydataClean.isEmpty()) && ydataClean.last() == 1)
    {
        if (ydataClean.size() >= 2 )
        {
            ydataClean.removeLast();
            xdataClean.removeLast();
        }
        else
        {
            break;
        }
    }

    //剔除ydata开始概率始终未0的点
    while (!(ydataClean.isEmpty()) && ydataClean.first() == 0)
    {
        if (ydataClean.size() >= 2 )
        {
            ydataClean.removeFirst();
            xdataClean.removeFirst();
        }
        else
        {
            break;
        }
    }

    //剔除数列中间为损伤概率为1的点
    for (int i=0; i<xdataClean.size()-1; i++)
    {
        if (ydataClean.at(i) == 1 && ydataClean.at(i+1) != 1)
        {
            xdataClean.removeAt(i);
            ydataClean.removeAt(i);
        }
    }

    a = (SquareSum(xdataClean) * VectorSum(ydataClean) - VectorProductSum(xdataClean,ydataClean) * VectorSum(xdataClean))/ \
            (xdataClean.size() * SquareSum(xdataClean) - VectorSum(xdataClean) * VectorSum(xdataClean));

    b = (xdataClean.size() * VectorProductSum(xdataClean,ydataClean) - VectorSum(xdataClean) * VectorSum(ydataClean))/ \
            (xdataClean.size() * SquareSum(xdataClean) - VectorSum(xdataClean) * VectorSum(xdataClean));
}

double QExpResultWidget::SquareSum(QVector<double> vect)
{
    double lfsum = 0.0;
    for (int i=0; i<vect.size(); i++)
    {
        lfsum += vect[i] * vect[i];
    }
    return lfsum;
}

double QExpResultWidget::VectorProductSum(QVector<double> vect1, QVector<double> vect2)
{
    double lfsum = 0.0;
    if (vect1.size() != vect2.size())
    {
        return -1;
    }
    else
    {
        for (int i=0; i<vect1.size(); i++)
        {
            lfsum += vect1[i] * vect2[i];
        }
    }
    return lfsum;
}

double QExpResultWidget::VectorSum(QVector<double> vect)
{
    double lfsum = 0.0;
    for (int i=0; i<vect.size(); i++)
    {
        lfsum += vect[i];
    }
    return lfsum;
}
