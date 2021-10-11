#include "keyrotationcontroller.h"

KeyRotationController::KeyRotationController(QObject *parent) : QObject(parent)
{
    pathOnBoard = "APM";
    keyFileName = "key.pem";
    pathOnSystem = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    uploading = false;
}

void KeyRotationController::startKeyRotation(QString URL)
{
    url = URL;
    if(!fetchKeyFromDrone()){
        return;
    }
}

bool KeyRotationController::fetchKeyFromDrone()
{

    FTPManager* ftpManager = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->ftpManager();
    QString a, b;
    connect(ftpManager, SIGNAL(downloadComplete(const QString& , const QString& )), this, SLOT(downloadComplete(const QString&, const QString&)));

    if(!ftpManager->download(pathOnBoard + "/" + keyFileName, pathOnSystem)){
        qInfo() << " Cannot Download Key.pem";
        return false;
    }
    return true;
}

bool KeyRotationController::uploadKeyToServer()
{
    qInfo() << "Uploading key" << pathOnSystem + "/" + keyFileName;
    uploading = true;
    DataClass* _dataClass = qgcApp()->getDataClass();
    _dataClass->uploadKeyToServer(url, pathOnSystem + "/" + keyFileName);
    return true;
}

void KeyRotationController::downloadComplete(const QString &path, const QString &error)
{
    qInfo()<<"File Feteched From Drone and Saved to "<<path<<"Step#2";
    if(error.size() == 0 && !uploading){
        uploadKeyToServer();
    }
}
