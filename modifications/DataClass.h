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
#include <jwt-cpp/jwt-cpp/jwt.h>
#include <QDateTime>

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
    void getAllFlightPlansReturnedNull();

    void getAllActivitiesSuccessful();
    void getAllActivitiesFailed();
    void getAllActivitiesReturnedNull();

    void getAllOperatorsSuccessful();
    void getAllOperatorsFailed();
    void getAllOperatorsReturnedNull();

    void getAllPilotsSuccessful();
    void getAllPilotsFailed();
    void getAllPilotsReturnedNull();

    void createFlightOperationSuccessful();
    void createFlightOperationFailed();



public slots:
    ///This function makes the the request to the server to generate the auth token from clientID and clientSecret.
    void generateToken();
    ///This function makes a request to management server to check whether the drone is active or not.
    void checkDroneStatus(QString location);
    ///This function makes a request to upload public key to the server.
    void uploadKeyToServer(QString location, QString pathOfKey);
    ///This function makes a request to upload flight plan to the server.
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
    void readyReadPublicKey();
    ///Reads the reply of uploadPlanToServer request.
    /// If the server responds that the plan is uploaded then planUploadSuccessful() is emitted. Otherwise planUploadFailed() is emitted
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
    QString configurationFilePath = QDir::currentPath() + "/" +configFileName;
    QString clientId;
    QString clientSecret;
    QString audience;
    QString grantType;
    QString scope;
    QString serverUrl;
    QString oAuthServerUrl;
    QString serverPublicKey = R"(-----BEGIN PUBLIC KEY-----
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAt7RHafy7vKDaHmeP83f4
W4npHfdwD9Y59pBbPxn3uX0vrTS8eBYkRI1tQqcsCfMa+KIz6aLoGPhL0IYFRsj0
4882pv2MQTKdBWICGsTyzXws554RF/MLoGc5HFdqvhtXAsnSQRMk5/4sn4XcvRTt
rt0klrKgfFQ0dpTTz9wTBYVmw5Ln4ccw5szHPeQHJOBpxY/0zoLqFxjVpgfOmEks
LzX+uxMgUIj6A5iAW9St5ioHHIlrrU6PlcRKx/Z9FpD4rsXXH14FADq05x9RC7II
GGeoAM6qNK8CiuCgnMaPbTw9Lpqs6oOT2/OzkLE+ksiZuxNfh50qBrhrl5JnWkTH
rhkh5GsQmr3YEYIQxUi8H3Q7Q5qkxpmLp5I/MfUGGhfyeHqdMKdn0mPD9QQbVI9C
PEOR/KnD7U/LiEktEgTcBLeuWz+T+tih9zK+Fvc5sgC8QmpSVRMyWPOu9O+yCopQ
+T5ggrCVidDbMaLAW2uFH3BgiNWbgGKSli71SVJr40kPkN7EVhZX8jeNtirGFhDX
0V9n90qtcEIEIEXZnW/LSgImKWnaDjXlkCQajdXjBwXNli6lto+if1Wz9T0ueZfH
rkKWk/mIeTQ6vg1RmgTcEcJgYLbUb+vHBWlUxxQ9tgDfjv5/4+M76j0HXy1q7d/u
nuPEa5QVdyk85YJFN2THfqUCAwEAAQ==
-----END PUBLIC KEY-----)";

    bool isSignatureValid(QString data){
        auto verify =
        jwt::verify().allow_algorithm(jwt::algorithm::rs256(serverPublicKey.toStdString())).with_issuer("https://id.openskies.sh/");

        auto decoded = jwt::decode(data.toStdString());

        std::error_code c;
        try {
            verify.verify(decoded, c);
            std::cout<<c.message()<<std::endl;
            if(c.value() == 0){
                return true;
            }
            else{
                throw(c);
            }
        }
        catch(const std::error_code& e)
        {
            std::cout << "Exception: " << e.message();
        }
        return false;
    }

};

#endif // DATACLASS_H
