/***************************************************************************
**                                                                                               **
**QDevManagerWidget为所有设备的控制窗口 ，无实体窗口    **
** 只是个容器
** 提供的功能函数为：
** InitUI                              初始化所有设备控制窗口
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "qdevmanagerwidget.h"

QDevManagerWidget::QDevManagerWidget(QDevManagerCtrl* pCtrl,QWidget *parent) : QWidget(parent)
{
    _pDevManagerCtrl = pCtrl;



    InitUI();
}

void QDevManagerWidget::InitUI()
{
    //能量计设备
    m_pEnergMeterWidget = new QEnergyMeterWidget(_pDevManagerCtrl->m_pEnergyMeterDevManager,&_pDevManagerCtrl->m_pDevConfigData->m_tEnergyConfigInfoList);
    //示波器
    m_pOsciiWidget = new QOsciCtrlWidget(_pDevManagerCtrl->m_pOsciDevCtrl,this);

    //科学CCD
    m_pKxccdCtrlWidget = new QKxccdctrlWidget(_pDevManagerCtrl->m_pKxccdCtrl,this);

    //显微监视
    m_pMicroWidget = new QIDSWidget(this);
    m_pMicroWidget->getDevHandle(_pDevManagerCtrl->m_pIDSDevCtrl);
    m_pMicroWidget->showDevStatus();

    //电机设备
    m_pMotorCtrlWidget = new QMotorCtrlWidget(&_pDevManagerCtrl->m_pAllDevConfigInfo->m_tMotorConfigInfoList,this);
    m_pMotorCtrlWidget->getDevHandle(_pDevManagerCtrl->m_pMotorDevManager);

    //光闸
    m_pShutterCtrlWidget = new QShutterCtrlWidget(_pDevManagerCtrl);


}
