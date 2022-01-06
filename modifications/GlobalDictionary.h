#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "QByteArray"
#include "QString"
#include "QDebug"
#include "QStandardPaths"

namespace AerobridgeGlobals {

    const QStringList supportedflightControllers = { "PX4v", "Pixhawk", "fmuv", "CubeOrange", "CUAV"};
    const QString checkDroneStatusUrl= "registry/aircraft/rfm/";
    const QString uploadPublicKeyUrl = "pki/credentials/";
    const QString uploadFlightPlanUrl = "gcs/flight-plans";
    const QString getAllActivitiesUrl = "registry/activities";
    const QString getAllFlightPlansUrl = "gcs/flight-plans";
    const QString getAllOperatorsUrl = "registry/operators";
    const QString getAllPilotsUrl = "registry/pilots";
    const QString createFlightOperationUrl = "gcs/flight-operations";
    const QString getFlightPermissionUrl = "gcs/flight-operations/%1/permission";
    const QString pathOnBoardForPublicKeyDownload = "APM/";
    const QString dronePublicKeyFilename = "dronePublicKey.pem";
    const QString pathOnSystemForPublicKeyStorage = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString configFileName = "config.xml";
    const int checkDeviceConnectedTimerDuration = 500;
    const int checkDroneStatusTimerDuration =  5000;

}
#endif // DICTIONARY_H
