#include "CameraProperties.h"

#include "ui_CameraProperties.h"

CameraProperties::CameraProperties(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::CameraProperties)
{
    m_ui->setupUi(this);
}

CameraProperties::~CameraProperties()
{
}
