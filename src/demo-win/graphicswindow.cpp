#include "graphicswindow.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

GraphicsWindow::GraphicsWindow(QWindow *parent)
    : QWindow(parent),
      m_pContext(0),
      m_pDevice(0),
      m_update_pending(0),
      m_animating(0)
{
    this->setSurfaceType(QWindow::OpenGLSurface);
}

GraphicsWindow::~GraphicsWindow() {
    if (m_pDevice)
        delete m_pDevice;
    if (m_pContext)
        delete m_pContext;
}

void GraphicsWindow::initialize(){}

void GraphicsWindow::render(QPainter *painter){
    Q_UNUSED(painter);  // save for later usage
}

void GraphicsWindow::render(){
    if (!m_pDevice)
        m_pDevice = new QOpenGLPaintDevice;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_pDevice->setSize(this->size());
    QPainter painter(m_pDevice);
    render(&painter);
}

// slots
void GraphicsWindow::renderNow(){
    if (!this->isExposed())
        return;
    bool needInit = false;
    if (!m_pContext){
        m_pContext = new QOpenGLContext(this);
        m_pContext->setFormat(this->requestedFormat());
        m_pContext->create();
        needInit = true;
    }
    m_pContext->makeCurrent(this);
    if (needInit){
        this->initializeOpenGLFunctions();
        this->initialize();
    }
    this->render();
    m_pContext->swapBuffers(this);
    if (m_animating)
        this->renderLater();
}

void GraphicsWindow::renderLater(){
    if (!m_update_pending){
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}
// end slots

bool GraphicsWindow::event(QEvent* event){
    switch (event->type()){
    case QEvent::UpdateRequest:
        m_update_pending = false;
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GraphicsWindow::exposeEvent(QExposeEvent *event){
    Q_UNUSED(event);
    if (this->isExposed())
        this->renderNow();
}

void GraphicsWindow::setAnimating(bool anim){
    m_animating = anim;
    if (anim)
        this->renderLater();
}
