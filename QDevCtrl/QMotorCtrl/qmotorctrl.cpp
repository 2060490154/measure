/***************************************************************************
**                                                                                               **
**QMotorCtrl为电机设备控制类                                                 **
** 提供的功能函数为：
** OpenDev                              打开设备
** CloseDev                              关闭控制箱
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qmotorctrl.h"
#include <QDebug>
QMotorCtrl::QMotorCtrl( int nDevIndex,QObject *parent) : QObject(parent)
{
    _hDevice = NULL;
    m_bStopRunTask = false;
    _DevIndex = nDevIndex;

    m_pTimer = new QTimer();

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(ProcessMotorCmd()));

    m_pTimer->start(100);
}

QMotorCtrl::~QMotorCtrl()
{

    m_pTimer->stop();

    CloseDev();
}

//打开设备
bool QMotorCtrl::OpenDev()
{
    if(_hDevice != NULL)
    {
        return true;
    }

    HANDLE hDevice =  USB1020_CreateDevice(_DevIndex);

    if(hDevice != NULL && hDevice != INVALID_HANDLE_VALUE)
    {
       _hDevice = hDevice;

       for(int i = 0; i < M_PRE_MOTOR_NUM;i++)
       {
           InitMotorParam(i);
       }

       return true;
    }


    return false;
}
//关闭设备
bool QMotorCtrl::CloseDev()
{
    if(_hDevice != NULL)
    {
         return USB1020_ReleaseDevice(_hDevice);
         _hDevice = NULL;
    }
    return true;
}

//初始化参数
bool QMotorCtrl::InitMotorParam(int nMotorIndex,int nLvDv)
{
    if(_hDevice == NULL)
    {
        return false;
    }

    m_DataList[nMotorIndex].Multiple = 200;     // 倍率 (1~500)//yhy0315
    m_DataList[nMotorIndex].StartSpeed = 500;		// 初始速度(1~8000)
    m_DataList[nMotorIndex].DriveSpeed = 4000;		// 驱动速度(1~8000)
    m_DataList[nMotorIndex].Acceleration = 300;		// 加速度(125~1000000)
    m_DataList[nMotorIndex].Deceleration = 300;		// 减速度(125~1000000)
    m_DataList[nMotorIndex].AccIncRate = 1000;		// 加速度变化率(954~62500000)
    m_DataList[nMotorIndex].DecIncRate = 1000;		// 减速度变化率(954~62500000)


    m_LCData[nMotorIndex].AxisNum = nMotorIndex;		// 轴号 (X轴 | Y轴 | X、Y轴)
    m_LCData[nMotorIndex].LV_DV = nLvDv;					// 驱动方式  (连续 | 定长 )
    m_LCData[nMotorIndex].DecMode = USB1020_AUTO;				// 减速方式  (自动减速 | 手动减速)
	m_LCData[nMotorIndex].PulseMode = USB1020_CPDIR;				// 脉冲方式 (CW/CCW方式 | CP/DIR方式)
    m_LCData[nMotorIndex].PLSLogLever = 0;			// 设定驱动脉冲的方向（默认正方向）
    m_LCData[nMotorIndex].DIRLogLever = 0;			// 设定方向信号输出的逻辑电平（0：低电平为正向，1：高电平为正向）
    m_LCData[nMotorIndex].Line_Curve = USB1020_LINE;			// 运动方式	(直线 | 曲线)
    m_LCData[nMotorIndex].Direction = USB1020_PDIRECTION;				// 运动方向 (正方向 | 反方向)
    m_LCData[nMotorIndex].nPulseNum = 0;			// 定量输出脉冲数(0~268435455)


    USB1020_SetLP(_hDevice, nMotorIndex, 0); // 逻辑位置计数器清零
    USB1020_SetEP(_hDevice, nMotorIndex, 0); // 实位计数器清零

    // 初始化当前轴
    if(!USB1020_InitLVDV(_hDevice, &m_DataList[nMotorIndex],&m_LCData[nMotorIndex]))
    {
       return false;//初始化失败
    }

    return true;
}

//运动到绝对位置 nPosition为离最左端的距离 操作步骤为先运动到最左端 再向右运动nPosition距离
bool QMotorCtrl::MoveToAbsPosition(int nMotorIndex, float nPosition)
{
    bool bMoveRight = false;
    int nStartTime = GetTickCount();
    int nEndTime = nStartTime;
    m_bStopRunTask = false;

    if(!OpenDev())
    {
        m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_FAILD;
       return false;
    }

    m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_RUNNING;//置正在运动


    //归零
    this->AddMotorCmd(new QWorkTask("运动到最右端",M_DEV_MOTOR,nMotorIndex,M_MOTOR_ZERO));//归零

    while(nEndTime -nStartTime < M_WAITFOR_STEP_TIME  && m_bStopRunTask == false )
    {
        GetMotorStatus(nMotorIndex);
        nEndTime =  GetTickCount();
        if (m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 0)
        {
            if(m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed == 0 && m_tMotorStatusdata[nMotorIndex].m_nRightLimitStatus == 1 )//到达右限位
            {
                this->AddMotorCmd(new QWorkTask("向左运动到指定位置",M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,nPosition));
                bMoveRight = true;
            }
            if(m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed == 0 &&bMoveRight == true && abs(abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV) - nPosition*m_pMotorTypeCoeff[nMotorIndex].m_nMotorCoeff)<6000)   // && abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentFV - nPosition)<100  //&& m_tMotorStatusdata[nMotorIndex].m_nCurrentAcc == 0
            {
                m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_SUCCESS;//到达
                break;
            }
        }
        else if (m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 1)
        {
            if(m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed == 0 && m_tMotorStatusdata[nMotorIndex].m_nLeftLimitStatus == 1 )//到达左限位
            {
                this->AddMotorCmd(new QWorkTask("向左运动到指定位置",M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,nPosition));
                bMoveRight = true;
            }
            if(m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed == 0 &&bMoveRight == true && abs(abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV) - nPosition*m_pMotorTypeCoeff[nMotorIndex].m_nMotorCoeff)<12000)   // && abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentFV - nPosition)<100  //&& m_tMotorStatusdata[nMotorIndex].m_nCurrentAcc == 0
            {
                m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_SUCCESS;//到达
                break;
            }
        }
        else
        {
            return false;
        }


    }

    if(nEndTime -nStartTime > M_WAITFOR_STEP_TIME)
    {
        //监控失败原因
        qDebug()<<nMotorIndex<<"====="<<m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed<<m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV<<abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV - nPosition);
        m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_FAILD;//超时
    }
    return true;
}


//运动到指定的相对位置 输入 电机ID 位置信息(脉冲数 需要转换) 脉冲数为正 则正向  为负数则为反方向  相对位置运动
bool QMotorCtrl::MoveToPosition(int nMotorIndex,float nPosition,int nRate)
{
    if(_hDevice == NULL)
    {
        return false;
    }

    USB1020_SetR(_hDevice,nMotorIndex,nRate);

    //ytry 怎么与手动调试模块的运动方向保持一致
#if 0
    if(nPosition > 0)
    {
        m_LCData[nMotorIndex].Direction = USB1020_PDIRECTION;				// 运动方向 (正方向 | 反方向)
    }
    else
        m_LCData[nMotorIndex].Direction = USB1020_MDIRECTION;				// 运动方向 (正方向 | 反方向)
    //;
#else
    if(nPosition > 0)
    {
        if(m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 0)
        {
            m_LCData[nMotorIndex].Direction = USB1020_MDIRECTION;				// 运动方向：反向
        }
        else if (m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 1)              //运动方向：正向
        {
            m_LCData[nMotorIndex].Direction = USB1020_MDIRECTION;
        }
    }
    else
    {

        if(m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 0)
        {
            m_LCData[nMotorIndex].Direction = USB1020_PDIRECTION;				// 运动方向：反向
        }
        else if (m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 1)              //运动方向：正向
        {
            m_LCData[nMotorIndex].Direction = USB1020_PDIRECTION;
        }
    }

#endif

    m_LCData[nMotorIndex].nPulseNum = abs(nPosition*m_pMotorTypeCoeff[nMotorIndex].m_nMotorCoeff);			// 定量输出脉冲数(0~268435455)
    // 初始化当前轴
    if(!USB1020_InitLVDV(_hDevice, &m_DataList[nMotorIndex],&m_LCData[nMotorIndex]))
    {
       return false;//初始化失败
    }

    //	启动单轴
    if(!USB1020_StartLVDV(_hDevice,nMotorIndex))
    {
        return false;
    }

    return true;
}


//清空逻辑计数
bool QMotorCtrl::clearLP(int nMotorIndex)
{
    USB1020_SetLP(_hDevice, nMotorIndex, 0);
    return true;
}

//获取逻辑计数
bool QMotorCtrl::readLP(int nMotorIndex)
{
    m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV = USB1020_ReadLP(_hDevice, nMotorIndex);// 读取逻辑计数器
    return true;
}



//获取当前电机的状态
bool QMotorCtrl::GetMotorStatus(int nMotorIndex)
{
    if(_hDevice == NULL)
    {
        return false;
    }

    USB1020_PARA_RR2 tRR2Param;

    m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed = USB1020_ReadCV(_hDevice, nMotorIndex);
    m_tMotorStatusdata[nMotorIndex].m_nCurrentAcc = USB1020_ReadCA(_hDevice, nMotorIndex);
    m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV = USB1020_ReadLP(_hDevice, nMotorIndex);// 读取逻辑计数器
    m_tMotorStatusdata[nMotorIndex].m_nCurrentFV = USB1020_ReadEP(_hDevice, nMotorIndex);// 读取实位计数器

    USB1020_GetRR2Status(_hDevice,nMotorIndex,&tRR2Param);
    m_tMotorStatusdata[nMotorIndex].m_nRightLimitStatus = tRR2Param.HLMTP;//右限位状态
    m_tMotorStatusdata[nMotorIndex].m_nLeftLimitStatus = tRR2Param.HLMTM;//左限位状态
//    qDebug()<<m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV;
    //分别判断平移台和旋转台的限位状态
    if (m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 0)
    {
        if (m_tMotorStatusdata[nMotorIndex].m_nRightLimitStatus == 1 && m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed == 0)
        {
            USB1020_SetLP(_hDevice,nMotorIndex,0);
            m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV = USB1020_ReadLP(_hDevice, nMotorIndex);// 读取逻辑计数器
        }
    }
    else if (m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 1)
    {
        if (m_tMotorStatusdata[nMotorIndex].m_nLeftLimitStatus == 1 && m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed == 0)
        {
            USB1020_SetLP(_hDevice,nMotorIndex,0);
            m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV = USB1020_ReadLP(_hDevice, nMotorIndex);// 读取逻辑计数器
        }
    }
    else
    {
        return false;
    }

    QVariant var;
    var.setValue(m_tMotorStatusdata[nMotorIndex]);
    emit UpdateStatus(_DevIndex,nMotorIndex,var);
    return true;
}

//获取RR0状态信息 全局只有一个RR0寄存器 可以获取驱动器是否在驱动
bool QMotorCtrl::GetRR0Status()
{
    if(_hDevice == NULL)
    {
        return false;
    }

    return USB1020_GetRR0Status(_hDevice,&m_tParamRR0Data);
}

//立即停止
bool QMotorCtrl::ImmediateStop(int nMotorIndex)
{
    if(_hDevice == NULL)
    {
        return false;
    }

    m_bStopRunTask = true;
    Sleep(100);
    return USB1020_InstStop(_hDevice, nMotorIndex);
}

//显示用
bool QMotorCtrl::getDevStatus()
{
    if(_hDevice == NULL)
    {
        return false;
    }
    else
        return true;
}




/*******************************************************************
**功能：执行电机任务---运动到相对位置
**输入：pTask为任务信息
**输出：运动状态
**返回值：
*******************************************************************/
bool QMotorCtrl::MoveToRelativePosition(int nMotorIndex,float nPosition)
{
        int nStartTime = GetTickCount();
        int nEndTime = nStartTime;
        m_bStopRunTask = false;

        if(!OpenDev())
        {
            m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_FAILD;
           return false;
        }

        m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_RUNNING;//置正在运动

        //运动到指定位置
        LONG t_LastLPV;
        t_LastLPV = m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV;
        this->AddMotorCmd(new QWorkTask("向右运动到指定位置",M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,nPosition));//运动到指定位置
        Sleep(1);//yhy





        while(nEndTime -nStartTime < M_WAITFOR_STEP_TIME  && m_bStopRunTask == false )
        {


            if (nPosition == -10)
            {
//                qDebug()<<"nPosition: "<<nPosition<<"Before LPV: "<<t_LastLPV<<"Current LPV: "<<m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV<<"Delta LPV: "<<abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV - t_LastLPV)<<"Ideal LPVDelta: "<<abs(nPosition*m_pMotorTypeCoeff[nMotorIndex].m_nMotorCoeff);
            }
            nEndTime =  GetTickCount();

            if(m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed == 0 && abs(abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV - t_LastLPV) - abs(nPosition*m_pMotorTypeCoeff[nMotorIndex].m_nMotorCoeff))<4000)
            {
                m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_SUCCESS;//到达
                break;
            }
        }

        if(nEndTime -nStartTime > M_WAITFOR_STEP_TIME)
        {
            qDebug()<<nMotorIndex<<"====="<<m_tMotorStatusdata[nMotorIndex].m_nCurrentSpeed<<m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV<<t_LastLPV<<abs(abs(m_tMotorStatusdata[nMotorIndex].m_nCurrentLPV - t_LastLPV) - nPosition*m_pMotorTypeCoeff[nMotorIndex].m_nMotorCoeff);
              m_tMotorStatusdata[nMotorIndex].m_nRunState = M_STEP_STATUS_FAILD;//超时
        }
        return true;
}

