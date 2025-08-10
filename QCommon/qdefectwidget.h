#ifndef QDEFECTWIDGET_H
#define QDEFECTWIDGET_H

#include <QWidget>
#include "relabel.h"
#include <QPushButton>
class QDefectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDefectWidget(QWidget *parent = 0);
public:
    ReLabel *m_pOriImgLab;
    ReLabel *m_pDefImgLab;
    QPushButton *m_pYesBtn;
    QPushButton *m_pNoBtn;
    int m_basesize;
    QString m_sDefImgPath;
    QString m_sSavePath;
    QVariant m_tCurrentTaskExpInfo;
public:
    void setOriImg(QImage OriImg);
    void setDefImg(QImage DefImg);
signals:
    void signal_YesBtnClicked(QVariant tTaskExpInfo);
    void signal_NoBtnClicked(QVariant tTaskExpInfo);
public slots:
    void onYesBtnClicked();
    void onNoBtnClicked();
    void SaveDefImg();
};

#endif // QDEFECTWIDGET_H
