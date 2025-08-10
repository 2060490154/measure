#ifndef QDATAPROCESSCTRL_H
#define QDATAPROCESSCTRL_H

#include <QObject>
#include <Eigen/Dense>
#include <QImage>
#include <QList>
#include <QVector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "QKxccdCtrl/qhigimage.h"

using namespace Eigen;

const int M_CALI_WIDTH = 5;
const int M_CALI_HEIGH = 34;

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

typedef struct _BLOCK
{
    QVector<int> m_iblk;
    QVector<int> m_jblk;
    QVector<double> m_OffCenter;
    double m_dRadius;
    double m_dXPos; //Geo-Center
    double m_dYPos;
    double m_dMassCenterX;  //Mass-Center
    double m_dMassCenterY;
} qblock;

class QDataProcessCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QDataProcessCtrl(QObject *parent = 0);
    ~QDataProcessCtrl();
public:
    YImage *m_OriImg = NULL;
    YImage *m_DefImg = NULL;
    MatrixXi m_rgbDelta[3];
    MatrixXi m_rgbDeltaNoabs[3];
    int m_rgbRate[3][256];
    QList<double> m_wavePoints; //    MatrixXd m_detecPoints;

public:
    QVector<qblock> m_490blk;
    QVector<qblock> m_590blk;
    QVector<qblock> m_Defblk;
public:
    double m_lfRate;//去噪的比例系数
    int m_lfThreshold[3];//R G b通道的阈值
    QHigImage *m_phigImg;

public:
    int subImagDenosing();//待处理图像减去原始图像数据 去噪声
    void thresholdImagDenosing(double rR, double gR, double bR);//阈值去噪声
    void DeltaImageDenosing();  //差值矩阵去噪声
    void caclThreshold(int nType);//计算阈值
    void caclSaturation();//计算饱和度特征值
    void findBlock490();
    void findBlock590();
    int findDefect();
    bool find490Ori(int row, int col);
    void find490Add(int row, int col);
    bool find590Ori(int row, int col);
    void find590Add(int row, int col);
    void CalcuBlockPara(qblock &waveblock);
    bool IsBlksCross(qblock blk1, qblock blk2);
    void VectorMaxMin(QVector<int> datavector, double& maxvalue, double& minvalue);
    void VectorMaxMin(QVector<double> datavector, double& maxvalue, double& minvalue);
    double VectorAver(QVector<int> datavector);

    bool caclworkflow(QString OriImgPath, QString DefImgPath);//计算流程
    bool Defect_Luminate(QString sBeforePicPath, QString sAfterPicPath);//明场计算方法
    bool CalclSpotArea(double &lfarea, double &lfarea_e2);
    double getWaveLength(double rgbratio, double order);
    double smaller(double x,double y,double z);


    void clearResource();

    //为及时清理MatrixXd
    void SubRGBmin(int num, MatrixXd &Rarray, MatrixXd &Garray, MatrixXd &Barray, MatrixXd &RGBPos);
    void CalcuRb(MatrixXd &Rb, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray);
    void CalcuGs(MatrixXd &Gs, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray);
    void CalcuRs(int num,MatrixXd &Rs, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray);
    void CalcuGb(int num,MatrixXd &Gb, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray);
    void CalcuC12s(int num, MatrixXi &C12s, MatrixXd Rarray, MatrixXd Garray);
    void CalcuBbs(int num, MatrixXd &Bb, MatrixXd &Bs,MatrixXd Rb, MatrixXd Rs, MatrixXd Gb, MatrixXd Gs);
    void CalcuRGB2(int num,MatrixXd &R2, MatrixXd &G2, MatrixXd &B2, MatrixXd Bb, MatrixXd Bs,MatrixXd Rb, MatrixXd Rs, MatrixXd Gb, MatrixXd Gs);
    void CalcuRGBRatio(MatrixXd &RGBRatio, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray, MatrixXd Rb, MatrixXd R2,MatrixXd Gb,MatrixXd G2, MatrixXd Bb,MatrixXd B2);
    void CalcuOrderAndRatio(int num, MatrixXd &Order, MatrixXd &RGBRatio, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray);
    void CalcuRGBNoneZero(MatrixXd &R, MatrixXd &G, MatrixXd &B);

public:
    QString m_sSavePath;

public:
    //明场诊断
    bool DefectHandan(QString sBeforePicPath, QString sAfterPicPath);
    void simpleContours(std::vector<std::vector<cv::Point>> &contours, std::vector<int> &IndexVector);
    bool IsContainInVector(std::vector<int> &Indexvector, int nIndex);
    bool IsContoursContain(std::vector<cv::Point> &contour1,std::vector<cv::Point> &contour2);
    void sortQVector(QVector<double> tVector, QVector<int> &nIndex);
    void simpleRects(QVector<QRect> &rectsVector, std::vector<int> &IndexVector);
    bool IsRectsContain(QRect rect1, QRect rect2);
public:
    QVector<double> m_dXPos;    //明场计算方法输出参数
    QVector<double> m_dYPos;
    QVector<double> m_dArea;
    QVector<double> m_dWidth;
    QVector<double> m_dHeight;
};

#endif // QDATAPROCESSCTRL_H
