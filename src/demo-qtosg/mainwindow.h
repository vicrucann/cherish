#ifndef MAINWINDOW
#define MAINWINDOW

/* MainWindow
 * It creates an OSGWidget and sets it as its center widget
 */

#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
private:

};

#endif // MAINWINDOW

