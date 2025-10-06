#ifndef CALCAMPLITUD_H
#define CALCAMPLITUD_H

#include <QDialog>

namespace Ui {
class calcAmplitud;
}

class calcAmplitud : public QDialog
{
    Q_OBJECT

public:
    explicit calcAmplitud(QWidget *parent = nullptr);
    ~calcAmplitud();

private slots:
    void on_pushButton_clicked();

private:
    Ui::calcAmplitud *ui;
};

#endif // CALCAMPLITUD_H
