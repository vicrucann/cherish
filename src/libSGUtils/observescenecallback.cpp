#include "observescenecallback.h"
#include <iostream>

void ObserveSceneCallback::operator ()(osg::Node *node, osg::NodeVisitor *nv)
{
    std::string content;
    if (node->getName() != "")
        content += node->getName() + "; ";
    if (_text.valid())
        _text->setText(content);
}

void ObserveSceneCallback::setScenePointer(RootScene *scene){
    _scene = scene;
}
