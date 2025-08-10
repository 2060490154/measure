#ifndef QHIGIMAGE_H
#define QHIGIMAGE_H

#include <QObject>
#include "kxccddefine.h"
#include <QRgb>


class QHigImage : public QObject
{
    Q_OBJECT
public:
    explicit QHigImage(QObject *parent = 0);

signals:

public slots:


public:
    WORD* m_pHigImage;//所有数据
    WORD* m_pHigData;//纯数据内容
    uchar* m_pBmpData;//bmp显示数据

    //由HIg生成QIMage
    uchar* m_pHig2BmpData;//hig数据映射到bmp数据时的映射矩阵
    QVector<QRgb> m_vcolorTable; //生成灰度颜色表


    FRAME_HEADER* m_pHeaderInfo;//文件头信息


    //测量时参数信息
    MeasureData m_tMeasureData;


public:
    void loadFile(QString sFilePath);
    void initDataBuf(int nRow,int nCol,int nBitLen,char* pImagBuff);//初始化内存
    void clearDataBuff();//清除创建内存
    void setHiGrayTab(int sg, int wg,int nBit);//创建hig与bmp数据的映射矩阵

    bool saveHigImage(QString sFileName,int nBitLen=0);//保存hig数据到hig文件
    bool saveBmpImage(QString sFileName);//保存成bmp图像

    void processHig2Bmp();//将hig图像转换为bmp图像

    WORD getPixelValue(int nPosx,int nPosy);//获取像素值
    int  getAreaData(QPoint tStartPoint,QPoint tEndPoint);//获取当前区域的像素值
    void calMeasureValue(int datalength);//计算参数
};

#endif // QHIGIMAGE_H
