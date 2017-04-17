#include "reportsetup.h"
#include "ui_reportsetup.h"
#include <QCalendarWidget>
#include <QMouseEvent>
#include <QTimer>

//===================================================================
//class FloatingCalender : public QCalendarWidget {
//private:
//    QPoint  m_ptPosition;

//protected:
//    //virtual void mousePressEvent(QMouseEvent* pe)
//    virtual void clicked(QCalendarWidget* pe)
//    {
//        close();
//        deleteLater();
//        m_ptPosition = pe->pos();
//    }

//    virtual void mouseMoveEvent(QMouseEvent* pe)
//    {
//        move(pe->globalPos() - m_ptPosition);
//    }

//public:
//    FloatingCalender(QWidget* pwgt = 0)
//        : QCalendarWidget(pwgt)//,  Qt::FramelessWindowHint | Qt::Window)
//    {
//        setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
//    }
//};
//===================================================================

ReportSetup::ReportSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReportSetup)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Report2.png"));
    setMinimumSize(size());
    setMaximumSize(size());
    //ui->lblGropItem->setVisible(false);

    fRepType = 0;
    connect(this, SIGNAL(chooseRepType(int)), SLOT(setRepType(int)));

    //ui->deBegin->setDate(QDate::currentDate().addDays(-1));
    ui->deBegin->setDate(QDate::fromString("2017-03-01", Qt::ISODate));
    ui->deBegin->setTime(QTime::fromString("00:00"));
    //ui->deEnd->setDate(QDate::currentDate ());
    ui->deEnd->setDate(QDate::fromString("2017-03-05", Qt::ISODate));
    ui->deEnd->setTime(QTime::fromString("23:59"));
//    test = -1;
}

ReportSetup::~ReportSetup()
{
    delete ui;
}

int ReportSetup::repType()
{
    return fRepType;
}

QDateTime ReportSetup::repBegin()
{
    return ui->deBegin->dateTime();
}

QDateTime ReportSetup::repEnd()
{
    return ui->deEnd->dateTime();
}

void ReportSetup::setRepType(int newRepType)
{
    fRepType = newRepType;
}

void ReportSetup::on_rbnHourly_clicked(bool checked)
{
    if (checked) emit chooseRepType(0);
}

void ReportSetup::on_rbnDaily_clicked(bool checked)
{
    if (checked) emit chooseRepType(1);
}

void ReportSetup::on_rbnMonthly_clicked(bool checked)
{
    if (checked) emit chooseRepType(2);
}

void ReportSetup::on_rbnQuaterly_clicked(bool checked)
{
    if (checked) emit chooseRepType(3);
}

void ReportSetup::on_rbnYearly_clicked(bool checked)
{
    if (checked) emit chooseRepType(4);
}

void ReportSetup::on_bnMakeReport_clicked()
{
    done(QDialog::Accepted);
}

void ReportSetup::on_bnCancel_clicked()
{
    done(QDialog::Rejected);
}

void ReportSetup::on_commandLinkButton_clicked()
{
//    fCal->move(100, 100);
//    fCal->show();
}

//void ReportSetup::on_ReportSetup_finished(int result)
//{
//    test = result;
////    fCal->hide();
//}
