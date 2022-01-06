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
#include <QMessageBox>

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

    void showMessage(QString msg, int errorCode);
    QString errorCodeToString(int errorCode){
        switch (errorCode) {
            case -1: return "";
            case 0: return "\nError: Please check your internet connection";
            case 400: return "\nError: Bad Request";
            case 401: return "\nError: Unauthorized Request";
            case 404: return "\nError: Not Found";
            case 500: return "\nError: Internal Server Error";
            case 502: return "\nError: Bad Gateway";
            case 503: return "\nError: Service Unavailable";
            default: return  QString("Unknown Error Occured\nHttpResponse Code:") + QString(errorCode);
        }
    }

signals:
    void tokenGenerated();
    void tokenNotGenerated();
    void droneActive();
    void droneNotActive();
    void keyUploadSuccessful();
    void keyUploadFailed();
    void planUploadSuccessful();
    void planUploadFailed();
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

    void generateToken();
    void checkDroneStatus(QString location);
    void uploadDronePublicKeyToServer(QString location, QString pathOfKey);
    void getServerPublicKey();
    void uploadPlanToServer(QString location, QJsonObject plan, QString planName);
    void clearDroneData();
    void getAllFlightPlans();
    void getAllActivities();
    void getAllOperators();
    void getAllPilots();
    void createFlightOperation(QString operationName, int planIndex, int operationType);
    void getFlightPermission();


private slots:
    void readyReadToken();
    void readyReadDroneStatus();
    void readyReadUploadDronePublicKeyToServer();
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
    struct Drone{
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
    QMessageBox msgBox;


};

#endif // DATACLASS_H
