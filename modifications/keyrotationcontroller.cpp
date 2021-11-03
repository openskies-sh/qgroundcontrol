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
        return false;
    }
    return true;
}

bool KeyRotationController::uploadKeyToServer()
{
    uploading = true;
    DataClass* _dataClass = qgcApp()->getDataClass();
    _dataClass->uploadKeyToServer(url, pathOnSystem + "/" + keyFileName);
    return true;
}

void KeyRotationController::downloadComplete(const QString &path, const QString &error)
{
    if(error.size() == 0 && !uploading){
        uploadKeyToServer();
    }
}
