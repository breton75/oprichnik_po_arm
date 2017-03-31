#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "reportwindow.h"

QMutex MUT;

//==========================================================================================
// компонент - цистерна на мнемосхеме
class GraphicTankItem: public QGraphicsRectItem {

private:
    // точка на графике (на сцене)
    int posX;
    int posY;
    int sizeX;
    int sizeY;
    // координаты точки нажатия мышки внутри эллипса
    int mPosX;
    int mPosY;
    QString tankName;
    int tankId;
    qreal tankFuelMax;
    qreal tankFuelCurrent;
    QGraphicsTextItem* tiName;


public:
    explicit GraphicTankItem(QGraphicsItem * parent = 0): QGraphicsRectItem(parent) {
        setAcceptHoverEvents(true);
        posX = 0;
        posY = 0;
        sizeX = 50;
        sizeY = 50;
        mPosX = 0;
        mPosY = 0;
        setBrush(QBrush(Qt::lightGray));
        setPen(QPen(QBrush(Qt::black), 2));
        setRect(posX, posY, sizeX, sizeY);
        //setFlag(QGraphicsItem::ItemIsMovable, true);
        tiName = 0;//new QGraphicsTextItem(this);
    }

    // задание координат на сцене
    void setXY(qreal xPos, qreal yPos, qreal xSize, qreal ySize) {
        posX = xPos;
        posY = yPos;
        sizeX = xSize;
        sizeY = ySize;
        setRect(posX, posY, sizeX, sizeY);
        if(tiName) {
            QRectF r = boundingRect();
            //tiName->setPos(r.left(), r.bottom() - 20);
            qreal tLeft = r.left() + r.width() / 2 - tiName->boundingRect().width() / 2;
            tiName->setPos(tLeft, r.bottom() - 25);
        }
    }

    void setTankParams(QString aTankName, int aTankId, qreal aTankFuelMax) {
        tankName = aTankName;
        tankId = aTankId;
        tankFuelMax = aTankFuelMax;
        tiName = new QGraphicsTextItem(aTankName, this);
        //tiName->setPos(this->boundingRect().left(),  this->boundingRect().top() + sizeY - 20);
        QRectF r = boundingRect();
        //tiName->setPos(r.left(), r.bottom() - 20);
        qreal tLeft = r.left() + r.width() / 2 - tiName->boundingRect().width() / 2;
        tiName->setPos(tLeft, r.bottom() - 20);
        //this->paint();
    }

    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * ) {
        QApplication::setOverrideCursor(Qt::PointingHandCursor);
    }

    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * ) {
        QApplication::restoreOverrideCursor();
    }

//    virtual void paintEvent(QPaintEvent* pe) {
//        QPainter pntr;//this->parentWidget());
//        QRect r(40, 40, 100, 100);
//        if(pe->region().contains(r)) {
////            pntr.begin(this->pa);
//            pntr.drawRect(r);
//        }
//    }

    // работает; разобраться - как
//    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
//        if(this->isVisible()) qDebug() << "paint tank " << tankId;
//        QRect rect = this->rect().toRect();//boundingRect().toRect();//option->rect;
//        QLinearGradient gradient(0, 0, rect.width(), rect.height());

//        gradient.setColorAt(0, Qt::white);
//        gradient.setColorAt(0.5, Qt::blue);
//        gradient.setColorAt(1, Qt::green);
//        painter->setBrush(gradient);
//        painter->drawRect(rect);
//        //QGraphicsRectItem::paint(painter, option, widget);
//    }
};
//==========================================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , fTanksCount(0), fSensorsCount(0), fConsumersCount(0)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Anchor.png"));
    ui->centralWidget->setLayout(ui->gridLayout);

    scene = new QGraphicsScene();
    picItem = 0;
    // статичная картинка
    //QPixmap *fuel_pic = new QPixmap("/home/user/distrib/12700_pict/fuel_sys.jpg");
    //QPixmap *fuel_pic = new QPixmap(":/fuel_system_7.png");//"./fuel_system_7.png");

//    QPixmap *fuel_pic = new QPixmap(":/fuel_system_9.jpg");
//    scene->setSceneRect(fuel_pic->rect());
//    picItem = new QGraphicsPixmapItem(*fuel_pic);
//    scene->addItem(picItem);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
//    ui->graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft); //!!
    ui->graphicsView->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
//    resizeTimer = new QTimer();
    //resizeTimer->st
//    connect(resizeTimer, SIGNAL(timeout()), this, SLOT(formResized()));

