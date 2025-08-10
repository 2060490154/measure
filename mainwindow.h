#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qdockwidget.h>
#include <qfile.h>
#include <QGridLayout>
#include <qsplashscreen.h>
#include "qworkflowmainwidget.h"
#include "qnavwidget.h"
#include "qdevstatuswidget.h"
#include "qexpinfowidget.h"
#include "qsystemparamwidget.h"
#include "QStystemMove2SafeWidget.h"
#include "qdevmanagerwidget.h"
class QQueryWidget;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public:
    void InitUI();

public slots:
    void ShowCheckStatusBox(QWorkTask* pTask);
    void ontryShowWindow(QVariant tTaskExpInfo);
    void on_updateDetectMethod();

public:
    QNavWidget* m_pNavWidget;

    QDevStatusWidget* m_pDevStatusWidget;//设备状态显示

    QStackedWidget* m_pUserViews;//多视图
    QDevManagerWidget* m_pDevManagerWidget;//设备视图
    QDevManagerCtrl* m_pDevManagerCtrl;//设备控制类

    QStystemMove2SafeWidget* m_pSystemMove2SafeWidget;//系统安全位置
    QSystemParamWidget* m_pSystemParamWidget;//系统参数配置界面

    QExpInfoWidget* m_pExpInfoWidget;   //实验信息汇总

    QQueryWidget* m_pQueryWidget;   //數據查詢

    QDatabaseProcess* m_pDbProcess;
    QDBData* m_pDbData;//数据库中的配置数据
    QConfigProcess* m_pConfigProcess;//配置文件处理


    //流程控制界面
    QWorkFlowWidget* m_p1On1Widget;
    QWorkFlowWidget* m_pSOn1Widget;
    QWorkFlowWidget* m_pROn1Widget;
    QWorkFlowWidget* m_pRasterWidget;


    //try
    QWidget *m_pWidgetShow;
    QPushButton *m_pBtnShow;


};

#endif // MAINWINDOW_H
