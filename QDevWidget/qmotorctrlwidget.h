#ifndef QMOTORCTRLWIDGET_H
#define QMOTORCTRLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <qlistwidget.h>
#include <qcombobox.h>
#include "QMotorCtrl/qmotorctrl.h"
#include "qmotordevmanager.h"
#include "qdbdata.h"
#include "qmotorrunthread.h"


//单个电机的控制模块
class QMotorCtrlItemWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit QMotorCtrlItemWidget(const tMotorConfigInfo* pMotorConfigInfo,QMotorCtrl* pMotorCtrl,QWidget *parent = 0);
    void InitUI();
    void showLimitStatus(int nLeftStatus,int nRightStatus);
    void setlogInfo(QString slog);

    void creatPostionUI();
    void creatNoPostionUI();


signals:

public slots:
    void onUpdateMotorInfo(int nMotorCtrlIndex,int nIndex,QVariant varStatus);

    void onMoveLeft();
    void onMoveRight();
   // void OnStopMove();
    void OnMove2Zero();

    void onStartMove();//开始运动
    void onStopMove();//停止运动




public:

    //控制信息
    QMotorCtrl* m_pMotorCtrl;
   const  tMotorConfigInfo* m_pMotorConfigInfo;
    int m_nMotorIndex;//电机编号 0-3
    int m_nMotorCtrlIndex;//控制箱编号0-2

    //不带档位信息的UI
    QTextEdit* m_pPostionEdit;
    QPushButton* m_pMoveLeftBtn;
    QPushButton* m_pMoveRightBtn;
    QPushButton* m_pImmidStopBtn;
    QPushButton* m_pReturnZeroBtn;

    QLabel* m_pLeftLimitStatusLabel;//左限位状态显示
    QLabel* m_pRightLimitStatusLabel;//由限位状态显示
    QLabel* m_pCurrentPosLabel;

    //配置档位信息的Ui
    QComboBox* m_pPostionCombox;
    QPushButton* m_pStartMoveBtn;


    QListWidget* m_pLogList;

    QMotorRunThread* m_pRunThread;

};

class QMotorCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMotorCtrlWidget(QList<tMotorConfigInfo>* pConfigList,QWidget *parent = 0);
    void InitUI();
    void getDevHandle(QMotorDevManager* pMotorDevManager);

signals:

public slots:

public:
    QMotorCtrlItemWidget* m_pMotorWidget[M_MOTOR_TOTALNUM];
    QMotorCtrl* m_pMotorCtrl;

    QList<tMotorConfigInfo>* m_pMotorConfigInfoList;

};






#endif // QMOTORCTRLWIDGET_H