//    db = new QSqlDatabase();//"QPSQL");
//    query = new QSqlQuery();

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("172.16.4.83");
    db.setPort(5432);
    db.setDatabaseName("drain_client");
    db.setUserName("postgres");
    db.setPassword("postgres");
    if(!db.open())
    {
        QString errSt = db.lastError().text();
        qDebug() << "Ошибка открытия БД проекта: " << errSt;
        QMessageBox::critical(this, "Ошибка соединения с БД проекта", errSt);
        return;
    } 
    
    
    /* свиридов */
    _tanks_map = QMap<int, SvTankGraphicsItem*>();
    
    /* читаем баки из БД */
    QString sql_sqlect_tanks = "SELECT "
                               " tanks.id as id, "
                               " tanks.tank_type as tank_type,"
                               " tanks.sensor_type as sensor_type,"
                               " tanks.name as tank_name "
//                               " sensors.id as sensor_id,"
//                               " sensors.is_active as is_active"
                               " FROM tanks "
                               " ORDER BY id";
//                               " LEFT JOIN sensors on sensors.tank_id = tanks.id";
    
    
    QSqlQuery* q = new QSqlQuery(db);
    if(!q->exec(sql_sqlect_tanks))
    {
      qDebug() << q->lastError().text();
      q->finish();
      delete q;
      return;
    }
    
    
    while(q->next())
    {
      int id = q->value("id").toInt();
      QString name = q->value("tank_name").toString();
      SvTankGraphicsItem* tank = new SvTankGraphicsItem(this, id, name);
      tank->setPos(120*((id-1)%3), 125*int((id-1)/3));
       
      _tanks_map.insert(id, tank);
      scene->addItem(tank);
    }
    
    q->finish();
    
    /* читаем потребителей из БД */
    sql_sqlect_tanks = "SELECT "
                               " consumers.id as id, "
                               " consumers.consumer_type as consumer_type,"
                               " consumers.meter_type as meter_type,"
                               " consumers.name as consumer_name "
                               " FROM consumers "
                               " ORDER BY id";
//                               " LEFT JOIN sensors on sensors.tank_id = tanks.id";
    
    
    q = new QSqlQuery(db);
    if(!q->exec(sql_sqlect_tanks))
    {
      qDebug() << q->lastError().text();
      q->finish();
      delete q;
      return;
    }
    
    while(q->next())
    {
      int id = q->value("id").toInt();
      QString name = q->value("consumer_name").toString();
      SvConsumerGraphicsItem* consumer = new SvConsumerGraphicsItem(this, id, name);
      consumer->setPos(400, 125*(id-1));
      
      _consumers_map.insert(id, consumer);
      scene->addItem(consumer);
    }
    
    q->finish();
    delete q;

    _pg_thr = new SvPGThread(db);
    connect(_pg_thr, SIGNAL(finished()), _pg_thr, SLOT(deleteLater()));
    connect(_pg_thr, SIGNAL(tank_updated(int, TankDataStruct)), this, SLOT(repaintTank(int, TankDataStruct)));
    connect(_pg_thr, SIGNAL(consumer_updated(int, ConsumerDataStruct)), this, SLOT(repaintConsumer(int, ConsumerDataStruct)));
    connect(_pg_thr, SIGNAL(sensor_updated(int, SensorDataStruct)), this, SLOT(repaintSensor(int, SensorDataStruct)));
    connect(_pg_thr, SIGNAL(recalc()), this, SLOT(recalc()));
    _pg_thr->startTimer(1000);

    
    
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName(QByteArray("UTF-8"))); // это и так по умолчанию
    fRepSetup = new ReportSetup(this);//fm_report_params(this);
    fRepSetup->hide();

    fRep = new ReportWindow();//this);
    fRep->hide();

    fFuelMove = new FuelMove();//this);
    fFuelMove->hide();

    fCheck = new CheckForm();//this);
    fCheck->hide();

    fEventsLog = new EventsLog();
    fEventsLog->hide();
    
}

MainWindow::~MainWindow()
{
    fEventsLog->deleteLater();
    fCheck->deleteLater();
    fRep->deleteLater();
    fRepSetup->deleteLater();
    delete ui;
}

// подготовка сцены, создание элементов
void MainWindow::prepareScheme()
{
    if(!query) return;
    scene->items().clear();
    int i;
    qreal x = 0;
    qreal y = 0;
    qreal w = 150;
    qreal h = 150;
    //QGraphicsRectItem* tankRect;
    GraphicTankItem* tankRect;
    //QGraphicsTextItem* tankName, tankRest;

    // tanks
    QString qry = "WITH tm AS (SELECT tank_id, max(v_volume) as max_volume FROM tank_measures GROUP BY tank_id) "\
          "SELECT t.id, t.name, tt.name as type_name, to_char(tm.max_volume, 'FM9990.000') as max_volume FROM tanks t "\
          "JOIN tm ON tm.tank_id = t.id JOIN tank_types tt ON tt.id = t.tank_type ORDER BY t.id;";
    if(query->exec(qry)) {
        fTanksCount = query->size();
        query->first();
        for (i = 0; i < fTanksCount; ++i) {
            x += 10;
            y += 10;
            tankRect = new GraphicTankItem();//QGraphicsRectItem(x, y, w, h);
            tankRect->setXY(x, y, w, h);
            tankRect->setTankParams(query->value("name").toString(), query->value("id").toInt(), query->value("max_volume").toFloat());
            scene->addItem(tankRect);
            query->next();
        }
    } else fTanksCount = 0;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->scale(8, 8);//5, 5);
}

