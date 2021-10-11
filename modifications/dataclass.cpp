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
    qInfo()<<"Entered the upload key to server function";
    qInfo()<<"Reading file from "<<pathOfKey;
    qInfo()<<"Uploading file to "<<location;
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
    QByteArray postData;
    postData.append("{\"token\":\"");
    postData.append(drone.publicKey);
    postData.append("\",\"name\":\"");
    postData.append("Public Key");
    postData.append("\",\"token_type\":\"");
    postData.append("0");
    postData.append("\",\"association\":\"");
    postData.append("3");
    postData.append("\",\"is_active\":\"");
    postData.append("true");
    postData.append("\",\"aircraft\":\"");
    postData.append(drone.uuid);
    postData.append("\"}");

    qInfo()<<"DataPosted "<<postData;
    QNetworkReply* reply = manager.post(request, postData);
    qInfo()<<"REQUEST SENT";
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadPublicKey);

}

void DataClass::readyReadPublicKey()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qInfo()<<statusCode;
    if(statusCode == 201 || statusCode == 200)
    {
        emit keyUploadSuccessful();
    }
    else
    {
        emit keyUploadFailed();
    }

}

