#include "qdefectprocess.h"
#include <QDebug>
#include <qfile.h>
#include <QByteArray>
#include <QImageReader>
#include <QTime>
//#include <omp.h>
#include "omp.h"
#include "outmatrix.h"
#include "output.h"
#include "outputi.h"
#include <QHash>
#include <QList>
#include <QtMath>
#include "cwise.h"
#include <algorithm>
#include <Vector>

#include <QPainter>

//#include <QLabel>

using namespace std;
QDefectProcess::QDefectProcess()
{
    m_lfRate = 0.99;
    memset(m_rgbRate,0,sizeof(m_rgbRate));

}

QDefectProcess::~QDefectProcess()
{
    if (!(m_OriImg == NULL))
    {
        delete m_OriImg;
        m_OriImg = NULL;
    }

    if (!(m_DefImg == NULL))
    {
        delete m_DefImg;
        m_DefImg = NULL;
    }
}

/*******************************************************************
**功能：加载原始图像
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectProcess::loadOriginalImage(QString sPath)
{
//    m_OrignalImage.load(sPath,"QImage::Format_RGB888");
    m_OrignalImage.load(sPath);
    m_OrignalImage = m_OrignalImage.convertToFormat(QImage::Format_RGB888);
}


/*******************************************************************
**功能：加载待处理图像
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectProcess::loadDestImage(QString sPath)
{
//    m_DestImage.load(sPath,"QImage::Format_RGB888");
    m_DestImage.load(sPath);
    m_DestImage = m_DestImage.convertToFormat(QImage::Format_RGB888);

    m_nWidth = m_DestImage.width();
    m_nHeigh = m_DestImage.height();
}

/*******************************************************************
**功能：减去原始图像去噪声
**输入：
**输出：
**返回值：
*******************************************************************/
int QDefectProcess::subImagDenosing()
{
#if 0
    QColor pixel1;
    QColor pixel2;
    if(m_DestImage.width() != m_OrignalImage.width() || m_DestImage.height() != m_OrignalImage.height() )
    {
        return 1;//图像不相同 无法处理
    }

    int nwidth = m_DestImage.width();
    int nheigth = m_DestImage.height();

    qDebug()<<"";
    qDebug()<<"W & H: "<<nwidth<<nheigth;
    qDebug()<<"Format value: "<<m_OrignalImage.format();

    pixel1 = m_DestImage.pixelColor(0,0);


    qDebug()<<"=====================================";
    QRgb pp1 = m_OrignalImage.pixel(2447,2047);
//    qDebug()<<"[R]: "<<pp1.red();
//    qDebug()<<"[G]: "<<pp1.green();
//    qDebug()<<"[B]: "<<pp1.blue();
    qDebug()<<"[R]: "<<qRed(pp1);
    qDebug()<<"[G]: "<<qGreen(pp1);
    qDebug()<<"[B]: "<<qBlue(pp1);
    qDebug()<<"=====================================";










    //output RGB
//    int *Gout = new int[2048*2448];
//    int *Rout = new int[2048*2448];
//    int *Bout = new int[2048*2448];


    for(int i = 0; i < nheigth;i++)
    {
        for(int j = 0; j < nwidth;j++)
        {
            pixel1 = m_DestImage.pixelColor(j,i);
            pixel2 = m_OrignalImage.pixelColor(j,i);

            Gout[nwidth*i+j] = pixel2.green();
            Rout[nwidth*i+j] = pixel2.red();
            Bout[nwidth*i+j] = pixel2.blue();

            int red = pixel1.red()-pixel2.red();
            int green = pixel1.green() - pixel2.green();
            int blue = pixel1.blue() - pixel2.blue();

            qRgbData data;
            data.nBlue = blue;
            data.nRed = red;
            data.nGreen = green;

            m_tRgbData.append(data);    //顺序append，无法并行,换成substitute或许可以
        }
    }
//    Outmatrix(Gout,2048,2448,"..\\Gout.txt");
//    Outmatrix(Rout,2048,2448,"..\\Rout.txt");
//    Outmatrix(Bout,2048,2448,"..\\Bout.txt");
//    delete []Gout;
//    delete []Rout;
//    delete []Bout;
    //;
#else
    if (m_DefImg->m_width != m_OriImg->m_width || m_DefImg->m_height != m_OriImg->m_height)
    {
        return 1;//图像不相同 无法处理
    }


//    m_rgbDelta[0] = (m_DefImg->m_R - m_OriImg->m_R).cwiseAbs().cast<int>();
//    m_rgbDelta[1] = (m_DefImg->m_G - m_OriImg->m_G).cwiseAbs().cast<int>();
//    m_rgbDelta[2] = (m_DefImg->m_B - m_OriImg->m_B).cwiseAbs().cast<int>();
    m_rgbDeltaNoabs[0] = (m_DefImg->m_R - m_OriImg->m_R).cast<int>();
    m_rgbDeltaNoabs[1] = (m_DefImg->m_G - m_OriImg->m_G).cast<int>();
    m_rgbDeltaNoabs[2] = (m_DefImg->m_B - m_OriImg->m_B).cast<int>();
    m_rgbDelta[0] = m_rgbDeltaNoabs[0].cwiseAbs();
    m_rgbDelta[1] = m_rgbDeltaNoabs[1].cwiseAbs();
    m_rgbDelta[2] = m_rgbDeltaNoabs[2].cwiseAbs();
//    OutPutI("..\\TRoutputE.txt",m_rgbDeltaNoabs[0]);
//    OutPutI("..\\TGoutputE.txt",m_rgbDeltaNoabs[1]);
//    OutPutI("..\\TBoutputE.txt",m_rgbDeltaNoabs[2]);
#endif
}


/*******************************************************************
**功能：计算阈值
**输入：nType 阈值类型 0--R 1-G 2-B
**输出：
**返回值：
*******************************************************************/
void QDefectProcess::caclThreshold(int nType)
{

#if 0
    int nPinshu = 0;
    int nData = 0;
    m_lfThreshold[nType] = 1;
    int nwidth =  m_DestImage.width();
    int nheigh =  m_DestImage.height();
    int nTotalSize = m_tRgbData.size();

    while(nPinshu < m_lfRate * nTotalSize)
    {
        nPinshu = 0;
//#pragma omp parallel for num_threads(4)
//#pragma omp parallel  //for循环内有共同资源nPinshu，不可并行，或者需要加锁
        for(int i = 0; i < nTotalSize; i++) //对所有像素点遍历
        {
            if(nType == 0)
            {
                nData = m_tRgbData.at(i).nRed;
            }
            if(nType == 1)
            {
                nData = m_tRgbData.at(i).nGreen;
            }
            if(nType == 2)
            {
                nData = m_tRgbData.at(i).nBlue;
            }

            if(nData < m_lfThreshold[nType])
            {
                nPinshu++;
            }

        }
         m_lfThreshold[nType]++;

    }
    qDebug()<<"pinshu: "<<nPinshu;
//    QHash<int,int> myhash;
////    int myx=12;
////    int myy=456;
////    myhash.insert(myx*1000+myy,0);
//    QTime hashtime;
//    hashtime.start();
////#pragma omp parallel
//    for(int i=0; i<270000; i++)
//    {
//        int myx = i%10;
//        int myy = i%10;
//        myhash.insert(myx*100+myy,i);
//    }
//    qDebug()<<"HashSize: "<<myhash.size();
//        QList<int> hashlist = myhash.keys();
//    qDebug()<<"Hashtime: "<<hashtime.elapsed();
////    int fetch = myhash.key(101);

//    qDebug()<<"Key(8): "<<hashlist.size()<<hashlist.at(5);


//    qDebug()<<nType<<":"<<m_lfThreshold[nType];
    //;
#else
    int t_pixelnum = m_OriImg->m_pixelsnum;
    int t_colnum = m_OriImg->m_width;
    //statistics

    for (int i=0; i<t_pixelnum; i++)    //should be cpmpared with another loop method(by t_rownum and t_colnum)
    {   int t_row = i/t_colnum;
        int t_col = i%t_colnum;
        m_rgbRate[nType][m_rgbDelta[nType](t_row,t_col)]++;
    }

    int test=0;
    for (int i=0; i<256; i++)
    {
        test = test + m_rgbRate[1][i];
    }

    //rate
    int t_sum = 0;
    m_lfThreshold[nType] = 1;
    for (int i=0; i<256; i++)
    {
        t_sum = t_sum + m_rgbRate[nType][i];
        if (t_sum > t_pixelnum * m_lfRate)
        {
            m_lfThreshold[nType] = i+1;
            break;
        }
    }
#endif
}

