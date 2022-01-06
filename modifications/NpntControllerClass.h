#ifndef NPNTCONTROLLERCLASS_H
#define NPNTCONTROLLERCLASS_H

#include <QObject>
#include "QGCApplication.h"
#include "QTimer"
#include "QThread"
#include "KeyRotationController.h"
#include "GlobalDictionary.h"
#include "LinkManager.h"

class NpntControllerClass : public QObject
{
    Q_OBJECT
public:
    explicit NpntControllerClass(QObject *parent = nullptr);
signals:
      void hardwareConnected();
      void droneIsActive();
      void keyRotationCompleted();
      void npntComplete();
      void hardwareChangeDetected();

public slots:
      Q_INVOKABLE bool checkdeviceConnected();

private slots:
      bool checkIsBoardActive();
      void boardIsActive();
      void boardNotActive();
      bool keyRotation();
      void keyRotatedOK();
      void KeyRotateFailed();
      void hardwareChanged();

private:
    QTimer *checkDeviceConnectedTimer, *checkDroneStatusTimer;
    DataClass* _dataClass;
    QString m_url;
    bool keyRotating;
    KeyRotationController* m_keyController;
};

#endif // NPNTCONTROLLERCLASS_H
