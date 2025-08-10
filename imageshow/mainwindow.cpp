#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagedialog.h"
#include <QScreen>
#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    getROn1SavePath();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showFullScreenModalDialog(const QString &path1, const QString &path2) {
    ImageDialog dialog;
    dialog.PointNo = PointNo;
    dialog.ExpNo = ExpNo;
    dialog.Exptype = Exptype;
    dialog.showMaximized();
    dialog.setImagePaths(path1, path2);
    dialog.exec();
}

void MainWindow::get1On1SavePath(){
    int Pnumber = 0;
    Exptype = 0;
    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC");
    QString dsn = QString::fromLocal8Bit("QtLocalDSN");
    db.setDatabaseName(dsn);
    db.setHostName("local");  //数据库主机名
    db.setPort(1433);
    db.setUserName("sa");
    db.setPassword("123456");
    QString sql = QString("select MAX(pointNo) from (select pointNo from T_DEFECT_SYSTEM_DATA where ExpNo='%1') AS SUBQUERY").arg(ExpNo);
    db.open();
    QSqlQuery sqlquery(db);
    sqlquery.exec(sql);
    sqlquery.next();
    Pnumber = sqlquery.value(0).toInt();

    for (int i = 0;i < Pnumber + 1;i++) {
        int res = 0;
        if(0)
        {
            qDebug("=== %s",qPrintable(db.lastError().text()));
            QMessageBox msgBox;
            msgBox.setWindowTitle("连接失败");
            msgBox.setText("!db.open()");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();

        }
        else
        {
            //QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and PointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
            QString sSql = QString("select defect from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and pointNo=%2 and imagetype=1").arg(ExpNo).arg(i);
            QSqlQuery sqlQuery(db);
            bool bOk = sqlQuery.exec(sSql);
            if (bOk==true)
            {

            }
            else
            {
                QMessageBox msgBox;
                msgBox.setWindowTitle("操作失败");
                msgBox.setText(sqlQuery.lastError().text());
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();
            }
            while(sqlQuery.next())
            {
                res=sqlQuery.value(0).toInt();
            }
        }
        if(res){
            QString strOriImg = SavePath + QString("/IDSIMAGE/%1_%2_0_0.jpg").arg(ExpNo).arg(i);
            QString strDefImg = SavePath + QString("/IDSIMAGE/%1_%2_0_1.jpg").arg(ExpNo).arg(i);
            PointNo = i;
            showFullScreenModalDialog(strOriImg,strDefImg);
        }
    }
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void MainWindow::getROn1SavePath(){
    int Pnumber = 0;
    int Tnumber = 0;
    Exptype = 1;
    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC");
    QString dsn = QString::fromLocal8Bit("QtLocalDSN");
    db.setDatabaseName(dsn);
    db.setHostName("local");  //数据库主机名
    db.setPort(1433);
    db.setUserName("sa");
    db.setPassword("123456");
    QString sql = QString("select MAX(pointNo) from (select pointNo from T_DEFECT_SYSTEM_DATA where ExpNo='%1') AS SUBQUERY").arg(ExpNo);
    db.open();
    QSqlQuery sqlquery(db);
    sqlquery.exec(sql);
    sqlquery.next();
    Pnumber = sqlquery.value(0).toInt();

    for (int i = 0;i < Pnumber + 1;i++) {
        int res = 0;
        if(0)
        {
            qDebug("=== %s",qPrintable(db.lastError().text()));
            QMessageBox msgBox;
            msgBox.setWindowTitle("连接失败");
            msgBox.setText("!db.open()");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        else
        {
            //QString sSql = QString("select imagedata from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and PointNo=%2 and shotNo=%3 and imagetype=%4").arg(pTask->m_tExpInfo.m_sExpNo).arg(pTask->m_tExpInfo.m_nPointNo).arg(pTask->m_tExpInfo.m_nTimes).arg(pTask->m_nParam);
            QString sSql = QString("select defect shotNo from T_DEFECT_SYSTEM_DATA where ExpNo='%1' and pointNo=%2 and defect=1").arg(ExpNo).arg(i);
            QSqlQuery sqlQuery(db);
            bool bOk = sqlQuery.exec(sSql);
            if (bOk)
            {

            }
            else
            {
                QMessageBox msgBox;
                msgBox.setWindowTitle("操作失败");
                msgBox.setText(sqlQuery.lastError().text());
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();
            }
            while(sqlQuery.next())
            {
                res=sqlQuery.value(0).toInt();
                Tnumber=sqlQuery.value(1).toInt();
            }

        }
        if(res == 1){
            QString strOriImg = SavePath + QString("/IDSIMAGE/%1_%2_%3_0.jpg").arg(ExpNo).arg(i).arg(Tnumber);
            QString strDefImg = SavePath + QString("/IDSIMAGE/%1_%2_%3_1.jpg").arg(ExpNo).arg(i).arg(Tnumber);
            PointNo = i;
            showFullScreenModalDialog(strOriImg,strDefImg);
        }
    }
    db.close();
}