/*******************************************************************
**功能：阈值去噪声
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectProcess:: thresholdImagDenosing()
{
#if 0
    for(int i = 0; i < m_tRgbData.size();i++)
    {
        if(m_tRgbData[i].nRed< m_lfThreshold[0] && m_tRgbData[i].nGreen< m_lfThreshold[1] &&m_tRgbData[i].nBlue< m_lfThreshold[2] )
        {
            m_tRgbData[i].nRed = 0;
            m_tRgbData[i].nGreen = 0;
            m_tRgbData[i].nRed = 0;
        }
    }
    //;
#else
//    OutPut("..\\RbeforeE.txt",m_DefImg->m_R);
//    OutPut("..\\GbeforeE.txt",m_DefImg->m_G);
//    OutPut("..\\BbeforeE.txt",m_DefImg->m_B);
    int t_pixelnum = m_OriImg->m_pixelsnum;
    int t_colnum = m_OriImg->m_width;
    qDebug()<<m_lfThreshold[0]<<m_lfThreshold[1]<<m_lfThreshold[2];
//#pragma omp parallel
    for (int i=0; i<t_pixelnum; i++)    //should be cpmpared with another loop method(by t_rownum and t_colnum)
    {   int t_row = i/t_colnum;
        int t_col = i%t_colnum;
        //问题在于使用差值矩阵的绝对值和阈值比较还是用原值与阈值比较，用绝对值比较会使计算时间大大加长（计算的像素点增多）
        if (m_rgbDeltaNoabs[0](t_row,t_col)<m_lfThreshold[0] && m_rgbDeltaNoabs[1](t_row,t_col)<m_lfThreshold[1] && m_rgbDeltaNoabs[2](t_row,t_col)<m_lfThreshold[2])
        {
            m_DefImg->m_R(t_row,t_col) = 0;
            m_DefImg->m_G(t_row,t_col) = 0;
            m_DefImg->m_B(t_row,t_col) = 0;
            //;
        }
        else if (m_rgbDeltaNoabs[0](t_row,t_col) == 0 || m_rgbDeltaNoabs[1](t_row,t_col) == 0 || m_rgbDeltaNoabs[2](t_row,t_col) == 0)
        {
            m_DefImg->m_R(t_row,t_col) = 0;
            m_DefImg->m_G(t_row,t_col) = 0;
            m_DefImg->m_B(t_row,t_col) = 0;
        }
    }
//    qDebug()<<"out1";
//    OutPut("..\\RbeforeE.txt",m_DefImg->m_R);
//    OutPut("..\\GbeforeE.txt",m_DefImg->m_G);
//    OutPut("..\\BbeforeE.txt",m_DefImg->m_B);
//    QTime timeloop;
//    timeloop.start();
//#pragma omp parallel for num_threads(4)
//    for (int i=0; i<t_pixelnum; i++)    //should this loop be merged with the above one? try it.
//    {
//        int t_row = i/t_colnum;
//        int t_col = i%t_colnum;
//        if (m_DefImg->m_R(t_row, t_col) * m_DefImg->m_G(t_row, t_col) * m_DefImg->m_B(t_row, t_col) == 0)
//        {
//            m_DefImg->m_R(t_row,t_col) = 0;
//            m_DefImg->m_G(t_row,t_col) = 0;
//            m_DefImg->m_B(t_row,t_col) = 0;
//        }
//    }
//    qDebug()<<"time loop: "<<timeloop.elapsed();



    //second method
    QTime timeeigen;
    timeeigen.start();
    MatrixXd t_R01;
    MatrixXd t_G01;
    MatrixXd t_B01;
    MatrixXd t_RGB01;
//    t_R01 = m_DefImg->m_R.cwiseSign().cast<double>(); //比较方法
//    t_G01 = m_DefImg->m_G.cwiseSign().cast<double>();
//    t_B01 = m_DefImg->m_B.cwiseSign().cast<double>();
    t_R01 = m_DefImg->m_R.cwiseNotEqual(m_DefImg->m_R*0).cast<double>();
    t_G01 = m_DefImg->m_G.cwiseNotEqual(m_DefImg->m_G*0).cast<double>();
    t_B01 = m_DefImg->m_B.cwiseNotEqual(m_DefImg->m_B*0).cast<double>();
    t_RGB01 = t_R01.cwiseProduct(t_G01).cwiseProduct(t_B01);
    m_DefImg->m_R = m_DefImg->m_R.cwiseProduct(t_RGB01);
    m_DefImg->m_G = m_DefImg->m_G.cwiseProduct(t_RGB01);
    m_DefImg->m_B = m_DefImg->m_B.cwiseProduct(t_RGB01);
    qDebug()<<"time eigen: "<<timeeigen.elapsed();
//    OutPut("..\\RafterE.txt",m_DefImg->m_R);
//    OutPut("..\\GafterE.txt",m_DefImg->m_G);
//    OutPut("..\\BafterE.txt",m_DefImg->m_B);
    //;
#endif
}

/*******************************************************************
**功能：计算响应曲线
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectProcess::caclResponse()
{
    float tempdata[351];
    int ndata = 400;
    for(int i = 0; i < 351;i++)
    {
        m_lfRResponse[i] = 0.67*exp(-4*log(2)*((ndata-620)/65)*((ndata-620)/65));

         m_lfGResponse[i] = 0.56*exp(-4*log(2)*((ndata-525)/85)*((ndata-525)/85));

          m_lfBResponse[i] = 0.2*exp(-4*log(2)*((ndata-440)/50)*((ndata-440)/50));

    }


}

/*******************************************************************
**功能：计算饱和度特征值并对照标定表将饱和度特征值转换成波长信息
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectProcess::caclSaturation()
{
#if 0
    m_tdata.clear();
    m_lfCoeff.clear();
    m_nShulie.clear();


    QColor pixel;
    int minTemp = 0;
    int nMax = 0;
    int nMid =0;
    int nSmaller = 0;
    for(int i = 0; i < m_tRgbData.size();i++)
    {
        int red = m_tRgbData.at(i).nRed;
        int green =m_tRgbData.at(i).nGreen ;
        int blue = m_tRgbData.at(i).nBlue;

        nMax = max(red,green,blue);
        nMid = middle(red,green,blue);
        nSmaller = smaller(red,green,blue);

        m_tRgbData[i].nRed = red-nSmaller;
        m_tRgbData[i].nGreen = green-nSmaller;
        m_tRgbData[i].nBlue = blue-nSmaller;


        qData data;
        data.nx = i%m_nWidth;
        data.ny = i/m_nWidth;

        if(nMax != 0 && nMid != 0)
        {
            data.lfCoeff = (float)nMax/(float)nMid;
            data.nShulie = getIndex(red,green,blue,nMax)*100+getIndex(red,green,blue,nMid)*10+getIndex(red,green,blue,nSmaller);
          }

        for(int k = 0; k < M_BIAO_HEIGH;k++)
        {
            if(data.lfCoeff > g_lfbiaodata[k*5 + 2] && data.lfCoeff < g_lfbiaodata[k*5 + 3] &&data.nShulie==g_lfbiaodata[k*5 + 4])
            {
                data.nwavelength = g_lfbiaodata[k*5 ];
                if(data.nwavelength >= 400 && data.nwavelength<= 490)//提取符合正常波段的点集的过程 小于400为不正常
                {
                    data.bIsDefect = true;
                }
                else
                    data.bIsDefect = false;

                break;
            }
        }

        m_tdata.append(data);

    }
    //;
#else
    /*将去噪后的、稀疏的RGB矩阵中非零元素提取，转存为列向量*/
    //计算R矩阵中的非零元素个数
    QTime testtime;
    testtime.start();
    MatrixXd t_summat;
    t_summat = m_DefImg->m_R.cwiseNotEqual(m_DefImg->m_R*0).cast<double>();
    int t_sum;
    t_sum = t_summat.sum();
    int t_row = m_DefImg->m_height;
    int t_col = m_DefImg->m_width;
    int t_index = 0;
    MatrixXd R_array(t_sum,1);
    MatrixXd G_array(t_sum,1);
    MatrixXd B_array(t_sum,1);
    MatrixXd rgb_pos(t_sum,2);
    for (int i=0; i<t_row; i++)
    {
        for (int j=0; j<t_col; j++)
        {
            if (m_DefImg->m_R(i,j) != 0)
            {
                R_array(t_index,0) = m_DefImg->m_R(i,j);
                G_array(t_index,0) = m_DefImg->m_G(i,j);
                B_array(t_index,0) = m_DefImg->m_B(i,j);
                rgb_pos(t_index,0) = i;
                rgb_pos(t_index,1) = j;
                t_index++;
            }
        }
    }
    qDebug()<<"partial: "<<testtime.elapsed();

