#ifndef CAMERAPROPERTIES_H
#define CAMERAPROPERTIES_H

#include <QDialog>
#include <QScopedPointer>

//#include "ui_CameraProperties.h"

namespace Ui {
class CameraProperties;
}

/*! \class CameraProperties
 * \brief Widget that contains camera aperture properties. */
class CameraProperties : public QDialog
{
    Q_OBJECT
public:
    /*! Constructor */
    explicit CameraProperties(double fov=60.f, QWidget *parent = 0);
    ~CameraProperties();

    float getFOV() const;

signals:
    /*! Signal emitter when user changes FOV by using a slider. */
    void fovChangedBySlider(double);

    /*! Signal emitted when user cheks "ortho" property. */
    void orthoChecked(bool);

public slots:
    /*! Slot to be called when "ortho" property is checked. */
    void onOrthoChecked(bool val);

    /*! Slot to be called when FOV value was changed, e.g. by when setting up a bookmark.  */
    void onFOVSet(double fov);

private slots:
    void onSliderMoved(int position);

private:
    QScopedPointer<Ui::CameraProperties> m_ui;

};

#endif // CAMERAPROPERTIES_H
