#include "qworkflowtab.h"
#include <QGridLayout>
#include <QHeaderView>
#include <QDebug>

QWorkflowTab::QWorkflowTab()
{
    m_pWorkFlowList = NULL;
    m_pUpdateStatusTimer = new QTimer();
    connect(m_pUpdateStatusTimer,&QTimer::timeout,this,&QWorkflowTab::upDateStauts);



    m_pLabel = new QLabel(this);
    m_pListWidget = new QTableWidget(this);

    QGridLayout* playout = new QGridLayout(this);


    playout->setRowStretch(0,1);
    playout->setRowStretch(1,20);
    playout->setSpacing(0);

    playout->addWidget(m_pLabel,0,0,1,1);
    playout->addWidget(m_pListWidget,1,0,1,1);

    InitTabHeader();


}
QWorkflowTab::~QWorkflowTab()
{
    m_pUpdateStatusTimer->stop();
}

//设置标题
void QWorkflowTab::setLabelTitle(QString sTitle,QString sIConPath,QString sRgb)
{
    m_pLabel->setStyleSheet("color:rgb(255,255,255);background-color:"+sRgb+";");

    if(sTitle !="" && sIConPath !="")
    {
        m_pLabel->setText("\
                        <table width=\"100%\">\
                        <tbody>\
                        <tr>\
                        <td><img src=\""+sIConPath+"\" ></td>\
                        <td align=\"left\" valign=\"middle\"> "+sTitle+"</td>\
                        </tr>\
                        </tbody>\
                        </table>");

    }

     QFont font =  m_pLabel->font();
     font.setFamily("微软雅黑");
     font.setBold(true);
     font.setPixelSize(14);

     m_pLabel->setFont(font);
}

