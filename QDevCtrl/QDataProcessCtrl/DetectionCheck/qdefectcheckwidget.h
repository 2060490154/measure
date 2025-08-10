#ifndef QDEFECTCHECKWIDGET_H
#define QDEFECTCHECKWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include "qdefectprocess.h"

class QDefectCheckWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDefectCheckWidget(QWidget *parent = nullptr);

signals:

public slots:
    void onSelectOriginalButtonClick();
    void onSelectDestImageButtonClick();

    void onProcessImageButtonClick();

public:
    void initUI();
    void showImage(QPixmap pPixmap,QLabel* pshowLabel);





public:
    QTextEdit* m_pOriginalImageEdit;//损伤前图片
    QTextEdit* m_pDestImageEdit;//待处理图片

    QPushButton* m_pSelectOriginalButton;
    QPushButton* m_pSelectDestImageButton;
    QPushButton* m_pProcessButton;

    QLabel* m_pOriginalImagLabel;
    QLabel* m_pDestImagLabel;
    QLabel* m_pProcessImagLabel;
    QLabel* m_pImg;

    QDefectProcess* m_pDefectProcess;
};

#endif // QDEFECTCHECKWIDGET_H
