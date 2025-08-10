#ifndef QCONFIGPROCESS_H_
#define QCONFIGPROCESS_H_

#include <QObject>
#include "define.h"

class QConfigProcess : public QObject
{
    Q_OBJECT
public:
    explicit QConfigProcess(QObject *parent = nullptr);

    bool getDevConfig();

    bool getPlatformConfig();

    bool updateWaveLengthStatus();
    bool updateEnergyAdjCoeff();
    bool updateEnergyDevCoeff();
signals:

public slots:

public:
    QString m_sPlatformConfigPath;
    QString m_sDevConfigPath;

    tPlatformConfig m_tPlatformConfig;
    tDevConfigParam m_tDevConfigParam;
};

#endif // QPLATFORMCONFIG_H
