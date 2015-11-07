#ifndef DUREUAPPLICATION_H
#define DUREUAPPLICATION_H

#include <QApplication>
#include <QTabletEvent>

class DureuApplication : public QApplication {
    Q_OBJECT
public:
    DureuApplication(int& argv, char** argc) : QApplication(argv, argc) {}
signals:
    void sendTabletActivity(bool active);
protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
private:

};

#endif // DUREUAPPLICATION_H