//    OutPut("..\\rgbpos.txt",rgb_pos);
    //提取饱和度特征
    MatrixXd t_minmat(t_sum,1);
    t_minmat = R_array.cwiseMin(G_array);
    t_minmat = t_minmat.cwiseMin(B_array);
    //用RGB三个矩阵减去t_minmat，提取饱和度特征,结果已验证成功
    R_array = R_array - t_minmat;
    G_array = G_array - t_minmat;
    B_array = B_array - t_minmat;
//    OutPut("..\\R_array.txt",R_array);
//    OutPut("..\\G_array.txt",G_array);
//    OutPut("..\\B_array.txt",B_array);


    MatrixXd Rb,R2,Rs,Gb,G2,Gs,Bb,B2,Bs; //保存RGB矩阵最大最小值位置
    MatrixXd Rcpb,Rcps,Gcpb,Gcps;       //RGB三个矩阵的部分copy
    MatrixXi C12b,C12s,C13b,C13s,C23b,C23s;   //比较过程中产生的中间矩阵
    MatrixXd Cori;  //同维度的单位阵
    MatrixXd Order;
    MatrixXd rgbMax1;
    MatrixXd rgbMax2;
    MatrixXd rgbRatio;
    Cori.setOnes(t_sum,1);
//    OutPut("..\\Cori.txt",Cori);
    //R与G矩阵比大小
    C12b = R_array.binaryExpr(G_array,GComp<double>());
//    OutPutI("..\\C12b.txt",C12b);
    C12s = Cori.cast<int>() - C12b;

    //RGB矩阵部分取值
    Rcpb = R_array.cwiseProduct(C12b.cast<double>());
    Rcps = R_array.cwiseProduct(C12s.cast<double>());
    Gcpb = G_array.cwiseProduct(C12s.cast<double>());
    Gcps = G_array.cwiseProduct(C12b.cast<double>());

    //R与B矩阵比大小
    C13b = Rcpb.binaryExpr(B_array,GComp<double>());
    Rb = C12b.cwiseProduct(C13b).cast<double>();    //此处点乘是否有必要
    C13s = Rcps.binaryExpr(B_array,SComp<double>());
    Rs = C12s.cwiseProduct(C13s).cast<double>();
//    OutPut("..\\Rb.txt",Rb);
//    OutPut("..\\Rs.txt",Rs);


    //G与B矩阵比大小
    C23b = Gcpb.binaryExpr(B_array,GComp<double>());
    Gb = C12s.cwiseProduct(C23b).cast<double>();
    C23s = Gcps.binaryExpr(B_array,SComp<double>());
    Gs = C12b.cwiseProduct(C23s).cast<double>();
//    OutPut("..\\Gb.txt",Gb);
//    OutPut("..\\Gs.txt",Gs);


    //处理B矩阵
    Bb = Cori - Rb - Gb;
    Bs = Cori - Rs - Gs;
    R2 = Cori - Rb - Rs;
    G2 = Cori - Gb - Gs;
    B2 = Cori - Bb - Bs;//此处报内存错误

//    //RGB顺序矩阵
    Order = (Rb+Gb*2+Bb*3)*100 + (R2+G2*2+B2*3)*10 + (Rs+Gs*2+Bs*3);
//    OutPut("..//Order.txt",Order);

//    //rgbmax1 & rgbmax2
    rgbMax1 = R_array.cwiseProduct(Rb) + G_array.cwiseProduct(Gb) + B_array.cwiseProduct(Bb);
    rgbMax2 = R_array.cwiseProduct(R2) + G_array.cwiseProduct(G2) + B_array.cwiseProduct(B2);

//    //rgbratio
    rgbRatio = rgbMax1.cwiseQuotient(rgbMax2);
//    OutPut("..//rgbratio.txt",rgbRatio);

    //剔除rgbratio中inf和nan
    for (int i=0; i<t_sum; i++)
    {
        if (!(qIsNaN(rgbRatio(i,0)) || qIsInf(rgbRatio(i,0))))
        {
            double t_wave = getWaveLength(rgbRatio(i,0),Order(i,0));
            if (t_wave != 0)
            {
                m_wavePoints.push_back(rgb_pos(i,0));
                m_wavePoints.push_back(rgb_pos(i,1));
                m_DefImg->m_wavelength(rgb_pos(i,0),rgb_pos(i,1)) = getWaveLength(rgbRatio(i,0),Order(i,0));
            }
        }
    }
    m_DefImg->m_wavelengthCP1 = m_DefImg->m_wavelength;
    m_DefImg->m_wavelengthCP2 = m_DefImg->m_wavelength;
//    OutPut("..\\wavelth.txt",m_DefImg->m_wavelength);
    /*
    //找出三个矩阵对应位置上的最小值，并保存在t_minmat中
    MatrixXd t_minmat(m_DefImg->m_height,m_DefImg->m_width);
    t_minmat = m_DefImg->m_R.cwiseMin(m_DefImg->m_G);
    t_minmat = t_minmat.cwiseMin(m_DefImg->m_B);
    //用RGB三个矩阵减去t_minmat，提取饱和度特征
    m_DefImg->m_R = m_DefImg->m_R - t_minmat;
    m_DefImg->m_G = m_DefImg->m_G - t_minmat;
    m_DefImg->m_B = m_DefImg->m_B - t_minmat;
    //饱和度提取已经验证成功;
//    OutPut("..\\dem_R.txt",m_DefImg->m_R);
//    OutPut("..\\dem_G.txt",m_DefImg->m_G);
//    OutPut("..\\dem_B.txt",m_DefImg->m_B);


    QTime testtime;
    testtime.start();
    //计算饱和度特征矩阵RGB的比例矩阵和顺序矩阵
    MatrixXd Rb,R2,Rs,Gb,G2,Gs,Bb,B2,Bs; //保存RGB矩阵最大最小值位置
    MatrixXd Rcpb,Rcps,Gcpb,Gcps;       //RGB三个矩阵的部分copy
    MatrixXi C12b,C12s,C13b,C13s,C23b,C23s;   //比较过程中产生的中间矩阵
    MatrixXd Cori;  //同维度的单位阵
    MatrixXd Order;
    MatrixXd rgbMax1;
    MatrixXd rgbMax2;
    MatrixXd rgbRatio;
    Cori.setOnes(m_DefImg->m_height,m_DefImg->m_width);
//    OutPut("..\\Cori.txt",Cori);
    //R与G矩阵比大小
    C12b = m_DefImg->m_R.binaryExpr(m_DefImg->m_G,GComp<double>());
//    OutPutI("..\\C12b.txt",C12b);
    C12s = Cori.cast<int>() - C12b;

    //RGB矩阵部分取值
    Rcpb = m_DefImg->m_R.cwiseProduct(C12b.cast<double>());
    Rcps = m_DefImg->m_R.cwiseProduct(C12s.cast<double>());
    Gcpb = m_DefImg->m_G.cwiseProduct(C12s.cast<double>());
    Gcps = m_DefImg->m_G.cwiseProduct(C12b.cast<double>());

    //R与B矩阵比大小
    C13b = Rcpb.binaryExpr(m_DefImg->m_B,GComp<double>());
    Rb = C12b.cwiseProduct(C13b).cast<double>();
    C13s = Rcps.binaryExpr(m_DefImg->m_B,SComp<double>());
    Rs = C12s.cwiseProduct(C13s).cast<double>();
//    OutPut("..\\Rb.txt",Rb);
//    OutPut("..\\Rs.txt",Rs);


    //G与B矩阵比大小
    C23b = Gcpb.binaryExpr(m_DefImg->m_B,GComp<double>());
    Gb = C12s.cwiseProduct(C23b).cast<double>();
    C23s = Gcps.binaryExpr(m_DefImg->m_B,SComp<double>());
    Gs = C12b.cwiseProduct(C23s).cast<double>();
//    OutPut("..\\Gb.txt",Gb);
//    OutPut("..\\Gs.txt",Gs);


    //处理B矩阵
    Bb = Cori - Rb - Gb;
    Bs = Cori - Rs - Gs;
    R2 = Cori - Rb - Rs;
    G2 = Cori - Gb - Gs;
//    B2 = Cori - Bb - Bs;//此处报内存错误

//    //RGB顺序矩阵
//    Order = (Rb+Gb*2+Bb*3)*100 + (R2+G2*2+B2*3)*10 + (Rs+Gs*2+Bs*3);
//    OutPut("..//Order.txt",Order);

//    //rgbmax1 & rgbmax2
//    rgbMax1 = m_DefImg->m_R.cwiseProduct(Rb) + m_DefImg->m_G.cwiseProduct(Gb) + m_DefImg->m_B.cwiseProduct(Bb);
//    rgbMax2 = m_DefImg->m_R.cwiseProduct(R2) + m_DefImg->m_G.cwiseProduct(G2) + m_DefImg->m_B.cwiseProduct(B2);

//    //rgbratio
//    rgbRatio = rgbMax1.cwiseQuotient(rgbMax2);
    qDebug()<<"partial: "<<testtime.elapsed();
    //将比例rgbratio为nan或者inf的点置零
//    rgbRatio.binaryExpr(rgbRatio,IsNaN_or_Inf<double>());


*/

