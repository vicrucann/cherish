#ifndef DUREUAPPLICATION_H
#define DUREUAPPLICATION_H

#include <QApplication>
#include <QTabletEvent>

/*! \class DureuApplication
 * Class description
*/
class DureuApplication : public QApplication {
    Q_OBJECT
public:
    DureuApplication(int& argv, char** argc);
    ~DureuApplication();
signals:
    void sendTabletActivity(bool active);
public slots:
protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
private:
};

#endif // DUREUAPPLICATION_H
