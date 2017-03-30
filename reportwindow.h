#ifndef REPORTWINDOW_H
#define REPORTWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QLabel>

namespace Ui {
class ReportWindow;
}

class ReportWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportWindow(QWidget *parent = 0);
    ~ReportWindow();
    void prepareReport(int repType, QDateTime repBegin, QDateTime repEnd);

private:
    Ui::ReportWindow *ui;
    QLabel *status1;
    QLabel *status2;
    QLabel *status3;
    QLabel *status4;

    void setStatus(int idx, QString newStatus);

protected:
    void closeEvent();

private slots:
    void on_bnPrint_clicked();
    void on_bnSave_clicked();
    void on_bnClose_clicked();
};

#endif // REPORTWINDOW_H
