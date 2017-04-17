#ifndef CHECKFORM_H
#define CHECKFORM_H

#include <QMainWindow>

namespace Ui {
class CheckForm;
}

class CheckForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit CheckForm(QWidget *parent = 0);
    ~CheckForm();

private:
    Ui::CheckForm *ui;
};

#endif // CHECKFORM_H
