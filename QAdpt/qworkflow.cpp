/***************************************************************************
**                                                                                               **
**QWorkFlowThread为任务创建线程，负责任务的生成和执行    **
** 提供的功能函数为：
** clearWorkFlow                      清除测试流程
** CreatStandbyWorkFlow         创建准备流程
** create1On1WorkFlow           创建1On 1测试流程
** CreateSOn1WorkFlow           创建SOn 1测试流程
** CreateROn1WorkFlow           创建ROn 1测试流程
** ProcessMoveWorkFlow         执行运动流程
** ProcessSetWorkFlow             执行测量设备设置流程
** ProcessCapWorkFlow            执行采集流程
** ProcessDataWorkFlow           执行数据处理流程
** ProcessStandByFlow              执行准备流程
** waitForStepRel                       等待步骤执行结果
** waitForWorkflowRel              等待流程执行结果
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qworkflow.h"
#include <QDebug>
#include <windows.h>
#include <QMessageBox>

QWorkFlowThread::QWorkFlowThread(QDevManagerCtrl* pCtrl)
{
    _pDevManagerCtrl = pCtrl;
    m_bImmediateStop = false;
    m_bSuspend = false;
    m_bManualAlter = false;
}

QWorkFlowThread::~QWorkFlowThread()
{
    clearWorkFlow(&m_pStandbyFlow);
    clearWorkFlow(&m_pManualWorkFlowlist);
    clearWorkFlow(&m_pWorkFlowList);
    clearWorkFlow(&m_pAverAreaWorkFlowlist);
    clearWorkFlow(&m_pAverEnergyMeterWorkFlowlist);
}
/*************************************************************************
功能：清理资源链表
输入：链表指针
输出:无
返回值：无
****************************************************************************/
void QWorkFlowThread::clearWorkFlow(QList<QWorkFlowItem*> * pItem)
{
        qDeleteAll(*pItem);
        pItem->clear();
}

/*************************************************************************
功能：初始化准备流程
输入：nWavelength  波长选择 0---1064nm  1---532nm  2----351nm
输出: 准备流程
返回值：无
****************************************************************************/
void QWorkFlowThread::CreatStandbyWorkFlow(int nWavelength)
{
    clearWorkFlow(&m_pStandbyFlow);

    QWorkFlowItem* pItem = new QWorkFlowItem();
    pItem->m_pMoveWorkFlow.append(QWorkTask("关闭基頻光闸", M_DEV_SHUTTER, M_SHUTTER_0, M_CLOSE_SHUTTER,0,0,true,NULL,true));
    pItem->m_pMoveWorkFlow.append(QWorkTask("关闭三倍頻光闸", M_DEV_SHUTTER, M_SHUTTER_1, M_CLOSE_SHUTTER,0,0,true,NULL,true));
	if (nWavelength == 0)//1064nm
	{
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为1064nm，移动M1-2平移台", M_DEV_MOTOR, M_MOTOR_0_1, M_MOTOR_MOVESTEP,110,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为1064nm，移动M1-1平移台", M_DEV_MOTOR, M_MOTOR_0_0, M_MOTOR_MOVESTEP,74.4,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为1064nm，移动M2-1平移台", M_DEV_MOTOR, M_MOTOR_1_0, M_MOTOR_MOVESTEP,6,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为1064nm，调节能量调节器", M_DEV_IDS,0,M_COM,0,3,true,0));//应该是1这里ywc
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为1064nm，移动M3-2旋转台", M_DEV_MOTOR, M_MOTOR_2_1, M_MOTOR_MOVESTEP,20.5,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为1064nm，移动M3-1样品台x轴",M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为1064nm，移动M3-4样品台y轴",M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,107,M_MOVE_ABS,true));
	}
	if (nWavelength == 1)//532nm
	{
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，移动M1-1平移台", M_DEV_MOTOR, M_MOTOR_0_0, M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，移动M1-2平移台", M_DEV_MOTOR, M_MOTOR_0_1, M_MOTOR_MOVESTEP,115.5,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，移动M2-1平移台", M_DEV_MOTOR, M_MOTOR_1_0, M_MOTOR_MOVESTEP,150,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，移动M3-2旋转台", M_DEV_MOTOR, M_MOTOR_2_1, M_MOTOR_MOVESTEP,254.8,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，移动M3-1样品台x轴",M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，移动M3-4样品台y轴",M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,107,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，移动M2-3波片旋转台",M_DEV_MOTOR,M_MOTOR_1_2,M_MOTOR_MOVESTEP,60,M_MOVE_ABS,true));//ywc
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为532nm，调节能量调节器",M_DEV_IDS,0,M_COM,0,2,true,0));

	}
	if (nWavelength == 2)//355nm
	{
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，移动M1-1平移台", M_DEV_MOTOR, M_MOTOR_0_0, M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，移动M1-2平移台", M_DEV_MOTOR, M_MOTOR_0_1, M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，移动M2-1平移台", M_DEV_MOTOR, M_MOTOR_1_0, M_MOTOR_MOVESTEP,60,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，移动M3-2旋转台", M_DEV_MOTOR, M_MOTOR_2_1, M_MOTOR_MOVESTEP,145.25,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，移动M3-1样品台x轴",M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,105,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，移动M3-4样品台y轴",M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
//        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，移动M2-2波片旋转台",M_DEV_MOTOR,M_MOTOR_1_1,M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
        pItem->m_pMoveWorkFlow.append(QWorkTask("工作波长为355nm，调节能量调节器",M_DEV_IDS,0,M_COM,0,3,true,0));

	}
    
//    pItem->m_pSetWorkFlow.append(QWorkTask("设置显微监视工作模式为触发",M_DEV_IDS,0,M_SET_TRIGGER,2));//M_INNER_VIDEO = 2连续内触发
    pItem->m_pSetWorkFlow.append(QWorkTask("设置CCD为工作模式为连续外触发",M_DEV_KXCCD,0,M_SET_TRIGGER,0,0,true));

    m_pStandbyFlow.append(pItem);//第一步  准备 无采集  设置只设置一次 初始化轮盘
}


/*************************************************************************
功能：创建1 On 1测试流程
输入：nCnt  每个能量打的发次数， lfEnergyIncream为能量台阶
输出:  1 On 1测试流程
返回值：无
备注：nPtsPerSectionCnt每个能量段对应点数（有用）
    dMinEnergy最小能量角度（有用）
    dMaxEnergy能量台阶角度（原为最大能量角度）
    nEnergySection能量台阶（无用）
    dMinDeg、dMaxDeg波片最小最大能量（有用）
****************************************************************************/
void QWorkFlowThread::create1On1WorkFlow(int nPtsPerSectionCnt, double dMinEnergy, double dMaxEnergy, int nEnergySection, QString sExpNo, int nWaveLengthType, double dMinDeg, double dMaxDeg)
{
#if 1
    int nEnergyIncreamCnt = 0;//能量提升或降低了几次
    //2020yi
    double dMoveStep_x = 0;
    double dMoveStep_y = 0;
    double dxPos_bak = 0;
    double dyPos_bak= 0 ;//计算下次运动距离和方向
    QString sStepName;

    clearWorkFlow(&m_pWorkFlowList);

    tExpInfo* pTempExpInfo = new tExpInfo();
    pTempExpInfo->m_sExpNo = sExpNo;
    pTempExpInfo->m_nMeasureType = M_MEASURETYPE_1On1;
    pTempExpInfo->m_bAutoTest = true;


    QVector<tExpInfo> tExpinfoList;
    tExpinfoList.clear();

    //工作阶段流程
    for(int i = 0; i < m_tPointList.size();i++)
    {
        const QPointInfo pTemp = m_tPointList[i];
        if(pTemp.nStatus != 0)//标记为不打靶状态
        {
            continue;
        }
        pTempExpInfo->m_nPointNo = i;
        pTempExpInfo->m_nTimes = 0; //统一从0开始，显示时再进一步处理


        QWorkFlowItem* pItem = new QWorkFlowItem();
        //先计算二维镜架运动距离 此时默认零点为左上角  运动流程
        dMoveStep_x = pTemp.dxPos - dxPos_bak;
        dMoveStep_y = pTemp.dyPos - dyPos_bak;
        dxPos_bak = pTemp.dxPos;
        dyPos_bak = pTemp.dyPos;


        int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
#if 0   //20210426yidongchi 執行流程前關閉所有光路光閘
        if (i == 0)
        {
            sStepName = "关闭光闸";
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER,0,0,true));
        }