//设置列表内容 nWorkflowType--0  准备流程  nWorkflowType--1测试流程 显示的内容不一样
void QWorkflowTab::setWorkFlowList(int nWorkflowType)
{
    QString str;
    int nCnt = 0;
    int nStartRowCnt = 0;
    if(m_pWorkFlowList == NULL)
    {
        return;
    }

    if(m_pUpdateStatusTimer->isActive())
    {
        m_pUpdateStatusTimer->stop();
    }

    InitTabHeader();
    m_pListWidget->setRowCount(0);
    m_pListWidget->clearContents();


    for(int i = 0; i < m_pWorkFlowList->size();i++)
    {
        int nMoveStepCnt = m_pWorkFlowList->at(i)->m_pMoveWorkFlow.size();
        int nSetStepCnt = m_pWorkFlowList->at(i)->m_pSetWorkFlow.size();
        int nCapStepCnt = m_pWorkFlowList->at(i)->m_pCapWorkFlow.size();
		int nDataProcessCnt = m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow.size();
		int nTotalCnt = nMoveStepCnt + nSetStepCnt + nCapStepCnt + nDataProcessCnt;

        for(int nRow = 0; nRow < nTotalCnt;nRow++)
        {
            m_pListWidget->insertRow(m_pListWidget->rowCount());
        }
        //合并单元格
        if(nTotalCnt > 1)
        {
            m_pListWidget->setSpan(nStartRowCnt,0,nTotalCnt,1);
        }

        if(nWorkflowType == M_MEASURETYPE_STANDBY)
        {
            str = QString("第%1步").arg(i+1);
        }
        else if (nWorkflowType == M_MEASURETYPE_ROn1 || nWorkflowType == M_MEASURETYPE_SOn1)
        {
            str = QString("第%1点 第%2发次").arg(m_pWorkFlowList->at(i)->m_pCapWorkFlow[0].m_tExpInfo.m_nPointNo + 1).arg(m_pWorkFlowList->at(i)->m_pCapWorkFlow[0].m_tExpInfo.m_nTimes + 1);
        }
        else
        {
            str = QString("第%1点").arg(i+1);
        }
//
        m_pListWidget->setItem(nStartRowCnt,0,new QTableWidgetItem(str));
        if(nMoveStepCnt != 0 )
        {
            if(nMoveStepCnt > 1)
            {
                 m_pListWidget->setSpan(nStartRowCnt,1,nMoveStepCnt,1);
            }

            m_pListWidget->setItem(nStartRowCnt,1,new QTableWidgetItem("运动"));
        }

        if(nSetStepCnt != 0 )
        {
            if(nSetStepCnt > 1)
            {
                m_pListWidget->setSpan(nStartRowCnt+nMoveStepCnt,1,nSetStepCnt,1);
            }

            m_pListWidget->setItem(nStartRowCnt+nMoveStepCnt,1,new QTableWidgetItem("设置"));
        }

        //SOn1需要特殊处理  每个点打N发 显示一起的话会太长
//        if(nCapStepCnt != 0  && nWorkflowType == M_MEASURETYPE_SOn1)
//        {
//            int nGapCnt = nCapStepCnt/m_pWorkFlowList->at(i)->m_nSOn1ShotCnt;
//            for(int k = 0; k < m_pWorkFlowList->at(i)->m_nSOn1ShotCnt;k++)
//            {
//                str = QString("第%1发采集").arg(k+1);
//                if(nGapCnt > 1)
//                {
//                   m_pListWidget->setSpan(nStartRowCnt+nMoveStepCnt+nSetStepCnt+k*nGapCnt,1,nGapCnt,1);
//                }

//                  m_pListWidget->setItem(nStartRowCnt+nMoveStepCnt+nSetStepCnt+k*nGapCnt,1,new QTableWidgetItem(str));
//            }
//        }
//        else    if(nCapStepCnt != 0 )
//        {
//            if(nCapStepCnt > 1)
//            {
//                m_pListWidget->setSpan(nStartRowCnt+nMoveStepCnt+nSetStepCnt,1,nCapStepCnt,1);
//            }

//            m_pListWidget->setItem(nStartRowCnt+nMoveStepCnt+nSetStepCnt,1,new QTableWidgetItem("采集"));
//        }



        if(nCapStepCnt != 0 )
        {
            if(nCapStepCnt > 1)
            {
                m_pListWidget->setSpan(nStartRowCnt+nMoveStepCnt+nSetStepCnt,1,nCapStepCnt,1);
            }

            m_pListWidget->setItem(nStartRowCnt+nMoveStepCnt+nSetStepCnt,1,new QTableWidgetItem("采集"));
        }

		if (nDataProcessCnt != 0)
		{
            if(nDataProcessCnt > 1)
            {
               m_pListWidget->setSpan(nStartRowCnt + nMoveStepCnt + nSetStepCnt + nCapStepCnt, 1, nDataProcessCnt, 1);
            }

			m_pListWidget->setItem(nStartRowCnt + nMoveStepCnt + nSetStepCnt + nCapStepCnt, 1, new QTableWidgetItem("数据处理"));
		}


        nStartRowCnt = nStartRowCnt + nTotalCnt;

        for(int j = 0; j < nMoveStepCnt;j++)
        {

            m_pListWidget->setItem(nCnt,2,new QTableWidgetItem(m_pWorkFlowList->at(i)->m_pMoveWorkFlow[j].m_sStepName));

            m_pListWidget->setItem(nCnt,3,new QTableWidgetItem("未执行"));
            nCnt++;
        }

        for(int j = 0; j < nSetStepCnt;j++)
        {
            m_pListWidget->setItem(nCnt,2,new QTableWidgetItem(m_pWorkFlowList->at(i)->m_pSetWorkFlow[j].m_sStepName));

            m_pListWidget->setItem(nCnt,3,new QTableWidgetItem("未执行"));
            nCnt++;
        }

        for(int j = 0; j < nCapStepCnt;j++)
        {
            m_pListWidget->setItem(nCnt,2,new QTableWidgetItem(m_pWorkFlowList->at(i)->m_pCapWorkFlow[j].m_sStepName));
            m_pListWidget->setItem(nCnt,3,new QTableWidgetItem("未执行"));
            nCnt++;
        }

		for (int j = 0; j < nDataProcessCnt; j++)
		{
			m_pListWidget->setItem(nCnt, 2, new QTableWidgetItem(m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow[j].m_sStepName));
			m_pListWidget->setItem(nCnt, 3, new QTableWidgetItem("未执行"));
			nCnt++;
		}

    }

    m_pUpdateStatusTimer->start(800);//1s刷新一次
}

