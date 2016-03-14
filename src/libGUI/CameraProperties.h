#ifndef CAMERAPROPERTIES_H
#define CAMERAPROPERTIES_H

#include <QDialog>
#include <QScopedPointer>

//#include "ui_CameraProperties.h"

namespace Ui {
class CameraProperties;
}

class CameraProperties : public QDialog
{
    Q_OBJECT
public:
    CameraProperties(QWidget *parent = 0);
    ~CameraProperties();

signals:

public slots:

private:
    QScopedPointer<Ui::CameraProperties> m_ui;
};

#endif // CAMERAPROPERTIES_H
