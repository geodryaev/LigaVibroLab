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

    double frequency;
    double maxPressure = 0;
    double height;
    double diametrs;
    const double delta = 0.001; // Разрешение погрешности при удалении подстройки (УСЛОВИЕ КОГДА ПОДСТРОЙКА СРАБОТАЛА)


    void cropAdjustment();
    bool cheackPointMax(int index, int countPoint);
    bool cheackpointMin(int index, int countPoint);

    /*
     * Создание истины ??
     */

public:

    QVector<stepVibro> steps;
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
