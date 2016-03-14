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
    explicit CameraProperties(double fov=60.f, QWidget *parent = 0);
    ~CameraProperties();

signals:
    void fovChanged(double);

public slots:

private slots:
    void onSliderMoved(int position);

private:
    QScopedPointer<Ui::CameraProperties> m_ui;

};

#endif // CAMERAPROPERTIES_H
