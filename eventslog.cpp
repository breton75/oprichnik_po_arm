#include "eventslog.h"
#include "ui_eventslog.h"

#include <QTextCodec>
#include <QFile>
//#include <locale.h>

//------------------------- вспомогательные процедуры и функции -----------------
QString str_char_vals(QString st) {
    int n = st.size();
    QString msg = "String char codes: ";
    for (int i = 0; i < n; ++i) {
        QChar ch = st.at(i);
        ushort ch_code = ch.unicode();
        msg += QString("%1(%2)-").arg(QString::number(ch_code, 10)).arg(QString::number(ch_code, 16).toUpper());
    }
    msg.truncate(msg.size() - 1);
    return msg;
}

QString ba_char_vals(QByteArray ba) {
    int n = ba.size();
    QString msg = "ByteArray char codes: ";
    for (int i = 0; i < n; ++i) {
        char c = ba.at(i);
        quint8 ch_code = (quint8) c;
        msg += QString("%1(%2)-").arg(QString::number(ch_code, 10)).arg(QString::number(ch_code, 16).toUpper());
    }
    msg.truncate(msg.size() - 1);
    return msg;
}
//-------------------------------------------------------------------------------

EventsLog::EventsLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EventsLog)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->glMain);
    setWindowIcon(QIcon(":/List.ico"));
    setWindowModality(Qt::WindowModal);

    ui->pbnReadFromFile->setVisible(false);
    ui->textEdit->setVisible(false);

    ui->deBegin->setDate(QDate::fromString("2017-03-01", Qt::ISODate));
    ui->deBegin->setTime(QTime::fromString("00:00"));
    //ui->deEnd->setDate(QDate::currentDate ());
    ui->deEnd->setDate(QDate::fromString("2017-03-05", Qt::ISODate));
    ui->deEnd->setTime(QTime::fromString("23:59"));

    query = new QSqlQuery();
}

EventsLog::~EventsLog()
{
    query->finish();
    delete query;
    delete ui;
}

void EventsLog::showEvent(QShowEvent *)
{
    createEventLog();
}

