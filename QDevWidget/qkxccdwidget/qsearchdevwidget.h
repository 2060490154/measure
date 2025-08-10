#ifndef QSEARCHDEVWIDGET_H
#define QSEARCHDEVWIDGET_H

#include <QWidget>
#include <qpushbutton.h>
#include <QListWidget>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include "QKxccdCtrl/qkxccdctrl.h"

class QSearchDevWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSearchDevWidget(QKxccdCtrl* pKxccdCtrl,QWidget *parent = nullptr);

signals:

public:
    void closeEvent(QCloseEvent *event);

public slots:
    void onSearchTimer();

    void onRefreshFun();

    void onCloseWindow();


private:
    QListWidget* m_pListWidget;
    QPushButton* m_pRefreshButton;//刷新按钮
    QPushButton* m_pCloseButton;

    QLabel* m_pshowInfoLabel;

    QTimer* m_pSearchTimer;

    int m_nSearchTime;


private:
    QKxccdCtrl* _pKxccdCtrl;
};

#endif // QSEARCHDEVWIDGET_H