//初始化列表头
void QWorkflowTab::InitTabHeader()
{
    m_pListWidget->clear();
    m_pListWidget->verticalHeader()->setHidden(true);//垂直表头 不可见
    m_pListWidget->horizontalHeader()->setHidden(true);//垂直表头 不可见
    m_pListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    m_pListWidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满

    m_pListWidget->setColumnCount(4);
    m_pListWidget->setColumnWidth(0,50);
    m_pListWidget->setColumnWidth(1,100);
    m_pListWidget->setColumnWidth(2,300);
    m_pListWidget->setColumnWidth(3,100);

}

 void QWorkflowTab::upDateStauts()
 {
     int nCnt = 0;
     if(m_pWorkFlowList == NULL)
     {
         return;
     }

     for(int i = 0; i < m_pWorkFlowList->size();i++)
     {
         for(int j = 0; j < m_pWorkFlowList->at(i)->m_pMoveWorkFlow.size();j++)
         {
             QTableWidgetItem* pItem = m_pListWidget->item(nCnt,3);
             if(m_pWorkFlowList->at(i)->m_pMoveWorkFlow[j].m_nStepStatus == 0)
             {
                  pItem->setText("未执行");
                  pItem->setBackground(QBrush(QColor(255, 255, 255)));
             }
             if(m_pWorkFlowList->at(i)->m_pMoveWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_RUNNING)
             {
                 pItem->setText("正在执行");
                 pItem->setBackground(QBrush(QColor(250, 255, 10)));
             }
             if(m_pWorkFlowList->at(i)->m_pMoveWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_FAILD)
             {
                 pItem->setText("执行失败");
                 pItem->setBackground(QBrush(QColor(255, 70, 70)));
             }
             if(m_pWorkFlowList->at(i)->m_pMoveWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SUCCESS)
             {
                 pItem->setText("执行成功");
                  pItem->setBackground(QBrush(QColor(0, 255, 0)));
             }
             if(m_pWorkFlowList->at(i)->m_pMoveWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SKIP)
             {
                 pItem->setText("跳过");
                 pItem->setBackground(QBrush(QColor(0, 200, 255)));
             }

              nCnt++;
         }

         for(int j = 0; j < m_pWorkFlowList->at(i)->m_pSetWorkFlow.size();j++)
         {
             QTableWidgetItem* pItem = m_pListWidget->item(nCnt,3);
             if(m_pWorkFlowList->at(i)->m_pSetWorkFlow[j].m_nStepStatus == 0)
             {
                  pItem->setText("未执行");
                  pItem->setBackground(QBrush(QColor(255, 255, 255)));
             }
             if(m_pWorkFlowList->at(i)->m_pSetWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_RUNNING)
             {
                 pItem->setText("正在执行");
                 pItem->setBackground(QBrush(QColor(250, 255, 10)));
             }
             if(m_pWorkFlowList->at(i)->m_pSetWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_FAILD)
             {
                 pItem->setText("执行失败");
                 pItem->setBackground(QBrush(QColor(255, 70, 70)));
             }
             if(m_pWorkFlowList->at(i)->m_pSetWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SUCCESS)
             {
                 pItem->setText("执行成功");
                 pItem->setBackground(QBrush(QColor(0, 255, 0)));
             }

             if(m_pWorkFlowList->at(i)->m_pSetWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SKIP)
             {
                 pItem->setText("跳过");
                 pItem->setBackground(QBrush(QColor(0, 200, 255)));
             }
            nCnt++;
         }


         for(int j = 0; j < m_pWorkFlowList->at(i)->m_pCapWorkFlow.size();j++)
         {
             QTableWidgetItem* pItem = m_pListWidget->item(nCnt,3);
             if(m_pWorkFlowList->at(i)->m_pCapWorkFlow[j].m_nStepStatus == 0)
             {
                  pItem->setText("未执行");
                  pItem->setBackground(QBrush(QColor(255, 255, 255)));
             }
             if(m_pWorkFlowList->at(i)->m_pCapWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_RUNNING)
             {
                 pItem->setText("正在执行");
                 pItem->setBackground(QBrush(QColor(250, 255, 10)));
             }
             if(m_pWorkFlowList->at(i)->m_pCapWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_FAILD)
             {
                 pItem->setText("执行失败");
                 pItem->setBackground(QBrush(QColor(255, 70, 70)));
             }
             if(m_pWorkFlowList->at(i)->m_pCapWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SUCCESS)
             {
                 pItem->setText("执行成功");
                 pItem->setBackground(QBrush(QColor(0, 255, 0)));
             }

             if(m_pWorkFlowList->at(i)->m_pCapWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SKIP)
             {
                 pItem->setText("跳过");
                 pItem->setBackground(QBrush(QColor(0, 200, 255)));
             }
            nCnt++;
         }

         for(int j = 0; j < m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow.size();j++)
         {
             QTableWidgetItem* pItem = m_pListWidget->item(nCnt,3);
             if(m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus == 0)
             {
                  pItem->setText("未执行");
                  pItem->setBackground(QBrush(QColor(255, 255, 255)));
             }
             if(m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_RUNNING)
             {
                 pItem->setText("正在执行");
                 pItem->setBackground(QBrush(QColor(250, 255, 10)));
             }
             if(m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_FAILD)
             {
                 pItem->setText("执行失败");
                 pItem->setBackground(QBrush(QColor(255, 70, 70)));
             }
             if(m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SUCCESS)
             {
                 pItem->setText("执行成功");
                 pItem->setBackground(QBrush(QColor(0, 255, 0)));
             }

             if(m_pWorkFlowList->at(i)->m_pDataProcessWorkFlow[j].m_nStepStatus ==M_STEP_STATUS_SKIP)
             {
                 pItem->setText("跳过");
                 pItem->setBackground(QBrush(QColor(0, 200, 255)));
             }
            nCnt++;
         }

     }


 }
