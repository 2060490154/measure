#include <qgridlayout.h>
#include "QStystemMove2SafeWidget.h"



QStystemMove2SafeWidget::QStystemMove2SafeWidget()
{
    m_pWorkflowTab = new QWorkflowTab();
    m_pWorkflowTab->setLabelTitle("运动安全位置流程", ":/png/dev.png", "rgb(68,69,73)");

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(m_pWorkflowTab,0,0,1,1);

}


QStystemMove2SafeWidget::~QStystemMove2SafeWidget()
{
}
