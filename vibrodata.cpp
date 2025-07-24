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
    steps.append(stepVibro(time,verticalPressure_KPA,shearPressure_KPA,cellPressure_KPA,porePressure_KPA,porePressureAux_kPA,verticalDeform_mm, shearDeform_mm,cellVolume_mm3, poreVolume_mm3, mode, u0, height, diametrs));
    steps[steps.size()-1].calc();
}

void vibroData::normalizeData()
{
    double windows = 1 / frequency / 60 *1.05;
    int countCicle = 1;
    QVector<stepVibro>::Iterator it = steps.begin();
    QVector<stepVibro>::Iterator min = steps.begin();
    QVector<stepVibro>::Iterator max = steps.begin();
    QVector<stepVibro>::Iterator point;

    steps.begin()->isDown = true;
    steps.begin()->isUp = true;
    steps.begin()->numberTact = 0;
    minPoints.append(steps.begin());
    maxPoints.append(steps.begin());
    it = min+1;

    point  = it;
    while (it != steps.end() and point != steps.end())
    {
        min = it+1;
        point++;
        QVector<stepVibro>::Iterator save = it;
        while (point != steps.end() and  point->m_time < save->m_time + windows)
        {
            if (point->m_verticalPressure_kPa < min->m_verticalPressure_kPa)
            {
                min = point;
            }
            point++;
        }
        qDebug() << "it ->"<< it->m_time << "\tpoint ->" << point->m_time << "\tmin ->"<<min->m_time<< "\tnextValue ->"<< save->m_time + windows <<"\n" ;
        if (it->m_time == 1.73015)
        {
            qDebug() <<"here";
        }
        if (min == it+1)
        {
            while (it->m_time < save->m_time + windows / 4)
            {
                if (it == steps.end())
                    break;
                it++;
            }
        }
        else
        {
            min->isDown = true;
            minPoints.append(min);
            min->numberTact = countCicle;
            countCicle++;
            while (it != steps.end() and it->m_time < save->m_time + windows)
            {
                if (it == steps.end())
                    break;
                it++;
            }
        }
    }



    it = max+1;
    point  = it;
    while (it != steps.end() and point != steps.end())
    {
        max = it+1;
        point++;
        QVector<stepVibro>::Iterator save = it;
        while (point != steps.end() && point->m_time < save->m_time + windows)
        {
            if (point->m_verticalPressure_kPa > max->m_verticalPressure_kPa)
            {
                max = point;
            }
            point++;
        }

        if (max == it+1)
        {
            while (it->m_time < save->m_time + windows / 4)
            {
                if (it == steps.end())
                    break;
                it++;
            }
        }
        else
        {
            max->isUp = true;
            maxPoints.append(max);
            while (it->m_time < save->m_time + windows)
            {
                if (it == steps.end())
                    break;
                it++;
            }
        }
    }


    maxPoints.removeLast();
    minPoints.removeLast();

    setNumberPoints();
    return;
}
