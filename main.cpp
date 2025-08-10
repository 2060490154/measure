#include <QApplication>
#include <qsplashscreen.h>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"

QSplashScreen* g_psplashScreen;


void LogMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch (type)
    {
    case QtDebugMsg:
        text = QString("Debug: ");
        break;
    case QtWarningMsg:
        text = QString("Warning: ");
        break;
    case QtCriticalMsg:
        text = QString("Critical: ");
        break;
    case QtFatalMsg:
        text = QString("Fatal: ");
        break;
    }

    //判断是否指定输出文件
    QString sFilename;
    QString sMsgContent;
    sMsgContent = msg.mid(1,msg.length()-2);

    if (sMsgContent.mid(0,4) == "FILE")
    {
        sFilename = sMsgContent.mid(5,sMsgContent.indexOf(":FILE")-5);
        sMsgContent = sMsgContent.mid(sMsgContent.indexOf(":FILE")+5,sMsgContent.length()-sMsgContent.indexOf(":FILE")-5);
    }
    else
    {
        sFilename = "log.txt";
        sMsgContent = msg;
    }

    QString context_info = "";
    QString current_datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("%1").arg(current_datetime);
    QString message = QString("%1 %2 %3 %4").arg(current_date).arg(text).arg(context_info).arg(sMsgContent);

    //write
    QString strAppDir = QCoreApplication::applicationFilePath();
    strAppDir = strAppDir.left(strAppDir.lastIndexOf("/"));
    QFile file(strAppDir + "/" +sFilename);

    if (file.size() > 5000000)
    {
        file.copy(sFilename+ "_" + QDateTime::currentDateTime().toString("yyMMddhhmmss") + ".txt");
        file.remove();
    }
    file.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream text_stream(&file);
    text_stream << message <<"\r\n";
    file.flush();
    file.close();
    mutex.unlock();
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    qInstallMessageHandler(LogMessage);
    QString sStartPath = app.applicationDirPath()+"/image/start150.png";

    QPixmap pixmap(sStartPath);
    g_psplashScreen = new QSplashScreen(pixmap);
    g_psplashScreen->show();


    g_psplashScreen->showMessage(QObject::tr("正在初始化应用程序..."),Qt::AlignBottom | Qt::AlignHCenter, Qt::white);//显示信息

    MainWindow w;
    w.show();
    g_psplashScreen->finish(&w);

    return app.exec();
}
