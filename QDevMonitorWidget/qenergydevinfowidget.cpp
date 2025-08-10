/***************************************************************************
**                                                                        **
** 文件描述：能量计数据状态监控
**                                                                        **
****************************************************************************
** 创建人：李刚
** 创建时间：
** 修改记录：
**
****************************************************************************/
#include "qenergydevinfowidget.h"

QEnergyDevInfoWidget::QEnergyDevInfoWidget(QList<tEnergyMeterConfigInfo*>* pConfigList,QWidget *parent) : QWidget(parent)
{
    m_nDevicNum = 0;
    m_pEnergyMeterConfig = pConfigList;
    if(m_pEnergyMeterConfig != NULL)
    {
        m_nDevicNum = m_pEnergyMeterConfig->size();
    }


    m_pTipLabel = new QLabel("未发现能量计设备");
    m_pTipLabel->setAlignment(Qt::AlignCenter);

    CreateUI();

}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QEnergyDevInfoWidget::CreateUI()
{
    QString sTemp;
    QVBoxLayout* pVlayout = new QVBoxLayout(this);

    if(m_nDevicNum == 0)
    {
        pVlayout->addWidget(m_pTipLabel);
        m_pTipLabel->setHidden(false);
    }
    else
        m_pTipLabel->setHidden(true);

    qDeleteAll(m_tTextEditArray);
    m_tTextEditArray.clear();

    for(int i = 0; i < m_nDevicNum;i++)
    {
        QHBoxLayout* pHlayout = new QHBoxLayout();

        sTemp = QString("%1数据:").arg(m_pEnergyMeterConfig->at(i)->sChannelName);

        QLabel* plabel = new QLabel(sTemp);

        QLabel* pTextEdit = new QLabel();
        pTextEdit->setText("0.0    mJ");
        m_tTextEditArray.append(pTextEdit);

        pTextEdit->setMaximumHeight(30);
        pHlayout->addWidget(plabel);
        pHlayout->addWidget(pTextEdit);
        pVlayout->addLayout(pHlayout);
    }

}

void QEnergyDevInfoWidget::onShowEnergyData(int nIndex, QString sValue, QVariant tTaskExpInfo)
{
    tEnergyMeterConfigInfo* pItemConfig = m_pEnergyMeterConfig->at(nIndex);
    sValue = QString::number(sValue.toDouble()* pItemConfig->lfCoeff);
    if(nIndex >=0 && nIndex <= m_nDevicNum - 1)
    {
        m_tTextEditArray[m_pEnergyMeterConfig->at(nIndex)->nDevIndex]->setText(sValue+" mJ");
    }

}
