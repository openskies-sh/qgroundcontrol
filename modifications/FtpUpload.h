#ifndef FTPUPLOAD_H
#define FTPUPLOAD_H

#include <QObject>
#include "MultiVehicleManager.h"
#include "QGCApplication.h"
#include "FTPManager.h"

class FTPUpload:public QObject
{
    Q_OBJECT
public:
    explicit FTPUpload(QObject *parent = nullptr);
    Q_INVOKABLE void downloadFile(QString file);
    Q_INVOKABLE void uploadFile(QString path);
};

#endif // FTPUPLOAD_H
