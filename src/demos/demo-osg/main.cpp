//#include <QApplication>
//#include <QMainWindow>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

int main(int argc, char** argv)
{
    osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("../../samples/cow.osgt");
    osgViewer::Viewer viewer;
    viewer.setSceneData(root.get());
    return viewer.run();
}

