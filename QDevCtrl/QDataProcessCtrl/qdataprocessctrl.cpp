#include "qdataprocessctrl.h"
#include <QFile>
#include <QByteArray>
#include <QtMath>
#include "cwise.h"
#include <QDebug>
#include <QApplication>
#include <iostream>
#include "define.h"

using namespace std;
using namespace cv;


QDataProcessCtrl::QDataProcessCtrl(QObject *parent) : QObject(parent)
{
    m_lfRate = 0.99;
    memset(m_rgbRate,0,sizeof(m_rgbRate));
}

QDataProcessCtrl::~QDataProcessCtrl()
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

YImage::YImage(QString Imgname)
{
    Mat image = imread(Imgname.toStdString());

    m_width = image.cols;
    m_height = image.rows;
    m_pixelsnum = m_width * m_height;
    MatrixXd tem_R(m_height,m_width);
    MatrixXd tem_G(m_height,m_width);
    MatrixXd tem_B(m_height,m_width);

    for (int i=0; i<m_height; i++)
    {
        for (int j=0; j<m_width; j++)
        {
            tem_B(i,j) = image.at<Vec3b>(i,j)[0];
            tem_G(i,j) = image.at<Vec3b>(i,j)[1];
            tem_R(i,j) = image.at<Vec3b>(i,j)[2];
        }
    }
    m_R = tem_R;
    m_G = tem_G;
    m_B = tem_B;
    image.release();
}


/*******************************************************************
**功能：减去原始图像去噪声
**输入：
**输出：
**返回值：
*******************************************************************/
int QDataProcessCtrl::subImagDenosing()
{
    if (m_DefImg->m_width != m_OriImg->m_width || m_DefImg->m_height != m_OriImg->m_height)
    {
        return 1;//图像不相同 无法处理
    }
    //损伤前后图片RGB差值
    m_rgbDeltaNoabs[0] = (m_DefImg->m_R - m_OriImg->m_R).cast<int>();
    m_rgbDeltaNoabs[1] = (m_DefImg->m_G - m_OriImg->m_G).cast<int>();
    m_rgbDeltaNoabs[2] = (m_DefImg->m_B - m_OriImg->m_B).cast<int>();
    return 0;
}


