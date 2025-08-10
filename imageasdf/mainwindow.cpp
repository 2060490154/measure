#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagedialog.h"
#include <QScreen>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showFullScreenModalDialog("","" );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showFullScreenModalDialog(const QString &path1, const QString &path2) {
    ImageDialog dialog;
    dialog.showMaximized();
    dialog.setImagePaths(path1, path2);
    dialog.exec();
}

