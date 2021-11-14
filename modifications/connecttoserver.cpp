#include "connecttoserver.h"

ConnectToServer::ConnectToServer(QObject *parent) : QObject(parent)
{
    _dataClass = qgcApp()->getDataClass();
    connect(_dataClass  , SIGNAL(tokenGenerated()) , this, SLOT(slotTokenGenerated()));
    connect(_dataClass  , SIGNAL(tokenNotGenerated()), this, SLOT(slotTokenNotGenerated()));
}

void ConnectToServer::slotTokenNotGenerated()
{
    emit connectionNotSuccessful();
}

void ConnectToServer::slotTokenGenerated()
{
    emit connectionSuccessful();
}
