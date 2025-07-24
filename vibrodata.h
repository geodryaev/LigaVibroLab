#ifndef VIBRODATA_H
#define VIBRODATA_H

#include <QVector>
#include <QDebug>

#include "stepvibro.h"


class vibroData
{
private:

    QVector<stepVibro> processedStepsUP;
    QVector<stepVibro> processedStepsDown;
    QVector<double> buffer;


    double maxPressure = 0;
               // Это значение с которого мы начинаем динамику (мин в юриной программе)
    const double delta = 0.001; // Разрешение погрешности при удалении подстройки (УСЛОВИЕ КОГДА ПОДСТРОЙКА СРАБОТАЛА)

    void setNumberPoints ();

public:
    QVector<stepVibro> steps;
    QVector<QVector<stepVibro>::Iterator> minPoints;
    QVector<QVector<stepVibro>::Iterator> maxPoints;

    double frequency;
    double height;
    double diametrs;
    double minForce;
    double ampl;


    vibroData(double height, double diametrs);
    ~vibroData(){
            // qDebug() << "Destruction vibroData";
    }
    void push (double time, double verticalPressure_KPA,
              double shearPressure_KPA, double cellPressure_KPA,
              double porePressure_KPA,double porePressureAux_kPA,
              double verticalDeform_mm, double shearDeform_mm,
              double cellVolume_mm3, double poreVolume_mm3,
              int mode, double u0);

    void normalizeData();

};


#endif // VIBRODATA_H
