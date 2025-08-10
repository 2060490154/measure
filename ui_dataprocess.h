/********************************************************************************
** Form generated from reading UI file 'dataprocess.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAPROCESS_H
#define UI_DATAPROCESS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DataProcess
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *DataProcess)
    {
        if (DataProcess->objectName().isEmpty())
            DataProcess->setObjectName(QStringLiteral("DataProcess"));
        DataProcess->resize(800, 600);
        centralwidget = new QWidget(DataProcess);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        DataProcess->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DataProcess);
        menubar->setObjectName(QStringLiteral("menubar"));
        DataProcess->setMenuBar(menubar);
        statusbar = new QStatusBar(DataProcess);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        DataProcess->setStatusBar(statusbar);

        retranslateUi(DataProcess);

        QMetaObject::connectSlotsByName(DataProcess);
    } // setupUi

    void retranslateUi(QMainWindow *DataProcess)
    {
        DataProcess->setWindowTitle(QApplication::translate("DataProcess", "DataProcess", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DataProcess: public Ui_DataProcess {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAPROCESS_H
