/***************************************************************************
**                                                                                               **
**QMotorDevManager为电机设备管理类                                  **
** 本系统共有10个电机  由3个控制箱完成控制
** 提供的功能函数为：
** getAllMotorDevStatus                              获取所有控制箱状态  是否在线
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qmotordevmanager.h"

QMotorDevManager::QMotorDevManager()
{
    for(int i = 0; i < M_MOTOR_CTRLDEV_NUM;i++)
    {
        m_pMotorCtrl[i] = new QMotorCtrl(i);
//        m_pMotorCtrl[i]->m_pMotorTypeCoeff = m_pMotorTypeCoeff;
//        m_pMotorCtrl[i]->m_pMotorConfigInfo = m_pDevConfigInfo;
        m_pMotorCtrl[i]->OpenDev();
    }

}

QMotorDevManager::~QMotorDevManager()
{
    delete[] m_pMotorCtrl;
}

//获取控制盒状态 是否均在线
bool QMotorDevManager::getAllMotorDevStatus()
{
    bool bRel = false;

    for(int i = 0; i < M_MOTOR_CTRLDEV_NUM;i++)
    {
        bRel = bRel | m_pMotorCtrl[i]->getDevStatus();
    }
    return bRel;
}

