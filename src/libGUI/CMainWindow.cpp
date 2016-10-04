#include "CMainWindow.h"
#include "ui_CMainWindow.h"

CMainWindow* CMainWindow::m_instance = nullptr;

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    Q_ASSERT_X(m_instance == 0, "MainWindow ctor", "MainWindow is a singleton and cannot be created more than once");
    m_instance = this;
}

CMainWindow::~CMainWindow()
{
    m_instance = nullptr;
    delete ui;
}

CMainWindow &CMainWindow::instance()
{
    Q_ASSERT_X(m_instance, "MainWindow::instance", "Singleton not constructed yet");
    return *m_instance;
}
