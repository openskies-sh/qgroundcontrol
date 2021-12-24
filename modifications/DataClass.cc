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
                        }else if(reader.name() == "oAuthServerAuthTokenUrl"){
                            oAuthServerAuthTokenUrl = reader.readElementText();
                        }else if(reader.name() == "oAuthServerPublicKeyUrl"){
                            oAuthServerPublicKeyUrl = reader.readElementText();
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

//API CALLS START
//*******************************************************************************************************************************************************************************
//TOKEN REQUEST
void DataClass::generateToken()
{
    QNetworkRequest request = QNetworkRequest(QUrl(oAuthServerAuthTokenUrl));
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
    }
    else{
        emit tokenNotGenerated();
    }
}
//*******************************************************************************************************************************************************************************
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
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonResponse = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        drone.status = jsonObject["status"].toInt();
        drone.uuid = jsonObject["id"].toString();
        if(drone.status){
            emit droneActive();
        }else{
            emit droneNotActive();
        }
    }else{
        //emit failure
    }
}
//*******************************************************************************************************************************************************************************
//DRONE PUBLIC KEY ROTATION
/// POST REQUEST
/// For more information refer "pki/credentials/" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
void DataClass::uploadDronePublicKeyToServer(QString location, QString pathOfKey)
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
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadUploadDronePublicKeyToServer);
}

void DataClass::readyReadUploadDronePublicKeyToServer()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        emit keyUploadSuccessful();
        getServerPublicKey();
    }else{
        emit keyUploadFailed();
    }

}
//*******************************************************************************************************************************************************************************
//SERVER PUBLIC KEY ROTATION
/// GET REQUEST
void DataClass::getServerPublicKey()
{
    QString location = oAuthServerPublicKeyUrl;
    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    QNetworkReply* reply = manager.get(request);
    connect(reply,&QNetworkReply::finished, this, &DataClass::readyReadGetServerPublicKey);
}

void DataClass::readyReadGetServerPublicKey()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray jsonArray = jsonReply.object()["keys"].toArray();
        if(jsonArray.size() > 0){
            QJsonObject jwks = jsonArray[0].toObject();
            std::string e = jwks["e"].toString().toStdString();
            std::string kid = jwks["kid"].toString().toStdString();
            std::string n = jwks["n"].toString().toStdString();
            if(!SignatureVerifier::convertJwkToPem(n,e,kid,serverPublicKey)){
                emit serverPublicKeyDownloadFailed();
                return;
            }
            emit serverPublicKeyDownloadSuccessful();
            // GET ALL THE DATA FROM SERVER
            getAllActivities();
            getAllFlightPlans();
            getAllOperators();
            getAllPilots();
        }else{
            emit serverPublicKeyDownloadFailed();
        }
    }else{
        emit serverPublicKeyDownloadFailed();
    }

}
//*******************************************************************************************************************************************************************************
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
        getAllFlightPlans();
        emit planUploadSuccessful();
    }else{
        emit planUploadFailed();
    }
}
//*******************************************************************************************************************************************************************************
// GET ALL FLIGHT PLANS
/// GET REQUEST
/// For more information refer "gcs/flight-plans" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml

void DataClass::getAllFlightPlans()
{
    QString location = serverUrl + AerobridgeGlobals::getAllFlightPlansUrl;
    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    QNetworkReply* reply = manager.get(request);
    connect(reply,&QNetworkReply::finished, this, &DataClass::readyReadAllFlightPlans);
}

void DataClass::readyReadAllFlightPlans()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray jsonArray = jsonReply.array();
        if(jsonArray.size() > 0){
            flightData.clear();
            for(int i=0; i<jsonArray.size(); i++){
                addToFlightData(jsonArray[i].toObject());
            }
             emit getAllFlightPlansSuccessful();
        }
        else{
            emit getAllFlightPlansFailed();
        }
    }else{
        emit getAllFlightPlansFailed();
    }
}
//*******************************************************************************************************************************************************************************
// GET ALL ACTIVITIES
/// GET REQUEST
/// For more information refer "registry/activities" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
void DataClass::getAllActivities()
{
    QString location = serverUrl + AerobridgeGlobals::getAllActivitiesUrl;
    QNetworkRequest request = QNetworkRequest(location);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    QNetworkReply* reply = manager.get(request);
    connect(reply,&QNetworkReply::finished, this, &DataClass::readyReadAllActivities);
}

void DataClass::readyReadAllActivities()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray jsonArray = jsonReply.array();
        if(jsonArray.size() > 0){
            activityID = jsonArray[0].toObject()["id"].toString();
            emit getAllActivitiesSuccessful();
        }else{
            emit getAllActivitiesFailed();
        }
    }else{
        emit getAllActivitiesFailed();
    }
}
//*******************************************************************************************************************************************************************************
// GET ALL OPERATORS
/// GET REQUEST
/// For more information refer "registry/operators" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml
void DataClass::getAllOperators()
{
    QString location = serverUrl + AerobridgeGlobals::getAllOperatorsUrl;
    QNetworkRequest request = QNetworkRequest(location);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    QNetworkReply* reply = manager.get(request);
    connect(reply,&QNetworkReply::finished, this, &DataClass::readyReadAllOperators);
}

