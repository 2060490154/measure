/***************************************************************************
**                                                                        **
**本文件定义了本项目中共用的结构体和宏定义，为公共资源，修改公共资源时
** 需谨慎修改
**                                                                        **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
** 1.添加Task描述信息       by lg  2018.06.19
****************************************************************************/


#ifndef DEFINE_H
#define DEFINE_H
#include <qstring.h>
#include <QList>
#include <QObject>
#include <minwindef.h>
#include <QVariant>


#define M_IDS_SAVE_IMG_PATH ("E:\\qtWorkSpace\\MingwPro\\FluxMeasureApp\\FluxMeasureApp\\image\\temp.jpg")

#define M_IDS_RUNTASK_SAVE_IMG_PATH ("E:/qtWorkSpace/MingwPro/FluxMeasureApp/FluxMeasureApp/IDSIMAGE/")

#define M_KXCCD_SAVE_IMAGE_PATH ("E:\\qtWorkSpace\\MingwPro\\FluxMeasureApp\\FluxMeasureApp\\CCDIMG\\ccdimg.bmp")

#define DATA_SAVE_PATH ("I:/FluxMeasureData/")

#define M_WAITFOR_TIME 6000 //1min
#define M_WAITFOR_STEP_TIME 65000//1min

#define M_WAITFOR_IDS_CAP_IMAG_TIME 2000

#define M_MOTOR_CTRLDEV_NUM 3//电机控制器个数
#define M_PRE_MOTOR_NUM  4//每个电机控制器控制的电机数量

const int M_MOTOR_TOTALNUM =  (M_MOTOR_CTRLDEV_NUM*M_PRE_MOTOR_NUM -2);//电机总数量

#define M_OSCI_CHANNEL_NUM     4

#define M_SHUTTER_TOTAL_NUM 2

#define M_SHUTTER_PORT  23

#define M_ENERGYMETER_DEV_NUM 2


const int M_SIZETYPE_RECT  = 1;//矩形
const int M_SIZETYPE_CIRCL  = 2;//圆形


//配置文件

//能量计配置信息
typedef struct  _tEnergyMeterConfigInfo
{
    int nDevIndex;
    QString  sChannelName;
    float lfCoeff;
    QString sSerialNUmber;
}tEnergyMeterConfigInfo;

typedef struct  _tOsciiChannel
{
    QString m_sChannelName;
    int m_nChannelNum;

}tOsciiChannel;

typedef struct  _tDevConfigParam
{
    QString m_sKxccdIPAddress;
    int m_nKxccdPort;

    QString m_sOSCIIIPAddress;
    QList<tOsciiChannel> m_tOSCIIVaildChannelList;

    QStringList m_tShutterDevIPList;
    int m_nShutterPort;

    QList<tEnergyMeterConfigInfo*>m_tEnergyConfigInfoList;

}tDevConfigParam;


typedef struct _Coeff
{
    float m_lfK;
    float m_lfB;
}tCoeff;

typedef struct  _tPlatformConfig
{

    int m_nCurrentWaveLength;//0-1053nm
    tCoeff m_st_Coeff[3];

}tPlatformConfig;


typedef struct  _QPointInfo
{
//    int nxPos;
//    int nyPos;
    //2020yi
    double dxPos;
    double dyPos;
    int nStatus;//0--未打 1--已打  2--跳过
    bool bSelect;
}QPointInfo;


typedef struct  _ExpInfo
{
    QString  m_sExpNo;//实验编号
    int m_nEnergySection;//当前点所在能量段，用于类似1on1的流程中
    int m_nEnergySecNum;//单点所打的能量段总数，用于类似Ron1的流程中
    int m_nTimesPerPoint;//单点发次数，用于Son1流程中
    int m_nTimes;//发次
    int m_nPointNo;//当前点号
    int m_nMeasureType;//测量模式
    bool m_bAutoTest;//true为auto  false为manual
}tExpInfo;
Q_DECLARE_METATYPE(tExpInfo)

class QWorkTask
{
public:
    QString m_sStepName;//测试步骤名称
    int    m_nStepStatus;//测试步骤状态 0--未执行  1--正在执行 2---执行成功 3---执行失败
    int    m_nSetpFailReason;//失败原因
    int    m_nDevType;//设备类型
    int    m_nDevNo;//设备编号  同一个类型可能有多个设备编号  对于电机而言 设备编号为0x00 00 高8位为控制器编号 低8位为设备编号
    int    m_nAction;//需要执行的动作
    float    m_nParam;//动作参数
    int   m_nParam1;//动作参数1

    bool m_bWaitForLastStep;//是否等待上一步执行完成
    bool m_bNeedUserCheck;//是否需要用户确认


