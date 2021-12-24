#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "QByteArray"
#include "QString"
#include "QDebug"
#include "QStandardPaths"

namespace AerobridgeGlobals {

    ///List of supported FlightControllers
    const QStringList supportedflightControllers = { "PX4v", "Pixhawk", "fmuv", "CubeOrange", "CUAV"};

    ///API end-points
    /// For more information visit: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
    const QString checkDroneStatusUrl= "registry/aircraft/rfm/";
    const QString uploadPublicKeyUrl = "pki/credentials/";
    const QString uploadFlightPlanUrl = "gcs/flight-plans";
    const QString getAllActivitiesUrl = "registry/activities";
    const QString getAllFlightPlansUrl = "gcs/flight-plans";
    const QString getAllOperatorsUrl = "registry/operators";
    const QString getAllPilotsUrl = "registry/pilots";
    const QString createFlightOperationUrl = "gcs/flight-operations";
    const QString getFlightPermissionUrl = "gcs/flight-operations/%1/permission";

    ///PATHS AND FILENAMES
    const QString pathOnBoardForPublicKeyDownload = "APM";
    const QString publicKeyFilename = "key.pem";
    const QString pathOnSystemForDronePulicKeyStorage = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString configFileName = "config.xml";

    const int checkDeviceConnectedTimerDuration = 500;
    const int checkDroneStatusTimerDuration =  5000;

}
#endif // DICTIONARY_H