#else
        if (i == 0)
        {
            pItem->m_pMoveWorkFlow.append(QWorkTask("关闭基頻光闸", M_DEV_SHUTTER, M_SHUTTER_0, M_CLOSE_SHUTTER,0,0,true,NULL,true));
            pItem->m_pMoveWorkFlow.append(QWorkTask("关闭三倍頻光闸", M_DEV_SHUTTER, M_SHUTTER_1, M_CLOSE_SHUTTER,0,0,true,NULL,true));
//            pItem->m_pCapWorkFlow.append(QWorkTask("能量衰减器",M_DEV_IDS,0,M_COM,0,1,true,pTempExpInfo));
//            pItem->m_pCapWorkFlow.append(QWorkTask("能量衰减器",M_DEV_IDS,0,M_COM,0,2,true,pTempExpInfo));
//            pItem->m_pCapWorkFlow.append(QWorkTask("能量衰减器",M_DEV_IDS,0,M_COM,0,3,true,pTempExpInfo));//ywc
//            pItem->m_pSetWorkFlow.append(QWorkTask("示波器采集脉宽",M_DEV_OSCII,0,M_CAP_OSCI_DATA,0,0,true));
        }
#endif

        if (pTempExpInfo->m_nPointNo != 0)
        {
            if(dMoveStep_x != 0)
            {
                sStepName = QString("二维电机水平轴运动到 %1 mm位置").arg(pTemp.dxPos);
                pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,-dMoveStep_x,M_MOVE_RELATIVE,false));//yhy0315
            }
            if(dMoveStep_y != 0)
            {
                sStepName = QString("二维电机垂直轴运动到 %1 mm位置").arg(pTemp.dyPos);
                pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,dMoveStep_y,M_MOVE_RELATIVE,false));//yhy0315
            }
        }


        //轮盘转动流程 根据条件进行增加该运动
        if(i == 0)//第一次 需要将能量计轮盘运动至最低能量 运动方式为绝对位置运动
        {
            sStepName = QString("能量调节器调节至最%1度").arg(dMinEnergy);//需要传入最小能量
            //系数  需要对接上
            int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
 //            double tem_dStart = dMinDeg + (dMaxDeg - dMinDeg)*dMinEnergy/100;
            double tem_dStart = dMinEnergy;
            pItem->m_pCapWorkFlow.append(QWorkTask(sStepName,M_DEV_IDS,0,M_COM,tem_dStart,nMotorIndex,true,pTempExpInfo));
        }
        else if((i)%nPtsPerSectionCnt == 0 && i > 0)// 一个能量结束  需要调整能量 lfEnergyIncream计算轮盘档位
        {
            nEnergyIncreamCnt++;
            int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
 //            double tem_Incre = (dMaxDeg - dMinDeg)*(dMaxEnergy - dMinEnergy)/100/nEnergySection;
            dMinEnergy += (dMaxDeg - dMinDeg)/qAbs(dMaxDeg - dMinDeg)*dMaxEnergy;
            sStepName = QString("能量调节器调节至下一个能量台阶");
            pItem->m_pCapWorkFlow.append(QWorkTask(sStepName,M_DEV_IDS,0,M_COM,dMinEnergy,nMotorIndex,true,pTempExpInfo));
        }

        pTempExpInfo->m_nEnergySection = nEnergyIncreamCnt;
        tExpinfoList.append(*pTempExpInfo);
        if (i == 0)
        {
            //只在第一点使用的设置流程
//            pItem->m_pSetWorkFlow.append(QWorkTask("设置显微监视工作模式为内触发",M_DEV_IDS,0,M_SET_TRIGGER,2,0,true));    //Y
           // pItem->m_pSetWorkFlow.append(QWorkTask("设置CCD参数",M_DEV_KXCCD,0,M_SET_TRIGGER,0,0,true));//by lg 20190611 去除CCD相关操作
//            pItem->m_pSetWorkFlow.append(QWorkTask("设置监视能量计",M_DEV_ENERGY, 0, M_SET_PARAM, nWaveLengthType ,0, true,pTempExpInfo));//
        }

        //采集流程
//        pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//打之前采集一副图像    //y
        pItem->m_pCapWorkFlow.append(QWorkTask("拍摄打点前照片",M_DEV_IDS,0,M_SEND_INFO,0,0,true,pTempExpInfo));//发送A1    //yhy
       // pItem->m_pCapWorkFlow.append(QWorkTask("保存圖片",M_DEV_IDS,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//打之前采集一副图像    //y
        pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,0,true));//延迟设置为0
//        pItem->m_pCapWorkFlow.append(QWorkTask("关闭光闸",M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER,0,0,true));//保证关闭光闸

        //光闸关闭后进行采集
        //pItem->m_pCapWorkFlow.append(QWorkTask("科学CCD采集数据",M_DEV_KXCCD,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//by lg 20190611 去除CCD相关操作
        pItem->m_pCapWorkFlow.append(QWorkTask("监视能量计采集数据",M_DEV_ENERGY, 0, M_CAP_DATA, 0 ,0, true,pTempExpInfo));//能量计采集//yhy0315
       // QMessageBox::warning(nullptr, "執行",QString("%1").arg(M_DEV_ENERGY));
//        pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,1,0,true,pTempExpInfo));//显微图像采集  //y
        pItem->m_pCapWorkFlow.append(QWorkTask("拍摄打点后照片",M_DEV_IDS,0,M_START_DEAL,1,0,true,pTempExpInfo));//发送A3  //yhy0315
        pItem->m_pCapWorkFlow.append(QWorkTask("获取诊断结果",M_DEV_IDS,0,M_GET_DATA,0,0,true,pTempExpInfo));//发送A2  //yhy
        pItem->m_pCapWorkFlow.append(QWorkTask("该点诊断完毕",M_DEV_IDS,0,M_END_DEAL,0,0,true,pTempExpInfo));//发送A4  //yhy
        //数据处理流程
        pItem->m_pDataProcessWorkFlow.append(QWorkTask("计算通量数据", M_DATA_PROCESS, 0, M_CALC_FLUX, 0, 0,true,pTempExpInfo));//数据处理

//        //1on1流程在最后一个点中加入所有点的损伤判断处理
//        if (i == m_tPointList.size() - 1)
//        {
//            for(int i = 0; i < m_tPointList.size();i++)
//            {
//                pItem->m_pDataProcessWorkFlow.append(QWorkTask("图像数据处理", M_DATA_PROCESS, 0, M_IMG_1ON1, 0, 0,true,&tExpinfoList[i]));//数据处理
//            }
//        }

        //一个点流程结束
        m_pWorkFlowList.append(pItem);
    }
#else
    clearWorkFlow(&m_pWorkFlowList);

    //工作阶段流程
    for(int i = 0; i < m_tPointList.size();i++)
    {
        QWorkFlowItem* pItem = new QWorkFlowItem();

        int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
        pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,0,true));//延迟设置为0
        //一个点流程结束
        m_pWorkFlowList.append(pItem);
    }
#endif

}

/*************************************************************************
功能：创建1 On 1手动测试流程
输入：lfEnergy为当前能量 nStep为Step的距离
输出:  1 On 1手动测试流程
返回值：无
****************************************************************************/
void QWorkFlowThread::create1On1ManualWorkFlow(double lfEnergy, double dStep, QString sExpNo, int nWaveLengthType)
{
    QString sStepName;
    clearWorkFlow(&m_pManualWorkFlowlist);

     tExpInfo* pTempExpInfo = new tExpInfo();
     pTempExpInfo->m_sExpNo = sExpNo;
     pTempExpInfo->m_nMeasureType = M_MEASURETYPE_1On1;
     pTempExpInfo->m_bAutoTest = false;

     //工作阶段流程
     QWorkFlowItem* pItem = new QWorkFlowItem();
     sStepName = "关闭光闸";
     int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
     pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER));
     sStepName = QString("二维电机水平轴运动到 %1 mm位置").arg(dStep);
     pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,dStep,M_MOVE_RELATIVE,true));//运动位置

    //能量调节 根据能量计算运动的角度
    tCoeff Temp = m_pEnergyAdjCoeff[nWaveLengthType];//y=kx+b
    double t_dRowNum = (lfEnergy - Temp.m_lfB)/Temp.m_lfK;
    int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
    sStepName = QString("转动轮盘Mr1,调整能量为%1").arg(lfEnergy);
    pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,t_dRowNum,M_MOVE_ABS,true));//需要修改

     //设置流程
     pItem->m_pSetWorkFlow.append(QWorkTask("设置CCD为工作模式为帧存模式",M_DEV_KXCCD,0,M_SET_TRIGGER,0,0,true));
     pItem->m_pSetWorkFlow.append(QWorkTask("设置显微监视工作模式为外触发",M_DEV_IDS,0,M_SET_TRIGGER,2,0,true));
     pItem->m_pSetWorkFlow.append(QWorkTask("设置能量计参数",M_DEV_ENERGY,0,M_SET_PARAM,nWaveLengthType,0,false));

     //采集流程
     pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//打之前采集一副图像
     pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,10,true));//打开1S后自动关闭 param2指定1S后自动关闭
     //光闸关闭后进行采集
     pItem->m_pCapWorkFlow.append(QWorkTask("能量计采集数据",M_DEV_ENERGY, 0, M_CAP_DATA, 0 ,0,true,pTempExpInfo));//能量计采集(几号能量计采集？)
     pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,1,0,false,pTempExpInfo));//显微图像采集
     pItem->m_pCapWorkFlow.append(QWorkTask("科学CCD采集数据",M_DEV_KXCCD,0,M_CAP_IMAG,0,0,false));
     pItem->m_pDataProcessWorkFlow.append(QWorkTask("计算通量数据", M_DATA_PROCESS, 0, M_CALC_FLUX, 0, 0,false,pTempExpInfo));//数据处理
     pItem->m_pDataProcessWorkFlow.append(QWorkTask("图像数据处理", M_DATA_PROCESS, 0, M_IMG_1ON1, 0, 0,false,pTempExpInfo));//数据处理
     //一个点流程结束
     m_pManualWorkFlowlist.append(pItem);
}

