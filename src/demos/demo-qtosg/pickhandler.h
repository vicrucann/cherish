#ifndef PICKHANDLER_H
#define PICKHANDLER_H

#include <osgGA/GUIEventHandler>

class PickHandler : public osgGA::GUIEventHandler
{
public:
    virtual ~PickHandler(){}
    virtual bool handle(const osgGA::GUIEventAdapter& ea,
                        osgGA::GUIActionAdapter& aa);
};

#endif // PICKHANDLER_H