    //附加信息  用于显示 和 数据库写入使用
    tExpInfo m_tExpInfo;



public:
    QWorkTask(QString sName,int nDevType,int nDevNo,int nAction,float nParam = 0,int nParam1 = 0,bool bWaitForLastStep = false,tExpInfo*expInfo = NULL, bool bNeedUserCheck = false)
    {
        m_nStepStatus = 0;
        m_sStepName = sName;
        m_nDevType = nDevType;
        m_nDevNo = nDevNo;
        m_nAction = nAction;
        m_nParam = nParam;
        m_nParam1 = nParam1;

        m_bWaitForLastStep = bWaitForLastStep;

        m_bNeedUserCheck = bNeedUserCheck;
       // m_bWaitForLastStep = true;

        if(expInfo != NULL)
        {
            m_tExpInfo.m_sExpNo = expInfo->m_sExpNo;
            m_tExpInfo.m_nTimes = expInfo->m_nTimes;
            m_tExpInfo.m_nPointNo = expInfo->m_nPointNo;
            m_tExpInfo.m_nMeasureType  = expInfo->m_nMeasureType;
            m_tExpInfo.m_bAutoTest = expInfo->m_bAutoTest;
            m_tExpInfo.m_nEnergySection = expInfo->m_nEnergySection;
        }
    }
};

class QWorkFlowItem
{
public:
    QList<QWorkTask> m_pMoveWorkFlow;
    QList<QWorkTask> m_pSetWorkFlow;
    QList<QWorkTask> m_pCapWorkFlow;
	QList<QWorkTask> m_pDataProcessWorkFlow;
    int m_nSOn1ShotCnt;
    int m_nRon1ShotCnt;
};

enum devType {
    M_DATA_PROCESS=0,//数据处理
    M_DEV_MOTOR = 1,//电机控制
    M_DEV_IDS = 2,//显微探视
    M_DEV_ENERGY =3,//能量计
    M_DEV_OSCII =4,//示波器
    M_DEV_SHUTTER=5,//光闸
    M_DEV_KXCCD = 6
};

enum MotorAction {
    M_MOTOR_ZERO = 1,//归零
    M_MOTOR_MOVESTEP = 2,//按照步数运动
    M_MOTOR_ROLL =3,//按照速度转动 应该用不到这个模式
};

enum MotorMovePosRelation {
    M_MOVE_ABS = 1,//绝对位置
    M_MOVE_RELATIVE = 0//相对位置
};

enum IDSAction {
    M_SET_TRIGGER = 1,//设置触发模式
    M_CAP_IMAG = 2,//采集图像
    M_NORECAP_IMAG = 3,
    M_SEND_PLAN = 4,//A0
    M_SEND_INFO = 5,//A1
    M_GET_DATA = 6,//A2
    M_START_DEAL = 7,//A3
    M_END_DEAL = 8,//A4
    M_COM=9
};//yhy

enum EnergyMeterAction {
    M_SET_PARAM = 1,//设置参数
    M_CAP_DATA = 2//采集数据
};

enum OsciAction {
    M_SET_OSCI_PARAM = 1,//设置参数
    M_CAP_OSCI_DATA = 2//采集波形
};

enum shutterAction {
    M_CLOSE_SHUTTER = 1,//吸合
    M_OPEN_SHUTTER = 2,//打开
};

enum DataProcessAction{
    M_IMG_1ON1 = 1,
    M_IMG_RON1 = 2,
    M_IMG_SON1 = 3,
    M_IMG_RASTERSCAN = 4,
    M_CALC_FLUX = 5,
    M_CALC_AREA = 6
};

enum _TASKSTATUS {
    M_STEP_STATUS_UNRUN = 0,//未执行
    M_STEP_STATUS_RUNNING = 1,//正在执行
    M_STEP_STATUS_FAILD = 2,//执行失败
    M_STEP_STATUS_SUCCESS = 3,//执行成功
    M_STEP_STATUS_HASDEFECTS = 4,//有损伤（用于图像处理流程中）
    M_STEP_STATUS_SKIP = 5  //跳过（用于Ron1流程）
};


enum _MEASURETYPE {
    M_MEASURETYPE_STANDBY = 0,
    M_MEASURETYPE_1On1 = 1,
    M_MEASURETYPE_SOn1 =2,
    M_MEASURETYPE_ROn1 =3,
    M_MEASURETYPE_RASTER = 4,
    M_MEASURETYPE_MANUAL = 5
};

enum wavelength {
    M_WAVE_LEN_1064 = 0,
    M_WAVE_LEN_532 = 1,
    M_WAVE_LEN_355 =2
};

enum pointStatus {
    M_STATUS_UNUSE = 0,//未打过
    M_STATUS_USED = 1,//已打过
    M_STATUS_DISABLE =2,//跳过点
     M_STATUS_DESTROY =3,//损坏点

};

