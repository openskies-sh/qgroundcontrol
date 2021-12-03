#include "ConnectToServer.h"

ConnectToServer::ConnectToServer(QObject *parent) : QObject(parent)
{
    _dataClass = qgcApp()->getDataClass();
    connect(_dataClass  , SIGNAL(tokenGenerated()) , this, SLOT(slotTokenGenerated()));
    connect(_dataClass  , SIGNAL(tokenNotGenerated()), this, SLOT(slotTokenNotGenerated()));
}

void ConnectToServer::slotTokenNotGenerated()
{
    QMessageBox Error;
    Error.setText("Couldnt fetch token from management server please check your internet connection");
    Error.exec();
    emit connectionNotSuccessful();
}

void ConnectToServer::slotTokenGenerated()
{
    emit connectionSuccessful();
}