/******************************
*功能:创建测试脉宽流程
*输入:
*输出:
*返回值:
*修改记录:yhy0320
*******************************/
void QWorkFlowThread::createPulsewidthWorkFlow(int nCnt, int nWaveLengthType)
{
    QString sStepName;
    clearWorkFlow(&m_pPulsewidthWorkFlowlist);

    tExpInfo* pTempExpInfo = new tExpInfo();
    pTempExpInfo->m_sExpNo = "00_Pulsewidth_00";
    pTempExpInfo->m_nMeasureType = M_MEASURETYPE_1On1;
    pTempExpInfo->m_bAutoTest = false;

    int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
    for (int i=0; i<nCnt; i++)
    {
        QWorkFlowItem* pItem = new QWorkFlowItem();
//        if (i == 0)
//        {
//            sStepName = "关闭光闸";
//            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER));
//        }
//        pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,0,true));
        pItem->m_pSetWorkFlow.append(QWorkTask("示波器采集脉宽",M_DEV_OSCII,0,M_CAP_OSCI_DATA,0,0,true));
        m_pPulsewidthWorkFlowlist.append(pItem);
    }
}

/******************************
*功能:创建测试平均光斑面积流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowThread::createAverAreaManualWorkFlow(int nCnt, int nWaveLengthType)
{
    QString sStepName;
    clearWorkFlow(&m_pAverAreaWorkFlowlist);

    tExpInfo* pTempExpInfo = new tExpInfo();
    pTempExpInfo->m_sExpNo = "00_AverArea_00";
    pTempExpInfo->m_nMeasureType = M_MEASURETYPE_1On1;
    pTempExpInfo->m_bAutoTest = false;

    int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
    for (int i=0; i<nCnt; i++)
    {
        QWorkFlowItem* pItem = new QWorkFlowItem();
        if (i == 0)
        {
            sStepName = "关闭光闸";
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER));
            pItem->m_pSetWorkFlow.append(QWorkTask("设置CCD为工作模式为帧存模式",M_DEV_KXCCD,0,M_SET_TRIGGER,0,0,true));
        }

        pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,0,true));
        pItem->m_pCapWorkFlow.append(QWorkTask("科学CCD采集数据",M_DEV_KXCCD,0,M_CAP_IMAG,0,0,true));
        pItem->m_pDataProcessWorkFlow.append(QWorkTask("计算面积数据", M_DATA_PROCESS, 0, M_CALC_AREA, 0, 0,true,pTempExpInfo));//数据处理
        m_pAverAreaWorkFlowlist.append(pItem);
    }
}


/******************************
*功能:生成新光闸测试流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowThread::createNewShutterTestWorkFlow(int nCnt)
{
    clearWorkFlow(&m_pManualWorkFlowlist);

    tExpInfo* pTempExpInfo = new tExpInfo();
    pTempExpInfo->m_sExpNo = "00_NewShutterTest_00";
    pTempExpInfo->m_nMeasureType = M_MEASURETYPE_1On1;
    pTempExpInfo->m_bAutoTest = false;

    for (int i=0; i<nCnt; i++)
    {
        pTempExpInfo->m_nPointNo = i+1;
        QWorkFlowItem* pItem = new QWorkFlowItem();
        pItem->m_pMoveWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,M_SHUTTER_1,M_OPEN_SHUTTER,1,0,true,pTempExpInfo));
        m_pManualWorkFlowlist.append(pItem);
    }
}

/******************************
*功能:创建样品台归零流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowThread::createSampleStage2ZeroWorkFlow()
{
    clearWorkFlow(&m_pManualWorkFlowlist);

    QWorkFlowItem* pItem = new QWorkFlowItem();
    pItem->m_pMoveWorkFlow.append(QWorkTask("移动M3-1样品台x轴",M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,65,M_MOVE_ABS,true));
    pItem->m_pMoveWorkFlow.append(QWorkTask("移动M3-4样品台y轴",M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,30,M_MOVE_ABS,true));
    m_pManualWorkFlowlist.append(pItem);
}


/******************************
*功能:测量平均分光比
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowThread::createAverEnergyRatioWorkFlow(int nCnt, int nWaveLengthType)
{
    QString sStepName;
    clearWorkFlow(&m_pAverEnergyMeterWorkFlowlist);

    tExpInfo* pTempExpInfo = new tExpInfo();
    pTempExpInfo->m_sExpNo = "00_AverEnergy_00";
    pTempExpInfo->m_nMeasureType = M_MEASURETYPE_1On1;
    pTempExpInfo->m_bAutoTest = false;

    int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
    for (int i=0; i<nCnt; i++)
    {
        QWorkFlowItem* pItem = new QWorkFlowItem();
        if (i == 0)
        {
            sStepName = "关闭光闸";
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER));
        }
        pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,0,true));
        pItem->m_pCapWorkFlow.append(QWorkTask("监视能量计采集数据",M_DEV_ENERGY, 0, M_CAP_DATA, 0 ,0, true,pTempExpInfo));//能量计采集
        pItem->m_pCapWorkFlow.append(QWorkTask("主能量计采集数据",M_DEV_ENERGY, 1, M_CAP_DATA, 0 ,0, true,pTempExpInfo));//能量计采集
        m_pAverEnergyMeterWorkFlowlist.append(pItem);
    }
}
/*************************************************************************
功能：创建S On 1测试流程
输入：nShotCnt  每个能量打的发次数， lfEnergyIncream为能量台阶
输出:  S On 1测试流程
返回值：无
****************************************************************************/
void QWorkFlowThread::CreateSOn1WorkFlow(int nShotCnt, double lfEnergyIncream, QString sExpNo, int nWaveLengthType, int nPointsPerEnergy)
{
    int nEnergyIncreamCnt = 0;//能量提升或降低了几次
    double dMoveStep_x = 0;
    double dMoveStep_y = 0;
    double dxPos_bak = 0;
    double dyPos_bak= 0 ;//计算下次运动距离和方向
    QString sStepName;


    clearWorkFlow(&m_pWorkFlowList);

     tExpInfo* pTempExpInfo = new tExpInfo();
     pTempExpInfo->m_sExpNo = sExpNo;
     pTempExpInfo->m_nMeasureType = M_MEASURETYPE_SOn1;
     pTempExpInfo->m_bAutoTest = true;
     pTempExpInfo->m_nTimesPerPoint = nShotCnt;

    //工作阶段流程
     for(int i = 0; i < m_tPointList.size();i++)
     {
         for (int j=0; j<pTempExpInfo->m_nTimesPerPoint; j++)
         {
             const QPointInfo pTemp = m_tPointList[i];
             if(pTemp.nStatus != 0)//标记为不打靶状态
             {
                 continue;
             }
             //一个发次一个Item
             pTempExpInfo->m_nPointNo = i;
             pTempExpInfo->m_nTimes = j;
             QWorkFlowItem* pItem = new QWorkFlowItem();
             pItem->m_nSOn1ShotCnt = pTempExpInfo->m_nTimesPerPoint;

             sStepName = "关闭光闸";
             int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
             pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER));

             //先计算二维镜架运动距离 此时默认零点为左上角  运动流程
             if (pTempExpInfo->m_nTimes == 0)    //每个点第一发次进行样品台移动 和调节能量
             {
                 dMoveStep_x = pTemp.dxPos - dxPos_bak;
                 dMoveStep_y = pTemp.dyPos - dyPos_bak;

                 dxPos_bak = pTemp.dxPos;
                 dyPos_bak = pTemp.dyPos;



                 if(dMoveStep_x != 0)
                 {
                     sStepName = QString("二维电机水平轴运动到 %1 mm位置").arg(pTemp.dxPos);
                     pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,-dMoveStep_x,M_MOVE_RELATIVE,true));
                 }
                 if(dMoveStep_y != 0)
                 {
                     sStepName = QString("二维电机垂直轴运动到 %1 mm位置").arg(pTemp.dyPos);
                     pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,dMoveStep_y,M_MOVE_RELATIVE,true));
                 }


                 //和1on1调节能量方式一致
                 //轮盘转动流程 根据条件进行增加该运动
                 if(i == 0)//第一次 需要将能量计轮盘运动至最低能量 运动方式为绝对位置运动
                 {
                     sStepName = QString("转动轮盘Mr1,调节能量至最低能量:%1 mJ").arg(1);//需要传入最小能量
                     //系数  需要对接上
                     int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
                     pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,10,M_MOVE_ABS,true));

                 }
                 else if((i)%nPointsPerEnergy == 0 && i > 0)// 一个能量结束  需要调整能量 lfEnergyIncream计算轮盘档位
                 {
                     nEnergyIncreamCnt++;
                     sStepName = QString("转动轮盘Mr1,能量增加%1").arg(lfEnergyIncream);
                     int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
                     pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,23,M_MOVE_RELATIVE,true));
                 }
             }


             if (pTempExpInfo->m_nPointNo == 0)  //只在第一个点的第一发次进行设备设置
             {
                 pItem->m_pSetWorkFlow.append(QWorkTask("设置显微监视工作模式为内触发",M_DEV_IDS,0,M_SET_TRIGGER,2,0,true));    //Y
                 //pItem->m_pSetWorkFlow.append(QWorkTask("设置CCD为帧存模式",M_DEV_KXCCD,0,M_SET_TRIGGER,0,0,false));//by lg 20190611
             }


             //采集流程
             pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//打之前采集一副图像    //y
             pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,0,true));//延迟设置为0
             //光闸关闭后进行采集
             //pItem->m_pCapWorkFlow.append(QWorkTask("科学CCD采集数据",M_DEV_KXCCD,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//by lg 20190611
             pItem->m_pCapWorkFlow.append(QWorkTask("监视能量计采集数据",M_DEV_ENERGY, 0, M_CAP_DATA, 0 ,0, false,pTempExpInfo));//能量计采集
             pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,1,0,true,pTempExpInfo));//显微图像采集  //y

             pItem->m_pDataProcessWorkFlow.append(QWorkTask("计算通量数据", M_DATA_PROCESS, 0, M_CALC_FLUX, 0, 0,true,pTempExpInfo));//数据处理
             pItem->m_pDataProcessWorkFlow.append(QWorkTask("图像数据处理", M_DATA_PROCESS, 0, M_IMG_1ON1, 0, 0,false,pTempExpInfo));//数据处理


             //一个点流程结束
             m_pWorkFlowList.append(pItem);
        }

    }

}

