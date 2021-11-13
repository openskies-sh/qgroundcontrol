#ifndef KEYROTATIONCONTROLLER_H
#define KEYROTATIONCONTROLLER_H

#include <QObject>
#include <FTPManager.h>
#include <QGCApplication.h>
#include <dataclass.h>
#include "dictionary.h"

///KeyRotationController class is primarily created for facilitating key rotation from the drone to the management server.
class KeyRotationController : public QObject
{
    Q_OBJECT
public:
    explicit KeyRotationController(QObject *parent = nullptr);

signals:

public slots:
    /// This function starts the process of key rotation from the drone to the management server.
    /// @param URL, the full url of the management server, where the key needs to be uploaded.
    void startKeyRotation(QString URL);

private slots:
    ///This function fetches key from the drone and places it on specified path of the system running QGroundControl.
    ///@return true:successful, false:failure
    bool fetchKeyFromDrone();
    ///This function uploads the key from the system path to the management server on the URL specified as a parameter in startKeyRotation function
    ///@return true:successful, false:failure
    bool uploadKeyToServer();
    /// This slot is connected to download complete function of the FTP manager class, once the fetch public key from drone is successful it calls
    /// uploadKeyToServer() function
    void downloadComplete(const QString &path, const QString &error);
    /// This slot is called whenever keyUploadTo Server is not in progress. It turns off the uploading mutex.
    void keyUploadToServerNotInProgress();

private:
    QString url;
    bool uploading;
};

#endif // KEYROTATIONCONTROLLER_H
