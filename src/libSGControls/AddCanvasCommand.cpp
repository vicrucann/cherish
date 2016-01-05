#include "AddCanvasCommand.h"

#include <QObject>

AddCanvasCommand::AddCanvasCommand(RootScene *scene, osg::MatrixTransform *transform, const std::string &name, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_canvas(new Canvas(transform, name))
{
    this->setText(QObject::tr("Add Canvas with name %1")
                  .arg(QString(name.c_str())));
}

AddCanvasCommand::~AddCanvasCommand()
{

}

void AddCanvasCommand::undo()
{
    // make sure current/previous rules hold
    if (m_canvas == m_scene->getCanvasCurrent())
        m_scene->setCanvasCurrent(m_scene->getCanvasPrevious());
    if (m_canvas == m_scene->getCanvasPrevious() ||
            m_scene->getCanvasCurrent() == m_scene->getCanvasPrevious()){
        for (unsigned int i = 0; i < m_scene->getUserScene()->getNumChildren(); ++i){
            Canvas* cnvi = dynamic_cast<Canvas*>( m_scene->getUserScene()->getChild(i));
            if (cnvi != NULL && cnvi != m_scene->getCanvasCurrent() && cnvi != m_canvas){
                m_scene->setCanvasPrevious(cnvi);
                break;
            }
        }
    }
    // now delete the canvas
    m_scene->getUserScene()->removeChild(m_canvas);
}

void AddCanvasCommand::redo()
{
    m_scene->getUserScene()->addChild(m_canvas);
    m_scene->setCanvasCurrent(m_canvas);
}

