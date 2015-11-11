#ifndef MAINWINDOW
#define MAINWINDOW

/* MainWindow
 * It creates an OSGWidget and sets it as its center widget
 */

#include <QMainWindow>
#include <QMdiArea>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
public slots:
    void getTabletDevice(bool active);
private slots:
    void onCreateView2(); // have to figure out how to turn it into one function
    // that takes number of views as an argument
    void onCreateView1();
    void onCreateEmptyScene2();
    void onCreateEmptyScene1();
signals:
    void sendTabletDevice(bool active);
private:
    QMdiArea* _mdiArea;
    bool _deviceActive;
}; // class MainWindow

#endif // MAINWINDOW