#endif

}


/******************************
*功能：初步寻找490波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDefectProcess::find490Ori(int row, int col)
{
    int ii[9]={0};
    int jj[9]={0};
    int count = 0;
    //需要检查的9个点，可能有超出矩阵范围的，需要进行筛选
    int t_dtci[9] = {row, row, row, row+1, row+1, row+1, row+2, row+2, row+2};
    int t_dtcj[9] = {col, col+1, col+2, col, col+1, col+2, col, col+1, col+2};

    for (int i=0; i<9; i++)
    {
        if (t_dtci[i] >= 0 && t_dtci[i] <= 2047 && t_dtcj[i] >= 0 && t_dtcj[i] <= 2447)
        {
            if(m_DefImg->m_wavelengthCP1(t_dtci[i],t_dtcj[i]) > 400 && m_DefImg->m_wavelengthCP1(t_dtci[i],t_dtcj[i]) < 490)
            {
                ii[count] = t_dtci[i];
                jj[count] = t_dtcj[i];
                count++;
            }
        }

    }
    //如果满足波长条件的点>=4个则生成并保存一个波块
    if (count >= 4)
    {
        block t_blk;
        for (int k=0; k<count; k++)
        {
            t_blk.m_iblk.push_back(ii[k]);
            t_blk.m_jblk.push_back(jj[k]);
        }
        m_490blk.push_back(t_blk);
//        if (m_490blk.size() == 303)
//        {
//            qDebug()<<"cur pos:"<<row<<col;
//        }
//        if (row == 1490 && col == 1951)
//        {
//            qDebug()<<m_490blk.size();
//        }

        return true;
    }
    else
    {
        return false;
    }
}




/******************************
*功能：扩展初步找到的490波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::find490Add(int row, int col)
{
    //对之前产生的490波块进行四个方向上的扩展

#if 0
    bool isexped = false;
    QVector<int> t_ii;
    QVector<int> t_jj;
    QVector<int> t_iitem;
    QVector<int> t_jjtem;

    //====================================================向右扩展====================================================
    int t_count = m_490blk.last().m_iblk.size();    //当前波块中所有的非零像素点个数
    for (int i=0; i<t_count; i++)
    {
        int t_c = m_490blk.last().m_jblk[i];
        if (t_c + 1 <= 2447)
        {
            if (t_c - col == 2)
            {
                int t_r = m_490blk.last().m_iblk[i];
                t_c = t_c + 1;
                if (m_DefImg->m_wavelengthCP1(t_r,t_c) > 400 && m_DefImg->m_wavelengthCP1(t_r,t_c) < 490)
                {
                    t_ii.push_back(t_r);
                    t_jj.push_back(t_c);
                    isexped = true;
                }
//                else if (m_DefImg->m_wavelengthCP1(t_r,t_c) == 0 && m_DefImg->m_wavelengthCP1(t_r,t_c+1) > 400 && m_DefImg->m_wavelengthCP1(t_r,t_c+1) < 490)
//                {
//                    t_ii.push_back(t_r);
//                    t_jj.push_back(t_c + 1);
//                    isexped = true;
//                }
            }
//            else
//            {isexped = false;}
        }
//        else
//        {isexped = false;}
    }
    while (isexped)
    {
        isexped = false;
        //进入while循环说明向右有了扩展。先更新波块中的像素点
        m_490blk.last().m_iblk += t_ii;
        m_490blk.last().m_jblk += t_jj;
        //暂存扩展的点
        t_iitem = t_ii;
        t_jjtem = t_jj;
        //清空扩展点vector
        t_ii.clear();
        t_jj.clear();
        //有待继续向右扩展点的个数
        int t_newsize = t_iitem.size();
        for (int i=0; i<t_newsize; i++)
        {
            if (t_jjtem[i]+1 <= 2447)
            {
                if (m_DefImg->m_wavelengthCP1(t_iitem[i],t_jjtem[i]+1) > 400 && m_DefImg->m_wavelengthCP1(t_iitem[i],t_jjtem[i]+1) < 490)
                {
                    t_ii.push_back(t_iitem[i]);
                    t_jj.push_back(t_jjtem[i]+1);
                    isexped = true;
                }
//                else
//                {isexped = false;}
            }
//            else
//            {isexped = false;}
        }
    }


    //====================================================向左扩展====================================================
    isexped = false;
    t_count = m_490blk.last().m_iblk.size();    //更新当前波块中所有的非零像素点个数
    t_ii.clear();   //清理扩展vector
    t_jj.clear();
    for (int i=0; i<t_count; i++)
    {
        int t_c = m_490blk.last().m_jblk[i];
        if (t_c - 1 >= 0)
        {
            if (t_c - col == 0)
            {
                int t_r = m_490blk.last().m_iblk[i];
                t_c = t_c - 1;
                if (m_DefImg->m_wavelengthCP1(t_r,t_c) > 400 && m_DefImg->m_wavelengthCP1(t_r,t_c) < 490)
                {
                    t_ii.push_back(t_r);
                    t_jj.push_back(t_c);
                    isexped = true;
                }
            }
//            else
//            {isexped = false;}
        }
//        else
//        {isexped = false;}
    }
    while (isexped)
    {
        isexped = false;
        //进入while循环说明向左有了扩展。先更新波块中的像素点
        m_490blk.last().m_iblk += t_ii;
        m_490blk.last().m_jblk += t_jj;
        //暂存扩展的点
        t_iitem = t_ii;
        t_jjtem = t_jj;
        //清空扩展点vector
        t_ii.clear();
        t_jj.clear();
        //有待继续向左扩展点的个数
        int t_newsize = t_iitem.size();
        for (int i=0; i<t_newsize; i++)
        {
            if (t_jjtem[i]-1 >= 0)
            {
                if (m_DefImg->m_wavelengthCP1(t_iitem[i],t_jjtem[i]-1) > 400 && m_DefImg->m_wavelengthCP1(t_iitem[i],t_jjtem[i]-1) < 490)
                {
                    t_ii.push_back(t_iitem[i]);
                    t_jj.push_back(t_jjtem[i]-1);
                    isexped = true;
                }
//                else
//                {isexped = false;}
            }
//            else
//            {isexped = false;}
        }
    }

    //====================================================向上扩展====================================================
    isexped = false;
    t_count = m_490blk.last().m_iblk.size();    //更新当前波块中所有的非零像素点个数
    t_ii.clear();   //清理扩展vector
    t_jj.clear();
    for (int i=0; i<t_count; i++)
    {
        int t_r = m_490blk.last().m_iblk[i];
        if (t_r - 1 >= 0)
        {
            if (t_r - row == 0)
            {
                int t_c = m_490blk.last().m_jblk[i];
                t_r = t_r - 1;
                if (m_DefImg->m_wavelengthCP1(t_r,t_c) > 400 && m_DefImg->m_wavelengthCP1(t_r,t_c) < 490)
                {
                    t_ii.push_back(t_r);
                    t_jj.push_back(t_c);
                    isexped = true;
                }
            }
//            else
//            {isexped = false;}
        }
//        else
//        {isexped = false;}

    }
    while (isexped)
    {
        isexped = false;
        //进入while循环说明向上有了扩展。先更新波块中的像素点
        m_490blk.last().m_iblk += t_ii;
        m_490blk.last().m_jblk += t_jj;
        //暂存扩展的点
        t_iitem = t_ii;
        t_jjtem = t_jj;
        //清空扩展点vector
        t_ii.clear();
        t_jj.clear();
        //有待继续向上扩展点的个数
        int t_newsize = t_iitem.size();
        for (int i=0; i<t_newsize; i++)
        {
            if (t_iitem[i]-1 >= 0)
            {
                if (m_DefImg->m_wavelengthCP1(t_iitem[i]-1,t_jjtem[i]) > 400 && m_DefImg->m_wavelengthCP1(t_iitem[i]-1,t_jjtem[i]) < 490)
                {
                    t_ii.push_back(t_iitem[i]-1);
                    t_jj.push_back(t_jjtem[i]);
                    isexped = true;
                }
//                else
//                {isexped = false;}
            }
//            else
//            {isexped = false;}
        }
    }

    //====================================================向下扩展====================================================
    isexped = false;

    t_count = m_490blk.last().m_iblk.size();    //更新当前波块中所有的非零像素点个数
    if (row == 1489 && col == 1950)
    {
        qDebug()<<m_490blk.last().m_iblk;
        qDebug()<<m_490blk.last().m_jblk;
        qDebug()<<"t_count:"<<t_count<<row<<col;
    }

    t_ii.clear();   //清理扩展vector
    t_jj.clear();
    for (int i=0; i<t_count; i++)
    {
        int t_r = m_490blk.last().m_iblk[i];
        if (t_r + 1 <= 2047)
        {
            if (t_r - row == 2)
            {
                int t_c = m_490blk.last().m_jblk[i];
                t_r = t_r + 1;
                if (m_DefImg->m_wavelengthCP1(t_r,t_c) > 400 && m_DefImg->m_wavelengthCP1(t_r,t_c) < 490)
                {
                    t_ii.push_back(t_r);
                    t_jj.push_back(t_c);
                    isexped = true;
                }
            }
//            else
//            {isexped = false;}
        }
//        else
//        {isexped = false;}
    }
//    if (row == 1489 && col == 1950)
//    {
//        qDebug()<<t_ii;
//        qDebug()<<t_jj;
//    }
    while (isexped)
    {
        isexped = false;
        //进入while循环说明向下有了扩展。先更新波块中的像素点
        m_490blk.last().m_iblk += t_ii;
        m_490blk.last().m_jblk += t_jj;
        //暂存扩展的点
        t_iitem = t_ii;
        t_jjtem = t_jj;
        //清空扩展点vector
        t_ii.clear();
        t_jj.clear();
        //有待继续向下扩展点的个数
        int t_newsize = t_iitem.size();
        if (row == 1489 && col == 1950)
        {
            qDebug()<<t_iitem;
            qDebug()<<t_jjtem;
//            qDebug()<<"new size:"<<t_newsize;
        }

        for (int i=0; i<t_newsize; i++)
        {
            if (t_iitem[i]+1 <= 2047)
            {
                if (m_DefImg->m_wavelengthCP1(t_iitem[i]+1,t_jjtem[i]) > 400 && m_DefImg->m_wavelengthCP1(t_iitem[i]+1,t_jjtem[i]) < 490)
                {
                    if (row == 1489 && col == 1950)
                    {
                        qDebug()<<"handan num: "<<t_iitem[i]+1<<t_jjtem[i]<<m_DefImg->m_wavelengthCP1(t_iitem[i]+1,t_jjtem[i]);
                    }
                    t_ii.push_back(t_iitem[i]+1);
                    t_jj.push_back(t_jjtem[i]);
                    isexped = true;
                }
//                else
//                {isexped = false;}
            }
//            else
//            {isexped = false;}
        }
    }
    //;
#else
    bool isexped = true;
    QVector<int> t_ii[4];   //四个方向上的扩展矩阵:右，下，左，上
    QVector<int> t_jj[4];
    QVector<int> t_iitem[4];
    QVector<int> t_jjtem[4];

    //非零元素分拣
    int t_count = m_490blk.last().m_iblk.size();    //当前波块中所有的非零像素点个数
    for (int i=0; i<t_count; i++)
    {
        int t_r = m_490blk.last().m_iblk[i];
        int t_c = m_490blk.last().m_jblk[i];
        if (t_r == row) //在第一行中
        {
            t_ii[3].push_back(t_r);
            t_jj[3].push_back(t_c);
        }
        else if (t_r - row == 2)    //在最后一行中
        {
            t_ii[1].push_back(t_r);
            t_jj[1].push_back(t_c);
        }

        if (t_c == col) //在最左边的列中
        {
            t_ii[2].push_back(t_r);
            t_jj[2].push_back(t_c);
        }
        else if (t_c - col == 2)    //在最右边的列中
        {
            t_ii[0].push_back(t_r);
            t_jj[0].push_back(t_c);
        }
    }

    while (isexped)
    {
        isexped = false;
        for (int temi=0; temi<4; temi++)
        {
            t_iitem[temi] = t_ii[temi];
            t_jjtem[temi] = t_jj[temi];
            t_ii[temi].clear();
            t_jj[temi].clear();
        }

        //向右扩展
        int t_rnum = t_iitem[0].size();
        for (int rn=0; rn<t_rnum; rn++)
        {
            for (int rnitem=0; rnitem<3; rnitem++)
            {
                int rn_r = t_iitem[0][rn]-1+rnitem;
                int rn_c = t_jjtem[0][rn]+1;
                if (rn_r >= 0 && rn_r <= 2047 && rn_c >= 0 && rn_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP1(rn_r,rn_c) > 400 && m_DefImg->m_wavelengthCP1(rn_r,rn_c) < 490)
                    {
                        t_ii[0].push_back(rn_r);
                        t_jj[0].push_back(rn_c);
                        isexped = true;
                    }
                }
            }
        }

        //向下扩展
        int t_dnum = t_iitem[1].size();
        for (int dn=0; dn<t_dnum; dn++)
        {
            for (int dnitem=0; dnitem<3; dnitem++)
            {
                int dn_r = t_iitem[1][dn]-1+dnitem;//;
                int dn_c = t_jjtem[1][dn]+1;
                if (dn_r >= 0 && dn_r <= 2047 && dn_c >= 0 && dn_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP1(dn_r,dn_c) > 400 && m_DefImg->m_wavelengthCP1(dn_r,dn_c) < 490)
                    {
                        t_ii[1].push_back(dn_r);
                        t_jj[1].push_back(dn_c);
                        isexped = true;
                    }
                }
            }
        }

        //向左扩展
        int t_lnum = t_iitem[2].size();
        for (int ln=0; ln<t_lnum; ln++)
        {
            for (int lnitem=0; lnitem<3; lnitem++)
            {
                int ln_r = t_iitem[2][ln]-1+lnitem;
                int ln_c = t_jjtem[2][ln]-1;
                if (ln_r >= 0 && ln_r <= 2047 && ln_c >= 0 && ln_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP1(ln_r,ln_c) > 400 && m_DefImg->m_wavelengthCP1(ln_r,ln_c) < 490)
                    {
                        t_ii[2].push_back(ln_r);
                        t_jj[2].push_back(ln_c);
                        isexped = true;
                    }
                }
            }
        }
        //向上扩展
        int t_unum = t_iitem[3].size();
        for (int un=0; un<t_unum; un++)
        {
            for (int unitem=0; unitem<3; unitem++)
            {
                int un_r = t_iitem[3][un]-1+unitem;
                int un_c = t_jjtem[3][un]-1;
                if (un_r >= 0 && un_r <= 2047 && un_c >= 0 && un_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP1(un_r,un_c) > 400 && m_DefImg->m_wavelengthCP1(un_r,un_c) < 490)
                    {
                        t_ii[3].push_back(un_r);
                        t_jj[3].push_back(un_c);
                        isexped = true;
                    }
                }
            }
        }

        //更新波块元素
        for (int temi=0; temi<4; temi++)
        {
            m_490blk.last().m_iblk += t_ii[temi];
            m_490blk.last().m_jblk += t_jj[temi];
        }

    }
    //;
#endif
}


/******************************
*功能：初步寻找590波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDefectProcess::find590Ori(int row, int col)
{
    int ii[9]={0};
    int jj[9]={0};
    int count = 0;
    //需要检查的9个点，可能有超出矩阵范围的，需要进行筛选
    int t_dtci[9] = {row, row, row, row+1, row+1, row+1, row+2, row+2, row+2};
    int t_dtcj[9] = {col, col+1, col+2, col, col+1, col+2, col, col+1, col+2};

    for (int i=0; i<9; i++)
    {
        if (t_dtci[i] >= 0 && t_dtci[i] <= 2047 && t_dtcj[i] >= 0 && t_dtcj[i] <= 2447)
        {
            if(m_DefImg->m_wavelengthCP2(t_dtci[i],t_dtcj[i]) > 520 && m_DefImg->m_wavelengthCP2(t_dtci[i],t_dtcj[i]) < 590)
            {
                ii[count] = t_dtci[i];
                jj[count] = t_dtcj[i];
                count++;
            }
        }

    }
    //如果满足波长条件的点>=4个则生成并保存一个波块
    if (count >= 4)
    {
        block t_blk;
        for (int k=0; k<count; k++)
        {
            t_blk.m_iblk.push_back(ii[k]);
            t_blk.m_jblk.push_back(jj[k]);
        }
        m_590blk.push_back(t_blk);
        if (m_590blk.size() == 1301)
        {
            qDebug()<<"row&col"<<row<<col;
            qDebug()<<"iii"<<m_590blk.last().m_iblk;
            qDebug()<<"jjj"<<m_590blk.last().m_jblk;
        }
        return true;
    }
    else
    {
        return false;
    }
}



/******************************
*功能：扩展590波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::find590Add(int row, int col)
{
    bool isexped = true;
    QVector<int> t_ii[4];   //四个方向上的扩展矩阵:右，下，左，上
    QVector<int> t_jj[4];
    QVector<int> t_iitem[4];
    QVector<int> t_jjtem[4];

    //非零元素分拣
    int t_count = m_590blk.last().m_iblk.size();    //当前波块中所有的非零像素点个数
    for (int i=0; i<t_count; i++)
    {
        int t_r = m_590blk.last().m_iblk[i];
        int t_c = m_590blk.last().m_jblk[i];
        if (t_r == row) //在第一行中
        {
            t_ii[3].push_back(t_r);
            t_jj[3].push_back(t_c);
        }
        else if (t_r - row == 2)    //在最后一行中
        {
            t_ii[1].push_back(t_r);
            t_jj[1].push_back(t_c);
        }

        if (t_c == col) //在最左边的列中
        {
            t_ii[2].push_back(t_r);
            t_jj[2].push_back(t_c);
        }
        else if (t_c - col == 2)    //在最右边的列中
        {
            t_ii[0].push_back(t_r);
            t_jj[0].push_back(t_c);
        }
    }

    while (isexped)
    {
        isexped = false;
        for (int temi=0; temi<4; temi++)
        {
            t_iitem[temi] = t_ii[temi];
            t_jjtem[temi] = t_jj[temi];
            t_ii[temi].clear();
            t_jj[temi].clear();
        }
        //向右扩展
        int t_rnum = t_iitem[0].size();
        for (int rn=0; rn<t_rnum; rn++)
        {
            for (int rnitem=0; rnitem<3; rnitem++)
            {
                int rn_r = t_iitem[0][rn]-1+rnitem;
                int rn_c = t_jjtem[0][rn]+1;
                if (rn_r >= 0 && rn_r <= 2047 && rn_c >= 0 && rn_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP2(rn_r,rn_c) > 520 && m_DefImg->m_wavelengthCP2(rn_r,rn_c) < 590)
                    {
                        t_ii[0].push_back(rn_r);
                        t_jj[0].push_back(rn_c);
                        isexped = true;
                    }
                }
            }
        }
        //向下扩展
        int t_dnum = t_iitem[1].size();
        for (int dn=0; dn<t_dnum; dn++)
        {
            for (int dnitem=0; dnitem<3; dnitem++)
            {
                int dn_r = t_iitem[1][dn]-1+dnitem;//;
                int dn_c = t_jjtem[1][dn]+1;
                if (dn_r >= 0 && dn_r <= 2047 && dn_c >= 0 && dn_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP2(dn_r,dn_c) > 520 && m_DefImg->m_wavelengthCP2(dn_r,dn_c) < 590)
                    {
                        t_ii[1].push_back(dn_r);
                        t_jj[1].push_back(dn_c);
                        isexped = true;
                    }
                }
            }
        }
        //向左扩展
        int t_lnum = t_iitem[2].size();
        for (int ln=0; ln<t_lnum; ln++)
        {
            for (int lnitem=0; lnitem<3; lnitem++)
            {
                int ln_r = t_iitem[2][ln]-1+lnitem;
                int ln_c = t_jjtem[2][ln]-1;
                if (ln_r >= 0 && ln_r <= 2047 && ln_c >= 0 && ln_c <=2447)
                {
                    if (m_DefImg->m_wavelengthCP2(ln_r,ln_c) > 520 && m_DefImg->m_wavelengthCP2(ln_r,ln_c) < 590)
                    {
                        t_ii[2].push_back(ln_r);
                        t_jj[2].push_back(ln_c);
                        isexped = true;
                    }
                }
            }
        }
        //向上扩展
        int t_unum = t_iitem[3].size();
        for (int un=0; un<t_unum; un++)
        {
            for (int unitem=0; unitem<3; unitem++)
            {
                int un_r = t_iitem[3][un]-1+unitem;
                int un_c = t_jjtem[3][un]-1;
                if (un_r >= 0 && un_r <= 2047 && un_c >= 0 && un_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP2(un_r,un_c) > 520 && m_DefImg->m_wavelengthCP2(un_r,un_c) < 590)
                    {
                        t_ii[3].push_back(un_r);
                        t_jj[3].push_back(un_c);
                        isexped = true;
                    }
                }
            }
        }
        //更新波块元素
        for (int temi=0; temi<4; temi++)
        {
            m_590blk.last().m_iblk += t_ii[temi];
            m_590blk.last().m_jblk += t_jj[temi];
        }
    }
}



/******************************
*功能：找出490波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::findBlock490()
{
    //波长矩阵中的非零像素点个数
    int t_ptnum = m_wavePoints.size()/2;
    qDebug()<<"non-0 num:"<<t_ptnum;
    for (int i=0; i<t_ptnum; i++)
    {
        int t_row = m_wavePoints[2*i];
        int t_col = m_wavePoints[2*i+1];
        if (m_DefImg->m_wavelengthCP1(t_row,t_col) != 0) //如果波长拷贝矩阵中该像素点处的波长不为零（考虑到后续计算中可能将某些原来非零的点置零）
        {
            //生成该非零点周围需要探测的像素点位置(以该点为中心的9个位置)
            int t_dtci[9] = {t_row-1, t_row-1, t_row-1, t_row, t_row, t_row, t_row+1, t_row+1, t_row+1};
            int t_dtcj[9] = {t_col-1, t_col, t_col+1, t_col-1, t_col, t_col+1, t_col-1, t_col, t_col+1};

            for (int k=0; k<9; k++)
            {
                if (t_dtci[k] >= 0 && t_dtci[k] <= 2047 && t_dtcj[k] >= 0 && t_dtcj[k] <= 2447)
                {
                    if (find490Ori(t_dtci[k],t_dtcj[k]))
                    {
                        find490Add(t_dtci[k],t_dtcj[k]);
                        CalcuBlockPara(m_490blk.last());
//                        qDebug()<<"before delete:"<<m_490blk.last().m_dXPos<<m_490blk.last().m_dYPos<<m_490blk.last().m_dRadius<<m_490blk.last().m_dMassCenterX<<m_490blk.last().m_dMassCenterY;
                        //将已经纳入该波块的像素点在wavelengthCP1矩阵中将波长置零
                        QVector<int> t_ii = m_490blk.last().m_iblk;
                        QVector<int> t_jj = m_490blk.last().m_jblk;
                        int t_blksize = t_ii.size();
                        for (int n=0; n<t_blksize; n++)
                        {
                            m_DefImg->m_wavelengthCP1(t_ii[n],t_jj[n]) = 0;
                        }
//                        qDebug()<<"before delete:"<<m_490blk.last().m_dXPos<<m_490blk.last().m_dYPos<<m_490blk.last().m_dRadius<<m_490blk.last().m_dMassCenterX<<m_490blk.last().m_dMassCenterY;
                        //remove the small block
                        if (m_490blk.last().m_dRadius < 3)
                        {
                            m_490blk.removeLast();
                        }
                    }
                }

            }
        }

    }
//    int wac = 389;
//    qDebug()<<"block num: "<<m_490blk[wac].m_iblk<<"="<<m_490blk[wac].m_jblk;
//    for (int i=0; i<m_490blk.size(); i++)
//    {
//        qDebug()<<"the"<<i<<":"<<m_490blk[i].m_iblk.size();
//    }
//    MatrixXd out490(2048,2448);
//    for (int i=0; i<m_490blk.size(); i++)
//    {
//        for (int j=0; j<m_490blk[i].m_iblk.size(); j++)
//        {
//            int t_iblk = m_490blk[i].m_iblk[j];
//            int t_jblk = m_490blk[i].m_jblk[j];
//            out490(t_iblk,t_jblk) = i;
//        }
//    }
//    OutPut("..\\out490.txt",out490);
    qDebug()<<m_490blk.size()<<"============";
//    showBlocks(m_490blk);
}



/******************************
*功能：找出590波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::findBlock590()
{
    OutPut("..\\out590.txt",m_DefImg->m_wavelengthCP2);
    //波长矩阵中的非零像素点个数
    int t_ptnum = m_wavePoints.size()/2;
    for (int i=0; i<t_ptnum; i++)
    {

        int t_row = m_wavePoints[2*i];
        int t_col = m_wavePoints[2*i+1];
        if (i == 4633)
        {
            qDebug()<<"row & col: "<<t_row<<t_col;
        }
        if (m_DefImg->m_wavelengthCP2(t_row,t_col) != 0) //如果波长拷贝矩阵中该像素点处的波长不为零（考虑到后续计算中可能将某些原来非零的点置零）
        {
            //生成该非零点周围需要探测的像素点位置(以该点为中心的9个位置)
            int t_dtci[9] = {t_row-1, t_row-1, t_row-1, t_row, t_row, t_row, t_row+1, t_row+1, t_row+1};
            int t_dtcj[9] = {t_col-1, t_col, t_col+1, t_col-1, t_col, t_col+1, t_col-1, t_col, t_col+1};
            for (int k=0; k<9; k++)
            {
                qDebug()<<"i==="<<i<<"k==="<<k;
                if (t_dtci[k] >= 0 && t_dtci[k] <= 2047 && t_dtcj[k] >= 0 && t_dtcj[k] <= 2447)
                {
                    if (find590Ori(t_dtci[k],t_dtcj[k]))
                    {
                        qDebug()<<"I found a 590Ori"<<t_dtci[k]<<t_dtcj[k];
                        find590Add(t_dtci[k],t_dtcj[k]);
                        qDebug()<<"=======================================I found a 590Add";
                        CalcuBlockPara(m_590blk.last());
                        //将已经纳入该波块的像素点在wavelengthCP1矩阵中将波长置零
                        QVector<int> t_ii = m_590blk.last().m_iblk;
                        QVector<int> t_jj = m_590blk.last().m_jblk;
                        int t_blksize = t_ii.size();
                        for (int n=0; n<t_blksize; n++)
                        {
                            m_DefImg->m_wavelengthCP2(t_ii[n],t_jj[n]) = 0;
                        }
                        //remove the small block
                        if (m_590blk.last().m_dRadius < 3)
                        {
                            m_590blk.removeLast();
                        }
                    }
                }

            }
        }

    }
//    MatrixXd out490(2048,2448);
//    for (int i=0; i<m_490blk.size(); i++)
//    {
//        for (int j=0; j<m_490blk[i].m_iblk.size(); j++)
//        {
//            int t_iblk = m_490blk[i].m_iblk[j];
//            int t_jblk = m_490blk[i].m_jblk[j];
//            out490(t_iblk,t_jblk) = i;
//        }
//    }
//    OutPut("..\\out490.txt",out490);
    qDebug()<<m_590blk.size()<<"590============";
}



/******************************
*功能：在波长矩阵的基础上找出损伤点
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::findDefect()
{
    int t_490size = m_490blk.size();
    int t_590size = m_590blk.size();
    bool t_IsFit = false;
    for (int i=0; i<t_490size; i++)
    {
        t_IsFit = false;
        for (int j=0; j<t_590size; j++)
        {
//            qDebug()<<"cross"<<i<<j<<t_490size<<t_590size;
            if (IsBlksCross(m_490blk[i],m_590blk[j]))   //第一判据
            {
                qDebug()<<"cross ij:"<<i<<j<<"=============";
                qDebug()<<m_490blk[i].m_dXPos<<m_490blk[i].m_dYPos<<m_490blk[i].m_dRadius<<"=============";
                qDebug()<<m_590blk[i].m_dXPos<<m_590blk[i].m_dYPos<<m_590blk[i].m_dRadius<<"=============";

                double t_centerPtX490, t_centerPtY490;
                double t_centerPtX590, t_centerPtY590;
                double t_Candela490, t_Candela590;
                double t_mintem, t_maxtem;
                //get mass center point of 490
                VectorMaxMin(m_490blk[i].m_OffCenter,t_maxtem,t_mintem);
                int t_index = m_490blk[i].m_OffCenter.indexOf(t_mintem);
                t_centerPtX490 = m_490blk[i].m_iblk[t_index];
                t_centerPtY490 = m_490blk[i].m_jblk[t_index];
                //get light intensity of 490--minimum value among the R, G, and B
                t_Candela490 = smaller(m_DefImg->m_R(t_centerPtX490,t_centerPtY490), m_DefImg->m_G(t_centerPtX490,t_centerPtY490), m_DefImg->m_B(t_centerPtX490,t_centerPtY490));

                //get mass center point of 590
                VectorMaxMin(m_590blk[i].m_OffCenter,t_maxtem,t_mintem);
                t_index = m_590blk[i].m_OffCenter.indexOf(t_mintem);
                t_centerPtX590 = m_590blk[i].m_iblk[t_index];
                t_centerPtY590 = m_590blk[i].m_jblk[t_index];
                //get light intensity of 590--minimum value among the R, G, and B
                t_Candela590 = smaller(m_DefImg->m_R(t_centerPtX590,t_centerPtY590), m_DefImg->m_G(t_centerPtX590,t_centerPtY590), m_DefImg->m_B(t_centerPtX590,t_centerPtY590));

                //compare light intensity
                if (t_Candela490 > t_Candela590)    //第二判据
                {
                    t_IsFit = true;
                    block t_defect;
                    t_defect.m_iblk = m_490blk[i].m_iblk + m_590blk[j].m_iblk;
                    t_defect.m_jblk = m_490blk[i].m_jblk + m_590blk[j].m_jblk;
                    m_Defblk.push_back(t_defect);
                    CalcuBlockPara(m_Defblk.last());
                    break;  //此处直接break，忽略了一个490块可能与多个590块相交的情况
                }

            }
            //;
        }
        if (t_IsFit == false) //不与任何590波块相交，当做小损伤直接pushback
        {
            m_Defblk.push_back(m_490blk[i]);
        }
    }
    qDebug()<<"defect blk size: "<<m_Defblk.size();
    showBlocks(m_Defblk);
}





/******************************
*功能：判断两个波块是否相交
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDefectProcess::IsBlksCross(block blk1, block blk2)
{
    if (qSqrt(qPow((blk1.m_dXPos - blk2.m_dXPos),2) + qPow((blk1.m_dYPos - blk2.m_dYPos),2)) < (blk1.m_dRadius + blk2.m_dRadius))
    {
        qDebug()<<"cross blk1"<<blk1.m_dXPos<<blk1.m_dYPos<<blk1.m_dRadius;
        qDebug()<<"cross blk2"<<blk2.m_dXPos<<blk2.m_dYPos<<blk2.m_dRadius;
        return true;
    }
    else
    {
        return false;
    }
}
/******************************
*功能：找出需要检测的像素点
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::findDetecPoints()
{
//    int t_ptnum = m_wavePoints.size()/2;
//    for (int i=0; i<t_ptnum; i++)
//    {
//        int t_ri = (int)m_wavePoints[i*2];
//        int t_ci = (int)m_wavePoints[i*2+1];

//        int t_rcnum = (t_ri-1)*10000 + (t_ci - 1);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri-1)*10000 + (t_ci);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri-1)*10000 + (t_ci + 1);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri)*10000 + (t_ci - 1);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri)*10000 + (t_ci);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri)*10000 + (t_ci + 1);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri + 1)*10000 + (t_ci - 1);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri + 1)*10000 + (t_ci);
//        m_dtcPtHash.insert(t_rcnum,0);

//        t_rcnum = (t_ri + 1)*10000 + (t_ci + 1);
//        m_dtcPtHash.insert(t_rcnum,0);
//    }
//    qDebug()<<"detected num: "<<m_dtcPtHash.size();
}










/*******************************************************************
**功能：计算流程
**输入：
**输出：
**返回值：
*******************************************************************/
void QDefectProcess::caclworkflow()
{
//    //load data from QImage
//    m_OriImg = new YImage(sOrigPath);
//    m_DefImg = new YImage(sDestPath);





//    OutPut("..\\RinputE1.txt",m_OriImg->m_R);
//    OutPut("..\\GinputE1.txt",m_OriImg->m_G);
//    OutPut("..\\BinputE1.txt",m_OriImg->m_B);
//    OutPut("..\\RinputE2.txt",m_DefImg->m_R);
//    OutPut("..\\GinputE2.txt",m_DefImg->m_G);
//    OutPut("..\\BinputE2.txt",m_DefImg->m_B);

    QTime subtime;
    subtime.start();
    subImagDenosing();
    qDebug()<<"============subtime:"<<subtime.elapsed();


    QTime caltime;
    caltime.start();
//#pragma omp parallel
    for(int i = 0 ;i < 3;i++)
    {
        caclThreshold(i); //calcu the threshhold of R/G/B
    }
    qDebug()<<"============caltime:"<<caltime.elapsed();


    QTime thretime;
    thretime.start();
    thresholdImagDenosing();
    qDebug()<<"============thretime:"<<thretime.elapsed();
//    qDebug()<<"finish denosing";
    //以上功能验证完毕


   // caclResponse();//计算响应曲线 好像没啥用
    QTime saturtime;
    saturtime.start();
    caclSaturation();
    qDebug()<<"============satutime:"<<saturtime.elapsed();


    QTime finddetime;
    finddetime.start();
    findBlock490();
    qDebug()<<"before 590";
    findBlock590();
    qDebug()<<"after 590";
    qDebug()<<"finddetime============finddetime:"<<finddetime.elapsed();


    QTime defecttime;
    defecttime.start();
    findDefect();
    qDebug()<<"Defecttime============defecttime:"<<defecttime.elapsed();

    qDebug()<<"finish";
}



