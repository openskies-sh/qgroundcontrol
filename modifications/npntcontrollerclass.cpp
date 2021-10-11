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

    connect(timer1  , SIGNAL(timeout()), this, SLOT(deviceConnected()));
    connect(timer2  , SIGNAL(timeout()), this, SLOT(checkIsBoardActive()));
    connect(_dataClass, SIGNAL(droneNotActive()), this, SLOT(boardNotActive()));
    connect(_dataClass, SIGNAL(droneActive()), this, SLOT(boardIsActive()));
    connect(_dataClass, SIGNAL(droneActive()), this, SLOT(boardIsActive()));
    connect(_dataClass  , SIGNAL(keyUploadFailed()) , this, SLOT(KeyRotateFailed()));
    connect(_dataClass  , SIGNAL(keyUploadSuccessful()), this, SLOT(keyRotatedOK()));
}


bool NpntControllerClass::deviceConnected()
{
    if(!qgcApp()->toolbox()->multiVehicleManager()->activeVehicleAvailable()){
        // No active Board Connected
        qInfo() << "Trying To find the board";
        timer1->start(500);
        return false;
    }
    timer1->stop();
    emit check1();
    checkIsBoardActive();
    return true;
}

bool NpntControllerClass::checkIsBoardActive()
{
      _dataClass->checkDroneStatus(m_url+"registry/aircraft/rfm/");
      timer2->start(5000);
      return true;
}

void NpntControllerClass::boardIsActive()
{
    timer2->stop();
    emit check2();
    // Initiate Next Step
    keyRotated();
}

void NpntControllerClass::boardNotActive()
{
    timer2->stop();
}

bool NpntControllerClass::keyRotated()
{
    if(!keyRotating)
    {
        keyRotating = true;
        qInfo()<<"Starting Key Rotation Step #1";
        m_keyController->startKeyRotation(m_url+"pki/credentials/");
        return true;
    }
    return false;
}
void NpntControllerClass::keyRotatedOK()
{

    keyRotating = false;
    emit check4();
    emit npntComplete();
}

void NpntControllerClass::KeyRotateFailed()
{
    keyRotating = false;
    return;
}
