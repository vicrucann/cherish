#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setDesktopWidget(QDesktopWidget* desktop, int mode);

private:
    void DesktopSetup();

    Ui::MainWindow *ui;
    QDesktopWidget* _desktop;
    int _mode; // 0 - single screen, 1 - full screen, 2 - virtual screen
};

#endif // MAINWINDOW_H
