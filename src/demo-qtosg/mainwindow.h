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
private slots:
    void onCreateView();

private:
    QMdiArea* _mdiArea;
}; // class MainWindow

#endif // MAINWINDOW

