#ifndef CONNECTTOSERVER_H
#define CONNECTTOSERVER_H

#include <QObject>
#include "QGCApplication.h"

///This class establishes a connection to the authentication server. If connection is succesful, it emits signal for the UI
/// to change the page from "ConnectServer.qml" to "NPNTProcess.qml"
class ConnectToServer : public QObject
{
    Q_OBJECT
public:
    explicit ConnectToServer(QObject *parent = nullptr);

signals:
    /// This signal is emitted whenever connection to management server is successful and authentication token is generated.
    void connectionSuccessful();
    /// This signal is emitted whenever authentication token is not generated.
    void connectionNotSuccessful();
private slots:
    void slotTokenNotGenerated();
    void slotTokenGenerated();

private:
        DataClass* _dataClass;
};

#endif // CONNECTTOSERVER_H
