/***************************************************************************
**                                                                        **
** 文件描述：显示波形数据  实时显示的话 应该
**  会有性能问题，实测后如果满足不了要求，可不实时显示该数据
****************************************************************************
** 创建人：李刚
** 创建时间：2018年10月15日
** 修改记录：
**
****************************************************************************/
#include "qosciidevinfowidget.h"
#include <QGridLayout>

QOSciiDevInfoWidget::QOSciiDevInfoWidget(QWidget *parent) : QWidget(parent)
{
    m_pOSciiPlot = new QCustomPlot(this);
    QGridLayout* playout = new QGridLayout(this);
    playout->addWidget(m_pOSciiPlot,0,0,1,1);


    //QBrush qBrush(QColor(35,45,55));//设置背景色
    //m_pOSciiPlot->setBackground(qBrush);
    m_pOSciiPlot->xAxis->setLabel("时间");
   // m_pOSciiPlot->xAxis->setLabelColor(QColor(244,244,244));
   // m_pOSciiPlot->xAxis->setTickLabelColor(QColor(244,244,244));
    m_pOSciiPlot->yAxis->setLabel("数据");
    //m_pOSciiPlot->yAxis->setLabelColor(QColor(244,244,244));
    //m_pOSciiPlot->yAxis->setTickLabelColor(QColor(244,244,244));
    m_pOSciiPlot->xAxis->setRange(0, 1024);//设置范围
    m_pOSciiPlot->yAxis->setRange(0, 1);

    m_pOSciiPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);

    m_pOSciiPlot->legend->setVisible(true);
    m_pOSciiPlot->legend->setFont(QFont("Helvetica",9));

    QPen pen;
    for(int i = 0; i < M_OSCI_CHANNEL_NUM;i++)
    {
        m_pOSciiPlot->addGraph();

        if(i == 0)
        {
            pen.setColor(Qt::red);
        }
        if(i == 1)
        {
            pen.setColor(Qt::green);
        }
        if(i == 2)
        {
            pen.setColor(Qt::blue);
        }
        if(i == 3)
        {
            pen.setColor(Qt::yellow);
        }

        m_pOSciiPlot->graph(i)->setPen(pen);
        m_pOSciiPlot->graph(i)->setName(QString("通道%1").arg(i+1));
    }

}

/*******************************************************************
**功能：显示波形数据，从文件中读取数据
**输入：
**输出：
**返回值：
*******************************************************************/
void QOSciiDevInfoWidget::onShowData(QString sFilePath,int nChannelNum ,QString sChannelName )
{
    QFile file(sFilePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox messageBox(QMessageBox::Warning,"提示","示波器数据文件打开错误");
        messageBox.setStandardButtons (QMessageBox::Ok);
        messageBox.setButtonText (QMessageBox::Ok,QString("确 定"));
        messageBox.exec ();
        return;
    }

    QStringList datalist;
    int nlineCnt = 0;
    bool bOk = false;
    m_nRecordLen = 0;
    m_pTimeData.clear();
    m_pWaveData.clear();
    m_lfMaxWaveData = -1000.0;
    m_lfMinWaveData = 1000.0;

    while(!file.atEnd())//读取并解析数据
    {
        nlineCnt ++;

                QByteArray line = file.readLine();
                QString str(line);

                datalist = str.split(",");

                if(nlineCnt == 6 )  //从第六行开始读取
                {
                    m_lfRecordSampleRate = datalist[0].toFloat(&bOk);
                }

                if(nlineCnt >= 6)
                {
                    //float lfTimedata = datalist[datalist.size()-2].toFloat(&bOk);
        //            float lfTimedata = (nlineCnt-2)*m_lfRecordSampleRate* 1e9;//使用相对时间
                    float lfTimedata = (datalist[0].toFloat(&bOk) - m_lfRecordSampleRate)*1e9;//使用相对时间
                    float lfWavedata = datalist[datalist.size()-1].toFloat(&bOk)*1000;//转换为mv
                    m_pTimeData.append(lfTimedata);
                    m_pWaveData.append(lfWavedata);

                    m_lfMaxWaveData = qMax(m_lfMaxWaveData,lfWavedata);
                    m_lfMinWaveData = qMin(m_lfMinWaveData,lfWavedata);
                    m_lfMaxTimeData = lfTimedata;
                }
    }
    //绘图显示数据
    m_pOSciiPlot->xAxis->setRange(0,m_lfMaxTimeData);
    m_pOSciiPlot->yAxis->setRange(m_lfMinWaveData-0.01,m_lfMaxWaveData+0.01);
    if(nChannelNum <= M_OSCI_CHANNEL_NUM)
    {
        m_pOSciiPlot->graph(nChannelNum)->setData(m_pTimeData,m_pWaveData);
        m_pOSciiPlot->graph(nChannelNum)->setName(sChannelName);
        m_pOSciiPlot->replot();
    }
}



