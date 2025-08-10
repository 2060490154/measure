#ifndef QEXPRESULTWIDGET_H
#define QEXPRESULTWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include "define.h"

class QExpResultWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QExpResultWidget(QString sExpNo, QWidget *parent = 0);

signals:

public slots:

public:
    QCustomPlot* m_pExpResultPlot;
    QVector<float> m_pFluxData;
    QVector<float> m_pDefeData;
    QVector<double> m_dEnergyAver;
    QVector<double> m_dEnergyRMS;
    double m_dAverFlux;
    double m_dRMSFlux;

public:
    void LinearFit(double &a, double &b, QVector<double> xdata, QVector<double> ydata);
    double SquareSum(QVector<double> vect);
    double VectorProductSum(QVector<double> vect1, QVector<double> vect2);
    double VectorSum(QVector<double> vect);
public:
    QString m_sExpNo;
    QString m_sSavePath;
public slots:
    void onShowResult(int ExpType, QString sPath);
signals:
    void signal_FluxThreshHold(double threshhold);
};

#endif // QEXPRESULTWIDGET_H
