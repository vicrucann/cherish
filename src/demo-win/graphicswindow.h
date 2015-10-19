#ifndef GRAPHICSWINDOW_H
#define GRAPHICSWINDOW_H

#include <QWindow>

class GraphicsWindow : public QWindow
{
    Q_OBJECT

public:
    GraphicsWindow(QScreen *parent = 0);
    ~GraphicsWindow();
};

#endif // GRAPHICSWINDOW_H
