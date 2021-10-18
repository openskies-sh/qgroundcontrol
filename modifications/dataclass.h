#ifndef DATACLASS_H
#define DATACLASS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <qregexp.h>

class DataClass : public QObject
{
    Q_OBJECT
public:
    explicit DataClass(QObject *parent = nullptr);
    QString getURL(){return serverURL;}
    bool droneIDChanged(QString vehicleID);

signals:
    void tokenGenerated();
    void tokenNotGenerated();
    void droneActive();
    void droneNotActive();
    void keyUploadFailed();
    void keyUploadSuccessful();
    void planUploadSuccessful();
    void planUploadFailed();


public slots:
    void generateToken();
    void checkDroneStatus(QString location);
    void uploadKeyToServer(QString location, QString pathOfKey);
    void uploadPlanToServer(QString location, QString pathOfPlan);

private slots:
    void readyReadToken();
    void readyReadDroneStatus();
    void readyReadPublicKey();
    void readyReadFlightPlan();

private:

    struct Drone
    {
        QString serialId;
        QString uuid;
        QString publicKey;
        bool status;
        bool npntCheck;
    } drone;
    QNetworkAccessManager manager;
    QString access_token;
    QString clientId = "VWRZ3Yo0nheU9BZv0Fw7tTm4MAPr2dFB5lPQMmJt";
    QString clientS3cret = "AqWQ0xW00Gb7k7K2yLAoz6hOCg7OGV8bOvFEd5Xq4wKlnZRPFa0tkgEAf8bDr9q2AApmWym2YUvBLW1lWY09EIkGPbhUlhHdoD8BEhYqR2gakKRHXATTI4ITt5z1clXr";
    QString audiance = "testflight.aerobridge.in";
    QString grant_type = "client_credentials";
    QString scope = "aerobridge.read aerobridge.write";
    QString serverURL = "https://aerobridgetestflight.herokuapp.com/";
};

#endif // DATACLASS_H
