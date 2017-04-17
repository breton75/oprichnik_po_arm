#ifndef FUELMOVE_H
#define FUELMOVE_H

#include <QDialog>

namespace Ui {
class FuelMove;
}

class FuelMove : public QDialog
{
    Q_OBJECT

public:
    explicit FuelMove(QWidget *parent = 0);
    ~FuelMove();

private slots:
    void on_bnCancel_clicked();

    void on_bnOK_clicked();

private:
    Ui::FuelMove *ui;
};

#endif // FUELMOVE_H
