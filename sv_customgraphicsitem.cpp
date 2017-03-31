#include "sv_customgraphicsitem.h"

SvCustomGraphicsItem::SvCustomGraphicsItem(QWidget* parent, int id, QString name)
{
  _id = id;
  _name = name;
}


SvTankGraphicsItem::SvTankGraphicsItem(QWidget *parent, int id, QString name):
  SvCustomGraphicsItem(parent, id, name)
{
  
}

void SvTankGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(widget);

  QPen pen = QPen(Qt::SolidLine);
  pen.setColor(QColor(0, 0, 0, 255));
  pen.setWidthF(0.5);
  painter->setPen(pen);
  
  QPainterPath path;
  
  
//    qreal rad = (1 - _val) * 10;
  path.addRoundedRect(QRectF(0, 0, 100, 100), 10, 10);
  painter->setBrush(QBrush(QColor(0, 255, 0,100), Qt::SolidPattern));
  
  /* добавляем название бака */
  QFont fnt("Courier New", 10);
  QFontMetrics fm(fnt);
  path.addText(QPointF(50 - fm.width(name())/2, -5), fnt, name());
  
  painter->drawPath(path);
  
  /* уровень топлива */
  _data.height = qreal(qrand()) / RAND_MAX;
  _data.volume= qreal(qrand()) / RAND_MAX;
  
  path.addRoundedRect(QRectF(0, 0, 100, 100 * (1 - _data.height)), 10, 10);
  painter->setBrush(QBrush(QColor(255, 0, 0,100), Qt::SolidPattern));
  
  QString s = QString::number( _data.height, 'g', 3);
  path.addText(QPointF(50 - fm.width(s)/2, 50 - fm.height()), fnt, s);
  
  s = QString::number( _data.volume, 'g', 3);
  path.addText(QPointF(50 - fm.width(s)/2, 50 + fm.height()), fnt, s);
  
  painter->drawPath(path); 
  
}
  

SvConsumerGraphicsItem::SvConsumerGraphicsItem(QWidget* parent, int id, QString name):
  SvCustomGraphicsItem(parent, id, name)
{
  
}

void SvConsumerGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(widget);

  QPen pen = QPen(Qt::SolidLine);
  pen.setColor(QColor(0, 0, 0, 255));
  pen.setWidthF(0.5);
  painter->setPen(pen);
  
  QPainterPath path;

  path.addRect(QRectF(0, 40, 100, 20));
  path.addEllipse(QRectF(0, 0, 100, 100));
//   path.setFillRule(Qt::WindingFill);
  painter->setBrush(QBrush(QColor(0, 0, 255,100), Qt::SolidPattern));
  
  /* добавляем название потребителя */
  QFont fnt("Courier New", 10);
  QFontMetrics fm(fnt);
  path.addText(QPointF(50 - fm.width(name())/2, -5), fnt, name());
  
  painter->drawPath(path);
  
  /* значение */
  _data.val1 = qreal(qrand()) / RAND_MAX;
  QString strval = QString::number( _data.val1, 'g', 1);
  path.addText(QPointF(50 - fm.width(strval)/2, 52/* - fm.height()/2*/), fnt, strval);
   
  painter->drawPath(path);  
      
}


SvSensorGraphicsItem::SvSensorGraphicsItem(QWidget *parent, int id, QString name):
  SvCustomGraphicsItem(parent, id, name)
{
  
}
