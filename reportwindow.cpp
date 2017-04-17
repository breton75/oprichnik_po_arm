#include "reportwindow.h"
#include "ui_reportwindow.h"
#include <QMessageBox>
//#include "u_log.h"

//===================================================================
QLabel* getNewStatusLabel(QWidget *parent)
{
     QLabel *lbl = new QLabel(parent);
     lbl->setMinimumSize(150, 20);
     lbl->setMaximumSize(150, 20);
     return lbl;
}
//===================================================================
QString subDividedFloat(qreal f, int precisionSize = 1) {
    if(precisionSize < 0) precisionSize = 0;
    QString res = QString::number(f, 'f', precisionSize);
    int pp;
    int l = res.length();
    if(precisionSize == 0) pp = l;
    else pp = res.indexOf(".");
    //qDebug() << "number: " << f << ", first string: " << res << ", length:" << l << ", point pos:" << pp;
    int divPos = 3;
    while (divPos < pp) {
        res = res.left(pp - divPos).append(" ").append(res.right(l++ - pp + divPos));
        //qDebug() << "iteration string: " << res << ", length:" << l << ", point pos:" << pp << ", divPos: " << divPos;
        divPos += 3;
    }
    return res;
}

ReportWindow::ReportWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReportWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Report2.png"));
    ui->centralwidget->setLayout(ui->gridLayout);
    status1 = getNewStatusLabel(this);      ui->statusbar->addPermanentWidget(status1);//ui->statusbar->addWidget(status1);
    status2 = getNewStatusLabel(this);      ui->statusbar->addPermanentWidget(status2);//ui->statusbar->addWidget(status2);
    status3 = getNewStatusLabel(this);      ui->statusbar->addPermanentWidget(status3);//ui->statusbar->addWidget(status3);
    status4 = getNewStatusLabel(this);      ui->statusbar->addPermanentWidget(status4);//ui->statusbar->addWidget(status4);
    query = new QSqlQuery();
}

ReportWindow::~ReportWindow()
{
    delete ui;
}