void EventsLog::on_pbnGetLog_clicked()
{
    createEventLog();
    return;

    QDateTime repBegin = ui->deBegin->dateTime();
    QDateTime repEnd = ui->deEnd->dateTime();
    if(repEnd.date().year() != repBegin.date().year()) repEnd = repBegin;

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Дата/время" << "Событие");

    // список событий
    QString qry = "SELECT"\
    "  sl.id, sl.dt, "\
    "  sl.event_type_id, et.name as event_name,"\
    "  st.description || ' - ' || coalesce(t.name, c.name) as sensor_name,"\
    "  coalesce(t.name, c.name) as place_name, "\
    "  sl.num1, "\
    "  sl.num2, "\
    "  sl.num3, "\
    "  sl.description "\
    "FROM system_log sl"\
    "       JOIN"\
    "     event_types et"\
    "       ON et.id = sl.event_type_id"\
    "       LEFT JOIN"\
    "     sensors s"\
    "       ON s.id = sl.num1"\
    "       LEFT JOIN"\
    "     sensor_types st"\
    "       ON st.id = s.sensor_type"\
    "       LEFT JOIN"\
    "     tanks t"\
    "       ON t.id = s.tank_id"\
    "       LEFT JOIN"\
    "     consumers c"\
    "       ON c.id = s.consumer_id "\
    "WHERE sl.dt >= :dt_begin AND sl.dt <= :dt_end "\
    "ORDER BY sl.dt;"; //
    query->finish();
    query->prepare(qry);
    query->bindValue(":dt_begin", repBegin);
    query->bindValue(":dt_end", repEnd);

    //if(!query->exec(qry))
    if(!query->exec())
    {
        qDebug() << "Ошибка формирования отчета с " << repBegin << " по " << repEnd << ":";
        qDebug() << query->lastError().text();
        query->finish();
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->resizeColumnsToContents();
        return;
    }

    int n = query->size();
    ui->tableWidget->setRowCount(n);
    QString reportDateTimeFormat = "dd.MM.yyyy г. hh:mm:ss";
    int param1, param2;
    qreal param3;
    //QString sensor_name;
    QString event_desc;
    QTableWidgetItem *newItem;
    for(int row = 0; row < n; ++row){
        if(!query->next()) return;

        newItem = new QTableWidgetItem(QDateTime(query->value("dt").toDateTime()).toString(reportDateTimeFormat));//dt_string);
        ui->tableWidget->setItem(row, 0, newItem);
        int event_type = query->value("event_type_id").toInt();
        param1 = query->value("num1").toInt();
        param2 = query->value("num2").toInt();
        param3 = query->value("num3").toInt();

        switch (event_type) {
        case 1: // превышение норм расхода
            if(param1 != 0) {
                // превышение
                event_desc = tr("Превышение нормы расхода топлива на %1 л/мин").arg(QString::number(param3, 'f', 2));
            } else {
                // возврат в норму
                event_desc = "Расход топлива вошел в норму";
            }
            break;
        case 2: // разница измерения уровня и расхода
            if(param1 != 0) {
                // превышение
                event_desc = tr("Разница измерения уровня и расхода топлива на %1 л").arg(QString::number(param3, 'f', 2));
            } else {
                // возврат в норму
                event_desc = "Разница измерения уровня и расхода топлива прекратилась";
            }
            break;
        case 3: // ручной ввод
            event_desc = "Ручной ввод";
            break;
        case 4: // нет связи
            if(param2 == 0) {
                // пропала связь
                event_desc = tr("Пропала связь, либо отказ датчика: %1").arg(query->value("sensor_name").toString());
            } else {
                // появилась связь
                event_desc = tr("Восстановилась связь с датчиком: %1").arg(query->value("sensor_name").toString());
            }
            break;
        case 5: // движение топлива
            if(param3 > 0) {
                // загрузка топлива
                event_desc = tr("Принято топлива: %1 т").arg(QString::number(param3, 'f', 2));
            } else {
                // отгрузка (выдача) топлива
                event_desc = tr("Отгрузка топлива: %1 т").arg(QString::number(-param3, 'f', 2));
            }
            break;
        default:
            event_desc = "???";
            break;
        }
        newItem = new QTableWidgetItem(event_desc);
        ui->tableWidget->setItem(row, 1, newItem);
    }
    //ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeColumnToContents(0);

return;

    int dc = repEnd.date().dayOfYear() - repBegin.date().dayOfYear() + 1;
    ui->tableWidget->setRowCount(dc);

    QDateTime dtCurrent = repBegin;
    for (int i = 0; i < dc; ++i) {
        //ui->tableWidget->cellWidget(i, 0) = new QTableWidgetItem(dtCurrent.toString("dd.MM.yy HH:mm:ss"));
        //ui->tableWidget->cellWidget(i, 1) = new QTableWidgetItem("");
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(dtCurrent.toString("dd.MM.yy HH:mm:ss")));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(""));
        dtCurrent = dtCurrent.addDays(1);
    }
    ui->tableWidget->resizeColumnToContents(0);
return;

//    // список событий
//    QString qry = "SELECT"\
//    "  sl.id, sl.dt, "\
//    "  sl.event_type_id, et.name as event_name,"\
//    "  st.description || ' - ' || coalesce(t.name, c.name) as sensor_name,"\
//    "  coalesce(t.name, c.name) as place_name, "\
//    "  sl.num1, "\
//    "  sl.num2, "\
//    "  sl.num3, "\
//    "  sl.description "\
//    "FROM system_log sl"\
//    "       JOIN"\
//    "     event_types et"\
//    "       ON et.id = sl.event_type_id"\
//    "       LEFT JOIN"\
//    "     sensors s"\
//    "       ON s.id = sl.num1"\
//    "       LEFT JOIN"\
//    "     sensor_types st"\
//    "       ON st.id = s.sensor_type"\
//    "       LEFT JOIN"\
//    "     tanks t"\
//    "       ON t.id = s.tank_id"\
//    "       LEFT JOIN"\
//    "     consumers c"\
//    "       ON c.id = s.consumer_id "\
//    "ORDER BY sl.dt;";

//    if(!query->exec(qry))
//    {
//      qDebug() << query->lastError().text();
//      query->finish();
//      ui->tableWidget->setRowCount(0);
//      ui->tableWidget->resizeColumnsToContents();
//      return;
//    }

//    int n = query->size();
//    ui->tableWidget->setRowCount(n);
//    QString reportDateTimeFormat = "dd.MM.yyyy г. hh:mm:ss";
//    int param1, param2;
//    qreal param3;
//    //QString sensor_name;
//    QString event_desc;
//    QTableWidgetItem *newItem;
//    for(int row = 0; row < n; ++row){
//        if(!query->next()) return;

//        newItem = new QTableWidgetItem(QDateTime(query->value("dt").toDateTime()).toString(reportDateTimeFormat));//dt_string);
//        ui->tableWidget->setItem(row, 0, newItem);
//        int event_type = query->value("event_type_id").toInt();
//        param1 = query->value("num1").toInt();
//        param2 = query->value("num2").toInt();
//        param3 = query->value("num3").toInt();

