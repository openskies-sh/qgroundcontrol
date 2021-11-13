#include "npntcontrollerclass.h"


NpntControllerClass::NpntControllerClass(QObject *parent) : QObject(parent)
{
    keyRotating = false;
    m_keyController         = new KeyRotationController();
    _dataClass              = qgcApp()->getDataClass();
    m_url                   = qgcApp()->getDataClass()->getURL();
    timer1                  = new QTimer(this);
    timer2                  = new QTimer(this);
    timer3                  = new QTimer(this);

    connect(timer1  , SIGNAL(timeout()), this, SLOT(checkdeviceConnected()));
    connect(timer2  , SIGNAL(timeout()), this, SLOT(checkIsBoardActive()));
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
        timer1->start(500);
        return false;
    }
    timer1->stop();
    emit hardwareConnected();
    checkIsBoardActive();
    return true;
}

bool NpntControllerClass::checkIsBoardActive()
{
      _dataClass->checkDroneStatus(m_url+checkDroneStatusUrl);
      timer2->start(5000);
      return true;
}

void NpntControllerClass::boardIsActive()
{
    timer2->stop();
    emit droneIsActive();
    keyRotation();
}

void NpntControllerClass::boardNotActive()
{
    timer2->stop();
}

bool NpntControllerClass::keyRotation()
{
    if(!keyRotating)
    {
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