/******************************
*功能：show blocks
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::showBlocks(QVector<block> blks)
{
    int t_size = blks.size();
//    double t_ratio = 2448/2048;
    double t_miniRatio = (double)700/2048;
    m_tImgLabel.setGeometry(100,100,1000,900);
    for (int i=0; i<t_size; i++)
    {
        m_tImgLabel.m_y.push_back(blks[i].m_dXPos*t_miniRatio);
        m_tImgLabel.m_x.push_back(blks[i].m_dYPos*t_miniRatio);
        m_tImgLabel.m_radius.push_back(blks[i].m_dRadius*t_miniRatio);
//        qDebug()<<"^&*%=="<<blks[i].m_dXPos<<blks[i].m_dYPos<<blks[i].m_dRadius<<t_miniRatio;
        qDebug()<<"circle para: "<<m_tImgLabel.m_x.last()<<m_tImgLabel.m_y.last()<<m_tImgLabel.m_radius.last();
    }
    m_tImgLabel.show();
}




/******************************
*功能：read data
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDefectProcess::readData()
{

    bool t_suc = false;
    QStringList namelist;
    double *t_array[6];
    int t_dms[12];
    int NUM = 6;
    namelist.append("..\\data\\matlab\\R1out.txt");
    namelist.append("..\\data\\matlab\\G1out.txt");
    namelist.append("..\\data\\matlab\\B1out.txt");
    namelist.append("..\\data\\matlab\\R5out.txt");
    namelist.append("..\\data\\matlab\\G5out.txt");
    namelist.append("..\\data\\matlab\\B5out.txt");

    t_dms[0] = m_OriImg->m_height;
    t_dms[1] = m_OriImg->m_width;
    t_dms[2] = t_dms[0];
    t_dms[3] = t_dms[1];
    t_dms[4] = t_dms[0];
    t_dms[5] = t_dms[1];

    t_dms[6] = m_DefImg->m_height;
    t_dms[7] = m_DefImg->m_width;
    t_dms[8] = t_dms[6];
    t_dms[9] = t_dms[7];
    t_dms[10] = t_dms[6];
    t_dms[11] = t_dms[7];

    for (int fnum=0; fnum<NUM; fnum++)
    {

        int t_row = t_dms[fnum*2];
        int t_col = t_dms[fnum*2 +1];
        qDebug()<<namelist.at(fnum)<<": rowcol: "<<t_row<<t_col;
        t_array[fnum] = new double[t_row * t_col];
        QVector<double> vector;
        QFile fl(namelist.at(fnum));
        if (!fl.open(QIODevice::ReadOnly))
        {
            qDebug()<<"open wrong";
            return t_suc;
        }
        else
        {
            int rowcnt = 0;
            int colcnt = 0;
            QTextStream content(&fl);
            while (!content.atEnd())
            {
                QString t_datastr = content.readLine();
                QStringList t_strlist = t_datastr.simplified().split(" ");
                colcnt = t_strlist.size();
                for (int i=0; i<colcnt; i++)
                {
                    vector.append(t_strlist.at(i).toDouble());
                }
                rowcnt++;
            }
            MatrixXd temm(2048,2448);
            m_temdata[fnum] = temm;
            //data is stored in the array with a column-wise
            for (int i=0; i<rowcnt; i++)
            {
                for (int j=0; j<colcnt; j++)
                {
                    m_temdata[fnum](i,j) = vector.at(i*colcnt+j);
                }
            }
            t_suc = true;
        }

    }

    return true;
}



/******************************
*功能：根据rgb比值判断对应的波长
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
double QDefectProcess::getWaveLength(double rgbratio, double order)
{
    for (int i=0; i<M_CALI_HEIGH; i++)
    {
        if (rgbratio > g_wavelengthCali[i][2] && rgbratio < g_wavelengthCali[i][3] && order == g_wavelengthCali[i][4])
        {
            return g_wavelengthCali[i][0];
            break;
        }
    }
    return 0;
}


/*******************************************************************
**功能：三个数最大值
**输入：
**输出：
**返回值：
*******************************************************************/
int QDefectProcess::max(int x,int y,int z)
{
    int max=x>y?x:y;
    max=max>z?max:z;
    return max;
}
/*******************************************************************
**功能：三个数最小值
**输入：
**输出：
**返回值：
*******************************************************************/
int QDefectProcess::smaller(int x,int y,int z)
{
    int smaller=0;
    smaller=x<y?x:y;
    smaller=smaller<z?smaller:z;
    return smaller;
}
/*******************************************************************
**功能：三个数最小值(reload)
**输入：
**输出：
**返回值：
*******************************************************************/
double QDefectProcess::smaller(double x,double y,double z)
{
    double smaller=0;
    smaller=x<y?x:y;
    smaller=smaller<z?smaller:z;
    return smaller;
}
/*******************************************************************
**功能：三个数中间值
**输入：
**输出：
**返回值：
*******************************************************************/
int QDefectProcess::middle(int x,int y,int z)
{
    int middle=0;
    middle=x+y+z-smaller(x,y,z)-max(x,y,z);
    return middle;
}

