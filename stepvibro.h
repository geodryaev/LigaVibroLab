#ifndef STEPVIBRO_H
#define STEPVIBRO_H
#include <QDebug>


class stepVibro
{
public:
    double m_u0;
    double m_time;
    double m_verticalPressure_kPa;
    double m_shearPressure_kPa;
    double m_cellPressure_kPa;
    double m_porePressure_KPA;
    double m_porePressureAux_kPA;
    double m_verticalDeform_mm;
    double m_shearDeform_mm;
    double m_cellVolume_mm3;
    double m_poreVolume_mm3;
    int m_mode;
    int m_step;

    double sigma1_;
    double sigma3_;
    double sigma1;
    double sigma3;
    double q;
    double p_;
    double PPR;
    double epsilon_;
    double m_h0;
    double m_d0;

    bool isUp = false;
    bool isDown = false ;
    int numberTact = -1;

    void calc();


    stepVibro(){}
    stepVibro(double time, double verticalPressure_KPA,
              double shearPressure_KPA, double cellPressure_KPA,
              double porePressure_KPA,double porePressureAux_kPA,
              double verticalDeform_mm, double m_shearDeform_mm,
              double cellVolume_mm3, double poreVolume_mm3,
              int mode, double u0, double h0, double d0);
    ~stepVibro()
    {
        // qDebug() << "Destruction stepVibro";
    }

    void edit(int choise, double value);
};

#endif // STEPVIBRO_H
