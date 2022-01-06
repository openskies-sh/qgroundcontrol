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
            file.close();
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

void DataClass::showMessage(QString msg, int errorCode)
{
    msgBox.setText(msg + " " + errorCodeToString(errorCode));
    msgBox.exec();
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
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201 || statusCode == 200){
        QJsonDocument jsonResponse = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        accessToken = jsonObject["access_token"].toString();
        if(accessToken!=""){
            emit tokenGenerated();
        }
    }else{
        emit tokenNotGenerated();
        showMessage("Unable to Connect to Server", statusCode);
    }
}
//*******************************************************************************************************************************************************************************
//DRONE STATUS CHECK
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
            showMessage("Drone Not Active", -1);
        }
    }else if(statusCode == 404){
        showMessage("The drone is not registered", -1);
    }
    else{
        showMessage("Unable to Connect to Server", statusCode);
    }
}
//*******************************************************************************************************************************************************************************
//DRONE PUBLIC KEY ROTATION
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
        showMessage("Unable to Upload Drone Public Key to Server", statusCode);
        emit keyUploadFailed();
    }
}
//*******************************************************************************************************************************************************************************
//SERVER PUBLIC KEY ROTATION
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
            showMessage("Unable to Fetch Public Key of Server", -1);
            emit serverPublicKeyDownloadFailed();
        }
    }else{
        showMessage("Unable to Fetch Public Key of Server", statusCode);
        emit serverPublicKeyDownloadFailed();
    }
}
//*******************************************************************************************************************************************************************************
// UPLOAD FLIGHT PLAN
void DataClass::uploadPlanToServer(QString location, QJsonObject plan, QString planName){

    QNetworkRequest request = QNetworkRequest(location);
    request.setRawHeader("Authorization",QByteArray("Bearer ").append(accessToken));
    request.setRawHeader("Content-Type", "application/json");

    QJsonObject obj;
    if(planName.size() == 0) {
        planName = QString("Plan_" + QDateTime::currentDateTime().toString());
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
        showMessage("Flight Plan Uploaded Successfully", -1);
        emit planUploadSuccessful();
        getAllFlightPlans();
    }else{
        showMessage("Unable to Upload Flight Plan", statusCode);
        emit planUploadFailed();
    }
}
//*******************************************************************************************************************************************************************************
// GET ALL FLIGHT PLANS
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
        showMessage("Unable to get flight plans from management server", statusCode);
    }
}
//*******************************************************************************************************************************************************************************
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
        showMessage("Unable to get activites from management server", statusCode);
    }
}
//*******************************************************************************************************************************************************************************
// GET ALL OPERATORS
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
        showMessage("Unable to get operators from management server", statusCode);
    }
}
//*******************************************************************************************************************************************************************************
// GET ALL PILOTS
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
        showMessage("Unable to get pilots from management server", statusCode);
    }
}
//*******************************************************************************************************************************************************************************
//CREATE FLIGHT OPERATION
void DataClass::createFlightOperation(QString operationName, int planIndex, int operationType)
{
    QString location = serverUrl + AerobridgeGlobals::createFlightOperationUrl;
    if(operationName.size() == 0) {
        operationName = QString("Operation_" + QDateTime::currentDateTime().toString());
    }
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
    QNetworkReply* reply = manager.post(request, postData);
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
            showMessage("Flight Operation Created\nWaiting for Clearance from the Management Server", -1);
            getFlightPermission();
        }else{
             emit createFlightOperationFailed();
             showMessage("Unable to Create Flight Operation", statusCode);
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
            if((permissionStatus == "denied")||(permissionStatus == "pending")){
                showMessage("Response: Not Permitted to Fly. Try Again Later",-1);
                return;
            }
            QString signature = jsonObj["token"].toObject()["access_token"].toString();
            if(SignatureVerifier::verifyJWT(signature.toStdString(),serverPublicKey) == 0){
                 emit permissionGranted();
                 showMessage("Response: Permission Granted",-1);
            }
            else{
               showMessage("Signature Not Verified",-1);
            }
        }
    }else{
        showMessage("Unable to get flight permission",statusCode);
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

