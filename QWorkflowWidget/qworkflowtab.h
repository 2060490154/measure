#ifndef QWORKFLOWTAB_H
#define QWORKFLOWTAB_H

#include <QWidget>
#include <qlabel.h>
#include <QTabWidget.h>
#include <QTableWidgetItem>
#include <qlist.h>
#include <QTimer>
#include "define.h"

class QWorkflowTab:public QWidget
{
    Q_OBJECT

public slots:
     void upDateStauts();
public:
    QWorkflowTab();
    ~QWorkflowTab();



    void setLabelTitle(QString sTitle,QString sIConPath,QString sRgb);
    void InitTabHeader();
    void setWorkFlowList(int nWorkflowType);

public:
    QLabel* m_pLabel;//显示标题
    QTableWidget* m_pListWidget;//测试流程显示

    QList<QWorkFlowItem*> *m_pWorkFlowList;
    QTimer* m_pUpdateStatusTimer;




};

#endif // QWORKFLOWTAB_H
