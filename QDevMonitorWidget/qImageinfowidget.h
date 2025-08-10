#ifndef QRunTimeImageWidget_H
#define QRunTimeImageWidget_H

#include <QWidget>
#include "qimagelabel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "define.h"

class QRunTimeImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QRunTimeImageWidget(QWidget *parent = 0);

signals:

public slots:
    void onShowIDSCapImage(QVariant tTaskExpInfo, int nImageType, QString sPath);
    void onShowKxccdCapImage(char* pbuff,int nwidth,int nhigh,QVariant tTaskExpInfo);
public:
    QImageLabel* m_pBeforeImageWidget;//测试前显微图像
    QImageLabel* m_pAfterImageWidget;//测试后显微图像
    QImageLabel* m_pKxccdImageWidget;//kxccd显微图像
    QImage m_ImageBuff;

    QVector<QRgb> m_vcolorTable; //生成颜色表
    int m_count = 0;
};

#endif // QRunTimeImageWidget_H
