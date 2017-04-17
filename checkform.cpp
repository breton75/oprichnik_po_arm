#include "checkform.h"
#include "ui_checkform.h"

CheckForm::CheckForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CheckForm)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Measure.png"));
    setWindowModality(Qt::WindowModal);
    ui->centralwidget->setLayout(ui->gridLayout);
}

CheckForm::~CheckForm()
{
    delete ui;
}