/*************************************************************************
功能：创建R On 1测试流程
输入： lfEnergyIncream为能量台阶
输出:  R On 1测试流程
返回值：无
****************************************************************************/
void QWorkFlowThread::CreateROn1WorkFlow(double lfMinEnergy,double lfMaxEnergy,int nEnergySecNum,QString sExpNo,int nWaveLengthType, double dMinDeg, double dMaxDeg)
{
    int nEnergyIncreamCnt = 0;//能量提升或降低了几次
    int nCurrentDeg = 0;    //当前轮盘角度
//    int nMoveStep_x = 0;
//    int nMoveStep_y = 0;
//    int nxPos_bak = 0;
//    int nyPos_bak= 0 ;//计算下次运动距离和方向
    //2020yi
    double dMoveStep_x = 0;
    double dMoveStep_y = 0;
    double dxPos_bak = 0;
    double dyPos_bak= 0 ;//计算下次运动距离和方向
    QString sStepName;

    clearWorkFlow(&m_pWorkFlowList);
    tExpInfo* pTempExpInfo = new tExpInfo();
    pTempExpInfo->m_sExpNo = sExpNo;
    pTempExpInfo->m_nMeasureType = M_MEASURETYPE_ROn1;
    pTempExpInfo->m_bAutoTest = true;
    pTempExpInfo->m_nEnergySecNum = nEnergySecNum;

    //工作阶段流程
    for(int i = 0; i < m_tPointList.size();i++)
    {
        for (int j=0; j<pTempExpInfo->m_nEnergySecNum; j++) //发次循环
        {
            const QPointInfo pTemp = m_tPointList[i];
            if(pTemp.nStatus != 0)//标记为不打靶状态
            {
                continue;
            }

            //一个发次一个Item
            pTempExpInfo->m_nPointNo = i;
            pTempExpInfo->m_nTimes = j;
            QWorkFlowItem* pItem = new QWorkFlowItem();
            pItem->m_nRon1ShotCnt = pTempExpInfo->m_nEnergySecNum;


            int tem_nShutterIndex = getShutterIndex(nWaveLengthType);
#if 0   //20210426yidongchi 執行流程前關閉所有光閘
            if (i == 0 && j == 0)
            {
                sStepName = "关闭光闸";
                pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER));
            }
#else
            if (i == 0 && j == 0)
            {
                pItem->m_pMoveWorkFlow.append(QWorkTask("关闭基頻光闸", M_DEV_SHUTTER, M_SHUTTER_0, M_CLOSE_SHUTTER,0,0,true,NULL,true));
                pItem->m_pMoveWorkFlow.append(QWorkTask("关闭三倍頻光闸", M_DEV_SHUTTER, M_SHUTTER_1, M_CLOSE_SHUTTER,0,0,true,NULL,true));
            }
#endif



            //先计算二维镜架运动距离 此时默认零点为左上角  运动流程
            if (pTempExpInfo->m_nTimes == 0) //每个点第一发次进行样品台移动
            {
                dMoveStep_x = pTemp.dxPos - dxPos_bak;
                dMoveStep_y = pTemp.dyPos - dyPos_bak;

                dxPos_bak = pTemp.dxPos;
                dyPos_bak = pTemp.dyPos;

                //将用户设定的初始位置坐标设置成第一点的坐标，不再进行移动
                if (pTempExpInfo->m_nPointNo != 0)
                {
                    if(dMoveStep_x != 0)
                    {
                        sStepName = QString("二维电机水平轴运动到 %1 mm位置").arg(pTemp.dxPos);
                        pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,-dMoveStep_x,M_MOVE_RELATIVE,true));
                    }
                    if(dMoveStep_y != 0)
                    {
                        sStepName = QString("二维电机垂直轴运动到 %1 mm位置").arg(pTemp.dyPos);
                        pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,dMoveStep_y,M_MOVE_RELATIVE,true));
                    }
                }

            }


            //每一发次都调节能量
            //轮盘转动流程 根据条件进行增加该运动
            //191016调整：逻辑改为————只有在第一个点执行归零，后续点能量衰减轮盘只做相对运动
            if (pTempExpInfo->m_nPointNo == 0 && pTempExpInfo->m_nTimes == 0)
            {
                nCurrentDeg = 0;
                sStepName = QString("能量衰减器归零");//需要传入最小能量
                //能量调节参数
                int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
                pItem->m_pCapWorkFlow.append(QWorkTask(sStepName,M_DEV_IDS,0,M_COM,0,nMotorIndex,true,pTempExpInfo));
            }
            //191016调整：逻辑改为每个点的第一发次将波片轮盘调整到起始能量处
            if(pTempExpInfo->m_nTimes == 0)//每个点的第一发次 需要将能量计轮盘运动至最低能量 运动方式为绝对位置运动
            {
                sStepName = QString("能量衰减器调节至最低能量角度%1度").arg(lfMinEnergy);//需要传入最小能量
                //能量调节参数
                int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
 //                double tem_dStart = lfMinEnergy;
               // double tem_dStart = -(nCurrentDeg - lfMinEnergy);//ywc
                nCurrentDeg = lfMinEnergy;  //将当前轮盘角度设置为最小能量角度
                pItem->m_pCapWorkFlow.append(QWorkTask(sStepName,M_DEV_IDS,0,M_COM,nCurrentDeg,nMotorIndex,true,pTempExpInfo));
 //                pItem->m_pMoveWorkFlow.append(QWorkTask("设置监视能量计到合适量程",M_DEV_ENERGY, 0, M_SET_PARAM, nWaveLengthType ,0, true,pTempExpInfo));//

            }
            else    // 一个能量结束  需要调整能量 lfEnergyIncream计算轮盘档位
            {
                nEnergyIncreamCnt++;
                double tem_Incre = (dMaxDeg - dMinDeg)/qAbs(dMaxDeg - dMinDeg)*lfMaxEnergy;
                nCurrentDeg += tem_Incre;
                sStepName = QString("能量衰减器调节波片轮盘至%1度").arg(nCurrentDeg);
                int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
                pItem->m_pCapWorkFlow.append(QWorkTask(sStepName,M_DEV_IDS,0,M_COM,nCurrentDeg,nMotorIndex,true,pTempExpInfo));
            }



