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
#include "dictionary.h"
#include "QDir"

///This is the core class that handles the communications with the management server, stores all the data related to vehicle.
/// This class is a singleton and never exposed to UI
/// The only instance of this class is made in "QGCApplication.h" file.
class DataClass : public QObject
{
    Q_OBJECT
public:
    explicit DataClass(QObject *parent = nullptr);
    QString getURL(){return serverURL;}
    bool checkdroneIDChanged(QString vehicleID);

signals:
    ///This signal is emitted when authentication token is generated for further communications with management server
    void tokenGenerated();
    ///This signal is emitted when authentication token is not generated for further communications with management server
    void tokenNotGenerated();
    ///This signal is emitted when drone status is active
    void droneActive();
    ///This signal is emitted when drone status is inactive
    void droneNotActive();
    ///This signal is emitted when key upload from GCS to management server is successful
    void keyUploadSuccessful();
    ///This signal is emitted when key upload from GCS to management server fails
    void keyUploadFailed();
    ///This signal is emitted when flight plan (currently kml) is upladed to management server
    void planUploadSuccessful();
    ///This signal is emitted when flight plan (currently kml) is not upladed to management server
    void planUploadFailed();
    ///This signal is emitted whenever the droneID change is oberved by the QGroundControl.
    void droneIDChanged();


public slots:
    ///This function makes the the request to the server to generate the auth token from clientID and clientSecret.
    void generateToken();
    ///This function makes a request to management server to check whether the drone is active or not.
    void checkDroneStatus(QString location);
    ///This function makes a request to upload public key to the server.
    void uploadKeyToServer(QString location, QString pathOfKey);
    ///This function makes a request to upload flight plan (currently kml) to the server.
    void uploadPlanToServer(QString location, QString pathOfPlan);
    ///Clears drone data in between a QGroundControl session. Currently not used. But can be used if logout feature is implemented.
    void clearDroneData();

private slots:
    ///Reads the reply of generateToken request
    ///If token generation is successful it emits tokenGenerated() signal otherwise emits tokenNotGenerated()
    void readyReadToken();
    ///Reads the reply of checkDroneStatus request
    /// /// If active droneActive() is emitted otherwise droneNotActive() is emitted.
    void readyReadDroneStatus();
    ///Reads the reply of uploadKeyToServer request
    ///  If the server responds that the file is uploaded then keyUploadSuccessful() is emitted. Otherwise keyUploadFailed() is emitted
    void readyReadPublicKey();
    ///Reads the reply of uploadPlanToServer request.
    /// If the server responds that the plan is uploaded then planUploadSuccessful() is emitted. Otherwise planUploadFailed() is emitted
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
    QString configurationFilePath = QDir::currentPath() + "/config.xml";
    QString clientId;
    QString clientS3cret;
    QString audiance;
    QString grant_type;
    QString scope;
    QString serverURL;

};

#endif // DATACLASS_H
