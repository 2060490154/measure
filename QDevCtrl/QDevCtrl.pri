HEADERS += \
    $$PWD/QEnergyMeter/EnergyMeter.h \
    $$PWD/QEnergyMeter/EnergyMeterdefine.h \
    $$PWD/QEnergyMeter/ophirlmmeasurement.h \
    $$PWD/QIDSCamerCtrl/qidscamerdevctrl.h \
    $$PWD/QIDSCamerCtrl/uEye.h \
    $$PWD/QMotorCtrl/USB1020.h \
    $$PWD/QMotorCtrl/qmotorctrl.h \
    $$PWD/qdevmanagerctrl.h \
    $$PWD/QMotorCtrl/qmotordevmanager.h\
    $$PWD/QOsciCtrl/activedsolib.h \
    $$PWD/QOsciCtrl/QOsciDevCtrl.h \
    $$PWD/QEnergyMeter/Energymeterdevicemanager.h \
    $$PWD/QKxccdCtrl/qkxccdctrl.h \
    $$PWD/QKxccdCtrl/kxccddefine.h \
    $$PWD/QKxccdCtrl/qhigimage.h \
    $$PWD/QShutterCtrl/QShutterManagerCtrl.h \
    $$PWD/QDataProcessCtrl/qdataprocessctrl.h \
    $$PWD/QDataProcessCtrl/cwise.h \
    $$PWD/comctrl/qattenuatordevctrl.h

SOURCES += \
    $$PWD/QEnergyMeter/EnergyMeter.cpp \
    $$PWD/QEnergyMeter/ophirlmmeasurement.cpp \
    $$PWD/QIDSCamerCtrl/qidscamerdevctrl.cpp \
    $$PWD/QMotorCtrl/qmotorctrl.cpp \
    $$PWD/qdevmanagerctrl.cpp \
    $$PWD/QMotorCtrl/qmotordevmanager.cpp\
    $$PWD/QOsciCtrl/activedsolib.cpp \
    $$PWD/QOsciCtrl/QOsciDevCtrl.cpp \
    $$PWD/QEnergyMeter/Energymeterdevicemanager.cpp \
    $$PWD/QKxccdCtrl/qkxccdctrl.cpp \
    $$PWD/QKxccdCtrl/qhigimage.cpp \
    $$PWD/QShutterCtrl/QShutterManagerCtrl.cpp \
    $$PWD/QDataProcessCtrl/qdataprocessctrl.cpp \
    $$PWD/comctrl/qattenuatordevctrl.cpp


 LIBS += -L$$PWD/QIDSCamerCtrl -luEye_api\
         -L$$PWD/QMotorCtrl -lUSB1020_32

 INCLUDEPATH += $$PWD/QDataProcessCtrl/eigen
