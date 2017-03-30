#include "reportwindow.h"
#include "ui_reportwindow.h"

//===================================================================
QLabel* getNewStatusLabel(QWidget *parent)
{
     QLabel *lbl = new QLabel(parent);
     lbl->setMinimumSize(150, 20);
     lbl->setMaximumSize(150, 20);
     return lbl;
}
//===================================================================

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
}

ReportWindow::~ReportWindow()
{
    delete ui;
}

void ReportWindow::prepareReport(int repType, QDateTime repBegin, QDateTime repEnd)
{
    QString st = "Вид отчета: ";
    int n = 5;
    //QString* dt_str;
    switch (repType) {
    case 0:
        st += "часовой";
        n = repBegin.daysTo(repEnd);
//        n = repEnd.time().hour() - repBegin.time().hour() + 1;
//        dt_str = &repBegin.time().toString("");
        break;
    case 1:
        st += "суточный";
        n = repBegin.daysTo(repEnd) + 1;
        break;
    case 2:
        st += "месячный";
        n = repEnd.date().month() - repBegin.date().month() + 1;
        break;
    case 3:
        st += "квартальный";
        n = repEnd.date().month() - repBegin.date().month() + 1;
        break;
    case 4:
        st = +"годовой";
        break;
        default:
            st = +"???";
            break;
    }
    st += tr(", интервал: с %1 по %2").arg(repBegin.toString("dd.MM.yy")).arg(repEnd.toString("dd.MM.yy"));
    ui->lblRepParams->setText(st);

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(n);
    ui->tableWidget->setColumnCount(6);
//    st = "Дата";    ui->tableWidget->horizontalHeaderItem(0)->setText(st);
//    st = "Расход";  ui->tableWidget->horizontalHeaderItem(1)->setText(st);
//    st = "В т.ч.ГТД1";    ui->tableWidget->horizontalHeaderItem(2)->setText(st);
//    st = "ГТД2";    ui->tableWidget->horizontalHeaderItem(3)->setText(st);
//    st = "Вспом.";    ui->tableWidget->horizontalHeaderItem(4)->setText(st);
    QStringList hLst;
    //hLst << "Дата" << "Расход, т" << "В т.ч.ГТД1" << "ГТД2" << "Вспом.";
    hLst << "Дата" << "Расход ДГ, т" << "Расход ГД, т" << "Расход - Всего, т" << "Принято, т" << "Выдано, т";// << "Вспом.";
    ui->tableWidget->setHorizontalHeaderLabels(hLst);
    //hLst.clear();

    QTableWidgetItem *newItem;
//    newItem = new QTableWidgetItem(tr("Дата"));
//    ui->tableWidget->setItem(0, 0, newItem);
//    newItem = new QTableWidgetItem(tr("Расход"));
//    ui->tableWidget->setItem(0, 1, newItem);
//    newItem = new QTableWidgetItem(tr("В т.ч.ГТД1"));
//    ui->tableWidget->setItem(0, 2, newItem);
//    newItem = new QTableWidgetItem(tr("ГТД2"));
//    ui->tableWidget->setItem(0, 3, newItem);
//    newItem = new QTableWidgetItem(tr("Вспом."));
//    ui->tableWidget->setItem(0, 4, newItem);

    for(int row=1; row <= n; ++row){
        for(int column=0; column!=ui->tableWidget->columnCount(); ++column) {
            newItem = new QTableWidgetItem(tr("%1").arg((row)*(column+1)));
            ui->tableWidget->setItem(row - 1, column, newItem);
        }
    }
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
}

void ReportWindow::on_bnSave_clicked()
{
    setStatus(2, "Сохранить - будет позже");
}

void ReportWindow::on_bnClose_clicked()
{
    hide();
}
