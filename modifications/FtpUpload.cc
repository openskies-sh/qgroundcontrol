#include "FtpUpload.h"
#include <QObject>
#include <QDebug>

FTPUpload::FTPUpload(QObject *parent):QObject(parent)
{

}

void FTPUpload::downloadFile(QString file)
{
     qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->ftpManager()->download(QString(file), QStandardPaths::writableLocation(QStandardPaths::TempLocation));
}

void FTPUpload::uploadFile(QString path)
{
    QString fileName ;
    for(auto it=path.rbegin(); it!= path.rend(); it++){
        if(*it == '/') {
            break;
        } else {
            fileName.push_front(*it);
        }
    }
    qgcApp()->toolbox()->multiVehicleManager()->activeVehicle()->ftpManager()->upload(AerobridgeGlobals::pathOnBoardForPublicKeyDownload + QString("%1").arg(fileName), path);
}
