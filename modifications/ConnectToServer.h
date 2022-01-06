#ifndef CONNECTTOSERVER_H
#define CONNECTTOSERVER_H

#include <QObject>
#include "QGCApplication.h"

class ConnectToServer : public QObject
{
    Q_OBJECT
public:
    explicit ConnectToServer(QObject *parent = nullptr);

signals:
    void connectionSuccessful();
    void connectionNotSuccessful();
private slots:
    void slotTokenNotGenerated();
    void slotTokenGenerated();

private:
        DataClass* _dataClass;
};

#endif // CONNECTTOSERVER_H
