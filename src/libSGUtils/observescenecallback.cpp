#include <iostream>

#include "observescenecallback.h"
#include "rootscene.h"
#include "canvas.h"

void ObserveSceneCallback::operator ()(osg::Node *node, osg::NodeVisitor *nv)
{
    std::string content;
    if (_scene.valid()){
        std::cout << "Scene number of children: " << _scene->getNumChildren() << std::endl;
        for (unsigned int i=0; i<_scene->getNumChildren(); ++i){
            if (_scene->getChild(i)) {
                //std::cout << "ObserveSceneCallback(): child with name: " << _scene->getChild(i)->getName() << std::endl;
                content += _scene->getChild(i)->getName() + "; ";
            }
        }
    }
    osgText::Text* text = new osgText::Text;
    if (_text.valid())
        _text->setText(content);
    //traverse(node,nv);
}


void ObserveSceneCallback::setScenePointer(osg::Group *scene){
    _scene = scene;
}

void ObserveSceneCallback::setTextPointer(osgText::Text *text){
    _text = text;
}
