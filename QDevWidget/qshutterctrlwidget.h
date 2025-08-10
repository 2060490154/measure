#ifndef QSHUTTERCTRLWIDGET_H
#define QSHUTTERCTRLWIDGET_H

#include <QWidget>
#include <QShutterCtrl/QShutterManagerCtrl.h>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QTextEdit>
#include "qdevmanagerctrl.h"
#include <QTextBrowser>//ywc
#include <QLineEdit>

class QShutterCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QShutterCtrlWidget( QDevManagerCtrl* pCtrl,QWidget *parent = nullptr);

signals:

public slots:


public:
    QShutterDevCtrl *m_pShutterCtrl;
    QAttenuatorDevCtrl *m_pPortCtrl2;
    QAttenuatorDevCtrl *m_pPortCtrl1;
//    QAttenuatorDevCtrl *m_pPortCtrl2;
    QAttenuatorDevCtrl *m_pPortCtrl3;

};


class QShutterCtrlItemWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit QShutterCtrlItemWidget( QShutterDevCtrl* pCtrl,int nIndex,QWidget *parent = nullptr);

signals:

public slots:

    void onOpenShutter();//打开 不吸合
    void onCloseShutter();//吸合
    void onCheckStatus();//查询
    void onSetLimitData();//设置门限值

    void onSetProcessStatus();//设置流程状态

public:
    void InitUI();

    void showStatus();




public:
    //界面元素
    QLabel* m_pStatus;
    QTextEdit* m_pSetLimitEdit;

    QTextEdit* m_psetPlusCntEdit;
    QTextEdit* m_psetDelayEdit;

    QLabel* m_pshowCurrentDataLabel;
    QPushButton* m_pOpenButton;
    QPushButton*m_pCloseButton;
    QPushButton*m_pRefreshButton;
    QPushButton*m_pSetLimitDataButton;
    QPushButton*m_psetProcessStatusButton;//设置流程模式


private:
    QShutterDevCtrl*_pShutterCtrl;
    int _nIndex;
};

class QCOMCtrlItemWidget : public QGroupBox//ywc
{
    Q_OBJECT
public:
    explicit QCOMCtrlItemWidget(QAttenuatorDevCtrl* pCtrl,int nIndex,QWidget *parent = nullptr);

signals:

public slots:

    void onOpenCOM();   //打开串口
    void onRelaseCOM();//释放串口
    void SlotClickSendMsgButton();
    void SlotClickSendPWRMsgButton();
    void SlotClickSendCommandMsgButton();
    void DisplayRcvTextAfterSendBtn();
    void SlotClickSetHomeButton();
//    void onCheckStatus();//查询
//    void onSetLimitData();//设置门限值

//    void onSetProcessStatus();//设置流程状态

public:
    void InitUI();
    void log(const QString& str);
    void showStatus();




public:
    //界面元素
    QLabel* m_pStatus;
    QLabel* plabel1;
    QLabel* plabel2;
    QLabel* plabel3;
    QLabel* plabel4;

    QLabel* plabel6;
    QLabel* plabel8;
    QTextEdit* MsgTextEdit;

    QLineEdit* PWRLineEdit;
    QLineEdit* CommandLineEdit;

    QPushButton* m_pOpenButton;
    QPushButton*m_pCloseButton;
    QPushButton*m_pRefreshButton;
    QPushButton*m_pSendMsgButton;
    QPushButton* m_pSendPWRMsgButton;
    QPushButton* m_pSendCommandMsgButton;
    QTextBrowser *textbrower;



private:
    QAttenuatorDevCtrl*_pPortCtrl;
    int _nIndex;

};

#endif // QSHUTTERCTRLWIDGET_H
