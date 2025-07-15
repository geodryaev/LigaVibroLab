#include "vibrodata.h"

void vibroData::cropAdjustment()
{
    frequency = 0;
    int count = 0;
    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {
        if (processedStepsUP[i].m_verticalPressure_kPa < maxPressure * 0.95)
        {
            processedStepsUP.remove(i);
        }

    }

    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {

        frequency += processedStepsUP[i].m_time- processedStepsUP[i-1].m_time;
        count++;

    }


    frequency /=count;
    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {
        if (processedStepsUP[i].m_time - processedStepsUP[i-1].m_time < frequency * 0.6 || processedStepsUP[i].m_time - processedStepsUP[i-1].m_time > frequency * 1.4)
            processedStepsUP.remove(i);
    }
    frequency=0;
    count = 0;
    for (int i = processedStepsUP.size() - 1 ; i > 0 ; i--)
    {

        frequency += processedStepsUP[i].m_time- processedStepsUP[i-1].m_time;
        count++;

    }
    frequency /=count;
    frequency = 1/ (frequency * 60);
    return;
}

vibroData::vibroData(double height, double diametrs)
{
    this->height = height;
    this->diametrs = diametrs;

}

void vibroData::push(double time, double verticalPressure_KPA,
                     double shearPressure_KPA, double cellPressure_KPA,
                     double porePressure_KPA,double porePressureAux_kPA,
                     double verticalDeform_mm, double shearDeform_mm,
                     double cellVolume_mm3, double poreVolume_mm3,
                     int mode, double u0)
{
    steps.append(stepVibro(time,verticalPressure_KPA,shearPressure_KPA,cellPressure_KPA,porePressure_KPA,porePressureAux_kPA,verticalDeform_mm, shearDeform_mm,cellVolume_mm3, poreVolume_mm3, mode, u0, height));
    steps[steps.size()-1].calc();
}

void vibroData::normalizeData() {
    int countSteps = 0;
    for (int i = 1; i < steps.size() - 1; i++) {
        if (steps[i + 1].m_verticalPressure_kPa < steps[i].m_verticalPressure_kPa && steps[i - 1].m_verticalPressure_kPa < steps[i].m_verticalPressure_kPa)
            if (cheackPointMax(i, 4))
            {
                qDebug() << steps[i - 1].m_verticalPressure_kPa << "--" << steps[i].m_verticalPressure_kPa << steps[i + 1].m_verticalPressure_kPa;
                processedStepsUP.append(steps[i]);
                maxPressure += steps[i].m_verticalPressure_kPa;
                countSteps++;
            }

        if (steps[i + 1].m_verticalPressure_kPa > steps[i].m_verticalPressure_kPa && steps[i - 1].m_verticalPressure_kPa > steps[i].m_verticalPressure_kPa)
            if (cheackpointMin(i, 4))
            {
                qDebug() << steps[i - 1].m_verticalPressure_kPa << "--" << steps[i].m_verticalPressure_kPa << steps[i + 1].m_verticalPressure_kPa;
                processedStepsDown.append(steps[i]);
            }
    }


    maxPressure /= countSteps;
    cropAdjustment();
}

bool vibroData::cheackPointMax(int index, int countPoint) {
    if (index + countPoint > steps.size() - 1 || index - countPoint < 0) {
        return false;
    }

    while (countPoint) {
        if (steps[index].m_verticalPressure_kPa > steps[index + countPoint].m_verticalPressure_kPa &&
            steps[index].m_verticalPressure_kPa > steps[index - countPoint].m_verticalPressure_kPa) {
            countPoint--;
        } else {
            return false;
        }
    }
    steps[index].isUp=true;
    return true;
}

bool vibroData::cheackpointMin(int index, int countPoint)
{
    if (index + countPoint > steps.size() - 1 || index - countPoint < 0)
    {
        return false;
    }

    while (countPoint) {
        if (steps[index].m_verticalPressure_kPa < steps[index + countPoint].m_verticalPressure_kPa && steps[index].m_verticalPressure_kPa < steps[index - countPoint].m_verticalPressure_kPa)
        {
            countPoint--;
        } else
        {
            return false;
        }
    }

    steps[index].isDown = true;
    return true;
}



