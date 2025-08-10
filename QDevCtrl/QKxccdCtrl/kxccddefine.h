#ifndef _KXCCD_DEFINE_H
#define _KXCCD_DEFINE_H
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/qhostaddress.h>
#include <qpoint.h>

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;

#define M_SEARCH_DEV_SEND_PORT    4001
#define M_SEARCH_DEV_RCV_PORT  5153
#define M_KXCCD_WAITFOR_TIME  1000 //ms
#define M_DATA_HEADER_LENTH  24
#define M_DATA_ROW_HEADER_LENTH  8

enum CapType
{
    M_CAPTYPE_NONE = 0,
    M_CAPTYPE_SYN_IN_ACTIVE = 1,//连续内同步
    M_CAPTYPE_SYN_OUT_ACTIVE = 2,//连续外同步
    M_CAPTYPE_SYN_OUT_NATIVE = 3//帧存模式
};


enum eTriggerMode
{
    M_TRIGGER_NONE = 0,
    M_TRIGGER_SYN_IN = 1,//内触发
    M_TRIGGER_SYN_OUT = 2//外触发
};


enum eTranslateMode
{
     M_TRANS_NONE = 0,
    M_TRANS_ACTIVE = 1,//主动
    M_TRANS_NATIVE = 2  //被动
};

enum _Enum_Cmd
{
    M_CMD_CAP_START =0x01,
    M_CMD_CAP_CLOSE =0x02,
    M_CMD_CAP_ONE = 0x03,
    M_CMD_LINK_DEV_OPEN = 0x04,
    M_CMD_LINK_DEV_CLOSE = 0x05,
    M_CMD_SAVE_PARAM = 0x06,//保存参数

    M_CMD_SET_EXPOSURE = 0x09,//设置曝光时间
    M_CMD_SET_GAIN = 0x07,//设置增益
    M_CMD_SET_OFFSET = 0x08,//设置偏置

    M_CMD_GET_EXPOSURE = 0x09,//查询曝光时间
    M_CMD_GET_GAIN = 0x07,//查询增益
    M_CMD_GET_OFFSET = 0x08,//查询偏置

    M_CMD_DEV_SYN_MODE = 0x0a, //设置同步方式 0为内同步  1为外同步
    M_CMD_DEV_TRANSLATE_MODE = 0xd,//设置传输模式 1为被动 0为主动

    M_CMD_DEV_MODIFY_IP = 0x0e,
    M_CMD_RECAP_TRANS_IMG_HEADER = 0x20,//发送图片头信息
    M_CMD_RECAP_TRANS_DATA = 0x21 //发送图片数据信息
};


enum CAMERASTATUS  //像机工作状态
{
    M_CAMERA_DISCONNECT = 0,//未连接
    M_CAMERA_CONNECT = 1,//已连接
    M_CAMERA_START_CAP = 2,//处于捕获状态
    M_CAMERA_STOP_CAP = 3,//停止捕获
    M_CAMERA_START_RECAP = 4//处于重传状态
};


struct DevSocket
{
    QHostAddress m_tHostIP;
    QString m_sDevIP;
     int m_nInitPort;

    //命令
    int m_nDevCmdPort;//命令发送/接收端口
    QUdpSocket* m_pCmdSocket;

    //数据
    int m_nDevDataPort;//数据发送/接收端口
    QUdpSocket* m_pDataSocket;

};

//参数设置
typedef struct _ParamInfo
{
    QString sParamName;//参数名称
    int nMaxValue;//最大值
    int nMinValue;//最小值
    int nCurrentValue;//当前设定值

    int nCmdVaule;//命令字

    bool bSetOk;//是否设置成功
}tParamInfo;

#pragma pack(1)
//控制命令

union data
{
    int m_ndata;
    BYTE m_data[4];
};

typedef struct
{
    union data m_tCmdData;
    BYTE m_response;//0：始发 1：反馈 buf[4]
    BYTE m_cmd;//命令					buf[5]
    WORD m_id;//命令ID			    buf[6]

}PACK_CMD;



typedef struct _DataHeaderInfo
{
    WORD   wStartFlag;        //0xAFAF
    int    nRevDataLen;//图像数据长度  需要后续接收的长度
    WORD   wRow;
    WORD   wCol;//列宽
    BYTE   bBit; //0x01 8位数据 0x02 16为数据
    BYTE   bOffset; // 每行数据起始地址的偏移量
    WORD   wFrameCount; //帧计数 0
    WORD   wRowDataCnt; //每个行数据包长度 为了解决数据包长度超过MTU时 出现的问题
    BYTE   bReserve[8];   //保留
}tDataHeaderInfo;

