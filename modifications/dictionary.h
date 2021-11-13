#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "QByteArray"
#include "QString"
#include "QDebug"
#include "QStandardPaths"

///List of supported FlightControllers
static QStringList supportedflightControllers = { "PX4v", "Pixhawk", "fmuv", "CubeOrange"};

///API end-points
/// For more information visit: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
static QString checkDroneStatusUrl= "registry/aircraft/rfm/";
static QString uploadPublicKeyUrl = "pki/credentials/";
static QString uploadFlightPlanUrl = "gcs/flight-plans";
static QString pathOnBoardForPublicKeyDownload = "APM";
static QString publicKeyFilename = "key.pem";
static QString pathOnSystemForDronePulicKeyStorage = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

///Customer Secret Should be Kept Private
static QString clientId = "VWRZ3Yo0nheU9BZv0Fw7tTm4MAPr2dFB5lPQMmJt";
static QString clientS3cret = "AqWQ0xW00Gb7k7K2yLAoz6hOCg7OGV8bOvFEd5Xq4wKlnZRPFa0tkgEAf8bDr9q2AApmWym2YUvBLW1lWY09EIkGPbhUlhHdoD8BEhYqR2gakKRHXATTI4ITt5z1clXr";
static QString audiance = "testflight.aerobridge.in";
static QString grant_type = "client_credentials";
static QString scope = "aerobridge.read aerobridge.write";
static QString serverURL = "https://aerobridgetestflight.herokuapp.com/";

#endif // DICTIONARY_H
