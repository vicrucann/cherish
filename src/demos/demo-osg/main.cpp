#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include <Windows.h>

int main(int argc, char** argv)
{
    ::SetProcessDPIAware();

    osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("../../../samples/cow.osgt");
    osgViewer::Viewer viewer;
    viewer.setSceneData(root.get());

    viewer.setUpViewOnSingleScreen(0);
    return viewer.run();
}

