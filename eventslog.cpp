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
}

EventsLog::~EventsLog()
{
    delete ui;
}

void EventsLog::on_pbnGetLog_clicked()
{
    QDateTime dtBegin = ui->dteBegin->dateTime();
    QDateTime dtEnd = ui->dteEnd->dateTime();
    if(dtEnd.date().year() != dtBegin.date().year()) dtEnd = dtBegin;
    int dc = dtEnd.date().dayOfYear() - dtBegin.date().dayOfYear() + 1;
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Дата/время" << "Событие");
    ui->tableWidget->setRowCount(dc);
    QDateTime dtCurrent = dtBegin;
    for (int i = 0; i < dc; ++i) {
        //ui->tableWidget->cellWidget(i, 0) = new QTableWidgetItem(dtCurrent.toString("dd.MM.yy HH:mm:ss"));
        //ui->tableWidget->cellWidget(i, 1) = new QTableWidgetItem("");
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(dtCurrent.toString("dd.MM.yy HH:mm:ss")));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(""));
        dtCurrent = dtCurrent.addDays(1);
    }
    ui->tableWidget->resizeColumnToContents(0);
//    int rc = ui->dteEnd->date().dayOfYear()
    return;

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
    int n = ba.size();
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
