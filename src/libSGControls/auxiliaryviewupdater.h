#ifndef AUXILIARYVIEWUPDATER
#define AUXILIARYVIEWUPDATER

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

class AuxiliaryViewUpdater : public osgGA::GUIEventHandler {
public:
    AuxiliaryViewUpdater();

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
    double _distance;
    float _offsetX, _offsetY;
    float _lastDragX, _lastDragY;
};

#endif // AUXILIARYVIEWUPDATER