//        switch (event_type) {
//        case 1: // превышение норм расхода
//            if(param1 != 0) {
//                // превышение
//                event_desc = tr("Превышение нормы расхода топлива на %1 л/мин").arg(QString::number(param3, 'f', 2));
//            } else {
//                // возврат в норму
//                event_desc = "Расход топлива вошел в норму";
//            }
//            break;
//        case 2: // разница измерения уровня и расхода
//            if(param1 != 0) {
//                // превышение
//                event_desc = tr("Разница измерения уровня и расхода топлива на %1 л").arg(QString::number(param3, 'f', 2));
//            } else {
//                // возврат в норму
//                event_desc = "Разница измерения уровня и расхода топлива прекратилась";
//            }
//            break;
//        case 3: // ручной ввод
//            event_desc = "Ручной ввод";
//            break;
//        case 4: // нет связи
//            if(param2 == 0) {
//                // пропала связь
//                event_desc = tr("Пропала связь, либо отказ датчика: ").arg(query->value("sensor_name").toString());
//            } else {
//                // появилась связь
//                event_desc = tr("Восстановилась связь с датчиком: ").arg(query->value("sensor_name").toString());
//            }
//            break;
//        case 5: // движение топлива
//            if(param3 > 0) {
//                // загрузка топлива
//                event_desc = tr("Принято топлива: % т").arg(QString::number(param3, 'f', 2));
//            } else {
//                // отгрузка (выдача) топлива
//                event_desc = tr("Отгрузка топлива: % т").arg(QString::number(-param3, 'f', 2));
//            }
//            break;
//        default:
//            event_desc = "???";
//            break;
//        }
//        newItem = new QTableWidgetItem(event_desc);
//        ui->tableWidget->setItem(row, 1, newItem);
//    }
//    ui->tableWidget->resizeColumnsToContents();

//return;



//    int rc = ui->dteEnd->date().dayOfYear()

    // отладка - проблемы с перекодировкой KOI-8R <-> UTF-8
//    QChar ch;
//    for (ushort i = 32; i < 256; ++i) {
//        ch = QChar(i);
//        //qDebug() << "char " << i << ": " << ch;
//        ui->textEdit->append(tr("char %1: %2").arg(i).arg(ch));
//    }
//    return;


    //setlocale(LC_ALL, "KOI8-R");
    //setlocale(LC_ALL, "KOI-8");
//    setlocale(LC_ALL, "koi-8r");

    QTextCodec* l_codec = QTextCodec::codecForLocale();
    //ui->textEdit->append(tr("local codec name: '%1'").arg(QString(l_codec->name())));

    //QString st = QString::to "ЦРТ №1";
    QString st = "ЦРТ №1";
    ui->textEdit->append(tr("Исходная строка: %1").arg(st));
    QString debug_st = "Коды символов строки: ";
    int st_len = st.size();
    QByteArray ba = st.toLocal8Bit();
    //ba.append(st);
    //int
            n = ba.size();
    ui->textEdit->append(tr("Размер строки: %1, массива байт: %2").arg(st_len).arg(n));
    for (int i = 0; i < st_len; ++i) {
        QChar ch = st.at(i);
        ushort ch_code = ch.unicode();
        //debug_st += tr("%1(%2)").arg(ch_code, 10).arg(ch_code, 16);
        debug_st += tr("%1(%2)-").arg(QString::number(ch_code, 10)).arg(QString::number(ch_code, 16).toUpper());
    }
    debug_st.truncate(debug_st.size() - 1);
    ui->textEdit->append(debug_st);

    debug_st = "Коды символов массива: ";
    for (int i = 0; i < n; ++i) {
        char c = ba.at(i);
        //ushort ch_code = c;
        quint8 ch_code = (quint8) c;
        //debug_st += tr("%1(%2)").arg(ch_code, 10).arg(ch_code, 16);
        debug_st += tr("%1(%2)-").arg(QString::number(ch_code, 10)).arg(QString::number(ch_code, 16).toUpper());
    }
    debug_st.truncate(debug_st.size() - 1);
    ui->textEdit->append(debug_st);

    int i, c;
    //QString msg = "Native bytearray: ";
    QString msg = "Исходный массив байт: ";
    for (i = 0; i < n; ++i) {
        c = ba.at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString(ba));
    ui->textEdit->append(msg);


return;

    n = st.size();
    msg = tr("string length: %1").arg(n);
    //ui->textEdit->append(msg);

