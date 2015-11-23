#include "observescenecallback.h"
#include <iostream>

#include "rootscene.h"
#include "canvas.h"

void ObserveSceneCallback::operator ()(osg::Node *node, osg::NodeVisitor *nv)
{
    std::string content;
    if (_scene.valid()){
        std::cout << "Scene number of children: " << _scene->getNumChildren() << std::endl;
        for (unsigned int i=0; i<_scene->getNumChildren(); ++i){
            if (_scene->getChild(i)) {
                std::cout << "ObserveSceneCallback(): child with name: " <<
                             _scene->getChild(i)->getName() << std::endl;
            }
        }
    }
    //traverse(node,nv);
}


void ObserveSceneCallback::setScenePointer(osg::Group *scene){
    std::cout << "  Observer is set" << std::endl;
    _scene = scene;
}