enum _MOTORDEFINE {
    M_MOTOR_0_0 = 0x0000,//驱动器0 编号0
    M_MOTOR_0_1 = 0x0001,//驱动器0 编号1
    M_MOTOR_0_2 = 0x0002,//驱动器0 编号2
    M_MOTOR_0_3 = 0x0003,//驱动器0 编号3

    M_MOTOR_1_0 = 0x0100,//驱动器1 编号0
    M_MOTOR_1_1 = 0x0101,//驱动器1 编号1
    M_MOTOR_1_2 = 0x0102,//驱动器1 编号2
    M_MOTOR_1_3 = 0x0103,//驱动器1 编号3


    M_MOTOR_2_0 = 0x0200,//驱动器2 编号0
    M_MOTOR_2_1 = 0x0201,//驱动器2 编号1
    M_MOTOR_2_2 = 0x0202,//驱动器2 编号3
    M_MOTOR_2_3 = 0x0203//驱动器2 编号4
//    M_MOTOR_X = 0x0200,//驱动器2 编号0
//    M_MOTOR_Y = 0x0201,//驱动器2 编号1
//    M_MOTOR_Z =0x0203,//驱动器2 编号3
//     M_MOTOR_4 =0x0204//驱动器2 编号4
};

enum _SHUTTERDEFINE {
    M_SHUTTER_0 =  0,
    M_SHUTTER_1=  1,
    M_SHUTTER_2 = 2,
    M_SHUTTER_3=  3
};



struct workflowParam
{
    //实验信息
    QString m_sExpNumber;
    QString m_sSampleName;
    //元器件尺寸信息
    //2020dong
    double m_dWidth;
    double m_dHigh;
    double m_dThick;
    double m_dRaidus;
    int m_nSizeType;
    //2020yi
    double m_dStep;//运动步长 测量步长

    //能量相关设置
    int m_nWaveLengthType;
    int m_n1On1PointNum;//1On1模式下 一个能量打多少发
    double m_lfEnergyIncream;//能量阶梯
    double m_lfMinimumEnergy;//最小能量
    double m_lfMaximumEnergy;//最大能量
    int m_nEnergySectionNum;//能量段数
    int m_nCnt;//1On1为每个能量所打的点数 Son1为每个点单个能量打靶的次数
    double m_dManualArea;

    double m_dMinDeg;
    double m_dMaxDeg;


};




//全局变量资源
//所有设备的配置信息

//电机档位信息
typedef struct  _tMotorPostionInfo
{
    QString  sPostionName;//位置名称
    float lfPostionNum;//位置距离 绝对距离 离零点位置  单位为mm
}tMotorPostionInfo;

//电机数据库配置信息
typedef struct  _tMotorConfigInfo
{
    int nMotorCtrlIndex;
    int nMotorIndex;
    QString  sMotorName;
    int nMotorCoeff;//对于平移台是1mm对应的步数  对于轮盘为1度对应的度数
    int nMortorType;

    QList<tMotorPostionInfo> tPostionConfig;
}tMotorConfigInfo;



typedef struct  _tAllDevConfigInfo
{
    QList<tMotorConfigInfo> m_tMotorConfigInfoList;

}tAllDevConfigInfo;

typedef struct  _tMotorTypeCoeff
{
    int m_nMotorType;
    int m_nMotorCoeff;

}tMotorTypeCoeff;


typedef struct _motorStatusdata
{
    LONG m_nCurrentSpeed;			// 当前速度
    LONG m_nCurrentAcc;				//当前加速度
    LONG m_nCurrentLPV;				// 当前逻辑计数   应该是步数计数
    LONG m_nCurrentFV;				// 当前实际计数器
    UINT  m_nRightLimitStatus;//右限位状态
    UINT  m_nLeftLimitStatus;//左限位状态

    UINT m_nRunState;//0--未运动  1--正在运动 2--到达运动点
} motorStatusdata;
Q_DECLARE_METATYPE(motorStatusdata)

typedef struct _ExpInfo4Show
{
    int ExpType;
    QString sExpNo;
    QString SampleSize;
    QString SampleName;
    QString DataTime;
}ExpInfo4Show;
Q_DECLARE_METATYPE(ExpInfo4Show)


typedef struct _AllExpData4show
{
    int m_nPointNo; //当前点号
    int m_nShotNo;     //当前发次号
    double m_dEnergy;
    double m_dFlux;
}AllExpData4show;
Q_DECLARE_METATYPE(AllExpData4show)



typedef struct _DevInfo4Show
{
    QString DevName;
    QString DevType;
    QString DevNo;
    QString DevCertiNo;
    QString DevComment;
}DevInfo4Show;
Q_DECLARE_METATYPE(DevInfo4Show)

typedef struct _ClientInfo
{
    QString CName;
    QString CAddress;
    QString CPhone;
}ClientInfo;
Q_DECLARE_METATYPE(ClientInfo)




#endif // DEFINE_H
