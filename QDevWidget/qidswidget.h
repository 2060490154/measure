#ifndef QIDSWidget_H
#define QIDSWidget_H

#include <QWidget>
#include <qcombobox.h>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QMouseEvent>
#include <qscrollbar.h>
#include <qlabel.h>
#include <QDockWidget>
#include <qscrollarea.h>
#include <QCheckBox>
#include "qidscamerdevctrl.h"

class QIDSWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QIDSWidget(QWidget *parent = 0);

signals:

public slots:
    void onConnectDev();
    void onDisConnectDev();
    void onContinueActiveMode();//连续触发采集
    void onHardwareActiveMode();

//    void onShowImage(char* pbuff,int nwidth,int nhigh);//显示图像
    void onShowImage(QString sFilePath);//显示图像
    void choosePathButton();//选择保存路径//yhy0319
    void onOpenImageFile();//打开图片
    void onSaveAsImageFile();//保存图片
    void onSetExposureTime();//设置曝光时间


public:
    void InitUI();
    void InitDock();
    QGroupBox* CreateWorkModeGroupBox();
    QGroupBox* CreateOperationGroupBox();
    QGroupBox* CreateImageProcessGroup();
    void SetButtonICon(QPushButton* pButton,QString sPngName);

    void SetCheckBoxStatus(QCheckBox* pBox,bool bCheck);
    void ShowLogInfo(QString slog);
    void showDevStatus();//显示当前设备状态信息
    void getDevHandle(QIDSCamerDevCtrl* pDevCtrl);

    void paintEvent(QPaintEvent *event);//显示图像
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);//滚轮事件

public:
    QDockWidget* m_pdockWidget;
    QLabel* m_pStatusLabel;

    QScrollArea* m_pScrollArea;//显示滚动条
    QLabel* m_pImageLabel;
    QListWidget* m_plogList;

    QCheckBox*m_pContinueActiveCheckBox;
    QCheckBox*m_pHardwareSynActiveCheckBox;

    //button
    QPushButton* m_pConnectButton;
    QPushButton* m_pDisConnectButton;
    QPushButton* m_pOpenImageButton;
    QPushButton* m_pSaveImageButton;
    QPushButton* m_pchoosePathButton;//yhy0319

    QLineEdit *m_pExposureTimeEdit;
    QLineEdit *m_ppathLineEdit;//yhy0319
    QPushButton *m_pSetExposureTimeBtn;

    QVector<QRgb> m_vcolorTable; //生成灰度颜色表
    QImage m_ImageBuff;
    QImage m_ImageTempBuff;
    QPixmap m_Pixmap;


    QIDSCamerDevCtrl* m_pIDSDevCtrl;


    int m_nRcvFrameCnt;
    QString m_sCurrentGray;//当前鼠标坐标 以及当前的灰度值
    QString Save_filename;
    float m_lfScal;
    int m_nOrginalWidth;//图像原始宽度
    int m_nOrigalHeight;//图像原始高度
};

#endif // QIDSWidget_H