//            if (pTempExpInfo->m_nPointNo == 0)  //只在第一个点的第一发次进行设备设置
//            {
//                pItem->m_pSetWorkFlow.append(QWorkTask("设置显微监视工作模式为内触发",M_DEV_IDS,0,M_SET_TRIGGER,2,0,true));    //Y
//                //pItem->m_pSetWorkFlow.append(QWorkTask("设置CCD为帧存模式",M_DEV_KXCCD,0,M_SET_TRIGGER,0,0,false));//by lg 20190611
//  //                pItem->m_pSetWorkFlow.append(QWorkTask("设置监视能量计",M_DEV_ENERGY, 0, M_SET_PARAM, nWaveLengthType ,0, true,pTempExpInfo));//
//            }


            //采集流程
//            pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//打之前采集一副图像    //y
            pItem->m_pCapWorkFlow.append(QWorkTask("拍摄打点前照片",M_DEV_IDS,0,M_SEND_INFO,0,0,true,pTempExpInfo));//发送A1    //yhy
            pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,tem_nShutterIndex,M_OPEN_SHUTTER,1,0,true));//延迟设置为0
//            pItem->m_pCapWorkFlow.append(QWorkTask("关闭光闸",M_DEV_SHUTTER,tem_nShutterIndex,M_CLOSE_SHUTTER,0,0,true));//保证关闭光闸
            //光闸关闭后进行采集
            //pItem->m_pCapWorkFlow.append(QWorkTask("科学CCD采集数据",M_DEV_KXCCD,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//by lg 20190611
            pItem->m_pCapWorkFlow.append(QWorkTask("监视能量计采集数据",M_DEV_ENERGY, 0, M_CAP_DATA, 0 ,0,true,pTempExpInfo));//能量计采集
//            pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,1,0,true,pTempExpInfo));//显微图像采集  //y
            pItem->m_pCapWorkFlow.append(QWorkTask("拍摄打点后照片",M_DEV_IDS,0,M_START_DEAL,1,0,true,pTempExpInfo));//发送A3  //yhy0315
            pItem->m_pCapWorkFlow.append(QWorkTask("获取诊断结果",M_DEV_IDS,0,M_GET_DATA,0,0,true,pTempExpInfo));//发送A2  //yhy
            pItem->m_pCapWorkFlow.append(QWorkTask("该点诊断完毕",M_DEV_IDS,0,M_END_DEAL,0,0,true,pTempExpInfo));//发送A4  //yhy

            pItem->m_pDataProcessWorkFlow.append(QWorkTask("计算通量数据", M_DATA_PROCESS, 0, M_CALC_FLUX, 0, 0,true,pTempExpInfo));//数据处理
//            pItem->m_pDataProcessWorkFlow.append(QWorkTask("图像数据处理", M_DATA_PROCESS, 0, M_IMG_1ON1, 0, 0,false,pTempExpInfo));//数据处理

            //一个点流程结束
            m_pWorkFlowList.append(pItem);
        }

    }

}


/******************************
*功能:创建Raster流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowThread::createRasterWorkFlow(int nCnt, double lfStartEnergy, double lfMaxEnergy, int nEnergySection, QString sExpNo, int nWaveLengthType)
{
    m_nRasterCycleCnt++;    //循环计数加一
    m_bCycleStatus = false; //默认本轮结束后不需要再进行循环
    int nEnergyIncreamCnt = 0;//能量提升或降低了几次
    double dMoveStep_x = 0;
    double dMoveStep_y = 0;
    double dxPos_bak = 0;
    double dyPos_bak= 0 ;//计算下次运动距离和方向
    QString sStepName;

    clearWorkFlow(&m_pWorkFlowList);

    tExpInfo* pTempExpInfo = new tExpInfo();
    pTempExpInfo->m_sExpNo = sExpNo;
    pTempExpInfo->m_nMeasureType = M_MEASURETYPE_RASTER;
    pTempExpInfo->m_bAutoTest = true;
    pTempExpInfo->m_nEnergySecNum = nEnergySection;


    QVector<tExpInfo> tExpinfoList;
    tExpinfoList.clear();

    //工作阶段流程
    for(int i = 0; i < m_tPointList.size();i++)
    {
        const QPointInfo pTemp = m_tPointList[i];
        if(pTemp.nStatus != 0)//标记为不打靶状态
        {
            continue;
        }
        pTempExpInfo->m_nPointNo = i;
        pTempExpInfo->m_nTimes = 0; //统一从0开始，显示时再进一步处理


        QWorkFlowItem* pItem = new QWorkFlowItem();
        sStepName = "关闭光闸";
        pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_SHUTTER,M_SHUTTER_0,M_CLOSE_SHUTTER));

        //在每次循环开始，将样品台归零
        if (i == 0)
        {
            sStepName = QString("二维电机水平轴归零");
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,0,M_MOVE_ABS,true));
            sStepName = QString("二维电机水平轴归零");
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,107,M_MOVE_ABS,true));
            sStepName = QString("转动轮盘，调节至最低能量");
            int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,10,M_MOVE_ABS,true));
        }


        //先计算二维镜架运动距离 此时默认零点为左上角  运动流程
        dMoveStep_x = pTemp.dxPos - dxPos_bak;
        dMoveStep_y = pTemp.dyPos - dyPos_bak;
        dxPos_bak = pTemp.dxPos;
        dyPos_bak = pTemp.dyPos;




        if(dMoveStep_x != 0)
        {
            sStepName = QString("二维电机水平轴运动到 %1 mm位置").arg(pTemp.dxPos);
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_0,M_MOTOR_MOVESTEP,-dMoveStep_x,M_MOVE_RELATIVE,true));
        }
        if(dMoveStep_y != 0)
        {
            sStepName = QString("二维电机垂直轴运动到 %1 mm位置").arg(pTemp.dyPos);
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,M_MOTOR_2_3,M_MOTOR_MOVESTEP,dMoveStep_y,M_MOVE_RELATIVE,true));
        }


        //轮盘转动流程 根据条件进行增加该运动
        if(i == 0)//第一次 需要将能量计轮盘运动至最低能量 运动方式为绝对位置运动
        {
            sStepName = QString("转动轮盘Mr1,调节能量至最低能量");//需要传入最小能量
            //系数  需要对接上
            int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,10,M_MOVE_ABS,true));

        }
        else if((i)%nCnt == 0 && i > 0)// 一个能量结束  需要调整能量 lfEnergyIncream计算轮盘档位
        {
            nEnergyIncreamCnt++;
            int nIncre = (55-10)/qMax(1,(nEnergySection-1));
            sStepName = QString("转动轮盘Mr1到下一个能量段");
            int nMotorIndex = getEnergyMotorIndex(nWaveLengthType);
            pItem->m_pMoveWorkFlow.append(QWorkTask(sStepName,M_DEV_MOTOR,nMotorIndex,M_MOTOR_MOVESTEP,nIncre,M_MOVE_RELATIVE,true));
        }



        pTempExpInfo->m_nEnergySection = nEnergyIncreamCnt;
        if (i == 0)
        {
            //只在第一点使用的设置流程
            pItem->m_pSetWorkFlow.append(QWorkTask("设置显微监视工作模式为内触发",M_DEV_IDS,0,M_SET_TRIGGER,2,0,true));
//            pItem->m_pSetWorkFlow.append(QWorkTask("设置能量计参数",M_DEV_ENERGY,0,M_SET_PARAM,nWaveLengthType,0,false));
            //pItem->m_pSetWorkFlow.append(QWorkTask("设置CCD参数",M_DEV_KXCCD,0,M_SET_TRIGGER,0,0,false));//by lg 20190611
        }

        //采集流程
        pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//打之前采集一副图像    //y

        pItem->m_pCapWorkFlow.append(QWorkTask("光闸打开",M_DEV_SHUTTER,M_SHUTTER_0,M_OPEN_SHUTTER,1,0,true));//延迟设置为0
        //光闸关闭后进行采集
        //pItem->m_pCapWorkFlow.append(QWorkTask("科学CCD采集数据",M_DEV_KXCCD,0,M_CAP_IMAG,0,0,true,pTempExpInfo));//by lg 20190611
        pItem->m_pCapWorkFlow.append(QWorkTask("能量计1采集数据",M_DEV_ENERGY, 0, M_CAP_DATA, 0 ,0, true,pTempExpInfo));//能量计采集
        pItem->m_pCapWorkFlow.append(QWorkTask("显微图像采集数据",M_DEV_IDS,0,M_CAP_IMAG,1,0,true,pTempExpInfo));//显微图像采集  //y

        //数据处理流程
        pItem->m_pDataProcessWorkFlow.append(QWorkTask("计算通量数据", M_DATA_PROCESS, 0, M_CALC_FLUX, 0, 0,false,pTempExpInfo));//数据处理
        pItem->m_pDataProcessWorkFlow.append(QWorkTask("图像数据处理", M_DATA_PROCESS, 0, M_IMG_1ON1, 0, 0,false,pTempExpInfo));//数据处理


        //一个点流程结束
        m_pWorkFlowList.append(pItem);
    }
}




/*************************************************************************
功能：执行运动流程
输入： pMoveItem 运动流程
输出:  无
返回值：执行是否成功
****************************************************************************/
bool QWorkFlowThread:: ProcessMoveWorkFlow(QWorkFlowItem* pMoveItem)
{
    if (pMoveItem->m_pMoveWorkFlow.size() > 0)
    {
        //191018修改： by yi 如果当前步骤状态为skip则不执行下面的流程
        if (pMoveItem->m_pMoveWorkFlow[0].m_nStepStatus == M_STEP_STATUS_SKIP)
        {
            return true;
        }
        int nRel = 0;

        qDeleteAll(m_tThreadList);
        m_tThreadList.clear();
        for(int j = 0; j < pMoveItem->m_pMoveWorkFlow.size();j++)
        {
            if(m_bImmediateStop == true)
            {
                return false;
            }
            if (m_bSuspend == true)
            {
                while(1)
                {
                    if (m_bSuspend == false)
                    {
                        break;
                    }
                    if(m_bImmediateStop == true)
                    {
                        return false;
        //                break;
                    }
                }
            }
            //开启运动线程  操作电机运动
            //实际并未进入此if条件中，多条线程无等待关系.已该为“等待上一步完成”
            if(pMoveItem->m_pMoveWorkFlow[j].m_bWaitForLastStep == true && j > 0)
            {
                //等待上一步完成
                nRel = waitForStepRel(&pMoveItem->m_pMoveWorkFlow[j-1]);
                if(nRel != 0)
                {
                    //终止流程
                    return false;
                }
            }

            QRunTaskThread* pThread = new QRunTaskThread(_pDevManagerCtrl);
            pThread->m_pTaskInfo = &pMoveItem->m_pMoveWorkFlow[j];
            pThread->start();

            m_tThreadList.append(pThread);
        }

        nRel = waitForWorkflowRel(&pMoveItem->m_pMoveWorkFlow);
        if(nRel != 0)
        {
            //流程超时 终止流程
            return false;
        }

        return true;
    }
    else
    {
        return true;
    }
}


