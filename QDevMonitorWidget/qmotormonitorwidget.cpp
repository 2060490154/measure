/***************************************************************************
**                                                                        **
** 文件描述：
**                                                                        **
****************************************************************************
** 创建人：李刚
** 创建时间：
** 修改记录：
**
****************************************************************************/
#include "qmotormonitorwidget.h"
#include "define.h"
#include <QDebug>

QMotorMonitorWidget::QMotorMonitorWidget(QList<tMotorConfigInfo>* pMotorConfigList,QWidget *parent) : QWidget(parent)
{

    _pMotorConfigList = pMotorConfigList;
    InitUI();

}


/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorMonitorWidget::InitUI()
{
    QString sTemp;
    QVBoxLayout* pVlayout = new QVBoxLayout(this);


    qDeleteAll(m_tTextEditArray);
    m_tTextEditArray.clear();

    for(int i = 0; i < _pMotorConfigList->size();i++)
    {
        QHBoxLayout* pHlayout = new QHBoxLayout();

        tMotorConfigInfo item = _pMotorConfigList->at(i);

        sTemp = QString("%1位置: ").arg(item.sMotorName);

        QLabel* plabel = new QLabel(sTemp);

        QLabel* pTextEdit = new QLabel();
        pTextEdit->setText("0");
        m_tTextEditArray.append(pTextEdit);

        pTextEdit->setMaximumHeight(30);
        pHlayout->addWidget(plabel);
        pHlayout->addWidget(pTextEdit);
        pVlayout->addLayout(pHlayout);
    }

}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorMonitorWidget::onShowMotorPos(int nMotorCtrlIndex,int nMotorIndex,QVariant varstatus)
{
    motorStatusdata tTempmotorStatus = varstatus.value<motorStatusdata>();
    motorStatusdata* pStatus  = &tTempmotorStatus;

    for(int i = 0; i < _pMotorConfigList->size();i++)
    {
        tMotorConfigInfo item = _pMotorConfigList->at(i);
        if(item.nMotorIndex == nMotorIndex &&item.nMotorCtrlIndex == nMotorCtrlIndex)
        {
            if(item.nMortorType == 0)
            {
                m_tTextEditArray[i]->setText(QString("%1 mm").arg((float)pStatus->m_nCurrentLPV/(float)item.nMotorCoeff,0,'f',3));
            }
            if(item.nMortorType == 1)
            {
                m_tTextEditArray[i]->setText(QString("%1 度").arg((float)pStatus->m_nCurrentLPV/(float)item.nMotorCoeff,0,'f',3));
            }
            break;
        }
    }
    this->update();
}
