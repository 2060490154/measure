#ifndef QIMAGEEXPANDWIDGET_H
#define QIMAGEEXPANDWIDGET_H

#include <QWidget>
#include <qscrollarea.h>
#include <qlabel.h>
#include <QPixmap>

//图像放大显示界面
class QImageExpandWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QImageExpandWidget(QWidget *parent = 0);
    void showImage(QPixmap* pPixmap);
    void paintEvent(QPaintEvent *event);

signals:

public slots:

public:
    QScrollArea* m_pScrollArea;//显示滚动条
    QLabel*      m_pImageLabel;//显示图像
    QPixmap* m_pPixmap;
};

#endif // QIMAGEEXPANDWIDGET_H
