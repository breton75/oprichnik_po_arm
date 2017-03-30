#ifndef REPORTSETUP_H
#define REPORTSETUP_H

#include <QDialog>
//class FloatingCalender;

namespace Ui {
class ReportSetup;
}

class ReportSetup : public QDialog
{
    Q_OBJECT

public:
    explicit ReportSetup(QWidget *parent = 0);
    ~ReportSetup();
    int repType();
    QDateTime repBegin();
    QDateTime repEnd();

    int test;

private:
    Ui::ReportSetup *ui;
    int fRepType;
//    FloatingCalender * fCal;

signals:
    void chooseRepType(int);

private slots:
    void setRepType(int newRepType);
    void on_rbnHourly_clicked(bool checked);
    void on_rbnDaily_clicked(bool checked);
    void on_rbnMonthly_clicked(bool checked);
    void on_rbnQuaterly_clicked(bool checked);
    void on_rbnYearly_clicked(bool checked);
    void on_bnMakeReport_clicked();
    void on_bnCancel_clicked();
    void on_commandLinkButton_clicked();
    void on_ReportSetup_finished(int result);
};

#endif // REPORTSETUP_H
