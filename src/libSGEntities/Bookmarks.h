#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <osg/Group>
#include <osgDB/ObjectWrapper>

/* Osg + Qt model of booksmarks
 * osg::Group is used in order to perform correct serialization, while
 * QListModel is used for display at the corresponding BookmarkWidget
*/

#include <vector>
#include <string>

#include <QObject>
#include <QModelIndex>
#include <QListWidgetItem>
#include "../libGUI/BookmarkWidget.h"

class BookmarkWidget;

namespace entity {
class Bookmarks : public QObject, public osg::Group
{
    Q_OBJECT

public:
    Bookmarks();
    Bookmarks(const Bookmarks& parent, osg::CopyOp copyop = osg::CopyOp::SHALLOW_COPY);
    META_Node(entity, Bookmarks)

    /* osg serialization setters and getters */
    void setEyes(const std::vector<osg::Vec3d>& eyes);
    const std::vector<osg::Vec3d>& getEyes() const;

    void setCenters(const std::vector<osg::Vec3d>& centers);
    const std::vector<osg::Vec3d>& getCenters() const;

    void setUps(const std::vector<osg::Vec3d>& ups);
    const std::vector<osg::Vec3d>& getUps() const;

    void setNames(const std::vector<std::string>& names);
    const std::vector<std::string>& getNames() const;

    /* other methods */
    void addBookmark(BookmarkWidget* widget,
                     const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up, const std::string& name);
    void resetModel(BookmarkWidget* widget);

signals:
    void sendBookmark(int row);

public slots:
    void onClicked(const QModelIndex& index);
    void onItemChanged(QListWidgetItem* item);

protected:
    ~Bookmarks() {}

private:
    std::vector<osg::Vec3d> m_eyes;
    std::vector<osg::Vec3d> m_centers;
    std::vector<osg::Vec3d> m_ups;
    std::vector<std::string> m_names;

    int m_row;
};
}


#endif // BOOKMARKS_H
