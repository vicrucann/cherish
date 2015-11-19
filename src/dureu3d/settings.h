#ifndef SETTINGS
#define SETTINGS

#include <iostream>
#include <string>

#include <osg/Vec4>

/*
 * SETTINGS is a configuration variables file for Dureu3d
 * Contains settings such as
 * - default color schemes (based on Solarized scheme)
 * - default entoty sizes
 * - etc (more to add)
 *
 * Victoria Rudakova 2015 <victoria.rudakova@yale.edu>
*/

/* The color scheme settings are based on
 * colorscehem solarized
 * For more info see <http://ethanschoonover.com/solarized>
 */
namespace  solarized {
const osg::Vec4 base03 = osg::Vec4(float(0)/255.0f, float(43)/255.0f, float(54)/255.0f, 1.0f);
const osg::Vec4 base02 = osg::Vec4(float(7)/255.0f, float(54)/255.0f, float(66)/255.0f, 1.0f);
const osg::Vec4 base01 = osg::Vec4(float(88)/255.0f, float(110)/255.0f, float(117)/255.0f, 1.0f);
const osg::Vec4 base00 = osg::Vec4(float(101)/255.0f, float(123)/255.0f, float(131)/255.0f, 1.0f);

const osg::Vec4 base0 = osg::Vec4(float(131)/255.0f, float(148)/255.0f, float(150)/255.0f, 1.0f);
const osg::Vec4 base1 = osg::Vec4(float(147)/255.0f, float(161)/255.0f, float(161)/255.0f, 1.0f);
const osg::Vec4 base2 = osg::Vec4(float(238)/255.0f, float(232)/255.0f, float(213)/255.0f, 1.0f);
const osg::Vec4 base3 = osg::Vec4(float(253)/255.0f, float(246)/255.0f, float(227)/255.0f, 1.0f);

const osg::Vec4 yellow = osg::Vec4(float(181)/255.0f, float(137)/255.0f, float(0)/255.0f, 1.0f);
const osg::Vec4 orange = osg::Vec4(float(203)/255.0f, float(75)/255.0f, float(22)/255.0f, 1.0f);
const osg::Vec4 red = osg::Vec4(float(220)/255.0f, float(50)/255.0f, float(47)/255.0f, 1.0f);
const osg::Vec4 magenta = osg::Vec4(float(211)/255.0f, float(54)/255.0f, float(130)/255.0f, 1.0f);

const osg::Vec4 violet = osg::Vec4(float(108)/255.0f, float(113)/255.0f, float(196)/255.0f, 1.0f);
const osg::Vec4 blue = osg::Vec4(float(38)/255.0f, float(139)/255.0f, float(210)/255.0f, 1.0f);
const osg::Vec4 cyan = osg::Vec4(float(42)/255.0f, float(161)/255.0f, float(152)/255.0f, 1.0f);
const osg::Vec4 green = osg::Vec4(float(133)/255.0f, float(153)/255.0f, float(0)/255.0f, 1.0f);
} // solarized



namespace dureu{

const double PI = 3.14159265359;

enum APPMODE{
    SCREEN_MAX = 0,
    SCREEN_MIN = 1,
    SCREEN_FULL = 2,
    SCREEN_VIRTUAL = 3,
    SCREEN_DETACHED = 4
};

enum MOUSE_MODE {
    MOUSE_PICK, // for selection and delete
    MOUSE_SKETCH, // to draw strokes and 2D geometric shapes
    MOUSE_NAVIGATE, // general rotate, zoom, pan the camera
    MOUSE_NAVIGATE_FIXED,
    MOUSE_EDIT, // rotate, translate, offset the selected entity
    MOUSE_BOOKMARK // create bookmark (take a snapshot of a selected viewer)
};

enum MANIP_MODE {
    MANIP_ROTATE,
    MANIP_ZOOM,
    MANIP_PAN,
    MANIP_NONE
};

const osg::Vec4 BACKGROUND_CLR = solarized::base3;

const osg::Vec4 CANVAS_CLR_CURRENT = solarized::magenta;
const osg::Vec4 CANVAS_CLR_PREVIOUS = solarized::violet;
const osg::Vec4 CANVAS_CLR_REST = solarized::base1;
const osg::Vec4 CANVAS_CLR_SELECTED = solarized::red;

const float CANVAS_MINW = 1.2f; // half width
const float CANVAS_MINH = 1.0f; // half height
const float CANVAS_MINB = 0.1f; // bound
const float CANVAS_CORNER = 0.2f; // corner size

const float AXES_SIZE = 0.3f;
const osg::Vec4 AXES_CLR_X = solarized::blue;
const osg::Vec4 AXES_CLR_Y = solarized::cyan;
const osg::Vec4 AXES_CLR_Z = solarized::red;

// entity "names"
const std::string NAME_CANVAS = "Canvas";
const std::string NAME_STROKE = "Stroke";
const std::string NAME_GEOMETRY = "Geometry";
const std::string NAME_IMAGE = "Image";

} // namespace dura

#endif // SETTINGS

