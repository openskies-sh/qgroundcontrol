#ifndef NPNTCONTROLLERCLASS_H
#define NPNTCONTROLLERCLASS_H

#include <QObject>
#include "QGCApplication.h"
#include "QTimer"
#include "QThread"
#include "keyrotationcontroller.h"

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

public slots:
      Q_INVOKABLE bool deviceConnected();
      void boardIsActive();
      void boardNotActive();

private slots:
      bool checkIsBoardActive();
      bool keyRotated();
      void keyRotatedOK();
      void KeyRotateFailed();

private:
    QTimer *timer1, *timer2, *timer3;
    DataClass* _dataClass;
    QString m_url;
    bool keyRotating;
    KeyRotationController* m_keyController;
};

#endif // NPNTCONTROLLERCLASS_H
