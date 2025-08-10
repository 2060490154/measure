/***************************************************************************
**                                                                        **
** 文件描述：设备状态数据监控
**                                                                        **
****************************************************************************
** 创建人：李刚
** 创建时间：
** 修改记录：
**
****************************************************************************/
#include "qdevmonitorwidget.h"

QDevMonitorWidget::QDevMonitorWidget(QDevManagerCtrl* pDevCtrl,QWidget *parent) : QWidget(parent)
{
    _pDevManagerCtrl = pDevCtrl;
    InitUI();

    //更新显示
    for(int i = 0; i < M_MOTOR_CTRLDEV_NUM;i++)
    {
        connect(_pDevManagerCtrl->m_pMotorDevManager->m_pMotorCtrl[i],&QMotorCtrl::UpdateStatus,m_pMotorDevInfoWidget,&QMotorMonitorWidget::onShowMotorPos);
    }
    connect(_pDevManagerCtrl,&QDevManagerCtrl::signal_capIDSImageBuff,m_pIDSDevInfoWidget,&QRunTimeImageWidget::onShowIDSCapImage);//显示图片
    connect(_pDevManagerCtrl,&QDevManagerCtrl::signal_updateEnergyData,m_pEnergyDevInfoWidget,&QEnergyDevInfoWidget::onShowEnergyData);//显示能量计数据
    connect(_pDevManagerCtrl,&QDevManagerCtrl::signal_capKxccdImageBuff,m_pIDSDevInfoWidget,&QRunTimeImageWidget::onShowKxccdCapImage);//显示kxccd图片
    connect(_pDevManagerCtrl->m_pOsciDevCtrl,&QOsciDevCtrl::signal_showOsciData,m_pOSciiDevInfoWidget,&QOSciiDevInfoWidget::onShowData);//显示示波器数据
}



 void QDevMonitorWidget::InitUI()
 {
      m_pDevMonitorTableWidget = new QTabWidget(this);

      m_pEnergyDevInfoWidget =  new QEnergyDevInfoWidget(&_pDevManagerCtrl->m_pDevConfigData->m_tEnergyConfigInfoList);
      m_pMotorDevInfoWidget = new QMotorMonitorWidget(&_pDevManagerCtrl->m_pAllDevConfigInfo->m_tMotorConfigInfoList);
      m_pIDSDevInfoWidget = new QRunTimeImageWidget();
      m_pOSciiDevInfoWidget = new QOSciiDevInfoWidget();
      m_pExpResultWidget = new QExpResultWidget(m_sExpNo);


      m_pDevMonitorTableWidget->addTab(m_pIDSDevInfoWidget,"实时图像数据");
      m_pDevMonitorTableWidget->addTab(m_pMotorDevInfoWidget,"电机数据显示");
      m_pDevMonitorTableWidget->addTab(m_pEnergyDevInfoWidget,"能量计数据显示");
      m_pDevMonitorTableWidget->addTab(m_pOSciiDevInfoWidget,"示波器数据显示");
      m_pDevMonitorTableWidget->addTab(m_pExpResultWidget,"损伤检测结果");


      QGridLayout* pLayout = new QGridLayout(this);
      pLayout->addWidget(m_pDevMonitorTableWidget);

 }


 void QDevMonitorWidget::getNewExpNo(QString sExpNo,QString sSampleName)
 {
     m_sExpNo = sExpNo;
     m_pExpResultWidget->m_sExpNo = sExpNo;
 }