//    msg += ", car vals: ";
//    for (i = 0; i < n; ++i) {
//        c = st.at(i);
//        QChar
//        msg += tr("%1 ").arg(c);
//    }

    //QByteArray baUnic = QByteArray().append(l_codec->toUnicode(ba));

    QString stUnic = l_codec->toUnicode(ba);
    QByteArray baUnic = QByteArray().append(stUnic);
    n = baUnic.size();
    msg = "LocalToUnicode bytearray: ";
    for (i = 0; i < n; ++i) {
        c = baUnic.at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString(baUnic));
    msg += tr(", was changed: %1").arg(baUnic == ba ? "no" : "yes");
    ui->textEdit->append(msg);

    QTextCodec* codec = QTextCodec::codecForName("KOI8-R");// CP866 (DOS), cp1251, "Windows-1251" (Win),

    QByteArray baUnicToKoi8R = codec->fromUnicode(ba);
    n = baUnicToKoi8R.size();
    msg = "UnicodeToKoi8R bytearray: ";
    for (i = 0; i < n; ++i) {
        c = baUnicToKoi8R.at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString(baUnicToKoi8R));
    ui->textEdit->append(msg);

    QString stKoi8ToUnic(codec->toUnicode(baUnicToKoi8R));
    QByteArray baKoi8rToUnic = QByteArray().append(stKoi8ToUnic);//codec->toUnicode(baUnicToKoi8R);
    n = baKoi8rToUnic.size();
    msg = "Koi8rToUnicode bytearray: ";
    for (i = 0; i < n; ++i) {
        c = baKoi8rToUnic.at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString(baKoi8rToUnic));
    ui->textEdit->append(msg);

    QByteArray baUnicToLocal = l_codec->fromUnicode(QString(baKoi8rToUnic));
    n = baUnicToLocal.size();
    msg = " bytearray: ";
    for (i = 0; i < n; ++i) {
        c = baUnicToLocal.at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString(baUnicToLocal));
    ui->textEdit->append(msg);

    QString stRes(baUnicToLocal);
    msg = tr("Result: %1").arg(stRes);
    ui->textEdit->append(msg);

//    QByteArray  = codec->();

    //codec->
/*
    QByteArray  = code->()
    n = .size();
    msg = " bytearray: ";
    for (i = 0; i < n; ++i) {
        c = .at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString());
    ui->textEdit->append(msg);



    QByteArray fConfigBA = codec->fromUnicode(ba);
    //codec->toUnicode();
    n = fConfigBA.size();

    msg = "fromUnicode bytearray: ";
    for (i = 0; i < n; ++i) {
        c = fConfigBA.at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString(fConfigBA));
    ui->textEdit->append(msg);

    //QTextCodec* codec = QTextCodec::codecForName("KOI8-R");
    QString st_ba(codec->toUnicode(fConfigBA));
    QByteArray fUnicoded = QByteArray().append(st_ba);//codec->toUnicode(fConfigBA);
    n = fUnicoded.size();
    msg = "toUnicode bytearray: ";
    for (i = 0; i < n; ++i) {
        c = fUnicoded.at(i);
        msg += tr("%1 ").arg(c);
    }
    msg += tr(", as string: %1").arg(QString(fUnicoded));
    ui->textEdit->append(msg);

    //QString st_ba(fUnicoded);
    ui->textEdit->append(st_ba);
*/
}

void EventsLog::on_pbnReadFromFile_clicked()
{
    // отладка проблемы с перекодировкой KOI-8R <-> UTF-8 - чтение из файла
    QFile fTest("./test.txt");
    if(!fTest.open(QIODevice::ReadOnly)) {
        //logMsg(msgError, tr("Ошибка открытия файла 'test.txt':<br>%1").arg(fTest.errorString()));
        ui->textEdit->append(tr("Ошибка открытия файла 'test.txt':<br>%1").arg(fTest.errorString()));
        return;
    }

    QByteArray ba = fTest.readAll();
    fTest.close();
    //qDebug() << "Test file size (bytes): " << ba.size();

    ui->textEdit->append("Исходный файл:");
    ui->textEdit->append(ba_char_vals(ba));

    // перекодировка
    QTextCodec* codec = QTextCodec::codecForName("KOI8-R");
    QString st_ba(codec->toUnicode(ba));
    QStringList fData = st_ba.split("\r\n");//(ba);
    int linesCount = fData.size();
    for (int i = 0; i < linesCount; ++i) {
        QString st = QString(fData.at(i)).trimmed();
        ui->textEdit->append(tr("string %1: %2").arg(i + 1).arg(st));
        ui->textEdit->append(str_char_vals(st));
    }

}