/*************************************************************************
功能：执行数据处理流程
输入： pDataProcessItem 数据处理流程
输出:  无
返回值：执行是否成功
****************************************************************************/
bool QWorkFlowThread::ProcessDataWorkFlow(QWorkFlowItem* pDataProcessItem)
{   
    if (pDataProcessItem->m_pDataProcessWorkFlow.size() > 0)
    {
        //191018修改： by yi 如果当前步骤状态为skip则不执行下面的流程
        if (pDataProcessItem->m_pDataProcessWorkFlow[0].m_nStepStatus == M_STEP_STATUS_SKIP)
        {
            return true;
        }

        int nRel = 0;

        qDeleteAll(m_tThreadList);
        m_tThreadList.clear();
        for (int j = 0; j < pDataProcessItem->m_pDataProcessWorkFlow.size(); j++)
        {
            if (m_bImmediateStop == true)
            {
//                break;
                return false;
            }
            if (m_bSuspend == true)
            {
                while(1)
                {
                    if (m_bSuspend == false)
                    {
                        break;
                    }
                    if(m_bImmediateStop == true)
                    {
                        return false;
        //                break;
                    }
                }
            }
            //开启运动线程  进行数据处理
            if (pDataProcessItem->m_pDataProcessWorkFlow[j].m_bWaitForLastStep == true && j > 0)
            {
                //等待上一步完成
                nRel = waitForStepRel(&pDataProcessItem->m_pDataProcessWorkFlow[j - 1]);
                if (nRel != 0)
                {
                    //终止流程
                    return false;
                }
            }

            QRunTaskThread* pThread = new QRunTaskThread(_pDevManagerCtrl);
            //connect(pThread, &QThread::finished, this, &QObject::deleteLater);
            pThread->m_pTaskInfo = &pDataProcessItem->m_pDataProcessWorkFlow[j];
            pThread->start();

            m_tThreadList.append(pThread);
        }

        nRel = waitForWorkflowRel(&pDataProcessItem->m_pDataProcessWorkFlow);
        if (nRel != 0)
        {
            //流程超时 终止流程
            return false;
        }

        return true;
    }
    else
    {
        return true;
    }

}




/*************************************************************************
功能：执行测量设备设置流程
输入： pSetItem 数据处理流程
输出:  无
返回值：执行是否成功
****************************************************************************/
bool QWorkFlowThread:: ProcessSetWorkFlow(QWorkFlowItem* pSetItem)
{
    if (pSetItem->m_pSetWorkFlow.size() > 0)
    {
        //191018修改： by yi 如果当前步骤状态为skip则不执行下面的流程
        if (pSetItem->m_pSetWorkFlow[0].m_nStepStatus == M_STEP_STATUS_SKIP)
        {
            return true;
        }

        int nRel = 0;
        qDeleteAll(m_tThreadList);
        m_tThreadList.clear();

        for(int j = 0; j < pSetItem->m_pSetWorkFlow.size();j++)
        {
            if(m_bImmediateStop == true)
            {
//                break;
                return false;
            }
            if (m_bSuspend == true)
            {
                while(1)
                {
                    if (m_bSuspend == false)
                    {
                        break;
                    }
                    if(m_bImmediateStop == true)
                    {
                        return false;
        //                break;
                    }
                }
            }
            //开启设置线程
            if(pSetItem->m_pSetWorkFlow[j].m_bWaitForLastStep == true && j > 0)
            {
                //等待上一步完成
                nRel = waitForStepRel(&pSetItem->m_pSetWorkFlow[j-1]);
                if(nRel != 0)
                {
                    //终止流程
                    return false;
                }
            }

            QRunTaskThread* pThread = new QRunTaskThread(_pDevManagerCtrl);
            pThread->m_pTaskInfo = &pSetItem->m_pSetWorkFlow[j];
            pThread->start();
            m_tThreadList.append(pThread);
        }
        nRel = waitForWorkflowRel(&pSetItem->m_pSetWorkFlow);
        if(nRel != 0)
        {
            //流程超时 终止流程
            return false;
        }

        return true;
    }
    else
    {
        return true;
    }

}

