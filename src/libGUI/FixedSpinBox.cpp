#include "FixedSpinBox.h"

FixedSpinBox::FixedSpinBox(QWidget *parent)
    : QSpinBox(parent)
    , m_vector(QVector<int>{0,5,7,14,18,24,35,45,50,70,85,105,300})
    , m_index(0)
{

}

void FixedSpinBox::stepBy(int steps)
{
    m_index += steps;
    m_index = qBound(0, m_index, m_vector.size()-1);
    this->setValue(m_vector.at(m_index));
}