void ReportWindow::prepareReport(int repType, QDateTime repBegin, QDateTime repEnd)
{
    QString st = "Вид отчета: ";
    QString qry;
    QString reportDateTimeFormat;
    int n = 5;
    //QString* dt_str;
    switch (repType) {
    case 0:// часовой
        {
        st += "часовой";
        n = (repBegin.daysTo(repEnd) + 1) * 24;
        reportDateTimeFormat = "dd.MM.yyyy г. hh:mm";
        qry = "SELECT dt, spend "\
              " FROM spend_hist"\
              " WHERE dt >= :dt_begin AND dt <= :dt_end"\
              " ORDER BY id;";
        }
        break;
    case 1: // суточный
        {
        st += "суточный";
        n = repBegin.daysTo(repEnd) + 1;
        reportDateTimeFormat = "dd.MM.yyyy г.";
        qry = "WITH a AS ("\
              "  SELECT "\
              "    dt, "\
              "    date_part('doy', dt) as rep_day, "\
              "    date_part('month', dt) as rep_month, "\
              "    date_part('year', dt) as rep_year, "\
              "    spend "\
              "  FROM spend_hist "\
              " WHERE dt >= :dt_begin AND dt <= :dt_end"\
              ") "\
              "SELECT "\
              "  date(min(dt)) as dt, "\
              "  sum(spend) as spend "\
              "FROM a "\
              "GROUP BY rep_year, rep_month, rep_day "\
              "ORDER BY rep_year, rep_month, rep_day;";
        }
        break;
    case 2: // месячный
        {
        st += "месячный";
        n = repEnd.date().month() - repBegin.date().month() + 1;
        reportDateTimeFormat = "MM.yyyy г.";
        qry = "WITH a AS ("\
              "  SELECT "\
              "    dt, "\
              "    date_part('month', dt) as rep_month, "\
              "    date_part('year', dt) as rep_year, "\
              "    spend"\
              "  FROM spend_hist"\
              " WHERE dt >= :dt_begin AND dt <= :dt_end"\
              ") "\
              "SELECT "\
              "  date(min(dt)) as dt, "\
              "  sum(spend) as spend "\
              "FROM a "\
              "GROUP BY rep_year, rep_month "\
              "ORDER BY rep_year, rep_month;";
        }
        break;
    case 3: // квартальный
        {
        st += "квартальный";
        n = repEnd.date().month() - repBegin.date().month() + 1;
        reportDateTimeFormat = "M квартал yyyy г.";
        qry = "WITH a AS ("\
              " SELECT "\
              "   dt, "\
              "   date_part('quarter', dt) as rep_quart, "\
              "   date_part('year', dt) as rep_year, "\
              "   spend"\
              " FROM spend_hist"\
              " WHERE dt >= :dt_begin AND dt <= :dt_end"\
              ") "\
              "SELECT "\
              "(rep_year::text || '-' || rep_quart::text || '-01')::date as dt,"
              "  sum(spend) as spend "\
              "FROM a "\
              "GROUP BY rep_year, rep_quart "\
              "ORDER BY rep_year, rep_quart;";
        }
        break;
    case 4: // годовой
        {
        st = +"годовой";
        n = repEnd.date().year() - repBegin.date().year() + 1;
        reportDateTimeFormat = "yyyy г.";
        qry = "WITH a AS ("\
              "  SELECT id, dt, date_part('year', dt) as rep_year, spend"\
              "  FROM spend_hist"\
              " WHERE dt >= :dt_begin AND dt <= :dt_end"\
              ") "\
              "SELECT "\
              "  date(min(dt)) as dt, "\
              "  sum(spend) as spend "\
              "FROM a "\
              "GROUP BY rep_year "\
              "ORDER BY rep_year;";
        }
        break;
    default: // Некорректный
        QMessageBox::critical(0, "Формирование отчета", "Некорректный тип отчета ");
        return;
        break;
    }

    st += tr(", интервал: с %1 по %2").arg(repBegin.toString("dd.MM.yy")).arg(repEnd.toString("dd.MM.yy"));
    qDebug() << "Формирование отчета: " << st;// << ", тип отчета: " << repType;
    //qDebug() << "Формат даты: " << reportDateTimeFormat;
    //qDebug() << "Запрос: " << qry;
    ui->lblRepParams->setText(st);
    ui->tableWidget->clear();

    query->finish();
    query->prepare(qry);
    query->bindValue(":dt_begin", repBegin);
    query->bindValue(":dt_end", repEnd);
    if(!query->exec()) {
        qDebug() << "Ошибка формирования отчета, отчет: " << st;
        qDebug() << query->lastError().text();
        //st = tr("Ошибка формирования отчета, отчет: %1, ошибка:<br>%2").arg(st).arg(query->lastError().text());
        //logMsg(msgError, st, ui->teLog);
        return;
    }

    n = query->size();

    ui->tableWidget->setRowCount(n);
    //ui->tableWidget->setColumnCount(6);
    QStringList hLst; // headers
    //hLst << "Дата" << "Расход, т" << "В т.ч.ГТД1" << "ГТД2" << "Вспом.";
    //hLst << "Дата" << "Расход ДГ, т" << "Расход ГД, т" << "Расход - Всего, т" << "Принято, т" << "Выдано, т";
    //hLst << "Дата" << "Расход, т";
    hLst << "Дата" << "Расход, т" << "В т.ч.ГД" << "ДГ";
    ui->tableWidget->setColumnCount(hLst.size());
    ui->tableWidget->setHorizontalHeaderLabels(hLst);

    QStringList mLst;// = QStringList() << "Январь" << "Февраль" << "Март" << "Апрель" << "Май" << "Июнь" << "Июль" << "Август" << "Сентябрь" << "Октябрь" << "Ноябрь" << "Декабрь";

    QTableWidgetItem *newItem;
    for(int row = 1; row <= n; ++row){
        if(!query->next()) return;
//        for(int column=0; column!=ui->tableWidget->columnCount(); ++column) {
//            //newItem = new QTableWidgetItem(tr("%1").arg((row)*(column+1)));
//            newItem = new QTableWidgetItem(query->value(column).toString());
//            ui->tableWidget->setItem(row - 1, column, newItem);
//        }
        QString dt_string;
        QDateTime dt = query->value("dt").toDateTime();
        switch (repType) {
        case 0: // почасовой
            {
            int h = dt.time().hour() + 1;
            if(h == 24) h = 0;
            QString h_st = QString::number(h);
            if(h < 10) h_st = "0" + h_st;
            dt_string = QDateTime(query->value("dt").toDateTime()).toString(reportDateTimeFormat).append("-").append(h_st).append(":00");
            }
            break;
        case 2: // помесячный
            {
            //mLst = QStringList() << "Январь" << "Февраль" << "Март" << "Апрель" << "Май" << "Июнь" << "Июль" << "Август" << "Сентябрь" << "Октябрь" << "Ноябрь" << "Декабрь";
            mLst << "Январь" << "Февраль" << "Март" << "Апрель" << "Май" << "Июнь" << "Июль" << "Август" << "Сентябрь" << "Октябрь" << "Ноябрь" << "Декабрь";
            int m = dt.date().month() - 1;
            dt_string = QString(mLst.at(m)).append(tr(" %1 г.").arg(dt.date().year()));
            }
            break;
        default:
            dt_string = QDateTime(query->value("dt").toDateTime()).toString(reportDateTimeFormat);
            break;
        }
        //newItem = new QTableWidgetItem(QDateTime(query->value("dt").toDateTime()).toString(reportDateTimeFormat));
        newItem = new QTableWidgetItem(dt_string);
        ui->tableWidget->setItem(row - 1, 0, newItem);
        //random()
        //qrand
        qreal spend = query->value("spend").toFloat();
        int gdPart = rand() % 10 + 75;
        qreal spendGD = spend * gdPart / 100;
        newItem = new QTableWidgetItem(subDividedFloat(spend));//QString::number(spend, 'f', 1));
        //newItem->setTextAlignment(Qt::AlignRight);
        //newItem->setTextAlignment(Qt::AlignRight && Qt::AlignVCenter);
        newItem->setTextAlignment(Qt::AlignRight + Qt::AlignVCenter);
        ui->tableWidget->setItem(row - 1, 1, newItem);
        newItem = new QTableWidgetItem(subDividedFloat(spendGD));//QString::number(spendGD, 'f', 1));
        newItem->setTextAlignment(Qt::AlignRight + Qt::AlignVCenter);
        ui->tableWidget->setItem(row - 1, 2, newItem);
        newItem = new QTableWidgetItem(subDividedFloat(spend - spendGD));//QString::number(spend - spendGD, 'f', 1));
        newItem->setTextAlignment(Qt::AlignRight + Qt::AlignVCenter);
        ui->tableWidget->setItem(row - 1, 3, newItem);
    }
//    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeColumnToContents(0);
    ui->tableWidget->resizeColumnToContents(1);
    int cw = ui->tableWidget->columnWidth(1) + 20;
    ui->tableWidget->setColumnWidth(1, cw);
    ui->tableWidget->setColumnWidth(2, cw);
    ui->tableWidget->setColumnWidth(3, cw);
//    ui->tableWidget->resizeColumnToContents(2);
}

void ReportWindow::setStatus(int idx, QString newStatus)
{
    ui->statusbar->showMessage(newStatus, 2000);
    return;

    switch (idx) {
    case 0:
    case 1:
        status1->setText(newStatus);
        break;
    case 2:
        status2->setText(newStatus);
        break;
    case 3:
        status3->setText(newStatus);
        break;
    case 4:
        status4->setText(newStatus);
        break;
    default:
        break;
    }
}

void ReportWindow::closeEvent()
{
    //save changes?
    setStatus(2, "");
}

void ReportWindow::on_bnPrint_clicked()
{
    setStatus(2, "Печать - будет позже");
    return;

    QString st = subDividedFloat(1234567890.1789, 5);
    qDebug() << "subDividedFloat: " << st;
}

void ReportWindow::on_bnSave_clicked()
{
    setStatus(2, "Сохранить - будет позже");
}

void ReportWindow::on_bnClose_clicked()
{
    hide();
}
