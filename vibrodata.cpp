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
    bool isChanges = false;
    int meridian = processedSteps.size() / 2 ;
    frequency = processedSteps[meridian].get(0) - processedSteps[meridian].get(0);
    for (int i = 0 ; i < processedSteps.size() - 1; i++)
    {
        if (processedSteps[i].get(0)-processedSteps[i+1].get(0) < meridian - delta ||processedSteps[i].get(0)-processedSteps[i+1].get(0) > meridian +  delta)
            {
                processedSteps.remove(i);
                isChanges = true;
                break;
            }
    }
    if (isChanges)
        cropAdjustment();

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
    for (int i = 1; i < steps.size() - 1; i++) {
        if (steps[i + 1].get(1) < steps[i].get(1) &&
            steps[i - 1].get(1) < steps[i].get(1))
            if (cheackpointMax(i, 4))
                processedSteps.append(steps[i]);
    }
    cropAdjustment();
}
