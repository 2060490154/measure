#ifndef QNAVWIDGET_H
#define QNAVWIDGET_H

#include <QWidget>
#include <qlistwidget.h>
#include <qstackedwidget.h>


struct Item
{
    int nItemCnt;//分组总个数
    int nItemIndex;//分组索引
    bool bExpand;
};


class QNavWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QNavWidget(QWidget *parent = 0);

signals:
    void changeview(int nIndex);//nRow分组 nIndex为当前分组的索引

public slots:
    void onExpandItem();
    void onlistCheck();
    void onchangeView(int nIndex);

public:
    void InitUI();
    void ShowNavListItem();
    void SetNavItem(QString sItemName,QStringList SubItemlist,QString sIcon);



public:
        QListWidget* m_pNavList;//导航栏

        QList<struct Item> m_tItemList;//导航分组

        QStackedWidget* m_pUserViews;//视图切换
};

#endif // QNAVWIDGET_H
