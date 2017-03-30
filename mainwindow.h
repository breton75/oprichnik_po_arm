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
#include <QPainter>
#include <QGraphicsItem>
#include <QMap>

#include <QTextItem>

class ReportWindow;

namespace Ui {
class MainWindow;
}

class SvTankGraphicsItem;

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
    QMap<int, SvTankGraphicsItem*> _item_map;
};

enum SvGraphicsItemTypes
{
  gtTank = 65537,
  gtSensor,
  gtCustomer,
  gtPipe
};

class SvTankGraphicsItem: public QGraphicsItem
{
  
public:
  explicit SvTankGraphicsItem(QWidget* parent, int type, int id, QString name)
  {
    _type = type;
    _id = id;
    _name = name;
  }
  
  ~SvTankGraphicsItem() { }
  
  int type() { return _type; }
  int id() { return _id; }
  QString name() { return _name; }
  
  void setValue(qreal val) { _val = val; }
  
private:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
  
  QRectF boundingRect() const { return QRectF(0, 0, 100, 100); }
  
  int _type;
  int _id;
  QString _name;
  qreal _val;
  
  
};

#endif // MAINWINDOW_H
