#ifndef FIXEDVIEWHANDLER
#define FIXEDVIEWHANDLER

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

class FixedViewHandler : public osgGA::GUIEventHandler {
public:
    FixedViewHandler();

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
    double _distance;
    float _offsetX, _offsetY;
    float _lastDragX, _lastDragY;
};

#endif // FIXEDVIEWHANDLER

