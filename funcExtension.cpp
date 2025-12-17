#include "funcExtension.h"

int getFirstPoint(vibroData *data, double min, double max, double frequency)
{
    double average = (max+ min) / 2;
    double arrMin = min;
    int index;


    for (int i = 0; i< data->steps.size() && data->steps[i].m_time*60 < frequency; i++)
    {
        if (arrMin > data->steps[i].m_verticalPressure_kPa)
        {
            index = i;
            arrMin = data->steps[i].m_verticalPressure_kPa;
        }
    }
    return index;
}
