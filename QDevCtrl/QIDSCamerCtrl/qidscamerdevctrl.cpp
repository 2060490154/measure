/***************************************************************************
**                                                                                               **
**QIDSCamerDevCtrl为显微设备控制类                                     **
** 提供的功能函数为：
** InitCamera                     初始化相机参数
** OpenCamera                  打开相机
** CloseCamera                  关闭相机
** InitDisplayMode            初始化显示模式
** GetMaxImageSize          获取最大的图像数据规格
** GetiplImgFormMem      从内存中读取图像数据
** SaveImage                     保存图片
** SetTriggerMode            设置触发模式
** StartCapImage               开始捕获图像
** StopCapImage              停止捕获图像
** RunIDSTask                   执行显微相机任务
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
**创建时间：2018.05.21
**修改记录：
**
****************************************************************************/
#include "qidscamerdevctrl.h"
#include <QtConcurrent/QtConcurrent>
#include <QPixmap>
#include <QApplication>
#include "qudpcs.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QtSql>
#include <QMessageBox>
QIDSCamerDevCtrl::QIDSCamerDevCtrl()
{
     m_pcImageMemory = NULL;

     m_lMemoryId = 0;
     m_hCam = 0;   //初始化相机句柄为0
     m_bHasNewImage = false;//是否存在新图像
     m_bRunTimeCap = false;//是否为实时采集
     m_pTimer = new QTimer();


     connect(m_pTimer, SIGNAL(timeout()), this, SLOT(GetiplImgFormMem()));

     m_pTimer->start(150);

}

QIDSCamerDevCtrl::~QIDSCamerDevCtrl()
{
    m_pTimer->stop();
    CloseCamera();
}

//初始化相机
int QIDSCamerDevCtrl::InitCamera()
{
    int nRet = is_InitCamera (&m_hCam, NULL);
    if (nRet == IS_STARTER_FW_UPLOAD_NEEDED)//相机的启动程序固件和驱动不兼容,需要更新固件版本
    {

        //相机的启动程序固件和驱动不兼容,需要更新固件版本

//        INT nUploadTime = 25000; //默认更新时间为25S
//        is_GetDuration (*hCam, IS_STARTER_FW_UPLOAD, &nUploadTime);

//        printf("This camera requires a new firmware !\n");
//        printf("The upload will take about %f seconds. Please wait ...\n",nUploadTime/1000);

//        //再次打开相机并自动更新固件
//        *hCam = (HIDS) (((INT)*hCam) | IS_ALLOW_STARTER_FW_UPLOAD);
//        nRet = is_InitCamera (hCam, NULL);
    }

    return nRet;

}

//打开相机
bool QIDSCamerDevCtrl::OpenCamera()
{
    int nRet = IS_NO_SUCCESS;
    bool bRel = false;

    if( m_hCam != 0)
    {
        return true;
    }

    CloseCamera();
    nRet = InitCamera();  //初始化相机

    if (nRet == IS_SUCCESS)
    {
        is_GetSensorInfo(m_hCam, &m_sInfo); // 查询相机所用传感器的类型
        GetMaxImageSize(&m_nSizeX, &m_nSizeY);
        nRet = InitDisplayMode();             //选择显示模式（位图）

        if (nRet == IS_SUCCESS)
        {
            //不接受消息
            is_EnableMessage(m_hCam, IS_DEVICE_REMOVED, NULL);
            is_EnableMessage(m_hCam, IS_DEVICE_RECONNECTED, NULL);
            is_EnableMessage(m_hCam, IS_FRAME, NULL);
//            is_CaptureVideo( m_hCam, IS_WAIT );   //设定捕捉模式：自由运行模式下的实时模式
        }

    }

    if(nRet == IS_SUCCESS)
    {
        bRel = true;
    }

    //设置曝光
    double tem_nExposureTime = 11;
    if (SetExposureMode(tem_nExposureTime))
    {
        bRel = true;
    }
    else
    {
        bRel = false;
    }

    return bRel;
}

//关闭相机
void QIDSCamerDevCtrl::CloseCamera()
{
    if( m_hCam != 0)
    {
        m_bRunTimeCap = false;
        Sleep(200);
        is_EnableMessage( m_hCam, IS_FRAME, NULL );
        is_StopLiveVideo( m_hCam, IS_WAIT );
        if( m_pcImageMemory != NULL )
        {
            is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
            m_pcImageMemory = NULL;
        }

        is_ExitCamera( m_hCam );
        m_hCam = 0;
    }
}

