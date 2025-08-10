#ifndef QIDSCAMERDEVCTRL_H
#define QIDSCAMERDEVCTRL_H
#include "uEye.h"
#include <qthread.h>
#include <QTimer>
#include "define.h"
enum TRIGGER_MODE
{

    M_EXTERN_HI = 1,//外触发 上升沿
    M_INNER_VIDEO = 2,//连续内触发  软触发模式

};

class QIDSCamerDevCtrl:public QObject
{

        Q_OBJECT
public:
    QIDSCamerDevCtrl();
    ~QIDSCamerDevCtrl();

signals:
//    void signal_showImageBuff(char* pbuff,int nwidth,int nhigh);
    void signal_showImageBuff(QString sfilePath);

public slots:
    bool GetiplImgFormMem(); //从视频数据流中将图像数据拷贝给IplImage

public:
    int InitCamera();
    bool OpenCamera();
    void CloseCamera();
    int  InitDisplayMode();
    void GetMaxImageSize(int *pnSizeX, int *pnSizeY);//查询相机支持的图像格式
    int SaveImage(QString FilePath);
    bool SetTriggerMode(int nMode);
    bool SetExposureMode(double fExposureTime);
    bool StartCapImage(bool bRunTime);
    void StopCapImage();

    bool getDevStatus();



 public:
    HIDS    m_hCam;             // 相机句柄
    HWND    m_hWndDisplay;       // window显示句柄   BITMAP不需要
    INT     m_nColorMode;       // Y8/RGB16/RGB24/REG32
    INT     m_nBitsPerPixel;    // 图像位深
    INT     m_nSizeX;       // 图像宽度
    INT     m_nSizeY;       // 图像高度
    char *m_pImageBuff;
    SENSORINFO m_sInfo;     // sensor information struct

    int m_nCurrentTriggerMode;//当前触发模式

    // 使用位图模式进行实时显示需要的内存
    int  m_lMemoryId;           // camera memory - buffer ID
    char*   m_pcImageMemory;    // camera memory - pointer to buffer

    bool m_bRunTimeCap;

    QTimer* m_pTimer;
    bool m_bHasNewImage;


};

#endif // QIDSCAMERDEVCTRL_H
