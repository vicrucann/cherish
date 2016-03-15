#ifndef FIXEDSPINBOX_H
#define FIXEDSPINBOX_H

#include <QSpinBox>
#include <QVector>

class FixedSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit FixedSpinBox(QWidget* parent=0);

protected:
    virtual void stepBy(int steps);

private:
    QVector<int> m_vector;
    int m_index;
};

#endif // FIXEDSPINBOX_H