/*************************************************************************
功能：执行采集流程
输入： pCapItem 采集流程
输出:  无
返回值：执行是否成功
****************************************************************************/
bool QWorkFlowThread:: ProcessCapWorkFlow(QWorkFlowItem* pCapItem)
{
    if (pCapItem->m_pCapWorkFlow.size() > 0)
    {
        //191018修改： by yi 如果当前步骤状态为skip则不执行下面的流程
        if (pCapItem->m_pCapWorkFlow[0].m_nStepStatus == M_STEP_STATUS_SKIP)
        {
            return true;
        }

        int nRel = 0;
        qDeleteAll(m_tThreadList);
        m_tThreadList.clear();
        for(int j = 0; j < pCapItem->m_pCapWorkFlow.size();j++)
        {
            if(m_bImmediateStop == true)
            {
//                break;
                return false;
            }
            if (m_bSuspend == true)
            {
                while(1)
                {
                    if (m_bSuspend == false)
                    {
                        break;
                    }
                    if(m_bImmediateStop == true)
                    {
                        return false;
        //                break;
                    }
                }
            }
            //开启设置线程
            if(pCapItem->m_pCapWorkFlow[j].m_bWaitForLastStep == true && j > 0)
            {
                //等待上一步完成
                nRel = waitForStepRel(&pCapItem->m_pCapWorkFlow[j-1]);
                if(nRel != 0)
                {
                    //终止流程
                    return false;
                }
            }

            QRunTaskThread* pThread = new QRunTaskThread(_pDevManagerCtrl);
            pThread->m_pTaskInfo = &pCapItem->m_pCapWorkFlow[j];
            pThread->start();
            m_tThreadList.append(pThread);
        }

        nRel = waitForWorkflowRel(&pCapItem->m_pCapWorkFlow);
        if(nRel != 0)
        {
            //流程超时 终止流程
            return false;
        }

        return true;
    }
    else
    {
        return true;
    }
}
/*************************************************************************
功能：执行准备流程
输入： pCapItem 采集流程
输出:  无
返回值：执行是否成功
****************************************************************************/
void QWorkFlowThread::ProcessStandByFlow()
{
   for(int i = 0; i < m_pStandbyFlow.size();i++)
   {
       QWorkFlowItem* pItem =  m_pStandbyFlow[i];
       //执行运动任务
       if(ProcessMoveWorkFlow(pItem) == false)
       {
           break;
       }
        //执行设置任务
       if(ProcessSetWorkFlow(pItem) == false)
       {
           break;
       }
        //执行采集任务
        if(ProcessCapWorkFlow(pItem) == false)
        {
            break;
        }
   }

}
/*************************************************************************
功能：执行测试流程
输入：
输出:  无
返回值：yhy0320//sleep删除
****************************************************************************/
void QWorkFlowThread::ProcessWorkFlow()
{
    //状态清零
    for (int i=0; i<m_pWorkFlowList.size(); i++)
    {
        for (int j=0; j<m_pWorkFlowList[i]->m_pMoveWorkFlow.size(); j++)
        {
            m_pWorkFlowList[i]->m_pMoveWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pWorkFlowList[i]->m_pSetWorkFlow.size(); j++)
        {
            m_pWorkFlowList[i]->m_pSetWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pWorkFlowList[i]->m_pCapWorkFlow.size(); j++)
        {
            m_pWorkFlowList[i]->m_pCapWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pWorkFlowList[i]->m_pDataProcessWorkFlow.size(); j++)
        {
            m_pWorkFlowList[i]->m_pDataProcessWorkFlow[j].m_nStepStatus = 0;
        }
    }


   for(int i = 0; i < m_pWorkFlowList.size();i++)
   {
       //默认人工不改变当前点损伤结果
       _pDevManagerCtrl->m_bManualChangeResult = false;

       QWorkFlowItem* pItem =  m_pWorkFlowList[i];

       //在Ron1 和 Son1时，前一发次有了损伤，则当前点剩余发次不予处理
       if ((pItem->m_pCapWorkFlow[0].m_tExpInfo.m_nMeasureType == M_MEASURETYPE_ROn1 || pItem->m_pCapWorkFlow[0].m_tExpInfo.m_nMeasureType == M_MEASURETYPE_SOn1) && pItem->m_pCapWorkFlow[0].m_tExpInfo.m_nPointNo == _pDevManagerCtrl->m_nStopPoint)
       {
           //191018修改： by yi 不跳过损伤点剩余发次的运动流程，即不跳过能量轮盘的转动流程，其余设置、采集、计算流程保持跳过
           for (int j=0; j<pItem->m_pSetWorkFlow.size(); j++)
           {
                pItem->m_pSetWorkFlow[j].m_nStepStatus = M_STEP_STATUS_SKIP;
           }
           for (int j=0; j<pItem->m_pCapWorkFlow.size(); j++)
           {
                pItem->m_pCapWorkFlow[j].m_nStepStatus = M_STEP_STATUS_SKIP;
           }
           for (int j=0; j<pItem->m_pDataProcessWorkFlow.size(); j++)
           {
                pItem->m_pDataProcessWorkFlow[j].m_nStepStatus = M_STEP_STATUS_SKIP;
           }
       }

       //执行运动任务
       if(ProcessMoveWorkFlow(pItem) == false)
       {
           break;
       }
       Sleep(300); //此处需要延时，否则流程中显微拍摄照片有滑动痕迹，具体时长有待调节

        //执行设置任务
       if(ProcessSetWorkFlow(pItem) == false)
       {
           break;
       }
//       Sleep(300);
        //执行采集任务
        if(ProcessCapWorkFlow(pItem) == false)
        {
            break;
        }
		//数据处理任务
		if (ProcessDataWorkFlow(pItem) == false)
		{
            break;
		}
//        Sleep(500);

        //判断是否有人工介入
        if (m_bManualAlter == true)
        {
            while (1)
            {
                Sleep(150);
                if (m_bManualAlter == false)
                {
                    if (_pDevManagerCtrl->m_bManualChangeResult == true)
                    {
                        QVariant var;
                        var.setValue(pItem->m_pDataProcessWorkFlow[1].m_tExpInfo);
                        emit signal_UserChangeResult(var,false);
                    }
                    else
                    {
                        QVariant var;
                        var.setValue(pItem->m_pDataProcessWorkFlow[1].m_tExpInfo);
                        emit signal_UserChangeResult(var,true);
                    }
                    break;
                }
            }
        }




        if (m_nMeasureType == M_MEASURETYPE_RASTER)
        {
            if (_pDevManagerCtrl->m_bIsRasterDefect == true)
            {
                m_tPointList[i].nStatus = M_STATUS_DISABLE;
            }
            else
            {
                //在所设定的能量段数范围内
                if (m_nRasterCycleCnt < pItem->m_pCapWorkFlow[0].m_tExpInfo.m_nEnergySecNum)
                {
                    m_bCycleStatus = true;  //存在未损伤点则确认下一轮需要循环
                }
            }
        }
   }
}

/*************************************************************************
功能：执行手动流程
输入：
输出:  无
返回值：无
****************************************************************************/
void QWorkFlowThread::ProcessManualWorkFlow()
{
    //状态清零
    for (int i=0; i<m_pManualWorkFlowlist.size(); i++)
    {
        for (int j=0; j<m_pManualWorkFlowlist[i]->m_pMoveWorkFlow.size(); j++)
        {
            m_pManualWorkFlowlist[i]->m_pMoveWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pManualWorkFlowlist[i]->m_pSetWorkFlow.size(); j++)
        {
            m_pManualWorkFlowlist[i]->m_pSetWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pManualWorkFlowlist[i]->m_pCapWorkFlow.size(); j++)
        {
            m_pManualWorkFlowlist[i]->m_pCapWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pManualWorkFlowlist[i]->m_pDataProcessWorkFlow.size(); j++)
        {
            m_pManualWorkFlowlist[i]->m_pDataProcessWorkFlow[j].m_nStepStatus = 0;
        }
    }
   for(int i = 0; i < m_pManualWorkFlowlist.size();i++)
   {
       QWorkFlowItem* pItem =  m_pManualWorkFlowlist[i];

       //执行运动任务
       if(ProcessMoveWorkFlow(pItem) == false)
       {
           break;
       }
       Sleep(500); //此处需要延时，否则流程中显微拍摄照片有滑动痕迹，具体时长有待调节

        //执行设置任务
       if(ProcessSetWorkFlow(pItem) == false)
       {
           break;
       }
       Sleep(1000);
        //执行采集任务
        if(ProcessCapWorkFlow(pItem) == false)
        {
            break;
        }
        //数据处理任务
        if (ProcessDataWorkFlow(pItem) == false)
        {
            break;
        }
        Sleep(2000);
   }
}

/******************************
*功能:执行脉宽测试流程
*输入:
*输出:
*返回值:
*修改记录:yhy0320
*******************************/
void QWorkFlowThread::ProcessPulsewidthWorkFlow()
{
    for (int i=0; i<m_pPulsewidthWorkFlowlist.size(); i++)
    {
        for (int j=0; j<m_pPulsewidthWorkFlowlist[i]->m_pMoveWorkFlow.size(); j++)
        {
            m_pPulsewidthWorkFlowlist[i]->m_pMoveWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pPulsewidthWorkFlowlist[i]->m_pSetWorkFlow.size(); j++)
        {
            m_pPulsewidthWorkFlowlist[i]->m_pSetWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pPulsewidthWorkFlowlist[i]->m_pCapWorkFlow.size(); j++)
        {
            m_pPulsewidthWorkFlowlist[i]->m_pCapWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pPulsewidthWorkFlowlist[i]->m_pDataProcessWorkFlow.size(); j++)
        {
            m_pPulsewidthWorkFlowlist[i]->m_pDataProcessWorkFlow[j].m_nStepStatus = 0;
        }
    }
   for(int i = 0; i < m_pPulsewidthWorkFlowlist.size();i++)
   {
       QWorkFlowItem* pItem =  m_pPulsewidthWorkFlowlist[i];

       if(ProcessMoveWorkFlow(pItem) == false)
       {
           break;
       }
       Sleep(500);
       if(ProcessSetWorkFlow(pItem) == false)
       {
           break;
       }

        if(ProcessCapWorkFlow(pItem) == false)
        {
            break;
        }

        if (ProcessDataWorkFlow(pItem) == false)
        {
            break;
        }
   }
}

