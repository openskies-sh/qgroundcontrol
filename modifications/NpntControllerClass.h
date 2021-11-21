#ifndef NPNTCONTROLLERCLASS_H
#define NPNTCONTROLLERCLASS_H

#include <QObject>
#include "QGCApplication.h"
#include "QTimer"
#include "QThread"
#include "KeyRotationController.h"
#include "GlobalDictionary.h"
#include "LinkManager.h"

/// This class controlles the whole NPNT init process.
class NpntControllerClass : public QObject
{
    Q_OBJECT
public:
    explicit NpntControllerClass(QObject *parent = nullptr);
signals:
      /// hardwareConnected() is a signal, it is emitted whenever a board is connected and Link is established.
      void hardwareConnected();
      /// droneIsActive() is a signal, it is emitted when the drone status is marked active on the management server.
      void droneIsActive();
      /// keyRotationCompleted() is a signal, emitted when the public key rotation is completed from the drone to the Management Server
      void keyRotationCompleted();
      /// npntComplete() marks the end of NPNT init process.
      void npntComplete();
      /// hardwareChangeDetected() is emitted whenever a hardware change is detected. This is a security feature.
      /// This will bring the user back to NPNT init process page if the user tries to change the flight controller after the NPNT init process.
      void hardwareChangeDetected();

public slots:
      ///checkdeviceConnected() checks whether any hardware is connected or not.
      ///@return true: hardware connected, false: hardware not connected.
      Q_INVOKABLE bool checkdeviceConnected();

private slots:
      ///checkIsBoardActive() checks whether the drone status is marked active on the management server.
      ///@return true: drone active, false: drone inactive.
      bool checkIsBoardActive();
      ///if checkIsBoardActive() returns true, then boardIsActive() emits droneIsActive() signal.
      void boardIsActive();
      void boardNotActive();
      /// keyRotation() initiates the process of public key rotation from drone to management server
      /// @return true: key rotation process started, false: key rotation process not started, because key rotation process is already running
      bool keyRotation();
      ///if public key is successfully rotated from the drone to the management server, then keyRotatedOK() emits keyRotationCompleted() signal.
      void keyRotatedOK();
      void KeyRotateFailed();
      /// if droneID is changed upon reconnect, then this function  hardwareChanged() is called, which emits hardwareChangeDetected() signal.
      void hardwareChanged();

private:
    QTimer *checkDeviceConnectedTimer, *checkDroneStatusTimer;
    DataClass* _dataClass;
    QString m_url;
    bool keyRotating;
    KeyRotationController* m_keyController;
};

#endif // NPNTCONTROLLERCLASS_H
