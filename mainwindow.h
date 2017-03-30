#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "reportsetup.h"
#include "fuelmove.h"
#include "checkform.h"
#include "eventslog.h"

#include <QtSql>

class ReportWindow;

namespace Ui {
class MainWindow;
}

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

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);

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
};

#endif // MAINWINDOW_H
