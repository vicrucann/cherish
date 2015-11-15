#ifndef BOOKMARKWIDGET
#define BOOKMARKWIDGET

#include <QWidget>
#include <QDockWidget>

class BookmarkWidget: public QDockWidget {
public:
    BookmarkWidget(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~BookmarkWidget();
private:
};

#endif // BOOKMARKWIDGET