typedef struct _RowDataHeaderInfo
{
    WORD   wStartFlag;  //0xBFBF
    WORD   wFrameCount; //帧计数 0
    WORD   wRowCount;    //行计数 0-(row -1)
    BYTE   bRowflag;    //0x01 代表前半行 0x02 代表前半行
    BYTE   bReserve;   //保留
}tRowDataHeaderInfo;

//接收到的图像数据结构
typedef struct _RcvKxccdData
{
    tDataHeaderInfo m_tRcvDataHeaderInfo;
    tRowDataHeaderInfo m_tRcvRowHeaderInfo;

    int m_nRevLen;//剩余长度
    uchar* m_pImageData;//图像数据
    int m_nDataLen;

    int m_nInvaildRowNum;
    bool m_bImageStart;//是否接收到起始数据

    bool m_bImageReCapOK;//重传结束


}tRcvKxccdData;

#pragma pack()


enum Line_type{
    E_PAINT_NON = 0,
    E_PAINT_LINE_X = 1,
    E_PAINT_LINE_Y = 2,
    E_PAINT_RECT = 3
};

#define M_HIG_RAWNOTHING_LENGTH  328680

/*****************************************************************************/
/*                             图像帧头信息                                  */
/*****************************************************************************/

#pragma pack(2)

typedef struct  //高灰度图像格式(*.HIG)文件头结构
{
    int		nType;			// 文件类型('HiG')
    int		nWidth;			// 物理宽度值
    int		nHeight;		// 物理高度值
    int		nBits;			// 图像数据A/D位数
    int		nColor;			// 图像数据格式(0--灰度，1--彩色)

    char	pName[32];		// 图像名
    char	pDate[32];		// 建立日期
    char	pTime[32];		// 建立时间
    char	pNote[256];		// 说明信息

    WORD	pParam[256];	// 摄像机参数区
    DWORD	nCount;			// 帧计数码.

    int		nBitsDisp;		// 图像显示的A/D位数
    int		nByteGraph;		// 附加图形的字节数

    double	dDPM;			// 几何单位(毫米/像素).
    double	dGamma;			// 伽玛校正系数.

    int		nGrayStart;		// 窗位.
    int		nGrayWidth;		// 窗宽.

    int		Reserved[32];	// 预留

} HIG_FILEHEADER;

typedef struct
{
    unsigned short w;       // 图像宽
    unsigned short h;       // 图像高

    unsigned short bits;      // 图像位数 8~16bits
    unsigned short format;      // 图像类型代码(0--灰度，1--Bayer_RG，2--Bayer_GR，3--Bayer_BG，5--RGB，6--YUV422，7--JPEG)

    unsigned short frame_type; // 帧类型(0--普通，1--抓拍图像，2--心跳帧)
    unsigned short frame_rev;  // 保留

    unsigned long  firmware_version; //固件程序版本
    unsigned long  device_no; // 设备编号

    unsigned long  len;     // 图像数据长度

    unsigned long  speed;   // 双线圈测速值(us)
    unsigned long  rs232;   // 串口信息(1~4字节)

    unsigned short year; // 图像采集时间
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;

    unsigned long ip;		// 采集当前帧的摄像机IP
    unsigned long frame_count;	//总共的抓拍帧数目
    unsigned long trigger_count; //总共的触发数
    unsigned long trigger_index; //触发组索引
    unsigned long frame_no; //帧号

    unsigned long gain; //当前抓拍参数
    unsigned long time; //曝光时间
    unsigned long gain_r; //红增益
    unsigned long gain_g; //绿增益
    unsigned long gain_b; //蓝增益

    unsigned long mode;  // 摄像机工作模式
    unsigned long JpegQ; // JPEG压缩品质
    unsigned long td1;   // 抓拍延时(单位us)
    unsigned long td2;   // 曝光延时(单位us)

    unsigned long rev[12]; //保留参数

    unsigned char user_info[64]; // 用户数据

} FRAME_HEADER; // 帧头


typedef struct _MeasureData //测量时计算出的参数
{
    WORD* pPixelData;//像素值
    WORD* pStatData;//直方图值
    int nStatDataLen;
    int nPixelDataLen;

    int nMaxvalue;
    int nMinvalue;
    double  lfMeanvalue;
    double lfRms;
    int nHeight;
    int nWidth;
    float lfm;
    float lfc;
    QPoint tStartPoint;
    QPoint tEndPoint;

} MeasureData;

#pragma pack()




#endif // DEFINE_H
