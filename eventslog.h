#ifndef EVENTSLOG_H
#define EVENTSLOG_H

#include <QMainWindow>

namespace Ui {
class EventsLog;
}

class EventsLog : public QMainWindow
{
    Q_OBJECT

public:
    explicit EventsLog(QWidget *parent = 0);
    ~EventsLog();

private slots:
    void on_pbnGetLog_clicked();

    void on_pbnReadFromFile_clicked();

private:
    Ui::EventsLog *ui;
};

#endif // EVENTSLOG_H
