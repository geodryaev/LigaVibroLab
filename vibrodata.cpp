#include "vibrodata.h"

void vibroData::setNumberPoints()
{
    int numTact  = 0;
    for (QVector<stepVibro>::Iterator it = steps.begin(); it != steps.end(); it++)
    {
        if (it->numberTact != -1)
        {
            numTact = it->numberTact;
        }
        else
        {
            it->numberTact = numTact;
        }
    }
}

vibroData::vibroData(double height, double diametrs, double minF, double maxF)
{
    this->height = height;
    this->diametrs = diametrs;
    minForce = minF;
    maxForce = maxF;
    ampl = (maxF - minF) / 2;
}

void vibroData::push(double time, QString step, double verticalPressure_KPA,
                     double shearPressure_KPA, double cellPressure_KPA,
                     double porePressure_KPA,double porePressureAux_kPA,
                     double verticalDeform_mm, double shearDeform_mm,
                     double cellVolume_mm3, double poreVolume_mm3,
                     int mode, double u0)
{
    steps.append(stepVibro(time,step, verticalPressure_KPA,shearPressure_KPA,cellPressure_KPA,porePressure_KPA,porePressureAux_kPA,verticalDeform_mm, shearDeform_mm,cellVolume_mm3, poreVolume_mm3, mode, u0, height, diametrs));
    steps[steps.size()-1].calc();
}

void vibroData::normalizeData(bool isHight)
{
    if(isHight)
    {
        int sizeStep = setCountStepsHigh();
        int indexFirstPoint = 0;
        int indexSecondPoint = 0;

        switch (sizeStep) {
        case 8:
            indexFirstPoint = 2;
            indexSecondPoint = 6;
            break;
        case 4:
            indexFirstPoint = 1;
            indexSecondPoint = 3;
            break;
        case 2:
            indexFirstPoint = 0;
            indexSecondPoint = 1;
            break;
        default:
            break;
        }



        for(auto el = steps.begin(); el < steps.end(); el ++)
        {
            if (el->m_mode == indexFirstPoint)
            {
                el->isUp = true;
                maxPoints.append(el);
            }

            if (el->m_mode == indexSecondPoint)
            {
                el->isDown = true;
                minPoints.append(el);
            }
        }
        qDebug() << "Finish Points Hights";
    }
    else
    {
        double commonLine = (maxForce + minForce) / 2;
        bool down = true;
        QVector<stepVibro>::Iterator currentPoint = steps.begin();
        QVector<stepVibro>::Iterator currentMin = steps.begin();
        QVector<stepVibro>::Iterator currentMax = steps.begin();

        if(currentPoint->m_verticalPressure_kPa < commonLine)
        {
            down = true;
        }
        else
        {
            down = false;
        }

        while(currentPoint != steps.end())
        {
            if (down)
            {
                currentMin = currentPoint;
                while (currentPoint != steps.end() && currentPoint->m_verticalPressure_kPa <= commonLine)
                {
                    if (currentMin->m_verticalPressure_kPa >  currentPoint->m_verticalPressure_kPa)
                        currentMin = currentPoint;
                    qDebug() << currentPoint++->m_time;
                }
                currentMin->isDown = true;
                minPoints.append(currentMin);
                down = false;
            }
            else
            {
                currentMax = currentPoint;
                while (currentPoint != steps.end() && currentPoint->m_verticalPressure_kPa >= commonLine)
                {
                    if (currentMax->m_verticalPressure_kPa <  currentPoint->m_verticalPressure_kPa)
                        currentMax = currentPoint;
                    qDebug() << currentPoint++->m_time;
                }
                currentMax->isUp = true;
                maxPoints.append(currentMax);
                down = true;
            }
        }
    }
}

int vibroData::setCountStepsHigh()
{
    int count = 0;
    int index = 0;

    while (steps[index].m_mode != 0)
    {
        index++;
    }

    for (; index + 1 < steps.size() && steps[index + 1].m_mode > steps[index].m_mode; index++)
    {
        count++;
    }

    return count + 1;
}

void vibroData::cropHigh(QVector<stepVibro>::iterator beginCrop, QVector<stepVibro>::iterator endCrop)
{
    steps.erase(beginCrop,endCrop);
}
