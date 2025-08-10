#include "qmeasurewidget.h"

QMeasureWidget::QMeasureWidget(QWidget* pParent)
{

    setAttribute(Qt::WA_QuitOnClose,false);

    memset(&m_tMeasureData,0,sizeof(m_tMeasureData));
    m_pCustomPlot = new QCustomPlot();

    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    m_pLineXRadioButton = new QRadioButton(this);
    m_pLineXRadioButton->setText("X线灰度");
    m_pLineYRadioButton = new QRadioButton(this);
    m_pLineYRadioButton->setText("Y线灰度");

    connect(m_pLineXRadioButton,&QRadioButton::clicked,this,&QMeasureWidget::SetLineType);
    connect(m_pLineYRadioButton,&QRadioButton::clicked,this,&QMeasureWidget::SetLineType);
    m_pMeasureDataLabel = new QLabel(this);

    showMeasureRel();
    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(m_pCustomPlot,0,0,1,4);
    layout->addWidget(m_pMeasureDataLabel,0,4,1,1);//参数显示
    layout->addWidget(m_pLineXRadioButton,1,0,1,2);
    layout->addWidget(m_pLineYRadioButton,1,2,1,2);



    this->setLayout(layout);

    this->setMinimumSize(700,300);
}

void QMeasureWidget::InitUI(int nType)
{
    if(nType == E_PAINT_LINE_X || nType == E_PAINT_LINE_Y)
    {
        m_pLineXRadioButton->show();
        m_pLineYRadioButton->show();

        m_pLineXRadioButton->setChecked(true);
        m_pLineYRadioButton->setChecked(false);
    }
    else
    {
        m_pLineXRadioButton->hide();
        m_pLineYRadioButton->hide();
    }
}

void QMeasureWidget::SetLineType()
{
    if(m_pLineXRadioButton->isChecked())
    {
        emit singnal_changeType(E_PAINT_LINE_X);
    }

    if(m_pLineYRadioButton->isChecked())
    {
        emit singnal_changeType(E_PAINT_LINE_Y);
    }

}


void QMeasureWidget::PlotLine(int nType)
{

    int nxMaxRange = 0;
    int nyMaxRange = 0;
    QVector<double> lfDatax(100);
    QVector<double> lfDatay(100);

    if(nType == E_PAINT_RECT)
    {
        nxMaxRange = m_tMeasureData.nStatDataLen;

        lfDatax.resize(m_tMeasureData.nStatDataLen);
        lfDatay.resize(m_tMeasureData.nStatDataLen);

        for (int i = 0; i < m_tMeasureData.nStatDataLen; i++)
        {
            lfDatax[i] = i;
            lfDatay[i] = m_tMeasureData.pStatData[i];
            nyMaxRange = qMax(nyMaxRange, (int)m_tMeasureData.pStatData[i]);
        }
    }

    if(nType == E_PAINT_LINE_X ||nType == E_PAINT_LINE_Y)
    {
        nxMaxRange = m_tMeasureData.nPixelDataLen;
        lfDatax.resize(m_tMeasureData.nPixelDataLen);
        lfDatay.resize(m_tMeasureData.nPixelDataLen);
        for (int i = 0; i < m_tMeasureData.nPixelDataLen; i++)
        {
            lfDatax[i] = i;
            lfDatay[i] = m_tMeasureData.pPixelData[i];
            nyMaxRange = qMax(nyMaxRange, (int)m_tMeasureData.pPixelData[i]);
        }
    }


    m_pCustomPlot->addGraph();
    m_pCustomPlot->graph(0)->setPen(QPen(Qt::red));
    m_pCustomPlot->graph(0)->setData(lfDatax, lfDatay);
    m_pCustomPlot->xAxis->setLabel("x");
    m_pCustomPlot->xAxis->setTickLabelColor(QColor(255,0,0));
    m_pCustomPlot->yAxis->setLabel("y");
    m_pCustomPlot->xAxis->setRange(0, nxMaxRange);//设置范围
    m_pCustomPlot->yAxis->setRange(0, nyMaxRange);
    m_pCustomPlot->replot();


    showMeasureRel();//显示计算参数

}


void QMeasureWidget::closeEvent(QCloseEvent* event)
{
    emit singnal_changeType(E_PAINT_NON);
    this->hide();
}

//显示测量计算结果
void QMeasureWidget::showMeasureRel()
{
    QString str;

    str = QString("\nMean:%1 \n\nMax:%2 \n\nMin:%3 \n\nRMS:%4 \n\nM:%5 \n\nC:%6 \n\nHeight:%7 \n\nWidth:%8 \n\n").arg(m_tMeasureData.lfMeanvalue).arg(m_tMeasureData.nMaxvalue).arg(m_tMeasureData.nMinvalue).arg(m_tMeasureData.lfRms).arg(m_tMeasureData.lfm).arg(m_tMeasureData.lfc).arg(m_tMeasureData.nHeight).arg(m_tMeasureData.nWidth);
    m_pMeasureDataLabel->setText(str);

}
