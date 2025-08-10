#ifndef QIMAGESHOWWIDGET_H
#define QIMAGESHOWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPixmap>
#include "qimageexpandwidget.h"

class QImageLabel : public QWidget
{
    Q_OBJECT
public:
    explicit QImageLabel(QString sImageInfo,QWidget *parent = 0);
    ~QImageLabel();

    void mouseDoubleClickEvent(QMouseEvent *event);

signals:

public slots:

public:
    QLabel* m_pImagelabel;//图像
    QLabel* m_pImageInfolabel;//图像标题
    QImageExpandWidget* m_pImageExpandWidget;


public:
    void showImage(QPixmap Pixmap);

public:
    QPixmap m_OrignalPixMap;//原始图像
};

#endif // QIMAGESHOWWIDGET_H
