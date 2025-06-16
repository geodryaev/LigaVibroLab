#include "vibrodata.h"

stepVibro::stepVibro()
{

}

stepVibro::stepVibro(double time, double verticalPresure_KPA, double shearPresure_KPA, double cellPressure_KPA, double porePresure_KPA, double porePressureAux_kPA, double verticalDeform_mm, double shearDeform_mm, double cellVolume_mm3, double poreVolume_mm3, int mode)
{
    m_time = time;
    m_verticalPresure_kPa = verticalPresure_KPA;
    m_shearPresure_kPa = shearPresure_KPA;
    m_cellPressure_kPa = cellPressure_KPA;
    m_porePresure_KPA = porePresure_KPA;
    m_porePressureAux_kPA = porePressureAux_kPA;
    m_verticalDeform_mm = verticalDeform_mm;
    m_shearDeform_mm = shearDeform_mm;
    m_cellVolume_mm3 = cellVolume_mm3;
    m_poreVolume_mm3 = poreVolume_mm3;
    m_mode = mode;
}

void stepVibro::edit(int choise, double value)
{
    switch (choise) {
    case 0:
        m_time = value;
        break;
    case 1:
        m_verticalPresure_kPa = value;
        break;
    case 2:
        m_shearPresure_kPa = value;
        break;
    case 3:
        m_cellPressure_kPa = value;
        break;
    case 4:
        m_porePresure_KPA =value;
        break;
    case 5:
        m_porePressureAux_kPA = value;
        break;
    case 6:
        m_verticalDeform_mm = value;
        break;
    case 7:
        m_shearDeform_mm = value;
        break;
    case 8:
        m_cellVolume_mm3 = value;
        break;
    case 9:
        m_poreVolume_mm3 = value;
        break;
    default:
        break;
    }
}

double stepVibro::get(int choise)
{
    switch (choise) {
    case 0:
        return m_time;
        break;
    case 1:
        return m_verticalPresure_kPa;
        break;
    case 2:
        return m_shearPresure_kPa;
        break;
    case 3:
        return m_cellPressure_kPa;
        break;
    case 4:
        return m_porePresure_KPA;
        break;
    case 5:
        return m_porePressureAux_kPA;
        break;
    case 6:
        return m_verticalDeform_mm;
        break;
    case 7:
        return m_shearDeform_mm;
        break;
    case 8:
        return m_cellVolume_mm3;
        break;
    case 9:
        return m_poreVolume_mm3 ;
        break;
    default:
        return 0;
        break;
    }

    return -1;
}

void vibroData::cropAdjustment()
{
    frequency = 0;
    int count = 0;
    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {
        if (processedStepsUP[i].get(1) < maxPresure * 0.95)
        {
            processedStepsUP.remove(i);
        }

    }

    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {

        frequency += processedStepsUP[i].get(0)- processedStepsUP[i-1].get(0);
        count++;

    }


    frequency /=count;
    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {
        if (processedStepsUP[i].get(0) - processedStepsUP[i-1].get(0) < frequency * 0.6 || processedStepsUP[i].get(0) - processedStepsUP[i-1].get(0) > frequency * 1.4)
            processedStepsUP.remove(i);
    }
    frequency=0;
    count = 0;
    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {

        frequency += processedStepsUP[i].get(0)- processedStepsUP[i-1].get(0);
        count++;

    }
    frequency /=count;
    frequency = 1/ (frequency * 60);
    return;
}

void vibroData::push(double time, double verticalPresure_KPA,
                     double shearPresure_KPA, double cellPressure_KPA,
                     double porePresure_KPA,double porePressureAux_kPA,
                     double verticalDeform_mm, double shearDeform_mm,
                     double cellVolume_mm3, double poreVolume_mm3,
                     int mode)
{
    steps.append(stepVibro(time,verticalPresure_KPA,shearPresure_KPA,cellPressure_KPA,porePresure_KPA,porePressureAux_kPA,verticalDeform_mm, shearDeform_mm,cellVolume_mm3, poreVolume_mm3, mode));
}

void vibroData::normalizeData() {
    int countSteps = 0;
    for (int i = 1; i < steps.size() - 1; i++) {
        if (steps[i + 1].get(1) < steps[i].get(1) && steps[i - 1].get(1) < steps[i].get(1))
            if (cheackpointMax(i, 4))
            {
                qDebug() << steps[i - 1].get(1) << "--" << steps[i].get(1) << steps[i + 1].get(1);
                processedStepsUP.append(steps[i]);
                maxPresure += steps[i].get(1);
                countSteps++;
            }

        if (steps[i + 1].get(1) > steps[i].get(1) && steps[i - 1].get(1) > steps[i].get(1))
            if (cheackpointMin(i, 4))
            {
                qDebug() << steps[i - 1].get(1) << "--" << steps[i].get(1) << steps[i + 1].get(1);
                processedStepsDown.append(steps[i]);
            }
    }


    maxPresure /= countSteps;
    cropAdjustment();
}

bool vibroData::cheackpointMax(int index, int countPoint) {
    if (index + countPoint > steps.size() - 1 || index - countPoint < 0) {
        return false;
    }

    while (countPoint) {
        if (steps[index].get(1) > steps[index + countPoint].get(1) &&
            steps[index].get(1) > steps[index - countPoint].get(1)) {
            countPoint--;
        } else {
            return false;
        }
    }

    return true;
}

bool vibroData::cheackpointMin(int index, int countPoint)
{
    if (index + countPoint > steps.size() - 1 || index - countPoint < 0)
    {
        return false;
    }

    while (countPoint) {
        if (steps[index].get(1) < steps[index + countPoint].get(1) && steps[index].get(1) < steps[index - countPoint].get(1))
        {
            countPoint--;
        } else
        {
            return false;
        }
    }

    return true;
}
