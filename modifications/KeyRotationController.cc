#include "KeyRotationController.h"

KeyRotationController::KeyRotationController(QObject *parent) : QObject(parent)
{
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
    connect(ftpManager, SIGNAL(downloadComplete(const QString& , const QString& )), this, SLOT(downloadComplete(const QString&, const QString&)));
    if(!ftpManager->download(AerobridgeGlobals::pathOnBoardForPublicKeyDownload + "/" + AerobridgeGlobals::publicKeyFilename, AerobridgeGlobals::pathOnSystemForDronePulicKeyStorage)){
        return false;
    }
    return true;
}

bool KeyRotationController::uploadKeyToServer()
{
    uploading = true;
    DataClass* _dataClass = qgcApp()->getDataClass();
    _dataClass->uploadDronePublicKeyToServer(url, AerobridgeGlobals::pathOnSystemForDronePulicKeyStorage + "/" + AerobridgeGlobals::publicKeyFilename);
    return true;
}

void KeyRotationController::downloadComplete(const QString &path, const QString &error)
{
    if(error.size() == 0 && !uploading){
        uploadKeyToServer();
    }
}

void KeyRotationController::keyUploadToServerNotInProgress()
{
    uploading = false;
}

