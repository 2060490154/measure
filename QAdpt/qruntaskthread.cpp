/***************************************************************************
**                                                                        **
**QRunTask为任务执行线程，负责任务的分发    **
**                                                                        **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qruntaskthread.h"
#include <qdebug.h>

QRunTaskThread::QRunTaskThread(QDevManagerCtrl* pCtrl)
{
    _pDevManagerCtrl = pCtrl;
}



void QRunTaskThread::run()
{

    switch(m_pTaskInfo->m_nDevType)
    {
        case M_DEV_MOTOR://电机任务
        {
            _pDevManagerCtrl->RunMotorTask(m_pTaskInfo);
        }
        break;
        case M_DEV_IDS://显微探视任务
        {
            _pDevManagerCtrl->RunIDSTask(m_pTaskInfo);
        }
        break;
        case M_DEV_ENERGY://能量计任务
        {
            _pDevManagerCtrl->RunEnergyMeterTask(m_pTaskInfo);
        }
         break;
        case M_DEV_OSCII://示波器任务
        {
            _pDevManagerCtrl->RunOsciTask(m_pTaskInfo);
        }
         break;
        case M_DEV_KXCCD://KxCCD任务
        {
             _pDevManagerCtrl->RunKxccdTask(m_pTaskInfo);
        }
        break;
        case M_DEV_SHUTTER://光闸
        {
             _pDevManagerCtrl->RunShutterTask(m_pTaskInfo);
        }
        break;
        case M_DATA_PROCESS://数据处理任务
        {
            _pDevManagerCtrl->RunDataProcess(m_pTaskInfo);
        }
        break;
    }

}