void DataClass::readyReadAllOperators()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray jsonArray = jsonReply.array();
        if(jsonArray.size() > 0){
            operatorID = jsonArray[0].toObject()["id"].toString();
            emit getAllOperatorsSuccessful();
        }else{
            emit getAllOperatorsFailed();
        }
    }else{
        emit getAllOperatorsFailed();
    }
}
//*******************************************************************************************************************************************************************************
// GET ALL PILOTS
/// GET REQUEST
/// For more information refer "registry/pilots" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml

void DataClass::getAllPilots()
{
    QString location = serverUrl + AerobridgeGlobals::getAllPilotsUrl;
    QNetworkRequest request = QNetworkRequest(location);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    QNetworkReply* reply = manager.get(request);
    connect(reply,&QNetworkReply::finished, this, &DataClass::readyReadAllPilots);
}

void DataClass::readyReadAllPilots()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray jsonArray = jsonReply.array();
        if(jsonArray.size() > 0){
            pilotID = jsonArray[0].toObject()["id"].toString();
            emit getAllPilotsSuccessful();
        }else{
             emit getAllPilotsFailed();
        }
    }else{
             emit getAllPilotsFailed();
    }
}
//*******************************************************************************************************************************************************************************
//CREATE FLIGHT OPERATION
/// PUT REQUEST
/// For more information refer "gcs/flight-operations" end point
/// Management Server API documentation: https://redocly.github.io/redoc/?url=https://raw.githubusercontent.com/openskies-sh/aerobridge/master/api/aerobridge-1.0.0.resolved.yaml

void DataClass::createFlightOperation(QString operationName, int planIndex, int operationType)
{
    QString location = serverUrl + AerobridgeGlobals::createFlightOperationUrl;
    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    request.setRawHeader("Content-Type", "application/json");
    QDateTime startTime = QDateTime::currentDateTimeUtc().addSecs(300);
    QDateTime endTime = startTime.addSecs(1800);
    QJsonObject obj;
    obj["name"] = operationName;
    obj["start_datetime"] = startTime.toString(Qt::ISODate);
    obj["end_datetime"] = endTime.toString(Qt::ISODate);
    obj["type_of_operation"] = operationType;
    obj["drone"] = drone.uuid;
    obj["flight_plan"] = flightData[planIndex].planID;
    obj["purpose"] = activityID;
    obj["operator"] = operatorID;
    obj["pilot"] = pilotID;
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson();
    QNetworkReply* reply = manager.put(request, postData);
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadCreateFlightOperation);

}

void DataClass::readyReadCreateFlightOperation()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonObject jsonObj = jsonReply.object();
        if(!jsonObj.empty()){
            flightOperationID = jsonObj["id"].toString();
            emit createFlightOperationSuccessful();
            getFlightPermission();
        }else{
             emit createFlightOperationFailed();
        }
    }
}
//*******************************************************************************************************************************************************************************
void DataClass::getFlightPermission()
{
    QString location = serverUrl + AerobridgeGlobals::getFlightPermissionUrl.arg(flightOperationID);
    QNetworkRequest request = QNetworkRequest(location);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    request.setRawHeader("Content-Type", "application/json");

    QJsonObject obj;
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson();

    QNetworkReply* reply = manager.put(request, postData);
    connect(reply, &QNetworkReply::finished, this, &DataClass::readyReadGetFlightPermission);

}
void DataClass::readyReadGetFlightPermission()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString replyStr = reply->readAll();
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonReply = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonObject jsonObj = jsonReply.object();
        if(!jsonObj.empty()){
            QString permissionStatus = jsonObj["status_code"].toString();
            if(permissionStatus == PermissionStatus(PERMISSION_STATUS::DENIED)||permissionStatus == PermissionStatus(PERMISSION_STATUS::PENDING)){
                return;
            }
            QString signature = jsonObj["token"].toObject()["access_token"].toString();
            if(SignatureVerifier::verifyJWT(signature.toStdString(),serverPublicKey) == 0){
                 emit permissionGranted();
                qDebug()<<"Signature Verified";

            }
            else{
               // emit signal of failure
            }
        }
        else{
             // emit signal of failure
        }

    }else{
         //emit signal of failure
    }
}
//*******************************************************************************************************************************************************************************
//API CALLS END
void DataClass::addToFlightData(QJsonObject obj)
{
    FlightData data;
    data.planID  = obj["id"].toString();
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