//初始化显示模式为DIB模式
int QIDSCamerDevCtrl::InitDisplayMode()
{
    int nRet = IS_NO_SUCCESS;

    if (m_hCam == 0)
    {
        return IS_NO_SUCCESS;
    }


    if (m_pcImageMemory != NULL) //释放通过 is_AllocImageMem() 函数分配的图像内存
    {
        is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
    }
    m_pcImageMemory = NULL;

    // 设置位图模式
    nRet = is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);

    if (m_sInfo.nColorMode == IS_COLORMODE_BAYER)
    {
        is_GetColorDepth(m_hCam, &m_nBitsPerPixel, &m_nColorMode);
    }
    else if (m_sInfo.nColorMode == IS_COLORMODE_CBYCRY)
    {
        m_nColorMode = IS_CM_BGRA8_PACKED;
        m_nBitsPerPixel = 32;
    }
    else
    {
        m_nColorMode = IS_CM_MONO8;
        m_nBitsPerPixel = 8;
    }

    //by lg modify
    m_nColorMode = IS_CM_BGRA8_PACKED;
    m_nBitsPerPixel = 32;

    // 分配图像内存，图像尺寸有 m_nSizeX和m_nSizeY确定，色彩位深由m_nBitsPerPixel确定，m_pcImageMemory返回起始地址，m_lMemoryId 返回已分配内存的ID
    if (is_AllocImageMem(m_hCam, m_nSizeX, m_nSizeY, m_nBitsPerPixel, &m_pcImageMemory, &m_lMemoryId ) != IS_SUCCESS)
    {
            return IS_NO_SUCCESS;
    }
    else
        is_SetImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );


    if (nRet == IS_SUCCESS)
    {
        // 设置显卡在保存或显示图像数据时所使用的色彩模
        is_SetColorMode(m_hCam, m_nColorMode);

        // set the image size to capture
        IS_SIZE_2D imageSize;
        imageSize.s32Width = m_nSizeX;
        imageSize.s32Height = m_nSizeY;
       //设置图像感兴趣区域（AOI）的大小和位置
        is_AOI(m_hCam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
    }

    return nRet;
}