/*******************************************************************
**功能：计算阈值
**输入：nType 阈值类型 0--R 1-G 2-B
**输出：
**返回值：
*******************************************************************/
void QDataProcessCtrl::caclThreshold(int nType)
{
    int t_pixelnum = m_DefImg->m_pixelsnum;
    int t_colnum = m_DefImg->m_width;
    //statistics
    for (int i=0; i<t_pixelnum; i++)    //should be cpmpared with another loop method(by t_rownum and t_colnum)
    {   int t_row = i/t_colnum;
        int t_col = i%t_colnum;
        m_rgbRate[nType][abs(m_rgbDeltaNoabs[nType](t_row,t_col))]++;
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
}


/*******************************************************************
**功能：阈值去噪声
**输入：
**输出：
**返回值：
*******************************************************************/
void QDataProcessCtrl:: thresholdImagDenosing(double rR, double gR, double bR)
{
    int t_pixelnum = m_DefImg->m_pixelsnum;
    int t_colnum = m_DefImg->m_width;
    for (int i=0; i<t_pixelnum; i++)    //should be cpmpared with another loop method(by t_rownum and t_colnum)
    {   int t_row = i/t_colnum;
        int t_col = i%t_colnum;
        //问题在于使用差值矩阵的绝对值和阈值比较还是用原值与阈值比较，用绝对值比较会使计算时间大大加长（计算的像素点增多）
        if (m_rgbDeltaNoabs[0](t_row,t_col)<m_lfThreshold[0]*rR && m_rgbDeltaNoabs[1](t_row,t_col)<m_lfThreshold[1]*gR && m_rgbDeltaNoabs[2](t_row,t_col)<m_lfThreshold[2]*bR)
        {
            m_DefImg->m_R(t_row,t_col) = 0;
            m_DefImg->m_G(t_row,t_col) = 0;
            m_DefImg->m_B(t_row,t_col) = 0;
        }
        else if (m_rgbDeltaNoabs[0](t_row,t_col) == 0 || m_rgbDeltaNoabs[1](t_row,t_col) == 0 || m_rgbDeltaNoabs[2](t_row,t_col) == 0)
        {
            m_DefImg->m_R(t_row,t_col) = 0;
            m_DefImg->m_G(t_row,t_col) = 0;
            m_DefImg->m_B(t_row,t_col) = 0;
        }
    }

    //清理掉差值矩阵的内存
    m_rgbDeltaNoabs[0].resize(0,0);
    m_rgbDeltaNoabs[1].resize(0,0);
    m_rgbDeltaNoabs[2].resize(0,0);
    CalcuRGBNoneZero(m_DefImg->m_R, m_DefImg->m_G, m_DefImg->m_B);
}


void QDataProcessCtrl::CalcuRGBNoneZero(MatrixXd &R, MatrixXd &G, MatrixXd &B)
{
    MatrixXd t_R01;
    MatrixXd t_G01;
    MatrixXd t_B01;
    MatrixXd t_RGB01;
    t_R01 = R.cwiseNotEqual(R*0).cast<double>();
    t_G01 = G.cwiseNotEqual(G*0).cast<double>();
    t_B01 = B.cwiseNotEqual(B*0).cast<double>();
    t_RGB01 = t_R01.cwiseProduct(t_G01).cwiseProduct(t_B01);

    R = R.cwiseProduct(t_RGB01);
    G = G.cwiseProduct(t_RGB01);
    B = B.cwiseProduct(t_RGB01);
}


/******************************
*功能:两幅图差值矩阵去噪声
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDataProcessCtrl::DeltaImageDenosing()
{
    //;
}


/******************************
*功能:
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDataProcessCtrl::SubRGBmin(int num, MatrixXd &Rarray, MatrixXd &Garray, MatrixXd &Barray, MatrixXd &RGBPos)
{
    int nindex = 0;
    int t_row = m_DefImg->m_height;
    int t_col = m_DefImg->m_width;
    for (int i=0; i<t_row; i++)
    {
        for (int j=0; j<t_col; j++)
        {
            if (m_DefImg->m_R(i,j) != 0)
            {
                Rarray(nindex,0) = m_DefImg->m_R(i,j);
                Garray(nindex,0) = m_DefImg->m_G(i,j);
                Barray(nindex,0) = m_DefImg->m_B(i,j);
                RGBPos(nindex,0) = i;
                RGBPos(nindex,1) = j;
                nindex++;
            }
        }
    }
    //提取饱和度特征

    MatrixXd t_minmat(num,1);
    t_minmat = Rarray.cwiseMin(Garray);
    t_minmat = t_minmat.cwiseMin(Barray);
    //用RGB三个矩阵减去t_minmat，提取饱和度特征,结果已验证成功
    Rarray = Rarray - t_minmat;
    Garray = Garray - t_minmat;
    Barray = Barray - t_minmat;

}

void QDataProcessCtrl::CalcuRb(MatrixXd &Rb, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray)
{
    MatrixXd Rcpb;       //RGB三个矩阵的部分copy
    MatrixXi C12b,C13b;   //比较过程中产生的中间矩阵
    C12b = Rarray.binaryExpr(Garray,GComp<double>());
    Rcpb = Rarray.cwiseProduct(C12b.cast<double>());
    C13b = Rcpb.binaryExpr(Barray,GComp<double>());
    Rb = C12b.cwiseProduct(C13b).cast<double>();    //此处点乘是否有必要
}

void QDataProcessCtrl::CalcuGs(MatrixXd &Gs, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray)
{
    MatrixXd Gcps;       //RGB三个矩阵的部分copy
    MatrixXi C12b,C23s;   //比较过程中产生的中间矩阵
    C12b = Rarray.binaryExpr(Garray,GComp<double>());
    Gcps = Garray.cwiseProduct(C12b.cast<double>());
    C23s = Gcps.binaryExpr(Barray,SComp<double>());
    Gs = C12b.cwiseProduct(C23s).cast<double>();
}


void QDataProcessCtrl::CalcuRs(int num, MatrixXd &Rs, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray)
{
    MatrixXd Rcps;
    MatrixXi C12s,C13s;
    CalcuC12s( num, C12s,Rarray,Garray);
    Rcps = Rarray.cwiseProduct(C12s.cast<double>());
    C13s = Rcps.binaryExpr(Barray,SComp<double>());
    Rs = C12s.cwiseProduct(C13s).cast<double>();
}

void QDataProcessCtrl::CalcuGb(int num, MatrixXd &Gb, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray)
{
    MatrixXd Gcpb;
    MatrixXi C12s,C23b;
    CalcuC12s( num, C12s,Rarray,Garray);
    Gcpb = Garray.cwiseProduct(C12s.cast<double>());
    C23b = Gcpb.binaryExpr(Barray,GComp<double>());
    Gb = C12s.cwiseProduct(C23b).cast<double>();
}

void QDataProcessCtrl::CalcuC12s(int num, MatrixXi &C12s, MatrixXd Rarray, MatrixXd Garray)
{
    MatrixXi C12b;
    MatrixXd Cori;  //同维度的单位阵
    Cori.setOnes(num,1);
    C12b = Rarray.binaryExpr(Garray,GComp<double>());
    C12s = Cori.cast<int>() - C12b;
}

void QDataProcessCtrl::CalcuBbs(int num, MatrixXd &Bb, MatrixXd &Bs, MatrixXd Rb, MatrixXd Rs, MatrixXd Gb, MatrixXd Gs)
{
    MatrixXd Cori;
    Cori.setOnes(num,1);
    Bb = Cori - Rb - Gb;
    Bs = Cori - Rs - Gs;
}

void QDataProcessCtrl::CalcuRGB2(int num, MatrixXd &R2, MatrixXd &G2, MatrixXd &B2, MatrixXd Bb, MatrixXd Bs, MatrixXd Rb, MatrixXd Rs, MatrixXd Gb, MatrixXd Gs)
{
    MatrixXd Cori;
    Cori.setOnes(num,1);
    R2 = Cori - Rb - Rs;
    G2 = Cori - Gb - Gs;
    B2 = Cori - Bb - Bs;
}

void QDataProcessCtrl::CalcuRGBRatio(MatrixXd &RGBRatio, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray, MatrixXd Rb, MatrixXd R2, MatrixXd Gb, MatrixXd G2, MatrixXd Bb, MatrixXd B2)
{
    MatrixXd rgbMax1;
    MatrixXd rgbMax2;
    //rgbmax1 & rgbmax2
    rgbMax1 = Rarray.cwiseProduct(Rb) + Garray.cwiseProduct(Gb) + Barray.cwiseProduct(Bb);
    rgbMax2 = Rarray.cwiseProduct(R2) + Garray.cwiseProduct(G2) + Barray.cwiseProduct(B2);

    //rgbratio
    RGBRatio = rgbMax1.cwiseQuotient(rgbMax2);
}

void QDataProcessCtrl::CalcuOrderAndRatio(int num, MatrixXd &Order, MatrixXd &RGBRatio, MatrixXd Rarray, MatrixXd Garray, MatrixXd Barray)
{
    MatrixXd Rb,R2,Rs,Gb,G2,Gs,Bb,B2,Bs; //保存RGB矩阵最大最小值位置
    CalcuRb(Rb,Rarray,Garray,Barray);
    CalcuGs(Gs,Rarray,Garray,Barray);
    CalcuRs(num,Rs,Rarray,Garray,Barray);
    CalcuGb(num,Gb,Rarray,Garray,Barray);

    CalcuBbs(num,Bb,Bs,Rb,Rs,Gb,Gs);
    CalcuRGB2(num,R2,G2,B2,Bb,Bs,Rb,Rs,Gb,Gs);

    //RGB顺序矩阵
    Order = (Rb+Gb*2+Bb*3)*100 + (R2+G2*2+B2*3)*10 + (Rs+Gs*2+Bs*3);

    CalcuRGBRatio(RGBRatio,Rarray,Garray,Barray,Rb,R2,Gb,G2,Bb,B2);
}
/*******************************************************************
**功能：计算饱和度特征值并对照标定表将饱和度特征值转换成波长信息
**输入：
**输出：
**返回值：
*******************************************************************/
void QDataProcessCtrl::caclSaturation()
{
    /*将去噪后的、稀疏的RGB矩阵中非零元素提取，转存为列向量*/
    //计算R矩阵中的非零元素个数
    int t_sum;
    t_sum = m_DefImg->m_R.cwiseNotEqual(m_DefImg->m_R*0).cast<double>().sum();
    MatrixXd R_array(t_sum,1);
    MatrixXd G_array(t_sum,1);
    MatrixXd B_array(t_sum,1);
    MatrixXd rgb_pos(t_sum,2);
    SubRGBmin(t_sum,R_array,G_array,B_array,rgb_pos);

    //RGB顺序矩阵
    MatrixXd Order,rgbRatio;
    CalcuOrderAndRatio(t_sum,Order,rgbRatio,R_array,G_array,B_array);
    //剔除rgbratio中inf和nan
    m_DefImg->m_wavelength.resize(m_DefImg->m_height,m_DefImg->m_width);
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
    m_DefImg->m_wavelengthCP1 = m_DefImg->m_wavelength; //拷贝波长矩阵于CP1中，用于寻找490波块
}



/******************************
*功能：初步寻找490波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDataProcessCtrl::find490Ori(int row, int col)
{
    int ii[16]={0};
    int jj[16]={0};
    int count = 0;
    //需要检查的16个点，可能有超出矩阵范围的，需要进行筛选
    int t_dtci[16] = {row, row, row,row, row+1, row+1, row+1, row+1, row+2, row+2, row+2, row+2, row+3, row+3, row+3, row+3};
    int t_dtcj[16] = {col, col+1, col+2,col+3, col, col+1, col+2,col+3, col, col+1, col+2, col+3, col, col+1, col+2, col+3};

    for (int i=0; i<16; i++)
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
    if (count == 16)
    {
        qblock t_blk;
        for (int k=0; k<count; k++)
        {
            t_blk.m_iblk.push_back(ii[k]);
            t_blk.m_jblk.push_back(jj[k]);
        }
        m_490blk.push_back(t_blk);

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
void QDataProcessCtrl::find490Add(int row, int col)
{
    //对之前产生的490波块进行四个方向上的扩展
    int nDimension = 4;

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
        else if (t_r - row == (nDimension-1))    //在最后一行中
        {
            t_ii[1].push_back(t_r);
            t_jj[1].push_back(t_c);
        }

        if (t_c == col) //在最左边的列中
        {
            t_ii[2].push_back(t_r);
            t_jj[2].push_back(t_c);
        }
        else if (t_c - col == (nDimension - 1))    //在最右边的列中
        {
            t_ii[0].push_back(t_r);
            t_jj[0].push_back(t_c);
        }
    }

    bool flag = true;
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
                        flag = true;
                        for (int rii = 0; rii<t_ii[0].size(); rii++)
                        {
                            if (rn_r == t_ii[0][rii] && rn_c == t_jj[0][rii])
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag == true)
                        {
                            t_ii[0].push_back(rn_r);
                            t_jj[0].push_back(rn_c);
                            isexped = true;
                        }
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
                int dn_r = t_iitem[1][dn]+1;//;
                int dn_c = t_jjtem[1][dn]-1+dnitem;
                if (dn_r >= 0 && dn_r <= 2047 && dn_c >= 0 && dn_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP1(dn_r,dn_c) > 400 && m_DefImg->m_wavelengthCP1(dn_r,dn_c) < 490)
                    {
                        flag = true;
                        for (int dii = 0; dii<t_ii[1].size(); dii++)
                        {
                            if (dn_r == t_ii[1][dii] && dn_c == t_jj[1][dii])
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag == true)
                        {
                            t_ii[1].push_back(dn_r);
                            t_jj[1].push_back(dn_c);
                            isexped = true;
                        }
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
                        flag = true;
                        for (int lii = 0; lii<t_ii[2].size(); lii++)
                        {
                            if (ln_r == t_ii[2][lii] && ln_c == t_jj[2][lii])
                            {
                                flag = false;
                                break;
                            }
                        }

                        if (flag == true)
                        {
                            t_ii[2].push_back(ln_r);
                            t_jj[2].push_back(ln_c);
                            isexped = true;
                        }
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
                int un_r = t_iitem[3][un]-1;
                int un_c = t_jjtem[3][un]-1+unitem;
                if (un_r >= 0 && un_r <= 2047 && un_c >= 0 && un_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP1(un_r,un_c) > 400 && m_DefImg->m_wavelengthCP1(un_r,un_c) < 490)
                    {
                        flag = 0;
                        for (int uii=0; uii<t_ii[3].size(); uii++)
                        {
                            if (un_r == t_ii[3][uii] && un_c == t_jj[3][uii])
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag == true)
                        {
                            t_ii[3].push_back(un_r);
                            t_jj[3].push_back(un_c);
                            isexped = true;
                        }
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
}


/******************************
*功能：初步寻找590波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDataProcessCtrl::find590Ori(int row, int col)
{
    int ii[16]={0};
    int jj[16]={0};
    int count = 0;
    //需要检查的9个点，可能有超出矩阵范围的，需要进行筛选
    int t_dtci[16] = {row, row, row,row, row+1, row+1, row+1, row+1, row+2, row+2, row+2, row+2, row+3, row+3, row+3, row+3};
    int t_dtcj[16] = {col, col+1, col+2,col+3, col, col+1, col+2,col+3, col, col+1, col+2, col+3, col, col+1, col+2, col+3};

    for (int i=0; i<16; i++)
    {
        if (t_dtci[i] >= 0 && t_dtci[i] <= 2047 && t_dtcj[i] >= 0 && t_dtcj[i] <= 2447)
        {
            if(m_DefImg->m_wavelengthCP1(t_dtci[i],t_dtcj[i]) > 520 && m_DefImg->m_wavelengthCP1(t_dtci[i],t_dtcj[i]) < 590)
            {
                ii[count] = t_dtci[i];
                jj[count] = t_dtcj[i];
                count++;
            }
        }

    }
    //如果满足波长条件的点>=4个则生成并保存一个波块
    if (count == 16)
    {
        qblock t_blk;
        for (int k=0; k<count; k++)
        {
            t_blk.m_iblk.push_back(ii[k]);
            t_blk.m_jblk.push_back(jj[k]);
        }
        m_590blk.push_back(t_blk);

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
void QDataProcessCtrl::find590Add(int row, int col)
{
    int nDimension = 4;
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
        else if (t_r - row == (nDimension - 1))    //在最后一行中
        {
            t_ii[1].push_back(t_r);
            t_jj[1].push_back(t_c);
        }

        if (t_c == col) //在最左边的列中
        {
            t_ii[2].push_back(t_r);
            t_jj[2].push_back(t_c);
        }
        else if (t_c - col == (nDimension - 1))    //在最右边的列中
        {
            t_ii[0].push_back(t_r);
            t_jj[0].push_back(t_c);
        }
    }
    bool flag = true;
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
                        flag = true;
                        for (int rii = 0; rii<t_ii[0].size(); rii++)
                        {
                            if (rn_r == t_ii[0][rii] && rn_c == t_jj[0][rii])
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag == true)
                        {
                            t_ii[0].push_back(rn_r);
                            t_jj[0].push_back(rn_c);
                            isexped = true;
                        }
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
                int dn_r = t_iitem[1][dn]+1;//;
                int dn_c = t_jjtem[1][dn]-1+dnitem;
                if (dn_r >= 0 && dn_r <= 2047 && dn_c >= 0 && dn_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP2(dn_r,dn_c) > 520 && m_DefImg->m_wavelengthCP2(dn_r,dn_c) < 590)
                    {
                        flag = true;
                        for (int dii = 0; dii<t_ii[1].size(); dii++)
                        {
                            if (dn_r == t_ii[1][dii] && dn_c == t_jj[1][dii])
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag == true)
                        {
                            t_ii[1].push_back(dn_r);
                            t_jj[1].push_back(dn_c);
                            isexped = true;
                        }
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
                        flag = true;
                        for (int lii = 0; lii<t_ii[2].size(); lii++)
                        {
                            if (ln_r == t_ii[2][lii] && ln_c == t_jj[2][lii])
                            {
                                flag = false;
                                break;
                            }
                        }

                        if (flag == true)
                        {
                            t_ii[2].push_back(ln_r);
                            t_jj[2].push_back(ln_c);
                            isexped = true;
                        }
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
                int un_r = t_iitem[3][un]-1;
                int un_c = t_jjtem[3][un]-1+unitem;
                if (un_r >= 0 && un_r <= 2047 && un_c >= 0 && un_c <= 2447)
                {
                    if (m_DefImg->m_wavelengthCP2(un_r,un_c) > 520 && m_DefImg->m_wavelengthCP2(un_r,un_c) < 590)
                    {
                        flag = 0;
                        for (int uii=0; uii<t_ii[3].size(); uii++)
                        {
                            if (un_r == t_ii[3][uii] && un_c == t_jj[3][uii])
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag == true)
                        {
                            t_ii[3].push_back(un_r);
                            t_jj[3].push_back(un_c);
                            isexped = true;
                        }
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
void QDataProcessCtrl::findBlock490()
{
    //波长矩阵中的非零像素点个数
    int t_ptnum = m_wavePoints.size()/2;
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
                        //将已经纳入该波块的像素点在wavelengthCP1矩阵中将波长置零
                        QVector<int> t_ii = m_490blk.last().m_iblk;
                        QVector<int> t_jj = m_490blk.last().m_jblk;
                        int t_blksize = t_ii.size();
                        for (int n=0; n<t_blksize; n++)
                        {
                            m_DefImg->m_wavelengthCP1(t_ii[n],t_jj[n]) = 0;
                        }
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
}



/******************************
*功能：找出590波块
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDataProcessCtrl::findBlock590()
{
    m_DefImg->m_wavelengthCP2 = m_DefImg->m_wavelength; //拷贝波长矩阵于CP2中，用于寻找590波块
    m_DefImg->m_wavelength.resize(0,0);
    //波长矩阵中的非零像素点个数
    int t_ptnum = m_wavePoints.size()/2;
    for (int i=0; i<t_ptnum; i++)
    {

        int t_row = m_wavePoints[2*i];
        int t_col = m_wavePoints[2*i+1];
        if (m_DefImg->m_wavelengthCP2(t_row,t_col) != 0) //如果波长拷贝矩阵中该像素点处的波长不为零（考虑到后续计算中可能将某些原来非零的点置零）
        {
            //生成该非零点周围需要探测的像素点位置(以该点为中心的9个位置)
            int t_dtci[9] = {t_row-1, t_row-1, t_row-1, t_row, t_row, t_row, t_row+1, t_row+1, t_row+1};
            int t_dtcj[9] = {t_col-1, t_col, t_col+1, t_col-1, t_col, t_col+1, t_col-1, t_col, t_col+1};
            for (int k=0; k<9; k++)
            {
                if (t_dtci[k] >= 0 && t_dtci[k] <= 2047 && t_dtcj[k] >= 0 && t_dtcj[k] <= 2447)
                {
                    if (find590Ori(t_dtci[k],t_dtcj[k]))
                    {
                        find590Add(t_dtci[k],t_dtcj[k]);
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
}



/******************************
*功能：在波长矩阵的基础上找出损伤点
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
int QDataProcessCtrl::findDefect()
{
    m_DefImg->m_wavelengthCP1.resize(0,0);
    m_DefImg->m_wavelengthCP2.resize(0,0);

    int t_490size = m_490blk.size();
    int t_590size = m_590blk.size();
    //yc
    qDebug()<<t_490size<<t_590size;
    bool t_IsFit = false;
    for (int i=0; i<t_490size; i++)
    {
        t_IsFit = false;
        for (int j=0; j<t_590size; j++)
        {
            if (IsBlksCross(m_490blk[i],m_590blk[j]))   //第一判据
            {
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
                VectorMaxMin(m_590blk[j].m_OffCenter,t_maxtem,t_mintem);
                t_index = m_590blk[j].m_OffCenter.indexOf(t_mintem);
                t_centerPtX590 = m_590blk[j].m_iblk[t_index];
                t_centerPtY590 = m_590blk[j].m_jblk[t_index];
                //get light intensity of 590--minimum value among the R, G, and B
                t_Candela590 = smaller(m_DefImg->m_R(t_centerPtX590,t_centerPtY590), m_DefImg->m_G(t_centerPtX590,t_centerPtY590), m_DefImg->m_B(t_centerPtX590,t_centerPtY590));

                //compare light intensity
                if (t_Candela490 > t_Candela590)    //第二判据
                {
                    t_IsFit = true;
                    qblock t_defect;
                    t_defect.m_iblk = m_490blk[i].m_iblk + m_590blk[j].m_iblk;
                    t_defect.m_jblk = m_490blk[i].m_jblk + m_590blk[j].m_jblk;
                    m_Defblk.push_back(t_defect);
                    CalcuBlockPara(m_Defblk.last());
//                    if (m_Defblk.last().m_dRadius < 4.8)
//                    {
//                        m_Defblk.removeLast();
//                    }
                    break;  //此处直接break，忽略了一个490块可能与多个590块相交的情况
                }

            }
            //;
        }
        if (t_IsFit == false) //不与任何590波块相交，当做小损伤直接pushback
        {
            if (m_490blk[i].m_dRadius >= 4.8)
            {
                m_Defblk.push_back(m_490blk[i]);
            }
        }
    }
    for (int i=0; i<m_Defblk.size(); i++)
    {
        qDebug()<<i<<":"<<m_Defblk[i].m_dXPos<<m_Defblk[i].m_dYPos<<m_Defblk[i].m_dRadius;
    }
    return m_Defblk.size();
}



/******************************
*功能：判断两个波块是否相交
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDataProcessCtrl::IsBlksCross(qblock blk1, qblock blk2)
{
    if (qSqrt(qPow((blk1.m_dXPos - blk2.m_dXPos),2) + qPow((blk1.m_dYPos - blk2.m_dYPos),2)) < (blk1.m_dRadius + blk2.m_dRadius))
    {
        return true;
    }
    else
    {
        return false;
    }
}


/*******************************************************************
**功能：计算流程
**输入：
**输出：
**返回值：
*******************************************************************/
bool QDataProcessCtrl::caclworkflow(QString OriImgPath, QString DefImgPath)
{
    m_OriImg = new YImage(OriImgPath);
    m_DefImg = new YImage(DefImgPath);


    subImagDenosing();
    delete m_OriImg;    //第一幅图的RGB矩阵已无用，此处delete释放内存
    m_OriImg = NULL;

    for(int i = 0 ;i < 3;i++)
    {
        caclThreshold(i); //calcu the threshhold of R/G/B
    }

    thresholdImagDenosing(1,1,1.2);
    caclSaturation();
    findBlock490();
    findBlock590();
    if (findDefect() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************
*功能：明场计算方法
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
bool QDataProcessCtrl::Defect_Luminate(QString sBeforePicPath, QString sAfterPicPath)
{
    //清理存储vector
    m_dXPos.clear();
    m_dYPos.clear();
    m_dArea.clear();

    Mat Image_Before = imread(sBeforePicPath.toStdString());
    Mat Image_After = imread(sAfterPicPath.toStdString());
    Mat Gray_Before;
    Mat Gray_After;
    cvtColor(Image_Before,Gray_Before,CV_BGR2GRAY);
    cvtColor(Image_After,Gray_After,CV_BGR2GRAY);
    Mat Gray_Delta;
    Mat Gray_Binary;
    Gray_Delta = Gray_Before - Gray_After;

    //阈值二值化
    double thresh = 13;
    threshold(Gray_Delta,Gray_Binary,thresh,0,THRESH_TOZERO);

    //查找轮廓
    vector<vector<Point>> contours;
    vector<vector<Point>> contours_clean;
    vector<Vec4i> hireachy;

    findContours(Gray_Binary,contours,hireachy,RETR_LIST,CHAIN_APPROX_SIMPLE,Point());
    for (size_t t = 0; t < contours.size();t++)
    {
        double area = contourArea(contours[t]);
        if (area > 100 && area < 2048*2048/9)
        {
            contours_clean.push_back(contours[t]);
            Moments m = moments(contours[t]);
            double cx = int(m.m10/m.m00);
            double cy = int(m.m01/m.m00);
            m_dXPos.push_back(cx);
            m_dYPos.push_back(cy);
            m_dArea.push_back(area);
        }
    }
    if (m_dArea.size() > 0)
    {return true;}
    else
    {return false;}
}

/******************************
*功能：根据rgb比值判断对应的波长
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
double QDataProcessCtrl::getWaveLength(double rgbratio, double order)
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
**功能：三个数最小值(reload)
**输入：
**输出：
**返回值：
*******************************************************************/
double QDataProcessCtrl::smaller(double x,double y,double z)
{
    double smaller=0;
    smaller=x<y?x:y;
    smaller=smaller<z?smaller:z;
    return smaller;
}

/******************************
*功能：计算波块的圆心和半径
*输入:
*输出：
*返回值：
*修改记录：
*******************************/
void QDataProcessCtrl::CalcuBlockPara(qblock &waveblock)
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
void QDataProcessCtrl::VectorMaxMin(QVector<int> datavector, double &maxvalue, double &minvalue)
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
void QDataProcessCtrl::VectorMaxMin(QVector<double> datavector, double &maxvalue, double &minvalue)
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
double QDataProcessCtrl::VectorAver(QVector<int> datavector)
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



bool QDataProcessCtrl::CalclSpotArea(double& lfarea, double& lfarea_e2)
{
    lfarea = 0;
    QFile file(QApplication::applicationDirPath() + QString("/CCDIMG/ccdimg.bmp"));
    if (!file.exists()){return false;}

    Mat image = imread((QApplication::applicationDirPath() + QString("/CCDIMG/ccdimg.bmp")).toStdString());
    Mat imageCV = image;
    //高斯去噪
    Mat ImageGuss;
    cvtColor(imageCV,ImageGuss,COLOR_RGB2GRAY);
    Mat ImageBlur;
    GaussianBlur(ImageGuss,ImageBlur,Size(15,15),0,0);


    //binarilization
    Mat ImageBinary;
    threshold(ImageBlur,ImageBinary,0,255,THRESH_OTSU);


    //查找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hireachy;

    findContours(ImageBinary,contours,hireachy,RETR_LIST,CHAIN_APPROX_SIMPLE,Point());
    int tem_nAreaContour;
    for (size_t t = 0; t < contours.size();t++)
    {
        double area = contourArea(contours[t]);
        if (area > lfarea)
        {
            lfarea = area;
            tem_nAreaContour = t;
        }
    }
//    lfarea = lfarea *169*1e-6;

    //对比度，调试度
    uint Gray = 0;
    uint GraySum = 0;
    uint GrayMax = 0;
    int PixelCnt = 0;
    for (int i=0; i<image.rows; i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if (pointPolygonTest(contours[tem_nAreaContour],Point2f(i,j),false) > 0)
            {
                PixelCnt++;
                Gray = (uint)m_phigImg->getPixelValue(i,j);
                GraySum += Gray;
                if (Gray > GrayMax)
                {
                    GrayMax = Gray;
                }
            }
        }
    }

//    lfarea_e2 = (double)GraySum/(double)GrayMax *169*1e-6;
    lfarea_e2 = (double)GraySum/(double)GrayMax;
    return true;
}


//清理资源
void QDataProcessCtrl:: clearResource()
{
    if (m_OriImg != NULL)
    {
        delete m_OriImg;
        m_OriImg = NULL;
    }
    if (m_DefImg != NULL)
    {
        delete m_DefImg;
        m_DefImg = NULL;
    }

}



/******************************
*功能:判断损伤（明场）
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDataProcessCtrl::DefectHandan(QString sBeforePicPath, QString sAfterPicPath)
{
    //清理存储vector
    m_dXPos.clear();
    m_dYPos.clear();
    m_dArea.clear();
    m_dWidth.clear();
    m_dHeight.clear();

    Mat Image_Before = imread(sBeforePicPath.toStdString());
    Mat Image_After = imread(sAfterPicPath.toStdString());
    Mat Gray_Before;
    Mat Gray_After;
    Mat rel_image = Image_After;
    cvtColor(Image_Before,Gray_Before,CV_BGR2GRAY);
    cvtColor(Image_After,Gray_After,CV_BGR2GRAY);

#ifdef PROGRESS_PIC_OUTPUT
    cv::namedWindow("Gray_before",CV_WINDOW_NORMAL);
    imshow("Gray_before",Gray_Before);
    cv::namedWindow("Gray_After",CV_WINDOW_NORMAL);
    imshow("Gray_After",Gray_After);
#endif

    Mat Gray_Delta;
    Mat Gray_Binary;
    Gray_Delta = Gray_Before - Gray_After;

#ifdef PROGRESS_PIC_OUTPUT
    cv::namedWindow("Gray_Delta",CV_WINDOW_NORMAL);
    imshow("Gray_Delta",Gray_Delta);
#endif

    //阈值二值化
    double thresh = 13;
    threshold(Gray_Delta,Gray_Binary,thresh,0,THRESH_TOZERO);
//    threshold(Gray_Delta,Gray_Binary,0,255,THRESH_OTSU);

#ifdef PROGRESS_PIC_OUTPUT
    cv::namedWindow("Gray_Binary",CV_WINDOW_NORMAL);
    imshow("Gray_Binary",Gray_Binary);
#endif

    //查找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hireachy;

    findContours(Gray_Binary,contours,hireachy,RETR_LIST,CHAIN_APPROX_SIMPLE,Point());

    QVector<double> tem_dXPos;
    QVector<double> tem_dYPos;
    QVector<double> tem_dArea;
    QVector<QRect> tem_rect;
    QVector<double> tem_rectArea;
    for (size_t t = 0; t < contours.size();t++)
    {
        double area = contourArea(contours[t]);
        if (area >= 5)
        {
            Rect tem_rec = boundingRect(contours[t]);
            QRect tem_transRect(tem_rec.x,tem_rec.y,tem_rec.width,tem_rec.height);
            tem_rect.push_back(tem_transRect);
            tem_rectArea.push_back(tem_rect.last().width() * tem_rect.last().height());
        }
        Moments m = moments(contours[t]);
        double cx = int(m.m10/m.m00);
        double cy = int(m.m01/m.m00);
        tem_dXPos.push_back(cx);
        tem_dYPos.push_back(cy);
        tem_dArea.push_back(area);
    }


    //轮廓合并
#if 0
    QVector<int> nIndex;
    sortQVector(tem_dArea,nIndex);
    vector<vector<Point>> sortcontours;
    for (int i=0; i<nIndex.size(); i++)
    {
        cout<<"sort1: "<<nIndex[i]<<endl;
        sortcontours.push_back(contours[nIndex[i]]);
    }

    vector<int> IndexVector;
    simpleContours(sortcontours,IndexVector);

    for (int i=0; i<IndexVector.size(); i++)
    {
        Rect tem_Rect = boundingRect(sortcontours[IndexVector[i]]);
//        if (tem_dArea[nIndex[IndexVector[i]]] > 25) //损伤面积判断，像元大小2um
        if (tem_Rect.width >=5 || tem_Rect.height >=5)//损伤边长判断，像元大小2um
        {
            m_dXPos.push_back(tem_dXPos[nIndex[IndexVector[i]]]);
            m_dYPos.push_back(tem_dYPos[nIndex[IndexVector[i]]]);
            m_dArea.push_back(tem_dArea[nIndex[IndexVector[i]]]);
//            drawContours(rel_image,sortcontours,static_cast<int>(IndexVector[i]),Scalar(0,0,255),2,5,hireachy);   //轮廓标记
            rectangle(rel_image,tem_Rect,Scalar(0,0,255),2);    //用矩形标记
            cout<<"draw: "<<IndexVector[i]<<endl;
        }
    }
#else
    QVector<int> nIndex;
    sortQVector(tem_rectArea,nIndex);
    QVector<QRect> sortRects;
    for (int i=0; i<nIndex.size(); i++)
    {
        sortRects.push_back(tem_rect[nIndex[i]]);
    }

    vector<int> IndexVector;
    simpleRects(sortRects,IndexVector);

    for (int i=0; i<IndexVector.size(); i++)
    {
        QRect tem_curRect = tem_rect[nIndex[IndexVector[i]]]   ;
//        Rect tem_curcvRect(tem_curRect.x(),tem_curRect.y(),tem_curRect.width(),tem_curRect.height());
        m_dXPos.push_back(tem_curRect.center().x());
        m_dYPos.push_back(tem_curRect.center().y());
        m_dArea.push_back(tem_dArea[nIndex[IndexVector[i]]]);
        m_dWidth.push_back(tem_curRect.width());
        m_dHeight.push_back(tem_curRect.height());
//        rectangle(rel_image,tem_curcvRect,Scalar(0,0,255),2);    //用矩形标记
    }
#endif

    if (m_dArea.size() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/******************************
*功能:合并opencv轮廓
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDataProcessCtrl::simpleContours(std::vector<std::vector<Point> > &contours, std::vector<int> &IndexVector)
{
    vector<int> skipIndex;
    skipIndex.clear();
    int tem_Cnt = contours.size();

    for (int i=0; i<tem_Cnt; i++)
    {
        if (!IsContainInVector(skipIndex,i))
        {
            for (int j=i+1; j<tem_Cnt; j++)
            {
                //轮廓j是否包含于i
                if (IsContoursContain(contours[j],contours[i]))
                {
                    skipIndex.push_back(j);
                }
            }
            IndexVector.push_back(i);
        }
    }
}


/******************************
*功能:查找整数是否包含在vector中
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDataProcessCtrl::IsContainInVector(std::vector<int> &Indexvector, int nIndex)
{
    int tem_Cnt = Indexvector.size();
    for (int i=0; i<tem_Cnt; i++)
    {
        if (nIndex == Indexvector[i])
        {
            return true;
        }
    }
    return false;
}



/******************************
*功能:判断轮廓1是否包含于轮廓2
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDataProcessCtrl::IsContoursContain(std::vector<Point> &contour1, std::vector<Point> &contour2)
{
    //轮廓contour1是否包含于contour2
    int tem_Cnt = contour1.size();
    for (int i=0; i<tem_Cnt; i++)
    {
        if (pointPolygonTest(contour2,Point2f(contour1[i].x,contour1[i].y),false) < 0)
        {
            return false;
        }
    }
    return true;
}

/******************************
*功能:vector排序，返回由大到小的序号
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDataProcessCtrl::sortQVector(QVector<double> tVector, QVector<int> &nIndex)
{
    nIndex.clear();
    QVector<double> ori = tVector;
    qSort(tVector.begin(),tVector.end());
    int tem_Cnt = tVector.size();
    for (int i=tem_Cnt-1; i>=0; i--)
    {
        for (int j=0; j<tem_Cnt; j++)
        {
            if (tVector[i] == ori[j])
            {
                nIndex.push_back(j);
                ori.replace(j,-100);
                break;
            }
        }
    }
}



/******************************
*功能:判断矩形轮廓rect1是否包含于矩形轮廓rect2
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDataProcessCtrl::IsRectsContain(QRect rect1, QRect rect2)
{
    double tem_dXDistance = qAbs(rect1.center().x() - rect2.center().x());
    double tem_dYDistance = qAbs(rect1.center().y() - rect2.center().y());

    if (tem_dXDistance < rect2.width()/2 && tem_dYDistance < rect2.height()/2)
    {
        return true;
    }
    else
    {
        return false;
    }
}



/******************************
*功能:合并矩形轮廓
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QDataProcessCtrl::simpleRects(QVector<QRect> &rectsVector, std::vector<int> &IndexVector)
{
    vector<int> skipIndex;
    skipIndex.clear();
    int tem_Cnt = rectsVector.size();

    for (int i=0; i<tem_Cnt; i++)
    {
        if (!IsContainInVector(skipIndex,i))
        {
            for (int j=i+1; j<tem_Cnt; j++)
            {
                if (rectsVector[j].width() >=5 || rectsVector[j].height() >= 5) //剔除尺寸小于10um的损伤
                {
                    //矩形j是否包含于i
                    if (IsRectsContain(rectsVector[j],rectsVector[i]))
                    {
                        skipIndex.push_back(j);
                        //用被包含的矩形扩充较大的矩形
                        double tem_dleft = qMin(rectsVector[j].left(),rectsVector[i].left());
                        double tem_dright = qMax(rectsVector[j].right(),rectsVector[i].right());
                        double tem_dtop = qMin(rectsVector[j].top(),rectsVector[i].top());
                        double tem_dbottom = qMax(rectsVector[j].bottom(),rectsVector[i].bottom());
                        rectsVector[i].setCoords(tem_dleft,tem_dtop,tem_dright,tem_dbottom);
                    }
                }
                else
                {
                    skipIndex.push_back(j);
                }
            }
            IndexVector.push_back(i);
        }
    }
}