//添加所要执行的命令
void QMotorCtrl::AddMotorCmd(QWorkTask* pNewTask )
{
    if(pNewTask != NULL)
    {
        m_pTaskList.append(pNewTask);
    }
}

//线程函数 读取状态
void QMotorCtrl::ProcessMotorCmd()
{
    bool bRel = false;

    if(_hDevice == NULL)
    {
        return;
    }

    for(int i = 0; i < m_pTaskList.size();i++)
    {
        if(m_pTaskList[i]->m_nAction == M_MOTOR_ZERO)
        {
            SearchHome(m_pTaskList[i]->m_nDevNo);
            m_pTaskList[i]->m_nDevNo = -1;
        }
        else
        {
            if (m_pMotorTypeCoeff[m_pTaskList[i]->m_nDevNo].m_nMotorType == 0)
            {
                bRel = MoveToPosition(m_pTaskList[i]->m_nDevNo,m_pTaskList[i]->m_nParam);
            }
            else
            {
                bRel = MoveToPosition(m_pTaskList[i]->m_nDevNo,m_pTaskList[i]->m_nParam*(-1));
            }

            if(bRel == true)
            {
                m_pTaskList[i]->m_nDevNo = -1;
            }
        }

        Sleep(20);

    }


    //清除已经执行过的命令
    for(int i = m_pTaskList.size() -1 ; i >=0;i--)
    {
        if(m_pTaskList[i]->m_nDevNo == -1)
        {
            m_pTaskList.removeAt(i);
        }
    }

    for(int i = 0; i < M_PRE_MOTOR_NUM;i++)
    {
        GetMotorStatus(i);
    }
    GetRR0Status();
}
/*******************************************************************
**功能：归零操作
**输入：
**输出：
**返回值：
*******************************************************************/
 bool QMotorCtrl::SearchHome(int nMotorIndex)
 {
#if 0
     USB1020_SetR(_hDevice,nMotorIndex,70);

     USB1020_PARA_AutoHomeSearch Para;

     if(_hDevice == NULL)
     {
         return false;
     }

     USB1020_SetInEnable(_hDevice,nMotorIndex,0,1);//nMotorIndex

     Para.ST1E = 1;
     Para.ST1D = 0;
     Para.LIMIT = 1;
     Para.PCLR = 1;
     USB1020_SetAutoHomeSearch(_hDevice,nMotorIndex,&Para);

     USB1020_SetHV(_hDevice,nMotorIndex,1000);
     USB1020_SetDec(_hDevice,nMotorIndex,1000);
     USB1020_SetA(_hDevice,nMotorIndex,125);
     USB1020_SetAccIncRate(_hDevice,nMotorIndex,954);

     return USB1020_StartAutoHomeSearch(_hDevice,nMotorIndex);
#else
     int nPos = 0;
     if(m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 0)   //平移台
     {
         nPos = -160;
     }
     else if (m_pMotorTypeCoeff[nMotorIndex].m_nMotorType == 1) //旋转台
     {
         nPos = 361;
     }
     else
     {
         return false;
     }
     MoveToPosition(nMotorIndex,nPos);
     return true;
#endif
#if 0
    return   MoveToPosition(nMotorIndex,nMaxPos,100);
#endif

 }

 /*******************************************************************
**功能：驱动器是否已连接
**输入：
**输出：
**返回值：
*******************************************************************/
 bool QMotorCtrl::checkIsConnect()
 {
     if(_hDevice == NULL)
     {
         return false;
     }
     return true;
 }
