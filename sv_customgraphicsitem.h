#ifndef SVCUSTOMGRAPHICSITEM_H
#define SVCUSTOMGRAPHICSITEM_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsItem>
#include <QMap>
#include <QTextItem>

#include <QDebug>

struct TankDataStruct {
  qreal height = 0.5;
  qreal volume = 50.0;
  qreal weight = 50.0;
  qreal max_volume = 100.0;
};

struct ConsumerDataStruct {
  qreal val1;
  qreal val2;
  qreal val3;
};

struct SensorDataStruct {
  qreal val1;
};

enum SvGraphicsItemTypes
{
  gtTank = 65537,
  gtConsumer,
  gtSensor,
  gtPipe
};


class SvCustomGraphicsItem: public QGraphicsItem
{
public:
  explicit SvCustomGraphicsItem(QWidget *parent, int id, QString name);
  
  ~SvCustomGraphicsItem() { }
  
  virtual int type() const { return -1; }
//  virtual auto value();
//  virtual void setValue();
  
  int id() const { return _id ; }
  QString name() const { return _name; }
  
private:
  int _id;
  QString _name;
  
};

class SvTankGraphicsItem: public SvCustomGraphicsItem
{
public:
  explicit SvTankGraphicsItem(QWidget* parent, int id, QString name);
  
  int type() { return gtTank; }
  
  void setValue(TankDataStruct &data) { _data = data; }
  TankDataStruct data() const { return _data; }

protected:  
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
  QRectF boundingRect() const { return QRectF(0, 0, 100, 100); }
  
private:
  TankDataStruct _data;
  
};

class SvConsumerGraphicsItem: public SvCustomGraphicsItem
{
public:
  explicit SvConsumerGraphicsItem(QWidget *parent, int id, QString name);
  
  int type() { return gtConsumer; }
  
  void setValue(ConsumerDataStruct &data) { _data = data; }
  ConsumerDataStruct data() const { return _data; }

protected:  
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
  QRectF boundingRect() const { return QRectF(0, 0, 100, 100); }
  
private:
  ConsumerDataStruct _data;
  
};


class SvSensorGraphicsItem: public SvCustomGraphicsItem
{
public:
  explicit SvSensorGraphicsItem(QWidget* parent, int id, QString name);
  
  int type() { return gtSensor; }
  
  void setValue(SensorDataStruct &data) { _data = data; }
  SensorDataStruct data() const { return _data; }

//protected:  
//  void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
//  QRectF boundingRect() const { return QRectF(0, 0, 100, 100); }
  
private:
  SensorDataStruct _data;
  
};


#endif // SVCUSTOMGRAPHICSITEM_H
