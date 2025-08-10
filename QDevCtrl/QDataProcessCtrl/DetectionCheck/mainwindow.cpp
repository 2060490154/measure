#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pDefectCheckWidget = new QDefectCheckWidget();

    this->setCentralWidget(m_pDefectCheckWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
