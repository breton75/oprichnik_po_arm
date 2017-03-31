#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "reportsetup.h"
#include "fuelmove.h"
#include "checkform.h"
#include "eventslog.h"

#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QMap>
#include<QMutex>

#include "sv_customgraphicsitem.h"

class ReportWindow;

namespace Ui {
class MainWindow;
}

class SvPGThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbnReport_clicked();
    void on_pbnFuelMove_clicked();
    void on_pbnCheck_clicked();
    void on_pbnEvents_clicked();

    void formResized();
    
    void repaintTank(int id, TankDataStruct data);
    void repaintConsumer(int id, ConsumerDataStruct data);
    void repaintSensor(int id, SensorDataStruct data);
    
    void recalc();

//protected:
//    virtual void resizeEvent(QResizeEvent *);
//    virtual void showEvent(QShowEvent *);

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;// can_db
    QSqlQuery* query;

    QGraphicsScene* scene;
    QGraphicsPixmapItem* picItem;
    QTimer* resizeTimer;

    int fTanksCount, fSensorsCount, fConsumersCount;

    ReportSetup* fRepSetup;
    ReportWindow* fRep;
    FuelMove* fFuelMove;
    CheckForm* fCheck;
    EventsLog* fEventsLog;

    void prepareScheme();
    void drawScene();
    
    /* свиридов */
    QMap<int, SvTankGraphicsItem*> _tanks_map;
    QMap<int, SvConsumerGraphicsItem*> _consumers_map;
    QMap<int, SvSensorGraphicsItem*> _sensors_map;
    
    SvPGThread *_pg_thr;
    
    qreal total_tanks;
    qreal total_in;
    qreal total_out;
};


class SvPGThread: public QThread
{
    Q_OBJECT
  
public:
  explicit SvPGThread(QSqlDatabase &db);
  
  ~SvPGThread();
  
  bool isWorking() { return _isWorking; }
  bool isFinished() { return _isFinished; }
  
protected:
//  void run() Q_DECL_OVERRIDE;
  void timerEvent(QTimerEvent *te);
  
private:
  QSqlDatabase _db;
  
  bool _isWorking = false;
  bool _isFinished = true;
  
//public slots:
//  void stop();
  
signals:
  void tank_updated(int , TankDataStruct);
  void consumer_updated(int , ConsumerDataStruct);
  void sensor_updated(int , SensorDataStruct);
  
  void recalc();
  
};

#endif // MAINWINDOW_H
