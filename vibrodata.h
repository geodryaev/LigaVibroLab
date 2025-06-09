#ifndef VIBRODATA_H
#define VIBRODATA_H

#include <QVector>
#include <QDebug>

class stepVibro
{
private:
    double m_time;
    double m_verticalPresure_kPa;
    double m_shearPresure_kPa;
    double m_cellPressure_kPa;
    double m_porePresure_KPA;
    double m_porePressureAux_kPA;
    double m_verticalDeform_mm;
    double m_shearDeform_mm;
    double m_cellVolume_mm3;
    double m_poreVolume_mm3;
    int m_mode;
    int m_step;
public:
    stepVibro();
    stepVibro(double time, double verticalPresure_KPA,
              double shearPresure_KPA, double cellPressure_KPA,
              double porePresure_KPA,double porePressureAux_kPA,
              double verticalDeform_mm, double m_shearDeform_mm,
              double cellVolume_mm3, double poreVolume_mm3,
              int mode);
    ~stepVibro()
    {
        qDebug() << "Destruction stepVibro";
    }
    void edit(int choise, double value);
    double get(int choise);
};

class vibroData
{
private:
    QVector<stepVibro> steps, processedSteps;
    QVector<double> buffer;

    double frequency;
    const double delta = 0.001; // Разрешение погрешности при удалении подстройки (УСЛОВИЕ КОГДА ПОДСТРОЙКА СРАБОТАЛА)


    void cropAdjustment();
    bool cheackpointMax(int index, int countPoint)
    {
        if (index + countPoint > steps.size() - 1 || index - countPoint < 0)
        {
            return false;
        }

        while(countPoint)
        {
            if (steps[index].get(1) > steps[countPoint].get(1) && steps[index].get(1) > steps[index - 1].get(1))
            {
                countPoint--;
            }
            else
            {
                return false;
            }
        }


        return true;
    }
public:
    vibroData()
    {

    }
    ~vibroData(){
            qDebug() << "Destruction vibroData";
    }
    void push (double time, double verticalPresure_KPA,
              double shearPresure_KPA, double cellPressure_KPA,
              double porePresure_KPA,double porePressureAux_kPA,
              double verticalDeform_mm, double shearDeform_mm,
              double cellVolume_mm3, double poreVolume_mm3,
              int mode);

    void normalizeData();
};


#endif // VIBRODATA_H