void EventsLog::createEventLog()
{
    QDateTime repBegin = ui->deBegin->dateTime();
    QDateTime repEnd = ui->deEnd->dateTime();
    if(repEnd.date().year() != repBegin.date().year()) repEnd = repBegin;

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Дата/время" << "Событие");

    // список событий
    QString qry = "SELECT"\
    "  sl.id, sl.dt, "\
    "  sl.event_type_id, et.name as event_name,"\
    "  st.description || ' - ' || coalesce(t.name, c.name) as sensor_name,"\
    "  coalesce(t.name, c.name) as place_name, "\
    "  sl.num1, "\
    "  sl.num2, "\
    "  sl.num3, "\
    "  sl.description "\
    "FROM system_log sl"\
    "       JOIN"\
    "     event_types et"\
    "       ON et.id = sl.event_type_id"\
    "       LEFT JOIN"\
    "     sensors s"\
    "       ON s.id = sl.num1"\
    "       LEFT JOIN"\
    "     sensor_types st"\
    "       ON st.id = s.sensor_type"\
    "       LEFT JOIN"\
    "     tanks t"\
    "       ON t.id = s.tank_id"\
    "       LEFT JOIN"\
    "     consumers c"\
    "       ON c.id = s.consumer_id "\
    "WHERE sl.dt >= :dt_begin AND sl.dt <= :dt_end "\
    "ORDER BY sl.dt;";

    query->finish();
    query->prepare(qry);
    query->bindValue(":dt_begin", repBegin);
    query->bindValue(":dt_end", repEnd);

    //qDebug() << "Query:\n" << query->lastQuery();

    //if(!query->exec(qry))
    if(!query->exec())
    {
        qDebug() << "Ошибка формирования отчета с " << repBegin << " по " << repEnd << ":";
        qDebug() << query->lastError().text();

        qDebug() << "Query:\n" << query->lastQuery();

        query->finish();
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->resizeColumnsToContents();
        return;
    }

    int n = query->size();
    ui->tableWidget->setRowCount(n);
    QString reportDateTimeFormat = "dd.MM.yyyy г. hh:mm:ss";
    int param1, param2;
    qreal param3;
    //QString sensor_name;
    QString event_desc;
    QTableWidgetItem *newItem;
    for(int row = 0; row < n; ++row){
        if(!query->next()) return;

        newItem = new QTableWidgetItem(QDateTime(query->value("dt").toDateTime()).toString(reportDateTimeFormat));//dt_string);
        ui->tableWidget->setItem(row, 0, newItem);
        int event_type = query->value("event_type_id").toInt();
        param1 = query->value("num1").toInt();
        param2 = query->value("num2").toInt();
        param3 = query->value("num3").toFloat();

        switch (event_type) {
        case 1: // превышение норм расхода
            if(param1 != 0) {
                // превышение
                event_desc = tr("Превышение нормы расхода топлива на %1 л/мин").arg(QString::number(param3, 'f', 2));
            } else {
                // возврат в норму
                event_desc = "Расход топлива вошел в норму";
            }
            break;
        case 2: // разница измерения уровня и расхода
            if(param1 != 0) {
                // превышение
                event_desc = tr("Разница измерения уровня и расхода топлива на %1 л").arg(QString::number(param3, 'f', 2));
            } else {
                // возврат в норму
                event_desc = "Разница измерения уровня и расхода топлива прекратилась";
            }
            break;
        case 3: // ручной ввод
            event_desc = "Ручной ввод";
            break;
        case 4: // нет связи
            if(param2 == 0) {
                // пропала связь
                event_desc = tr("Пропала связь, либо отказ датчика: %1").arg(query->value("sensor_name").toString());
            } else {
                // появилась связь
                event_desc = tr("Восстановилась связь с датчиком: %1").arg(query->value("sensor_name").toString());
            }
            break;
        case 5: // движение топлива
            if(param3 > 0) {
                // загрузка топлива
                event_desc = tr("Принято топлива: %1 т").arg(QString::number(param3, 'f', 2));
            } else {
                // отгрузка (выдача) топлива
                event_desc = tr("Отгрузка топлива: %1 т").arg(QString::number(-param3, 'f', 2));
            }
            break;
        default:
            event_desc = "???";
            break;
        }
        newItem = new QTableWidgetItem(event_desc);
        ui->tableWidget->setItem(row, 1, newItem);
    }
    //ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeColumnToContents(0);
}
