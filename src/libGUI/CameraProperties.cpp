#include "CameraProperties.h"

#include "ui_CameraProperties.h"

#include <math.h>
#include <QDebug>
#include <QtGlobal>

#include "Settings.h"

CameraProperties::CameraProperties(double fov, QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::CameraProperties)
{
    m_ui->setupUi(this);

    QObject::connect(m_ui->m_sliderFOV, SIGNAL(sliderMoved(int)),
                     this, SLOT(onSliderMoved(int)),
                     Qt::UniqueConnection);
    QObject::connect(m_ui->m_sliderFOV, SIGNAL(valueChanged(int)),
                     this, SLOT(onSliderMoved(int)),
                     Qt::UniqueConnection);

    QObject::connect(m_ui->m_checkOrtho, SIGNAL(clicked(bool)),
                     this, SLOT(onOrthoChecked(bool)),
                     Qt::UniqueConnection);

    m_ui->m_labelValue->setText(QString::number(fov));
}

CameraProperties::~CameraProperties()
{
}

float CameraProperties::getFOV() const
{
    return float(m_ui->m_sliderFOV->value());
}

void CameraProperties::onOrthoChecked(bool val)
{
    qDebug("CameraProperties: onOrthoChecked");
    emit this->orthoChecked(val);
}

void CameraProperties::onFOVSet(double fov)
{
    m_ui->m_sliderFOV->setValue(round(fov));
}

void CameraProperties::onSliderMoved(int position)
{
    m_ui->m_labelValue->setText(QString::number(position));
    emit fovChangedBySlider(double(position));
}
