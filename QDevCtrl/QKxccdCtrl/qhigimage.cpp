/**************************************************
*文件说明:hig图像处理类
*提供的功能:
*initDataBuf    初始化buff数据
*clearDataBuff  清理buff资源
*setHiGrayTab   设置映射数据
*processHig2Bmp hig转为bmp
*saveHigImage 保存hig图像到本地
*getPixelValue 得到灰度数据
*维护记录:
*2019-01-25  create by lg
******************************************************/
#include "qhigimage.h"
#include <QFile>
#include <qmessagebox.h>
#include <qdatetime.h>

QHigImage::QHigImage(QObject *parent) : QObject(parent)
{
    m_pHigImage = NULL;
    m_pHeaderInfo = NULL;
    m_pHigData = NULL;
    m_pHig2BmpData = NULL;
    m_pBmpData = NULL;

    for (int i = 0; i < 256; i++)
    {
        m_vcolorTable.append(qRgb(i, i, i));
    }

    memset(&m_tMeasureData,0,sizeof(MeasureData));
    m_tMeasureData.pPixelData = NULL;
    m_tMeasureData.pStatData = NULL;
    setHiGrayTab(0,((1<<12) -1),16);//默认为12bit

}





/*************************************************************************************************************************************
@功能：初始化数据
@ 输入：nRow 图像数据行数（高） nCol为图像数据的宽，nBitLen为图像数据的位宽(8bit  12bit),pImageBuff为图像数据
***************************************************************************************************************************************/
void QHigImage::initDataBuf(int nRow,int nCol,int nBitLen,char* pImagBuff)
{
    static unsigned int nOldDataLen = 0;
    unsigned int nDataLen = nRow*nCol;
    if (nOldDataLen != nDataLen)
    {
        nOldDataLen = nDataLen;//避免频繁分配内存

        clearDataBuff();

        m_pHigImage = new WORD[sizeof(FRAME_HEADER)/2+nDataLen];//长度为头信息+数据长度

        m_pHeaderInfo = (FRAME_HEADER*)m_pHigImage;

        m_pHigData = (WORD*)(m_pHigImage+sizeof(FRAME_HEADER));
    }

    if (nBitLen >8)
    {
        m_pHeaderInfo->len = nDataLen*2;
    }
    else
    {
        m_pHeaderInfo->len = nDataLen;
    }

    m_pHeaderInfo->bits = nBitLen;//默认给定为12bit
    m_pHeaderInfo->w = nCol;
    m_pHeaderInfo->h = nRow;

    if (pImagBuff != NULL)
    {
        memcpy((char*)m_pHigData,pImagBuff ,m_pHeaderInfo->len);
    }


}

/**************************************************
@功能：清除数据指针
***************************************************/
void QHigImage::clearDataBuff()
{
    m_pHeaderInfo = NULL;
    m_pHigData = NULL;

    if (m_pHigImage != NULL)
    {
        delete[] m_pHigImage;
        m_pHigImage = NULL;
    }


}

/********************************************************
 *
 * 加载HIG图像
 * **********************************************************/
void QHigImage::loadFile(QString sFilePath)
{
    HIG_FILEHEADER stImageHeader;
    unsigned int nFileLen = 0;//文件大小


    QFile file(sFilePath);
    file.open(QIODevice::ReadOnly);
    nFileLen = file.size();

    if(nFileLen >sizeof(HIG_FILEHEADER))
    {
        file.read((char*)&stImageHeader,sizeof(HIG_FILEHEADER));

        initDataBuf(stImageHeader.nHeight,stImageHeader.nWidth,stImageHeader.nBits,NULL);

        file.seek(nFileLen-m_pHeaderInfo->len);
        //int ndatasize = file.read((char*)m_pHigData,m_pHeaderInfo->len);//增加判断 ndatasize的长度与实际要求长度是否一致
        file.read((char*)m_pHigData,m_pHeaderInfo->len);
    }


}


/*************************************************************************
@功能：生成当前窗宽和窗位下 灰度映射到0-255区间的对应关系数据
***************************************************************************/
void QHigImage::setHiGrayTab(int sg, int wg,int nBit)
{
    double lfMax = 256.0;
    int nMaxGray = 1 << nBit;
    double Gr=0;
    double kf=0.0;
    double kg=0.0;

    if(m_pHig2BmpData != NULL)
    {
        delete[] m_pHig2BmpData;
    }

    m_pHig2BmpData = new uchar[nMaxGray+1];

    if(wg < 1) wg = 1;
    if(wg > nMaxGray) wg = nMaxGray;

    if(sg < 0) sg = 0;
    if(sg > nMaxGray - wg) sg = nMaxGray-wg;

    kf = lfMax/wg;
    kg = kf * sg;


    for(int i=0; i <= nMaxGray; i++)
    {
        if(i < sg)
        {
            m_pHig2BmpData[i] = 0;
        }
        else if(i > sg+wg-1)
        {
            m_pHig2BmpData[i] = 255;
        }
        else
        {
            Gr = i*kf-kg;
            if( Gr < 0) Gr = 0;
            if(Gr > 255.0f) Gr = 255.0f;
            m_pHig2BmpData[i] = (uchar)Gr;
        }
    }
     m_pHig2BmpData[nMaxGray-1] = 255;
}


