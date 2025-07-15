#include "calcamplitud.h"
#include "ui_calcamplitud.h"

calcAmplitud::calcAmplitud(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::calcAmplitud)
{
    ui->setupUi(this);
    ui->lamax->setText("Введите пиковое ускорение a<sub>max</sub>");
    ui->amax->setSuffix(" м/с²");
    ui->denzity->setSuffix(" кг/м³");
    ui->ltav->setText("τ<sub>av</sub>");
    ui->ltamp->setText("τ<sub>ampl</sub>");
}

calcAmplitud::~calcAmplitud()
{
    delete ui;
}

void calcAmplitud::on_pushButton_clicked()
{
    double g = 9.81f;
    double h = ui->height->value();
    double denzity = ui->denzity->value();
    double amax  = ui->amax->value();
    double sigma_z = denzity * h * g;
    double rd=1;
    if (h < 9.15f)
    {
        rd = 1.0 - 0.00765*h;
    }
    else
    {
        rd = 1.174 - 0.0267*h;
    }

    ui->tav->setValue(( amax * rd * sigma_z / g) / 1000);
    ui->tanpl->setValue(ui->tav->value()*4);
}

