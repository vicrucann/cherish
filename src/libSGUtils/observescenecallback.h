#ifndef OBSERVESCENECALLBACK
#define OBSERVESCENECALLBACK

/* Class to track the presense of entities on the scene
 * It is mainly used in order to display the list of
 * present entitines on HUD screen, or on QList-type of widget.
 * The trackable objects are kept in a form of a text which
 * can be handled to a corresponding GUI widget (or HUD camera)
 * for display
 */

#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/NodeCallback>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgText/Text>

class ObserveSceneCallback : public osg::NodeCallback {
public:
    virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv);
    void setScenePointer(osg::Group* scene);
    void setTextPointer(osgText::Text* text);
private:
    osg::observer_ptr<osg::Group> _scene;
    osg::ref_ptr<osgText::Text> _text;
};

#endif // OBSERVESCENECALLBACK