//void MainWindow::showEvent(QShowEvent *) {
//    ui->statusBar->showMessage("Show", 1000);
////    QRect r = ui->graphicsView->rect();
////    qDebug() << "graphicsView rect: " << r.width() << "x"  << r.height();
////    QRectF rf = ui->graphicsView->sceneRect();
////    qDebug() << "graphicsView scene rect: " << rf.left() << ":" << rf.top() << ", " << rf.width() << "x"  << rf.height();
////    //ui->graphicsView->scene()->b
////    //ui->graphicsView->adjustSize();
////    QSize s = ui->graphicsView->sizeHint();
////    qDebug() << "graphicsView size hint: " << s.width() << "x" << s.height();
////    //if(!picItem) return;
////    //qDebug() << picItem;
//    //prepareScheme();
//    if(scene->items().count() <= 0) return;
////      ui->graphicsView->fitInView(picItem, Qt::KeepAspectRatio);
//    //ui->graphicsView->setViewport(ui->graphicsView);
//    //ui->graphicsView->fitInView(scene->items().at(0), Qt::KeepAspectRatio);
//    //resizeEvent(0);
//    formResized();
//}

void MainWindow::formResized()
{
    resizeTimer->stop();
    if(scene->items().count() <= 0) return;

    if (!this->isVisible()) return;
    ui->statusBar->showMessage("Resize", 1000);
   // return;
    scene->setSceneRect(ui->graphicsView->rect());
//    QRectF rf = scene->sceneRect();
//    //qDebug() << "resize, scene rect: " << rf.left() << ":" << rf.top() << ", " << rf.width() << "x"  << rf.height();
//    qDebug() << "resize, scene rect: " << rf.width() << "x"  << rf.height();
//    QRect r = ui->graphicsView->rect();
//    qDebug() << "resize, graphicsView rect: " << r.width() << "x"  << r.height();
//    //ui->graphicsView->scene()->sceneRect()

//    r = this->rect();
    //qDebug() << "form rect: " << r.left() << ":" << r.top() << ", " << r.width() << "x"  << r.height();
//    qDebug() << "form rect: " << r.width() << "x"  << r.height();
    //scene->set
    //ui->graphicsView->scale(10, 10);

    if(scene->items().count() > 0) {
        qreal tankH = scene->height() / 5;
        qreal tankW = scene->width() / 4;
//        qDebug() << "scene W:H " << scene->width() << scene->height() << ", tank W:H " << tankW << tankH;
        qreal tankX, tankY;
        int tankIdx = -1;
        int tCol;// = i / 5;
        int tRow;// = i % 5;
        scene->setSceneRect(ui->graphicsView->rect());
        for (int i = 0; i < scene->items().count(); ++i) {
            //qDebug() <<
            QGraphicsItem* gi = scene->items().at(i);
            GraphicTankItem* ti = dynamic_cast<GraphicTankItem*>(gi);//GraphicTankItem(gi);
            if(ti) {
                tankIdx++;
                tCol = tankIdx / 5;
                tRow = tankIdx % 5;
                tankX = tCol * tankW;// * 1.5 - tankW;//tankW;//((i / 5) +  tankW * 2);
                tankY = tRow * tankH;// * 1.5 - tankH;//tankH * i;//i * (i / 5) * tankH;
                //qDebug() << "tank " <<  tankIdx << ", row:col " << tRow << tCol << ", X:Y " << tankX << tankY << ", W:H " << tankW << tankH;
                ti->setXY(tankX, tankY, tankW, tankH);
            }
            //if(gi->in)
        }
    }
    return;
    if(picItem)
        ui->graphicsView->fitInView(picItem, Qt::KeepAspectRatio);
}

//void MainWindow::resizeEvent(QResizeEvent *) {
//   resizeTimer->start(500);
//}

// масштабирование, корректировка, отрисовка
void MainWindow::drawScene()
{
    if(!query) return;
//    int i;
//    int x = 0;
//    int y = 0;
//    int tankW = ;
//    QGraphicsRectItem* tankRect;
//    QGraphicsTextItem* tankName, tankRest;

//    // tanks
//    if(query->exec("")) {
//        fTanksCount = query->size();
//        query->first();
//        for (i = 0; i < fTanksCount; ++i) {
//            x += 10;
//            y += 10;

//        }
//    } else fTanksCount = 0;
}