/*******************************************************************
**功能：三个数的索引值
**输入：
**输出：
**返回值：
*******************************************************************/
int QDefectProcess::getIndex(int x,int y,int z,int value)
{
    if(x == value)
    {
        return 1;
    }
    if(y == value)
    {
        return 2;
    }
    if(z == value)
    {
        return 3;
    }
    return 0;
}


YImage::YImage(QString Imgname)
{
    m_Img.load(Imgname);
    m_Img = m_Img.convertToFormat(QImage::Format_RGB888);
    QColor pixel;

    m_width = m_Img.width();
    m_height = m_Img.height();
    m_pixelsnum = m_width * m_height;

    MatrixXd tem_R(m_height,m_width);
    MatrixXd tem_G(m_height,m_width);
    MatrixXd tem_B(m_height,m_width);

    for(int i = 0; i < m_height;i++)
    {
        for(int j = 0; j < m_width;j++)
        {
            pixel = m_Img.pixel(j,i);
            tem_R(i,j) = pixel.red();//qRed(pixel);
            tem_G(i,j) = pixel.green();//qGreen(pixel);
            tem_B(i,j) = pixel.blue();//qBlue(pixel);
        }
    }
    m_R = tem_R;
    m_G = tem_G;
    m_B = tem_B;
}


/******************************
*功能：计算波块的圆心和半径
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::CalcuBlockPara(block &waveblock)
{
    double t_imin = 0;
    double t_imax = 0;
    double t_jmin = 0;
    double t_jmax = 0;
    VectorMaxMin(waveblock.m_iblk,t_imax,t_imin);
    VectorMaxMin(waveblock.m_jblk,t_jmax,t_jmin);
    waveblock.m_dXPos = (double)(t_imin + t_imax)/2;
    waveblock.m_dYPos = (double)(t_jmin + t_jmax)/2;
    waveblock.m_dRadius = qSqrt((double)(qPow((t_imax - t_imin),2) + qPow((t_jmax - t_jmin),2)))/2*1.2;
    waveblock.m_dMassCenterX = VectorAver(waveblock.m_iblk);
    waveblock.m_dMassCenterY = VectorAver(waveblock.m_jblk);
//    qDebug()<<waveblock.m_dXPos<<waveblock.m_dYPos<<waveblock.m_dRadius<<waveblock.m_dMassCenterX<<waveblock.m_dMassCenterY;
    double t_off = 0;
    int t_size = waveblock.m_iblk.size();
    for (int i=0; i<t_size; i++)
    {
        t_off = qSqrt(qPow(((double)waveblock.m_iblk[i] - waveblock.m_dMassCenterX),2) + qPow(((double)waveblock.m_jblk[i] - waveblock.m_dMassCenterY),2));
        waveblock.m_OffCenter.push_back(t_off);
    }

}



/******************************
*功能：计算vector的最大值或最小值
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::VectorMaxMin(QVector<int> datavector, double &maxvalue, double &minvalue)
{
    QVector<int> t_vector = datavector;
    qSort(t_vector.begin(),t_vector.end());
    minvalue = t_vector[0];
    maxvalue = t_vector[t_vector.size() - 1];
}


/******************************
*功能：计算vector的最大值或最小值(reload)
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDefectProcess::VectorMaxMin(QVector<double> datavector, double &maxvalue, double &minvalue)
{

    QVector<double> t_vector = datavector;
    qSort(t_vector.begin(),t_vector.end());
    minvalue = t_vector[0];
    maxvalue = t_vector[t_vector.size() - 1];
}



/******************************
*功能：计算vector的平均值
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
double QDefectProcess::VectorAver(QVector<int> datavector)
{
    int t_sumation = 0;
    double t_Aver = 0;
    int t_size = datavector.size();
    for (int i=0; i<t_size; i++)
    {
        t_sumation = t_sumation + datavector[i];
    }
    t_Aver = (double)t_sumation / t_size;
    return t_Aver;
}


/******************************
*功能：寻找item在vector中的index
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