/****************************************************
 *将HIG数据转换为BMP图像
 *
 * *****************************************************/
void QHigImage::processHig2Bmp()
{
    WORD nMaxGray = (1<<m_pHeaderInfo->bits) - 1;

    if(m_pBmpData != NULL)
    {
        delete[] m_pBmpData;
    }
    m_pBmpData = new uchar[m_pHeaderInfo->w*m_pHeaderInfo->h];


    for(int i=0; i < m_pHeaderInfo->w*m_pHeaderInfo->h; i++)
    {
        if(m_pHigData[i] <= nMaxGray)
        {
            m_pBmpData[i] = m_pHig2BmpData[m_pHigData[i]];
        }

    }
}


/****************************************************
 *将HIG内存数据保存为hig文件
 *nBitLen---可选择保存数据的bit位数
 * *****************************************************/
bool QHigImage::saveHigImage(QString sFileName,int nBitLen)
{
    QFile file(sFileName);
    file.open(QIODevice::WriteOnly);

    HIG_FILEHEADER tHigFileHeader;
    uchar* pbuf = new uchar[M_HIG_RAWNOTHING_LENGTH];
    WORD *pTempdata = new WORD[m_pHeaderInfo->h*m_pHeaderInfo->w];


    memset(&tHigFileHeader,0x00,sizeof(HIG_FILEHEADER));//文件头
    memset(pbuf,0x00,M_HIG_RAWNOTHING_LENGTH);//兼容信息
    memcpy((uchar*)pTempdata,(uchar*)m_pHigData,2*m_pHeaderInfo->w*m_pHeaderInfo->h);//数据信息


    if(nBitLen == 0)
    {
        nBitLen = m_pHeaderInfo->bits;
    }

    //文件头信息
    tHigFileHeader.nType = 'HiG';
    tHigFileHeader.nHeight = m_pHeaderInfo->h;
    tHigFileHeader.nWidth = m_pHeaderInfo->w;
    tHigFileHeader.nBits = m_pHeaderInfo->bits;//pheader->bits;

    QDateTime currentTime = QDateTime::currentDateTime();

    QString sTime = currentTime.toString("yyyy-MM-dd-hh-mm-ss-zzz");
    memcpy(tHigFileHeader.pDate,sTime.data(),32);

    sTime = currentTime.toString("hh-mm-ss-zzz");
    memcpy(tHigFileHeader.pTime,sTime.data(),32);

    file.write((char*)&tHigFileHeader,sizeof(HIG_FILEHEADER));

    //兼容信息
    file.write((char*)pbuf,M_HIG_RAWNOTHING_LENGTH);



    //数据信息
    if(nBitLen ==12)
    {
        for (int i = 0; i < m_pHeaderInfo->h*m_pHeaderInfo->w;i++)
        {
            pTempdata[i] = m_pHigData[i]&0xFFF;
        }
        file.write((char*)pTempdata,m_pHeaderInfo->w*m_pHeaderInfo->h*2);
    }
    if(nBitLen ==16)
    {
        file.write((char*)pTempdata,m_pHeaderInfo->w*m_pHeaderInfo->h*2);
    }

    if(nBitLen == 8)
    {
        for (int i = 0; i < m_pHeaderInfo->h*m_pHeaderInfo->w;i++)
        {
            pTempdata[i] = m_pHigData[i]&0xFF;
        }
        file.write((char*)pTempdata,m_pHeaderInfo->w*m_pHeaderInfo->h);
    }
    file.close();


    delete[] pbuf;
    delete[] pTempdata;
    pbuf = NULL;
    pTempdata = NULL;

    return true;
}

/****************************************************
 *保存bmp图像
 * *****************************************************/
bool QHigImage::saveBmpImage(QString sFileName)
{
    if(m_pBmpData == NULL)
    {
        return false;
    }

    QImage l_bmpimage(m_pBmpData, m_pHeaderInfo->w, m_pHeaderInfo->h, QImage::Format_Indexed8);  //封装QImage
    l_bmpimage.setColorTable(m_vcolorTable); //设置颜色表
    l_bmpimage = l_bmpimage.mirrored(false,true);

    return l_bmpimage.save(sFileName);
}





