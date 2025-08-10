/***************************************************************************
**                                                                                               **
**QMotorRunThread为运动线程
** 提供的功能函数为：
**
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qmotorrunthread.h"
#include "QDebug"

QMotorRunThread::QMotorRunThread(QMotorCtrl* pCtrl)
{
    _pMotorCtrl = pCtrl;

}


void QMotorRunThread::run()
{
      _pMotorCtrl-> MoveToAbsPosition(m_nMotorIndex,m_nPosition);

}
