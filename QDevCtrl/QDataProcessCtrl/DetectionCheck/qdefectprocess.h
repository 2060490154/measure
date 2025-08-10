#ifndef QDEFECTPROCESS_H
#define QDEFECTPROCESS_H

#include <QObject>
#include <QImage>
#include <Eigen/Dense>
#include <QHash>
#include <QList>
#include <QVector>
//#include <QLabel>
#include <relabel.h>
using namespace Eigen;

const int M_CALI_WIDTH = 5;
const int M_CALI_HEIGH = 34;
#define M_VECTOR_MAX 0
#define M_VECTOR_MIN 1

const double g_wavelengthCali[34][5] = {
            400,410,21.0591,24.3372,321,
            411,420,15.7620,21.0592,321,
            421,430,10.2044,15.7620,321,
            431,440,5.7143,10.2044,321,
            441,450,2.7678,5.7143,321,
            451,461,1.0546,2.7678,321,
            462,472,2.7678,2.9663,231,
            473,483,2.9663,11.0595,231,
            484,494,11.0595,49.1454,231,
            495,505,49.1454,260.2895,231,
            506,514,260.2895,1160.1,231,
            515,525,260.2895,312.0524,213,
            526,535,92.1645,260.2895,213,
            536,545,28.7453,92.1645,213,
            546,555,9.4676,28.7453,213,
            556,565,3.2929,9.4674,213,
            566,576,1.0000,3.2929,213,
            577,587,0.9999,2.5596,123,
            588,598,2.5596,6.7310,123,
            599,609,6.7310,16.5713,123,
            610,620,16.5713,38.1940,123,
            621,630,38.1940,77.0576,123,
            631,640,77.0576,147.2206,123,
            641,650,147.2206,251.6344,123,
            651,659,251.6344,433.4652,123,
            660,669,433.4652,707.0834,123,
            670,679,707.0834,1092.2,123,
            680,689,1092.2,1597.7,123,
            690,699,1597.7,2213.2,123,
            700,709,2213.2,2903.1,123,
            710,719,2903.1,3606.1,123,
            720,729,3606.1,4241.8,123,
            730,739,4241.8,4724.9,123,
            740,750,4724.9,4995.5,123};


class  qData
{
public:
     float lfCoeff;
     int nShulie;
     int nx;
     int ny;
     int nwavelength;
     bool bIsDefect;//是否为400-490波段数据
public:
     qData()
     {
         lfCoeff = 0.0;
         nShulie = 0;
         nwavelength = 0;
         bIsDefect = false;
     }
};

class YImage
{
public:
    QImage m_Img;
    int m_width;
    int m_height;
    int m_pixelsnum;
    MatrixXd m_R;
    MatrixXd m_G;
    MatrixXd m_B;
    MatrixXd m_wavelength;
    MatrixXd m_wavelengthCP1;
    MatrixXd m_wavelengthCP2;
public:
    YImage(QString Imgname);
};

class  qRgbData
{
public:
     int nRed;
     int nGreen;
     int nBlue;
public:
     qRgbData()
     {
         nRed = 0;
         nGreen = 0;
         nBlue = 0;
     }
};

class block
{
public:
    QVector<int> m_iblk;
    QVector<int> m_jblk;
    QVector<double> m_OffCenter;
    double m_dRadius;
    double m_dXPos; //Geo-Center
    double m_dYPos;
    double m_dMassCenterX;  //Mass-Center
    double m_dMassCenterY;
};

class QDefectProcess
{
public:
    QDefectProcess();
    ~QDefectProcess();
public:
    QImage m_OrignalImage;
    QImage m_DestImage;
    YImage *m_OriImg = NULL;
    YImage *m_DefImg = NULL;
//    MatrixXi m_TR;    //损伤图片与原始图片RGB差值
//    MatrixXi m_TG;
//    MatrixXi m_TB;
    MatrixXi m_rgbDelta[3];
    MatrixXi m_rgbDeltaNoabs[3];
    int m_rgbRate[3][256];

    //temporal data from matlab
    MatrixXd m_temdata[6];
    //QImage m_DenosingImage;

//    MatrixXd m_detecPoints;
    QList<double> m_wavePoints;
    QHash<int,int> m_dtcPtHash;
public:
    QVector<block> m_490blk;
    QVector<block> m_590blk;
    QVector<block> m_Defblk;

public:
    double m_lfRate;//去噪的比例系数
    int m_lfThreshold[3];//R G b通道的阈值

    double m_lfRResponse[351];//R响应曲线
    double m_lfGResponse[351];//G响应曲线
    double m_lfBResponse[351];//B响应曲线
    QVector<qData>m_tdata;
    QVector<float> m_lfCoeff;
    QVector<int> m_nShulie;//??储存三通道灰度值大小特征

    QVector<qRgbData> m_tRgbData;//图像的rgb数据

    int m_nWidth;
    int m_nHeigh;

public:
    void loadOriginalImage(QString sPath);
    void loadDestImage(QString sPath);

    int subImagDenosing();//待处理图像减去原始图像数据 去噪声
    void thresholdImagDenosing();//阈值去噪声

    void caclThreshold(int nType);//计算阈值
    void caclResponse();//计算响应曲线
    void caclSaturation();//计算饱和度特征值
    void findBlock490();
    void findBlock590();
    void findDefect();
    void findDetecPoints();
    bool find490Ori(int row, int col);
    void find490Add(int row, int col);
    bool find590Ori(int row, int col);
    void find590Add(int row, int col);
    void CalcuBlockPara(block &waveblock);
    bool IsBlksCross(block blk1, block blk2);
    void VectorMaxMin(QVector<int> datavector, double& maxvalue, double& minvalue);
    void VectorMaxMin(QVector<double> datavector, double& maxvalue, double& minvalue);
    double VectorAver(QVector<int> datavector);
//    int VectorWhere(QVector<double> datavector, double item);


    void caclworkflow();//计算流程
    void showBlocks(QVector<block> blks);
    ReLabel m_tImgLabel;

    bool readData();
    double getWaveLength(double rgbratio, double order);
    int getIndex(int x,int y,int z,int value);
    int max(int x,int y,int z);
    int smaller(int x,int y,int z);
    double smaller(double x,double y,double z);
    int middle(int x,int y,int z);
};





#endif // QDEFECTPROCESS_H
