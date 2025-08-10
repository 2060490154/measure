#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int PointNo = 0;
    QString SavePath = "I:/FluxMeasureData/##/ROn1_20240324185502";
    QString ExpNo = "ROn1_20240324185502";
    int Exptype = 0;
    void showFullScreenModalDialog(const QString &path1, const QString &path2);
    void get1On1SavePath();
    void getROn1SavePath();
};
#endif // MAINWINDOW_H