void MainWindow::on_pbnReport_clicked()
{
    int rc = fRepSetup->exec();
//    QMessageBox::information(this, tr("Отчет"), tr("Отчет - тест: %1").arg(fRepSetup->test));
    if (rc == QDialog::Accepted) {
        //QMessageBox::information(this, "Отчет", "Составляем отчет");
        fRep->prepareReport(fRepSetup->repType(), fRepSetup->repBegin(), fRepSetup->repEnd());
        fRep->show();
        fRep->setFocus();
    }
    else if (rc == QDialog::Rejected) {
        //QMessageBox::information(this, "Отчет", "Отменен отчет");
    }
}


void MainWindow::on_pbnFuelMove_clicked()
{
    //ui->statusBar->showMessage("Прием/выдача топлива - будет позже", 2000);
    int rc = fFuelMove->exec();
    if (rc == QDialog::Accepted) {
        //QMessageBox::information(this, "Отчет", "Составляем отчет");
        ui->statusBar->showMessage("Прием/выдача топлива - будет позже", 2000);
    }
    else if (rc == QDialog::Rejected) {
        //QMessageBox::information(this, "Отчет", "Отменен отчет");
        ui->statusBar->showMessage("Прием/выдача топлива - отказ от ввода данных", 2000);
    }
}

void MainWindow::on_pbnCheck_clicked()
{
    //ui->statusBar->showMessage("Проверка - будет позже", 2000);
    if(!(QApplication::keyboardModifiers() & Qt::ShiftModifier)) {
        fCheck->show();
        return;
    }

    if(!picItem) return;

    QString strFilter;// = picItem->pixmap().f;
    QString strDir = "";
    QString str = QFileDialog::getOpenFileName(0, tr("Select file"), strDir, "*.png *.jpg *.gif;;*.png;;*.jpg;;*.gif", &strFilter);
    //QMessageBox::information(this, "File selected", str);
    if (str == "") return;

    QPixmap *fuel_pic = new QPixmap(str);
//    ui->lblMnemo->setPixmap(*fuel_pic);
    picItem->setPixmap(*fuel_pic);
    scene->setSceneRect(fuel_pic->rect());
    ui->graphicsView->fitInView(picItem, Qt::KeepAspectRatio);
}

void MainWindow::on_pbnEvents_clicked()
{
    //ui->statusBar->showMessage("Журнал событий - будет позже", 2000);
    fEventsLog->show();
}


/* свиридов */
void MainWindow::repaintTank(int id, TankDataStruct data)
{
  _tanks_map.value(id)->setValue(data);
  _tanks_map.value(id)->update();
}

void MainWindow::repaintConsumer(int id, ConsumerDataStruct data)
{
  _consumers_map.value(id)->setValue(data);
  _consumers_map.value(id)->update();
}

void MainWindow::repaintSensor(int id, SensorDataStruct data)
{
  _sensors_map.value(id)->setValue(data);
  _sensors_map.value(id)->update();
}

void MainWindow::recalc()
{
  total_tanks = 0;
  foreach (SvTankGraphicsItem *item, _tanks_map.values())
    total_tanks += item->data().volume;
  
  total_out = 0;
  foreach (SvConsumerGraphicsItem *item, _consumers_map.values())
    total_out += item->data().val1;
  
//  qDebug() << total_tanks;
  ui->label_6->setText(QString("%1 л.").arg(total_tanks, 0, 'g', 3));
  ui->label_7->setText(QString("%1 л.").arg(total_out, 0, 'g', 1));
  
}

SvPGThread::SvPGThread(QSqlDatabase &db)
{
  _db = db;
}

SvPGThread::~SvPGThread()
{ 
  _isWorking = false;
  while(!_isFinished) QApplication::processEvents();
  deleteLater();
}

void SvPGThread::run()
{
  
}

void SvPGThread::timerEvent(QTimerEvent *te)
{
    QString sql_tanks = "SELECT tank_id, dt, fuel_height, pressure, volume, weight "
                  " FROM tanks_fuel_rest;";
    
    QString sql_consumers = "";
    
    
    MUT.lock();
    
    QSqlQuery *q = new QSqlQuery(_db);
    
    if(!q->exec(sql_tanks))
    {
      qDebug() << q->lastError().text();
      q->finish();
      delete q;
      MUT.unlock();
      return;
    }
    
    TankDataStruct data;
    while(q->next())
    {
      int id = q->value("tank_id").toInt();
      data.height = q->value("fuel_height").toReal();
      data.volume = q->value("volume").toReal();
      emit tank_updated(id, data);
    }
    
    q->finish();
    
    
    delete q;
    
    MUT.unlock();
    
    emit recalc();
    
}
