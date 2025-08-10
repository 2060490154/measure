#ifndef QQUERYWIDGET_H
#define QQUERYWIDGET_H

#include <QWidget>
#include "define.h"
class QTableWidget;
class QPushButton;
class QLabel;
class QExcel;
class QDateTimeEdit;
class QMenu;
class QLineEdit;

class QQueryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QQueryWidget(QWidget *parent = 0);
public:
    QTableWidget *m_pExpInfoTable;
    QTableWidget *m_pExpParasTable;
    QTableWidget *m_pExpDataTable;
    QTableWidget *m_pExpResultTable;
    QLabel *m_pLegendLab;
    QLabel *m_pExpInfoLab;
    QLabel *m_pParasLab;
    QLabel *m_pResultLab;
    QPushButton *m_pRefreshBtn;
    QPushButton *m_pHistoryBtn;
    QPushButton *m_pQueryBtn;
    QPushButton *m_pSaveIDSImgBtn;
    QPushButton *m_pSavePlotImgBtn;
    QPushButton *m_pExportAllDataBTn;
    QPushButton *m_pExportSelectedDataBtn;
    QPushButton *m_pAddNumBtn;
    QLineEdit *m_pAddNumEdit;
    QPushButton *m_pReOrderBtn;

    QWidget *m_pExpInfoWidget;
    QWidget *m_pParasWidget;
    QWidget *m_pResultWidget;


    QDialog *m_pDateDialog = NULL;
    QDateTimeEdit *m_pDateStartEdit;
    QDateTimeEdit *m_pDateEndEdit;
    QPushButton *m_pOkBtn;
public:
    QVector<ExpInfo4Show> m_tExpInfos;
    QVector<AllExpData4show> m_tExpData;
    QVector<int> m_tDefectInfo;
    QVector<double> m_tAverFlux;
    QVector<double> m_tDefectRatio;
    bool m_bQueryExpInfo;   //查询实验信息标志位
    bool m_bQueryExpDataInfo;   //查询实验数据标志位
    bool m_bQueryIDSDefectInfo;   //查询损伤数据标志位
    bool m_bQueryResultInfo;   //查询统计结果标志位
    QExcel *m_pExcel;
    QList<QColor> m_tColorList;
    //右键删除数据条目
    QMenu *RightClick;
    QAction *deleteAction;
    int iDeletcRow;
public:
    void InitUI();
    void InitExpInfoWidget();
    void InitParasTable();
    void updateParasTable(int nrow);
    void InitDataTable();
    void updateDataTable(int nIndex, QString sExpNo);
    void updatePointArea(int nRow);
    void InitResultTable();
    void updateResultTable(int nRow);
    void InitShowArea();
    void updateLegend(int nMannual, int nUnDefect, int nDefect);
    void updatePlot(int nrow);
    void connectAction();
    QString transToExpTypeStr(int nType);
    QString transToExpWaveStr(int nType);
    bool IsDateValid(int sy, int sm, int sd, int ey, int em, int ed, int iy, int im, int id);
    void setTableTitleLabel(QLabel *pLabel, QString sTitle, QString sIconPath, QString sRgb);
signals:
    void signal_getExpInfoFromdb(QVector<ExpInfo4Show> &tExpInfos);
    void signal_getExpDataFromdb(QVector<AllExpData4show> &tExpData, QString sExpNo);
    void signal_getDefectInfoFromdb(QVector<int> &tDefectInfo, QString sExpNo);
    void signal_getResultFromdb(QVector<double> &tAverFlux,QVector<double> &tDefectRatio, QString sExpNo);
public slots:
    void onRefreshBtnClicked();
    void onHistoryBtnClicked();
    void onQueryBtnClicked();
    void on_HistoryOkBtn();
    void onExpInfoTableDoubleClicked(int nrow, int ncol);
    void onQueryDbFinish(int nQueryType);
    void onSavePlotImg();
    void onExportAllDataBtnClicked();
    void onExportSelectedDataBtnClicked();
    //右键删除数据条目
    void RightClickSlot(QPoint pos);
    void RightClickDelete(QAction *act);
    void onReorder();
    void onAddNum();
};

#endif // QQUERYWIDGET_H
