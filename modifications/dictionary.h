#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "QByteArray"
#include "QString"
#include "QDebug"
static QStringList supportedflightControllers = { "PX4v", "Pixhawk", "fmuv", "CubeOrange"};


//For more info https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
static QString checkDroneStatusUrl= "registry/aircraft/rfm/";
static QString uploadPublicKeyUrl = "pki/credentials/";
static QString uploadFlightPlanUrl = "gcs/flight-plans";

#endif // DICTIONARY_H