/******************************
*功能:执行平均面积测试流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowThread::ProcessAverAreaWorkFlow()
{
    //状态清零
    for (int i=0; i<m_pAverAreaWorkFlowlist.size(); i++)
    {
        for (int j=0; j<m_pAverAreaWorkFlowlist[i]->m_pMoveWorkFlow.size(); j++)
        {
            m_pAverAreaWorkFlowlist[i]->m_pMoveWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pAverAreaWorkFlowlist[i]->m_pSetWorkFlow.size(); j++)
        {
            m_pAverAreaWorkFlowlist[i]->m_pSetWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pAverAreaWorkFlowlist[i]->m_pCapWorkFlow.size(); j++)
        {
            m_pAverAreaWorkFlowlist[i]->m_pCapWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pAverAreaWorkFlowlist[i]->m_pDataProcessWorkFlow.size(); j++)
        {
            m_pAverAreaWorkFlowlist[i]->m_pDataProcessWorkFlow[j].m_nStepStatus = 0;
        }
    }
    for(int i = 0; i < m_pAverAreaWorkFlowlist.size();i++)
    {
        QWorkFlowItem* pItem =  m_pAverAreaWorkFlowlist[i];

        //执行运动任务
        if(ProcessMoveWorkFlow(pItem) == false)
        {
            break;
        }
        Sleep(500); //此处需要延时，否则流程中显微拍摄照片有滑动痕迹，具体时长有待调节

        //执行设置任务
        if(ProcessSetWorkFlow(pItem) == false)
        {
            break;
        }
        Sleep(1000);
        //执行采集任务
        if(ProcessCapWorkFlow(pItem) == false)
        {
            break;
        }
        //数据处理任务
        if (ProcessDataWorkFlow(pItem) == false)
        {
            break;
        }
        Sleep(2000);
   }
}

/******************************
*功能:执行分光比测量流程
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
void QWorkFlowThread::ProcessAverEnergyMeterWorkFlow()
{
    //状态清零
    for (int i=0; i<m_pAverEnergyMeterWorkFlowlist.size(); i++)
    {
        for (int j=0; j<m_pAverEnergyMeterWorkFlowlist[i]->m_pMoveWorkFlow.size(); j++)
        {
            m_pAverEnergyMeterWorkFlowlist[i]->m_pMoveWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pAverEnergyMeterWorkFlowlist[i]->m_pSetWorkFlow.size(); j++)
        {
            m_pAverEnergyMeterWorkFlowlist[i]->m_pSetWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pAverEnergyMeterWorkFlowlist[i]->m_pCapWorkFlow.size(); j++)
        {
            m_pAverEnergyMeterWorkFlowlist[i]->m_pCapWorkFlow[j].m_nStepStatus = 0;
        }
        for (int j=0; j<m_pAverEnergyMeterWorkFlowlist[i]->m_pDataProcessWorkFlow.size(); j++)
        {
            m_pAverEnergyMeterWorkFlowlist[i]->m_pDataProcessWorkFlow[j].m_nStepStatus = 0;
        }
    }
   for(int i = 0; i < m_pAverEnergyMeterWorkFlowlist.size();i++)
   {
       QWorkFlowItem* pItem =  m_pAverEnergyMeterWorkFlowlist[i];

       //执行运动任务
       if(ProcessMoveWorkFlow(pItem) == false)
       {
           break;
       }
       Sleep(500); //此处需要延时，否则流程中显微拍摄照片有滑动痕迹，具体时长有待调节

        //执行设置任务
       if(ProcessSetWorkFlow(pItem) == false)
       {
           break;
       }
       Sleep(500);
        //执行采集任务
        if(ProcessCapWorkFlow(pItem) == false)
        {
            break;
        }
        //数据处理任务
        if (ProcessDataWorkFlow(pItem) == false)
        {
            break;
        }
        Sleep(500);
   }
}
/*************************************************************************
功能：等待步骤执行结果
输入：pStep 上一步指针
输出:  无
返回值：执行结果
****************************************************************************/
int QWorkFlowThread::waitForStepRel( QWorkTask* pStep)
{
    int nReturnRel = 1;
    while( 1 )
    {
        if(pStep->m_nStepStatus == M_STEP_STATUS_SUCCESS)
        {
            nReturnRel = 0;//上一步执行成功
            break;
        }
        if(pStep->m_nStepStatus == M_STEP_STATUS_FAILD)
        {
            nReturnRel = 1;//上一步执行成功
            break;
        }

        if(pStep->m_nStepStatus == M_STEP_STATUS_HASDEFECTS && pStep->m_bNeedUserCheck == false)
        {
            nReturnRel = 1;//上一步执行失败
            break;
        }

        //需要确认 确认结束后 会将m_nStepStatus  m_bNeedUserCheck进行更新
        //用户确认在有损伤的情况下依然继续实验流程，则人为将stepstatus设置为M_STEP_STATUS_SUCCESS
        if(pStep->m_nStepStatus == M_STEP_STATUS_HASDEFECTS && pStep->m_bNeedUserCheck == true)
        {
            continue;
        }

         Sleep(10);
    }

    return nReturnRel;

}

/*************************************************************************
功能：等待流程执行结果
输入：pSteplist 上一个流程指针
输出:  无
返回值：执行结果
****************************************************************************/
int QWorkFlowThread::waitForWorkflowRel(QList<QWorkTask>* pSteplist)
{
    bool bFinish = false;
    int nReturnRel = 1;

    while( bFinish==false )
    {
        for(int i = 0; i < m_tThreadList.size();i++)
        {
            bFinish = true;
            if(m_tThreadList[i]->isRunning())
            {
                bFinish = false;
                break;
            }
            else if (m_tThreadList[i]->m_pTaskInfo->m_nStepStatus == M_STEP_STATUS_HASDEFECTS && m_tThreadList[i]->m_pTaskInfo->m_bNeedUserCheck == true)
            {
                bFinish = false;
                break;
            }
        }
        Sleep(10);
    }

    //所有线程均结束
    nReturnRel = 0;//执行完成
    for(int j = 0; j < pSteplist->size();j++)
    {
        QWorkTask pItem = pSteplist->at(j);
        if(pItem.m_nStepStatus != M_STEP_STATUS_SUCCESS)
        {
            nReturnRel = 2;//执行失败
            break;
        }
    }

    return nReturnRel;

}

/*************************************************************************
功能：根据波长类型 选择不同的玻片对应的电机编号
输入：无
输出:  无
返回值：电机编号
****************************************************************************/
int QWorkFlowThread::getEnergyMotorIndex(int nWaveLengthType)
{
    if(nWaveLengthType == M_WAVE_LEN_1064)
    {
        return 1;
    }
    else if(nWaveLengthType == M_WAVE_LEN_532)
    {
        return 2;
    }
    else if(nWaveLengthType == M_WAVE_LEN_355)
    {
        return 3;
    }
    return 0;

}

/*************************************************************************
功能：根据波长类型 选择不同的shutter对应的电机编号
输入：无
输出:  无
返回值：电机编号
****************************************************************************/
int QWorkFlowThread::getShutterIndex(int nWaveLengthType)
{
    if(nWaveLengthType == M_WAVE_LEN_1064)
    {
        return M_SHUTTER_0;
    }
    else if(nWaveLengthType == M_WAVE_LEN_355)
    {
        return M_SHUTTER_1;
    }
    return 0;

}

/*************************************************************************
功能：run函数  thread->start启动
输入：无
输出:  无
返回值：执行结果
****************************************************************************/
void QWorkFlowThread::run()
{
    m_bImmediateStop = false;

    if(m_nWorkFlowTYpe == 1)
    {
        ProcessStandByFlow();
    }
    if(m_nWorkFlowTYpe == 2 )
    {
         ProcessWorkFlow();
    }
    if(m_nWorkFlowTYpe == 3)
    {
        ProcessManualWorkFlow();
    }
    if(m_nWorkFlowTYpe == 4)
    {
        ProcessAverAreaWorkFlow();
    }
    if(m_nWorkFlowTYpe == 5)
    {
        ProcessAverEnergyMeterWorkFlow();
    }
    if(m_nWorkFlowTYpe == 6)
    {
        ProcessPulsewidthWorkFlow();
    }

}
