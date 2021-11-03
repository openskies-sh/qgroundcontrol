#include "dataclass.h"

DataClass::DataClass(QObject *parent) : QObject(parent)
{
    this->generateToken();
}

bool DataClass::droneIDChanged(QString vehicleID){

    if(drone.serialId  == vehicleID)
    {
        return  false;
    }
    else
    {
        drone.serialId = vehicleID;
        return true;
    }
}

//TOKEN REQUEST
void DataClass::generateToken()
{
    QNetworkRequest request = QNetworkRequest(QUrl("https://id.openskies.sh/oauth/token/"));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    QByteArray postData = QUrl(QString("client_id=%1&client_secret=%2&audience=%3&scope=%4&grant_type=%5").arg(clientId,clientS3cret,audiance,scope,grant_type)).toEncoded();
    QNetworkReply* reply = manager.post(request, postData);
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadToken);
}

void DataClass::readyReadToken()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(replyStr.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    access_token = jsonObject["access_token"].toString();
    if(access_token!="")
    {
        emit tokenGenerated();
    }
    else
    {
        emit tokenNotGenerated();
    }
}

//DRONE STATUS CHECK
void DataClass::checkDroneStatus(QString location)
{
    location = location + drone.serialId;
    if(drone.serialId.size() == 0){
        return;
    }
    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(access_token));
    QNetworkReply* reply = manager.get(request);
    connect(reply,&QNetworkReply::finished, this, &DataClass::readyReadDroneStatus);

}

void DataClass::readyReadDroneStatus()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(replyStr.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    drone.status = jsonObject["status"].toInt();
    drone.uuid = jsonObject["id"].toString();

    if(drone.status)
    {
        emit droneActive();
    }
    else
    {
        emit droneNotActive();
    }
}

//PUBLIC KEY ROTATION
void DataClass::uploadKeyToServer(QString location, QString pathOfKey)
{
    QString contentOfKey;
    QFile file(pathOfKey);
    file.open(QIODevice::ReadOnly);
    contentOfKey = file.readAll();
    file.close();
    contentOfKey.chop(1);
    drone.publicKey = contentOfKey;
    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(access_token));
    request.setRawHeader("Content-Type", "application/json");

    QJsonObject obj;
    obj["token"] = drone.publicKey;
    obj["name"] = "Public Key";
    obj["token_type"] = "0";
    obj["association"] = "3";
    obj["is_active"] = "true";
    obj["aircraft"] = drone.uuid;
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson();

    QNetworkReply* reply = manager.post(request, postData);
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadPublicKey);

}

void DataClass::readyReadPublicKey()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200)
    {
        emit keyUploadSuccessful();
    }
    else
    {
        emit keyUploadFailed();
    }

}
// UPLOAD FLIGHT PLAN
void DataClass::uploadPlanToServer(QString location, QString pathOfPlan)
{
    QString contentOfPlan;
    QFile file(pathOfPlan);
    file.open(QIODevice::ReadOnly);
    contentOfPlan = file.readAll();
    file.close();
    contentOfPlan.chop(1);
    contentOfPlan.remove("\n");
    contentOfPlan = contentOfPlan.simplified();
    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(access_token));
    request.setRawHeader("Content-Type", "application/json");
    QJsonObject obj;
    obj["name"] = "test_new";
    obj["kml"] = contentOfPlan;
    obj["start_datetime"] = "2019-08-24T14:15:22";
    obj["end_datetime"] = "2019-08-24T14:15:22";
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson();
    QNetworkReply* reply = manager.post(request, postData);
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadFlightPlan);

}
void DataClass::readyReadFlightPlan()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200)
    {
        emit planUploadSuccessful();
    }
    else
    {
        emit planUploadFailed();
    }

}


