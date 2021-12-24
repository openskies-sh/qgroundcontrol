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
#include "GlobalDictionary.h"
#include "QDir"
#include <QDateTime>
#include <SignatureVerifier.h>

///This is the core class that handles the communications with the management server, stores all the data related to vehicle.
/// This class is a singleton and never exposed to UI
/// The only instance of this class is made in "QGCApplication.h" file.
class DataClass : public QObject
{
    Q_OBJECT
public:
    explicit DataClass(QObject *parent = nullptr);
    QString getURL(){return serverUrl;}
    QStringList getAllPlans(){
        QStringList allPlans = {};
        for(int i=0;i<flightData.length();i++){
            allPlans.append(flightData[i].planName);
        }
        return allPlans;
    }
    QJsonObject getSelectedPlan(int selectedPlanIndex){
        if(selectedPlanIndex<flightData.length()){
            return flightData[selectedPlanIndex].plan;
        }
        return QJsonObject();
    }
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

    void getAllFlightPlansSuccessful();
    void getAllFlightPlansFailed();

    void getAllActivitiesSuccessful();
    void getAllActivitiesFailed();

    void getAllOperatorsSuccessful();
    void getAllOperatorsFailed();

    void getAllPilotsSuccessful();
    void getAllPilotsFailed();

    void createFlightOperationSuccessful();
    void createFlightOperationFailed();

    void serverPublicKeyDownloadSuccessful();
    void serverPublicKeyDownloadFailed();

    void permissionGranted();
    void permissionDenied();
    void permissionPending();



public slots:
    ///This function makes the the request to the server to generate the auth token from clientID and clientSecret.
    void generateToken();
    ///This function makes a request to management server to check whether the drone is active or not.
    void checkDroneStatus(QString location);
    ///This function makes a request to upload public key to the server.
    void uploadDronePublicKeyToServer(QString location, QString pathOfKey);
    ///This function makes a request to upload flight plan to the server.

    void getServerPublicKey();

    void uploadPlanToServer(QString location, QJsonObject plan, QString planName);
    ///Clears drone data in between a QGroundControl session. Currently not used. But can be used if logout feature is implemented.
    void clearDroneData();

    void getAllFlightPlans();

    void getAllActivities();

    void getAllOperators();

    void getAllPilots();

    void createFlightOperation(QString operationName, int planIndex, int operationType);

    void getFlightPermission();


private slots:
    ///Reads the reply of generateToken request
    ///If token generation is successful it emits tokenGenerated() signal otherwise emits tokenNotGenerated()
    void readyReadToken();
    ///Reads the reply of checkDroneStatus request
    /// /// If active droneActive() is emitted otherwise droneNotActive() is emitted.
    void readyReadDroneStatus();
    ///Reads the reply of uploadKeyToServer request
    ///  If the server responds that the file is uploaded then keyUploadSuccessful() is emitted. Otherwise keyUploadFailed() is emitted
    void readyReadUploadDronePublicKeyToServer();
    ///Reads the reply of uploadPlanToServer request.
    /// If the server responds that the plan is uploaded then planUploadSuccessful() is emitted. Otherwise planUploadFailed() is emitted

    void readyReadGetServerPublicKey();
    void readyReadFlightPlan();

    void readyReadAllFlightPlans();

    void readyReadAllActivities();

    void readyReadAllOperators();

    void readyReadAllPilots();

    void readyReadCreateFlightOperation();

    void readyReadGetFlightPermission();



private:
    void addToFlightData(QJsonObject obj);

private:
    struct Drone
    {
        QString serialId;
        QString uuid;
        QString publicKey;
        bool status;
        bool npntCheck;
    } drone;

    struct FlightData{
        QString planID;
        QString planName;
        QJsonObject plan;
    };

    QVector<FlightData> flightData;
    QString operatorID;
    QString pilotID;
    QString flightOperationID;
    QString activityID;
    QJsonObject flightPermission;
    QNetworkAccessManager manager;
    QString accessToken;
    QString configurationFilePath = QDir::currentPath() + "/" + AerobridgeGlobals::configFileName;
    QString clientId;
    QString clientSecret;
    QString audience;
    QString grantType;
    QString scope;
    QString serverUrl;
    QString oAuthServerAuthTokenUrl;
    QString oAuthServerPublicKeyUrl;
    std::string serverPublicKey;
    enum PERMISSION_STATUS{
        GRANTED = 0,
        PENDING = 1,
        DENIED = 2,
    };
    static QString PermissionStatus(PERMISSION_STATUS status){
        switch (status) {
        case 0: return "granted";
        case 1: return "pending";
        default: return "denied";
        }
    }


};

#endif // DATACLASS_H
