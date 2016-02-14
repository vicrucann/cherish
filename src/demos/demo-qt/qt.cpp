#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>

int main(int argc, char** argv){
    QApplication qapp(argc, argv);
    QMainWindow window;
    QOpenGLWidget* widget = new QOpenGLWidget(&window);
    window.setCentralWidget(widget);
    window.show();
    return qapp.exec();
}
