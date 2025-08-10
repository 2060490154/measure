#ifndef QMOTORCTRL_H
#define QMOTORCTRL_H

#include <QObject>
#include <QThread>
#include "USB1020.h"
#include "define.h"
#include <QTimer>

#define M_MOTOR_MAX_STEP  160*6500
#define M_LP_CIRL_STEP   30900 //轮盘一周的步数



class QMotorCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QMotorCtrl(int nDevIndex,QObject *parent = 0);
    ~QMotorCtrl();

signals:
    void UpdateStatus(int nMotorCtrlIndex,int nMotorIndex,QVariant pStatus);//刷新电机状态

public slots:

 void ProcessMotorCmd();//获取电机当前参数和状态数据
public:
    bool checkIsConnect();
    //操作
    bool OpenDev();//打开设备
    bool CloseDev();//关闭设备
    bool InitMotorParam(int nMotorIndex,int nLvDvType = USB1020_DV);//初始化电机参数 默认为定长运动
    bool MoveToPosition(int nMotorIndex, float nPosition, int nRate=20);//运动到指定位置 定长直线运动 相对位置
    bool MoveToAbsPosition(int nMotorIndex, float nPosition);//运动到绝对位置
    bool MoveToRelativePosition(int nMotorIndex, float nPosition);//运动到相对位置
    bool GetMotorStatus(int nMotorIndex);//获取当前电机状态
    bool ImmediateStop(int nMotorIndex);//立即停止 保护使用
    bool GetRR0Status();//获取RR0状态寄存器的值

    bool getDevStatus();//显示用
    bool clearLP(int nMotorIndex);
    bool readLP(int nMotorIndex);

   void AddMotorCmd(QWorkTask* pNewTask );//添加所要执行的任务

   QList<QWorkTask*>m_pTaskList;//保证不出现冲突


   bool SearchHome(int nMotorIndex);//搜索零点

public:
    //变量
    USB1020_PARA_DataList   m_DataList[M_PRE_MOTOR_NUM];       // 公用参数 设置信息
    USB1020_PARA_LCData     m_LCData[M_PRE_MOTOR_NUM];         // 直线和S曲线参数

    motorStatusdata  m_tMotorStatusdata[M_PRE_MOTOR_NUM];//电机的速度 位置状态信息
    USB1020_PARA_RR0 m_tParamRR0Data;//包含四个轴的驱动状态

    QTimer* m_pTimer;

     bool m_bStopRunTask;//停止执行任务，响应立即停止
     tMotorTypeCoeff m_pMotorTypeCoeff[M_PRE_MOTOR_NUM];  //保存每个控制器下四个电机的电机类型和电机系数


private:
    HANDLE _hDevice;						// 设备句柄
    int _DevIndex;//控制盒编号

    bool _bStartThread;
};

#endif // QMOTORCTRL_H
