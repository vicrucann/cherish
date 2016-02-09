#ifndef GRAPHICSWINDOW_H
#define GRAPHICSWINDOW_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLPaintDevice>

// base class for graphics window, e.g. viewport
class GraphicsWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GraphicsWindow(QScreen *parent = 0); // or QScreen
    ~GraphicsWindow();

    virtual void render(QPainter* painter);
    virtual void render();
    virtual void initialize();
    void setAnimating(bool anim);

public slots:
    void renderNow();
    void renderLater();

protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent* event) Q_DECL_OVERRIDE;

private:
    // setup context, screen and format
    QOpenGLContext* m_pContext;

    // to paint to an opengl using QPainter draw commands
    QOpenGLPaintDevice* m_pDevice;

    bool m_update_pending;
    bool m_animating;
}; // class GraphicsWindow

#endif // GRAPHICSWINDOW_H