//获取当前支持的最大规格
void QIDSCamerDevCtrl::GetMaxImageSize(int *pnSizeX, int *pnSizeY)
{
    int nAOISupported = 0;
    bool bAOISupported = true;
    if (is_ImageFormat(m_hCam,IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,(void*)&nAOISupported, sizeof(nAOISupported)) == IS_SUCCESS)
    {
        bAOISupported = (nAOISupported != 0);
    }

    if (bAOISupported)
    {
        SENSORINFO sInfo;
        is_GetSensorInfo (m_hCam, &sInfo);
        *pnSizeX = sInfo.nMaxWidth;
        *pnSizeY = sInfo.nMaxHeight;
    }
    else
    {
        IS_SIZE_2D imageSize;
        is_AOI(m_hCam, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

        *pnSizeX = imageSize.s32Width;
        *pnSizeY = imageSize.s32Height;
    }
}

//实时获取图像时需要不断刷新此函数 bRunTime--是否为实时采集
bool QIDSCamerDevCtrl::GetiplImgFormMem()
{
    char *pLast = NULL, *pMem = NULL;
    int nNum = 0;

    if(m_hCam == 0)
    {
        return false;
    }

  //确定当前用于捕捉图像的图像内存 pMem,最后一个用于捕捉图像的图像内存pLast

    m_bHasNewImage = false;

    if(is_GetActSeqBuf (m_hCam, &nNum, &pMem, &pLast) == IS_SUCCESS)
    {
        m_bHasNewImage = true;
        m_pImageBuff = pLast;



        //需要实时显示的时候
        if(m_bRunTimeCap == true)
        {

            //by lg
            SaveImage(QApplication::applicationDirPath() + QString("/image/ImgShow.jpg"));
            if (QFile(QApplication::applicationDirPath() + QString("/image/ImgShow.jpg")).exists())
            {
                //m_pPixmap->load(M_IDS_SAVE_IMG_PATH);
                emit signal_showImageBuff(QApplication::applicationDirPath() + QString("/image/ImgShow.jpg"));
            }

        }

    }

     return true;
}


//保存图片
int QIDSCamerDevCtrl::SaveImage(QString FilePath)
{

//    IMAGE_FILE_PARAMS ImageFileParams;
    int nRet = 0;

//    ImageFileParams.pnImageID = NULL;
//    ImageFileParams.ppcImageMem = NULL;

//    //保存活动内存中的jpeg图像，图像画质为100
//    ImageFileParams.pwchFileName = (wchar_t*)FilePath.data();
//    ImageFileParams.nFileType = IS_IMG_JPG; //待保存文件的类型
//    ImageFileParams.nQuality = 100;  //100为最佳图像画质（无压缩)
//    nRet = is_ImageFile(m_hCam, IS_IMAGE_FILE_CMD_SAVE, (void*)&ImageFileParams,sizeof(ImageFileParams));

//    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QString("connection"));
//    QString dsn = QString::fromLocal8Bit("QtLocalDSN");
//    db.setDatabaseName(dsn);
//    db.setHostName("local");  //数据库主机名
//    db.setPort(1433);
//    db.setUserName("sa");
//    db.setPassword("123456");
//    if(!db.open())
//    {
//        qDebug("=== %s",qPrintable(db.lastError().text()));
//        QMessageBox msgBox;

//        msgBox.setWindowTitle("連接失败");

//        msgBox.setText("!db.open()");
//        msgBox.setStandardButtons(QMessageBox::Ok);

//        msgBox.exec();

//    }else
//    {
//        QString sSql = QString("select top 1 * from T_DEFECT_SYSTEM_DATA order by time desc");
//        QSqlQuery sqlQuery(db);
//        bool bOk = sqlQuery.exec(sSql);
//        if (bOk==true)
//        {

//        }
//        else
//        {
//            QMessageBox msgBox;

//            msgBox.setWindowTitle("連接失敗");

//            msgBox.setText(sqlQuery.lastError().text());
//            msgBox.setStandardButtons(QMessageBox::Ok);

//            msgBox.exec();
//        }
//        while(sqlQuery.next())
//        {
//            QByteArray l_byteArray;
//            l_byteArray = sqlQuery.value(0).toByteArray();
//            if(l_byteArray.size()>0)
//            {
//                QFile l_file(QString("%1").arg(FilePath));
//                l_file.open(QFile::ReadWrite);
//                l_file.write(l_byteArray);
//                l_file.close();
//            }
//        }
//    }







    return nRet;

}

//设置触发模式
bool QIDSCamerDevCtrl::SetTriggerMode(int nMode)
{
    if(m_hCam == 0)
    {
        return false;
    }
    if(nMode == M_EXTERN_HI)
    {
        is_SetExternalTrigger(m_hCam,IS_SET_TRIGGER_LO_HI);//硬件上升沿触发
    }

    if(nMode == M_INNER_VIDEO)
    {
        is_SetExternalTrigger(m_hCam,IS_SET_TRIGGER_SOFTWARE);//软件触发 目前只提供软件连续触发 单次触发没意义
        m_bHasNewImage = false;
    }

    m_nCurrentTriggerMode = nMode;

    return true;
}


/******************************
*功能:设置曝光模式
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QIDSCamerDevCtrl::SetExposureMode(double fExposureTime)
{
    int nRet = is_Exposure(m_hCam,IS_EXPOSURE_CMD_SET_EXPOSURE ,(void*)&fExposureTime,sizeof(double));
    if (nRet == IS_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//开始捕获图像
bool QIDSCamerDevCtrl::StartCapImage(bool bRunTime)
{
    if(m_hCam == 0)
    {
        return false;
    }
    if(m_nCurrentTriggerMode == M_EXTERN_HI)
    {
        is_FreezeVideo(m_hCam,IS_DONT_WAIT);//单次采集
    }

    if(m_nCurrentTriggerMode == M_INNER_VIDEO)
    {
        is_CaptureVideo(m_hCam,IS_DONT_WAIT);//软件触发 连续采集    IS_WAIT IS_DONT_WAIT

        m_bHasNewImage = false;
    }

    m_bRunTimeCap = bRunTime;

    return true;
}

//停止捕获
void QIDSCamerDevCtrl::StopCapImage()
{
    m_bRunTimeCap = false;
}


//显示用
bool QIDSCamerDevCtrl::getDevStatus()
{
    if (m_hCam == 0 )
    {
        return false;
    }
    else
        return true;
}