/****************************************************
 *获得当前坐标下的灰度值 图片旋转了180度
 * *****************************************************/
WORD QHigImage::getPixelValue(int nPosx,int nPosy)
{

    if(m_pHeaderInfo == NULL)
    {
        return 0;
    }



    if(nPosx >= m_pHeaderInfo->w || nPosx < 0 || nPosy >= m_pHeaderInfo->h || nPosy < 0)
    {
        return 0;//超限
    }

    return m_pHigData[(m_pHeaderInfo->h-nPosy)*m_pHeaderInfo->w + nPosx];

}

/****************************************************
 *获得范围内的灰度数据 返回值为当前数据长度 数据内容
 * *****************************************************/
int QHigImage::getAreaData(QPoint tStartPoint,QPoint tEndPoint)
{

    int minx = qMin (tStartPoint.x(), tEndPoint.x());
    int maxx = qMax (tStartPoint.x(), tEndPoint.x());
    int miny = qMin (tStartPoint.y(), tEndPoint.y());
    int maxy = qMax (tStartPoint.y(), tEndPoint.y());

    m_tMeasureData.nHeight = maxy - miny + 1;
    m_tMeasureData.nWidth = maxx - minx + 1;

    int nDataLen = m_tMeasureData.nHeight*m_tMeasureData.nWidth;

    //清除之前的数据
    if(m_tMeasureData.pPixelData != NULL)
    {
        delete[] m_tMeasureData.pPixelData;
        m_tMeasureData.pPixelData = NULL;
    }

    m_tMeasureData.pPixelData =  new WORD[m_tMeasureData.nHeight*m_tMeasureData.nWidth];
    memset(m_tMeasureData.pPixelData,0,nDataLen*sizeof(WORD));

    for(int nRow = miny; nRow < maxy + 1; ++nRow)
    {
        for(int nCol = minx; nCol < maxx + 1; ++nCol)
        {
            m_tMeasureData.pPixelData[(nRow-miny)*m_tMeasureData.nWidth + (nCol-minx)] = getPixelValue(nCol,nRow);
        }
    }

    return nDataLen;

}
/****************************************************
 *计算测量参数
 * *****************************************************/
void QHigImage::calMeasureValue(int nDataLen)
{
    double sqmean = 0.0 ;
    double sqsum = 0.0;
    WORD maxvalue = 0;
    WORD minvalue = m_tMeasureData.pPixelData[0];
    int  sum = 0;
    int nMaxPixel = 1<<m_pHeaderInfo->bits;

    m_tMeasureData.nPixelDataLen = nDataLen;
    m_tMeasureData.nStatDataLen = nMaxPixel;


    if(m_tMeasureData.pStatData != NULL)
    {
        delete[] m_tMeasureData.pStatData;
        m_tMeasureData.pStatData = NULL;
    }

    m_tMeasureData.pStatData = new WORD[nMaxPixel];
    memset(m_tMeasureData.pStatData,0,nMaxPixel*sizeof(WORD));

    for(int i = 0;i < nDataLen;++i)
    {
        maxvalue = qMax(maxvalue,m_tMeasureData.pPixelData[i]);
        minvalue = qMin(minvalue,m_tMeasureData.pPixelData[i]);
        sum += m_tMeasureData.pPixelData[i];

        m_tMeasureData.pStatData[m_tMeasureData.pPixelData[i]]++;
    }

     m_tMeasureData.nMaxvalue = maxvalue;
     m_tMeasureData.nMinvalue = minvalue;

    if(nDataLen > 0)
    {
        m_tMeasureData.lfMeanvalue = (double)sum/(double)nDataLen;
    }
    else
    {
        m_tMeasureData.lfMeanvalue = 0.0;
    }


    for(int j = 0;j < nDataLen; j++)
    {

        sqsum += (m_tMeasureData.pPixelData[j] - m_tMeasureData.lfMeanvalue)*(m_tMeasureData.pPixelData[j] - m_tMeasureData.lfMeanvalue);
    }

    if(nDataLen > 0)
    {
        sqmean = sqsum/nDataLen;
        m_tMeasureData.lfRms = sqrt(sqmean);
    }
    else
    {
        m_tMeasureData.lfRms = 0.0;
    }



    if(m_tMeasureData.lfMeanvalue > 1e-12)
    {
        m_tMeasureData.lfm = (double)m_tMeasureData.nMaxvalue/m_tMeasureData.lfMeanvalue;
        m_tMeasureData.lfc = (double)m_tMeasureData.lfRms/m_tMeasureData.lfMeanvalue;
    }
    else
    {
       m_tMeasureData.lfm = 0.0;
       m_tMeasureData.lfc = 0.0;
    }
}
