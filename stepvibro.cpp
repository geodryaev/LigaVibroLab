#include "stepvibro.h"

void stepVibro::calc()
{
    if (m_cellPressure_kPa != 0.0f)
    {
        PPR = (m_porePressure_KPA - m_u0)/ (m_cellPressure_kPa -m_u0);
    }
    else
    {
        PPR = 0.0f;
    }
    sigma1_ = m_cellPressure_kPa + m_verticalPressure_kPa - m_porePressure_KPA;
    sigma3_ = m_cellPressure_kPa - m_porePressure_KPA;
    sigma1_ = m_cellPressure_kPa + m_verticalPressure_kPa ;
    sigma3_ = m_cellPressure_kPa ;
    p_ = (sigma1_ + 2 * sigma3_)/3;
    q = (sigma1_ - sigma3_)/2;
    epsilon_ = m_verticalDeform_mm / m_h0;

}


stepVibro::stepVibro(double time, double verticalPressure_KPA, double shearPressure_KPA, double cellPressure_KPA, double porePressure_KPA, double porePressureAux_kPA, double verticalDeform_mm, double shearDeform_mm, double cellVolume_mm3, double poreVolume_mm3, int mode, double u0, double h0, double d0)
{
    m_h0 = h0;
    m_d0 = d0;
    m_u0 = u0;

    m_time = time;
    m_verticalPressure_kPa = verticalPressure_KPA;
    m_shearPressure_kPa = shearPressure_KPA;
    m_cellPressure_kPa = cellPressure_KPA;
    m_porePressure_KPA = porePressure_KPA;
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
        m_verticalPressure_kPa = value;
        break;
    case 2:
        m_shearPressure_kPa = value;
        break;
    case 3:
        m_cellPressure_kPa = value;
        break;
    case 4:
        m_porePressure_KPA =value;
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
