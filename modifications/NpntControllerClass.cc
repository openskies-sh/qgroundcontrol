#include "NpntControllerClass.h"


NpntControllerClass::NpntControllerClass(QObject *parent) : QObject(parent)
{
    keyRotating = false;
    m_keyController         = new KeyRotationController();
    _dataClass              = qgcApp()->getDataClass();
    m_url                   = qgcApp()->getDataClass()->getURL();
    checkDeviceConnectedTimer  = new QTimer(this);
    checkDroneStatusTimer     = new QTimer(this);

    connect(checkDeviceConnectedTimer  , SIGNAL(timeout()), this, SLOT(checkdeviceConnected()));
    connect(checkDroneStatusTimer  , SIGNAL(timeout()), this, SLOT(checkIsBoardActive()));
    connect(_dataClass, SIGNAL(droneNotActive()), this, SLOT(boardNotActive()));
    connect(_dataClass, SIGNAL(droneActive()), this, SLOT(boardIsActive()));
    connect(_dataClass  , SIGNAL(keyUploadFailed()) , this, SLOT(KeyRotateFailed()));
    connect(_dataClass  , SIGNAL(keyUploadSuccessful()), this, SLOT(keyRotatedOK()));
    connect(_dataClass  , SIGNAL(keyUploadSuccessful()), m_keyController, SLOT(keyUploadToServerNotInProgress()));
    connect(_dataClass  , SIGNAL(keyUploadFailed()), m_keyController, SLOT(keyUploadToServerNotInProgress()));
    connect(_dataClass  , SIGNAL(droneIDChanged()), this, SLOT(hardwareChanged()));
}


bool NpntControllerClass::checkdeviceConnected()
{
    if(!qgcApp()->toolbox()->multiVehicleManager()->activeVehicleAvailable()){
        checkDeviceConnectedTimer->start(checkDeviceConnectedTimerDuration.toInt());
        return false;
    }
    checkDeviceConnectedTimer->stop();
    emit hardwareConnected();
    checkIsBoardActive();
    return true;
}

bool NpntControllerClass::checkIsBoardActive()
{
      _dataClass->checkDroneStatus(m_url+checkDroneStatusUrl);
      checkDroneStatusTimer->start(checkDroneStatusTimerDuration.toInt());
      return true;
}

void NpntControllerClass::boardIsActive()
{
    checkDroneStatusTimer->stop();
    emit droneIsActive();
    keyRotation();
}

void NpntControllerClass::boardNotActive()
{
    checkDroneStatusTimer->stop();
}

bool NpntControllerClass::keyRotation()
{
    if(!keyRotating){
        keyRotating = true;
        m_keyController->startKeyRotation(m_url+uploadPublicKeyUrl);
        return true;
    }
    return false;
}
void NpntControllerClass::keyRotatedOK()
{

    keyRotating = false;
    emit keyRotationCompleted();
    emit npntComplete();
}

void NpntControllerClass::KeyRotateFailed()
{
    keyRotating = false;
    return;
}

void NpntControllerClass::hardwareChanged()
{
    emit hardwareChangeDetected();
    checkdeviceConnected();
}
