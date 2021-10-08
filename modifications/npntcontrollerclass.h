#ifndef NPNTCONTROLLERCLASS_H
#define NPNTCONTROLLERCLASS_H

#include <QObject>
#include "QGCApplication.h"
#include "QTimer"
#include <QMessageBox>

class NpntControllerClass : public QObject
{
    Q_OBJECT
public:
    explicit NpntControllerClass(QObject *parent = nullptr);

signals:
      void check1();
      void check2();
      void check3();
      void check4();

public slots:
       Q_INVOKABLE bool deviceConnected();
      void boardIsActive();
      void boardNotActive();

private slots:
      bool checkIsBoardActive();
      int ErrorMessageBox(QString errorMessage);

private:
    QTimer *timer1, *timer2, *timer3, *timer4;
    DataClass* _dataClass;
    QString m_url;
};

#endif // NPNTCONTROLLERCLASS_H
