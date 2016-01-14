#include "AddCanvasCommand.h"

#include <QObject>

AddCanvasCommand::AddCanvasCommand(entity::UserScene* scene, const osg::Matrix& R, const osg::Matrix& T, const std::string& name, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scen(scene)
    , m_canvas(new entity::Canvas)
{
    m_canvas->initializeSG();
    m_canvas->setName(name);
    m_canvas->setMatrixRotation(R);
    m_canvas->setMatrixTranslation(T);
    this->setText(QObject::tr("Add %1")
                  .arg(QString(name.c_str())));
}

AddCanvasCommand::AddCanvasCommand(entity::UserScene* scene, const entity::Canvas& copy, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_scen(scene)
    , m_canvas(new entity::Canvas(copy, osg::CopyOp::DEEP_COPY_ALL))
{
    this->setText(QObject::tr("Add %1")
                  .arg(QString(m_canvas->getName().c_str())));
}

AddCanvasCommand::~AddCanvasCommand()
{

}

void AddCanvasCommand::undo()
{
    // make sure current/previous rules hold
    if (m_canvas == m_scen->getCanvasCurrent())
        m_scen->setCanvasCurrent(m_scen->getCanvasPrevious());
    if (m_canvas == m_scen->getCanvasPrevious() ||
            m_scen->getCanvasCurrent() == m_scen->getCanvasPrevious()){
        for (unsigned int i = 0; i < m_scen->getNumChildren(); ++i){
            entity::Canvas* cnvi = dynamic_cast<entity::Canvas*>( m_scen->getChild(i));
            if (cnvi != NULL && cnvi != m_scen->getCanvasCurrent() && cnvi != m_canvas){
                m_scen->setCanvasPrevious(cnvi);
                break;
            }
        }
    }
    // now delete the canvas
    m_scen->removeChild(m_canvas);
}

void AddCanvasCommand::redo()
{
    m_scen->addChild(m_canvas);
    m_scen->setCanvasCurrent(m_canvas);
}

