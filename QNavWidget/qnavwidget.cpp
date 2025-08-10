/***************************************************************************
**                                                                        **
** 文件描述：本文件定义了导航栏的操作功能
**                                                                        **
****************************************************************************
** 创建人：李刚
** 创建时间：2018-05-16
** 修改记录：
**
****************************************************************************/
#include "qnavwidget.h"
#include <QGridLayout>
#include <qfile.h>
QNavWidget::QNavWidget(QWidget *parent) : QWidget(parent)
{
    InitUI();

}
/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::InitUI()
{

    m_pNavList = new QListWidget(this);
    m_pNavList->setObjectName("NavList");//设置ID

    connect(m_pNavList,&QListWidget::itemDoubleClicked,this,&QNavWidget::onExpandItem);//展开或收缩
    connect(m_pNavList,&QListWidget::itemClicked,this,&QNavWidget::onlistCheck);//展开或收缩

    //消息
    connect(this,&QNavWidget::changeview,this,&QNavWidget::onchangeView);

    //导航栏设置
    m_pNavList->addItem(" ");

    m_pNavList->setFocusPolicy(Qt::NoFocus);


   //布局
   QGridLayout* pGridlayout = new QGridLayout();
   pGridlayout->addWidget(m_pNavList,0,0,1,1);
   pGridlayout->setSpacing(0);
   pGridlayout->setMargin(0);

   this->setLayout(pGridlayout);
}


void QNavWidget::ShowNavListItem()
{
    for(int i = 0; i < m_tItemList.size();i++)
    {
        for(int j = m_tItemList[i].nItemIndex + 1; j < m_tItemList[i].nItemIndex + 1 + m_tItemList[i].nItemCnt;j++)
        {
            m_pNavList->item(j)->setHidden(!m_tItemList[i].bExpand);
        }

    }
}

/*******************************************************************
**功能：导航栏选择
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::onExpandItem()
{

    int nRowIndex = m_pNavList->currentRow();

    for(int i = 0; i < m_tItemList.size();i++ )
    {
        if(nRowIndex == m_tItemList[i].nItemIndex)
        {
            m_tItemList[i].bExpand = !m_tItemList[i].bExpand;
        }
    }

    ShowNavListItem();

}

/*******************************************************************
**功能：单击事件
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::onlistCheck()
{
    int nCurrentId = -1;
    int nRowIndex = m_pNavList->currentRow();

    for(int i = 0; i < m_tItemList.size();i++ )
    {
        if(nRowIndex > m_tItemList[i].nItemIndex && nRowIndex <= m_tItemList[i].nItemCnt + m_tItemList[i].nItemIndex)
        {
           emit changeview(nCurrentId + nRowIndex-m_tItemList[i].nItemIndex);//第几分组中的第几个
        }

        nCurrentId = nCurrentId + m_tItemList[i].nItemCnt;
    }


}


/*******************************************************************
**功能：新增分组
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::SetNavItem(QString sItemName,QStringList SubItemlist,QString sIcon)
{
    QFont font = m_pNavList->font();

    font.setFamily("微软雅黑");
    font.setBold(true);
    font.setPixelSize(22);

    int nRowCnt = m_pNavList->count();
    m_pNavList->addItem(sItemName);

     struct Item Temp;
     Temp.bExpand = true;
     Temp.nItemIndex = nRowCnt;
     Temp.nItemCnt = SubItemlist.size();
     m_tItemList.append(Temp);//添加到分组信息中


     for(int i = 0; i < SubItemlist.size();i++)
     {
         m_pNavList->addItem("       ● "+SubItemlist[i]);
     }

    m_pNavList->item(nRowCnt)->setSizeHint(QSize(60,80));
    m_pNavList->item(nRowCnt)->setFont(font);
    m_pNavList->item(nRowCnt)->setIcon(QIcon(sIcon));
}


/*******************************************************************
**功能：切换视图
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::onchangeView(int nIndex)
{

    m_pUserViews->setCurrentIndex(nIndex);

}
