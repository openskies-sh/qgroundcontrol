#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "QByteArray"
#include "QString"
#include "QDebug"
#include "QStandardPaths"

///List of supported FlightControllers
static QStringList supportedflightControllers = { "PX4v", "Pixhawk", "fmuv", "CubeOrange", "CUAV"};

///API end-points
/// For more information visit: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
static QString checkDroneStatusUrl= "registry/aircraft/rfm/";
static QString uploadPublicKeyUrl = "pki/credentials/";
static QString uploadFlightPlanUrl = "gcs/flight-plans";

///PATHS AND FILENAMES
static QString pathOnBoardForPublicKeyDownload = "APM";
static QString publicKeyFilename = "key.pem";
static QString pathOnSystemForDronePulicKeyStorage = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
static QString configFileName = "config.xml";

static QString checkDeviceConnectedTimerDuration = "500";
static QString checkDroneStatusTimerDuration =  "5000";

#endif // DICTIONARY_H
