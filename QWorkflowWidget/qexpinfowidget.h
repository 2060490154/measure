#ifndef QEXPINFOWIDGET_H
#define QEXPINFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include "define.h"
#include <QDialog>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QComboBox>

class QExpInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QExpInfoWidget(QWidget *parent = 0);
    ~QExpInfoWidget();
    void setLabelTitle(QLabel *plabel, QString sTitle,QString sIConPath,QString sRgb);
    void InitTabHeader(QTableWidget *ptable);
    void InitUI();
    bool IsDateValid(int sy, int sm, int sd, int ey, int em, int ed, int iy, int im, int id);

signals:
    void signal_getExpInfoFromdb(QVector<ExpInfo4Show> &ExpInfos);
    void signal_SaveNewDevInfo2db(DevInfo4Show DevInfo);
    void signal_SaveNewClientInfo2db(int index, ClientInfo ClientInfos);
    void signal_GetDevInFromdb(QVector<DevInfo4Show> &DevInfos);
    void signal_GetClientInfoFromDb(QVector<ClientInfo> &ClientInfos);
public slots:
    void on_GenerateBtn_clicked();
    void on_RefreshBtnClicked();
    void on_HistoryBtnClicked();
    void on_HistoryOkBtn();
    void on_AddDevSubmitBtn();
    void on_AddDevBtnClicked();
    void on_AddClientBtnClicked();
    void on_AddClientSubmitBtn();
    void on_ShowDevInfo();
    void on_ShowClientInfo();
    void on_ClientComBoxChanged(int index);
    void on_GetFluxThreshHold(double thresh);
    void on_GetExpNo(QString sExpNo, QString sSampleName);

public:
    QLabel *m_ExpLabel = NULL;    //设置标签
    QLabel *m_DevLabel = NULL;    //设置标签
    QLabel *m_InfoLabel = NULL;    //设置标签
    QTableWidget *m_ExpTableWidget = NULL;    //信息列表
    QTableWidget *m_DevTableWidget = NULL;    //信息列表
    QTableWidget *m_InfoTableWidget = NULL;    //信息列表

    QVector<ExpInfo4Show> m_tExpInfos;
    QVector<DevInfo4Show> m_tDevInfos;
    QVector<ExpInfo4Show> m_tExpInfosInRange;
    QVector<DevInfo4Show> m_tDevInfosInRange;
    QVector<ClientInfo> m_tClientInfos;
    QVector<QCheckBox*> m_tExpCheckBoxs;
    QVector<QCheckBox*> m_tDevCheckBoxs;

    QPushButton *m_pGenerateBtn = NULL;  //生成报表按钮
    QPushButton *m_pRefreshBtn = NULL;  //
    QPushButton *m_pAddDevBtn = NULL;  //
    QPushButton *m_pHistoryBtn = NULL;  //
    QPushButton *m_pAddClientBtn = NULL;
    QDialog *m_pDateDialog = NULL;
    QDialog *m_pAddDevDialog = NULL;
    QDialog *m_pAddClientDialog = NULL;


    QDateTimeEdit *m_pDateStartEdit;
    QDateTimeEdit *m_pDateEndEdit;
    QPushButton *m_pOkBtn;
    QPushButton *m_pSubmitBtn;
    QPushButton *m_pClientSubmitBtn;


    QLineEdit *m_DevNameEdit;
    QLineEdit *m_DevTypeEdit;
    QLineEdit *m_DevNoEdit;
    QLineEdit *m_DevCertiNoEdit;
    QLineEdit *m_DevSpeciEdit;
    QLineEdit *m_pClientNameEdit;
    QLineEdit *m_pClientAddressEdit;
    QLineEdit *m_pClientPhoneEdit;

    QComboBox *m_pClientInfoComBox;

public:
    double m_fFluxThreshHold;
    QString m_sExpNo;
    QString m_sSavePath;
};

#endif // QEXPINFOWIDGET_H
