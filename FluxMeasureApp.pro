#-------------------------------------------------
#
# Project created by QtCreator 2018-08-13T00:38:45
#
#-------------------------------------------------

QT       += core gui network serialport
QT += sql printsupport
QT += sql printsupport axcontainer
QT       += xml

RC_ICONS =app.ico

QMAKE_CXXFLAGS +=  -Wno-unused-parameter

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FluxMeasureApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



SOURCES += main.cpp\
        mainwindow.cpp \
    quiqss.cpp \
    qdevstatuswidget.cpp

HEADERS  += mainwindow.h \
    quiqss.h \
    qdevstatuswidget.h

FORMS    += mainwindow.ui


INCLUDEPATH +=$$PWD QNavWidget
include (./QNavWidget/QNavWidget.pri)

INCLUDEPATH +=$$PWD QWorkflowWidget
include (./QWorkflowWidget/QWorkflowWidget.pri)


INCLUDEPATH +=$$PWD QDevCtrl
INCLUDEPATH +=$$PWD QDevCtrl/QEnergyMeter
INCLUDEPATH +=$$PWD QDevCtrl/QIDSCamerCtrl
INCLUDEPATH +=$$PWD QDevCtrl/QMotorCtrl
include (./QDevCtrl/QDevCtrl.pri)



INCLUDEPATH +=$$PWD QDevWidget
include (./QDevWidget/QDevWidget.pri)

INCLUDEPATH +=$$PWD QAdpt
include (./QAdpt/QAdpt.pri)

INCLUDEPATH +=$$PWD QDBProcess
include (./QDBProcess/QDBProcess.pri)


INCLUDEPATH +=$$PWD QCommon
include (./QCommon/QCommon.pri)


INCLUDEPATH +=$$PWD QDevMonitorWidget
include (./QDevMonitorWidget/QDevMonitorWidget.pri)


INCLUDEPATH +=$$PWD QConfigProcess
include (./QConfigProcess/QConfigProcess.pri)

INCLUDEPATH +=$$PWD QUDPcs
include (./QUDPcs/QUDPcs.pri)

RESOURCES += \
    res/res.qrc


INCLUDEPATH += E:/OpenCV342_MingW/include/opencv2
INCLUDEPATH += E:/OpenCV342_MingW/include
INCLUDEPATH += E:/OpenCV342_MingW/include/opencv

LIBS += E:/OpenCV342_MingW/x86/mingw/lib/libopencv_core342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_imgproc342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_highgui342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_ml342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_video342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_features2d342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_calib3d342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_objdetect342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_flann342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_imgcodecs342.dll.a \ #imread
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_videostab342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_videoio342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_photo342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_superres342.dll.a \
        E:/OpenCV342_MingW/x86/mingw/lib/libopencv_stitching342.dll.a \

