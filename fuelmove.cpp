#include "fuelmove.h"
#include "ui_fuelmove.h"

FuelMove::FuelMove(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FuelMove)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Fuel.png"));//":/FuelMove.png"));
    setLayout(ui->gridLayout);
}

FuelMove::~FuelMove()
{
    delete ui;
}

void FuelMove::on_bnCancel_clicked()
{
    done(QDialog::Rejected);
}

void FuelMove::on_bnOK_clicked()
{
    done(QDialog::Accepted);
}
