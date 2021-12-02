#include "DataClass.h"
#include "QXmlStreamReader"

DataClass::DataClass(QObject *parent) : QObject(parent)
{
    QFile file(configurationFilePath);
    if(file.open(QIODevice::ReadOnly)) {
            QXmlStreamReader reader;
            reader.setDevice(&file);
            reader.readNext();
            if (reader.readNextStartElement()) {
                if (reader.name() == "configuration"){
                    while(reader.readNextStartElement()){
                        if(reader.name() == "clientId"){
                            clientId = reader.readElementText();
                        }else if(reader.name() == "clientSecret"){
                            clientSecret = reader.readElementText();
                        }else if(reader.name() == "audience"){
                            audience = reader.readElementText();
                        }else if(reader.name() == "grantType"){
                            grantType = reader.readElementText();
                        }else if(reader.name() == "scope"){
                            scope = reader.readElementText();
                        }else if(reader.name() == "serverUrl"){
                            serverUrl = reader.readElementText();
                        }else if(reader.name() == "oAuthServerUrl"){
                            oAuthServerUrl = reader.readElementText();
                        }else{
                            reader.skipCurrentElement();
                        }
                    }
                }
            }
    }else{
        qDebug()<<"Unable to load configuration file";
    }
    this->generateToken();
}

bool DataClass::checkdroneIDChanged(QString vehicleID){

    if(drone.serialId  == vehicleID){
        return  false;
    }
    drone.serialId = vehicleID;
    emit droneIDChanged();
    return true;
}

//TOKEN REQUEST
void DataClass::generateToken()
{
    QNetworkRequest request = QNetworkRequest(QUrl(oAuthServerUrl));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    QByteArray postData = QUrl(QString("client_id=%1&client_secret=%2&audience=%3&scope=%4&grant_type=%5").arg(clientId,clientSecret,audience,scope,grantType)).toEncoded();
    QNetworkReply* reply = manager.post(request, postData);
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadToken);
}

void DataClass::readyReadToken()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(replyStr.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    accessToken = jsonObject["access_token"].toString();
    if(accessToken!=""){
        emit tokenGenerated();
        getAllFlightPlans();
    }
    else{
        emit tokenNotGenerated();
    }
}

//DRONE STATUS CHECK
/// GET REQUEST
/// For more information refer "registry/aircraft/rfm/" end point
/// For more information visit: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
void DataClass::checkDroneStatus(QString location)
{
    location = location + drone.serialId;
    if(drone.serialId.size() == 0){
        return;
    }
    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
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
    if(drone.status){
        emit droneActive();
    }else{
        emit droneNotActive();
    }
}

//PUBLIC KEY ROTATION
/// POST REQUEST
/// For more information refer "pki/credentials/" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
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
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
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
    if(statusCode == 201 || statusCode == 200){
        emit keyUploadSuccessful();
    }else{
        emit keyUploadFailed();
    }

}
// UPLOAD FLIGHT PLAN
/// POST REQUEST
/// For more information refer "gcs/flight-plans" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
void DataClass::uploadPlanToServer(QString location, QJsonObject plan, QString planName){

    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    request.setRawHeader("Content-Type", "application/json");

    QJsonObject obj;
    if(planName.size() == 0) {
        planName = QString("Unnamed Flight Plan " + QString::number(flightData.size() + 1));
    }

    obj["name"] = planName;
    obj["plan_file_json"] = plan;

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
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonObject jsonObjectReply = jsonReply.object();
        addToFlightData(jsonObjectReply);

        emit planUploadSuccessful();
    }else{
        emit planUploadFailed();
    }
}

void DataClass::readyReadAllFlightPlans()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray jsonArray = jsonReply.array();
        for(int i=0; i<jsonArray.size(); i++){
            addToFlightData(jsonArray[i].toObject());
        }
        // emit signal of success

    }else{
        // emit signal of failure
    }

}

void DataClass::addToFlightData(QJsonObject obj)
{
    FlightData data;

    data.pilotID  = obj["id"].toString();
    data.planName = obj["name"].toString();
    data.plan     = obj["plan_file_json"].toObject();
    // pilot data to be added

    flightData.append(data);
}

//Clear Drone Data
void DataClass::clearDroneData()
{
    drone.npntCheck = false;
    drone.uuid.clear();
    drone.publicKey.clear();
    drone.serialId.clear();
}

void DataClass::getAllFlightPlans()
{
    QString location = serverUrl + getAllFlightPlansUrl;

    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    QNetworkReply* reply = manager.get(request);
    connect(reply,&QNetworkReply::finished, this, &DataClass::readyReadAllFlightPlans);
}

