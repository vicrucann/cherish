#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class CMainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

    static CMainWindow& instance();

private:
    Ui::CMainWindow *ui;
    static CMainWindow* m_instance;
};

#endif // CMAINWINDOW_H
