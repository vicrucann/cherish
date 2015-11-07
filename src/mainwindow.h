#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMdiArea>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
public slots:
    void getTabletActivity(bool active);
private slots:
    void onCreateViewer();
signals:
    void sendTabletActivity(bool active);
private:
    QMdiArea* _mdiArea;
    bool _tabletActive;
};

#endif // MAINWINDOW

