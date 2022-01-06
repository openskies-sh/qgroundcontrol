#ifndef KEYROTATIONCONTROLLER_H
#define KEYROTATIONCONTROLLER_H

#include <QObject>
#include <FTPManager.h>
#include <QGCApplication.h>
#include <DataClass.h>
#include "GlobalDictionary.h"

class KeyRotationController : public QObject
{
    Q_OBJECT
public:
    explicit KeyRotationController(QObject *parent = nullptr);

signals:

public slots:
    void startKeyRotation(QString URL);

private slots:

    bool fetchKeyFromDrone();
    bool uploadKeyToServer();
    void downloadComplete(const QString &path, const QString &error);
    void keyUploadToServerNotInProgress();

private:
    QString url;
    bool uploading;
};

#endif // KEYROTATIONCONTROLLER_H
