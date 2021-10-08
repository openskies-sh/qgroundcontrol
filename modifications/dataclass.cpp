#include "dataclass.h"

DataClass::DataClass(QObject *parent) : QObject(parent)
{
      this->generateToken();
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

void DataClass::checkDroneStatus(QString location)
{
    location = location + drone.serialId;
    if(drone.serialId.size() == 0){
        qInfo() << "Vehicle Id not Retrieved";
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

    if(drone.status)
    {
        emit droneActive();
    }
    else
    {
        emit droneNotActive();
    }
}

