#ifndef QOSCIIDEVINFOWIDGET_H
#define QOSCIIDEVINFOWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include "define.h"

class QOSciiDevInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QOSciiDevInfoWidget(QWidget *parent = 0);

signals:

public slots:
    void onShowData(QString sFilePath,int nChannelNum = 0,QString sChannelName ="");

public:
    QCustomPlot* m_pOSciiPlot;


    //显示波形相关数据
    int m_nRecordLen;
    float m_lfRecordSampleRate;
     QVector<double> m_pTimeData;//波形时间数据
     QVector<double> m_pWaveData;//波形数据

     float m_lfMaxWaveData;
     float m_lfMinWaveData;
     float m_lfMaxTimeData;
};

#endif // QOSCIIDEVINFOWIDGET_H
