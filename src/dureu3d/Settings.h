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

/* Debug macros */

//#define debugLogVec(msg, x, y, z) std::cout << msg << ": " << x << " " << y << " " << z << std::endl;
//#define debugLogVal(msg, a) std::cout << msg << ": " << a << std::endl;
//#define debugLogMsg(msg) std::cout << msg << std::endl;
//#define debugErrMsg(msg) std::cerr << msg << std::endl;

#define noticeMsg(msg) osg::notify(osg::NOTICE) << msg << std::endl;
#define noticeVal(msg, a) osg::notify(osg::NOTICE) << msg << ": " << a << std::endl;
#define warningMsg(msg) osg::notify(osg::WARN) << msg << std::endl;
#define warningVal(msg, a) osg::notify(osg::WARN) << msg << ": " << a << std::endl;
#define fatalMsg(msg) osg::notify(osg::FATAL) << msg << std::endl;

#define outLogMsg(msg) std::cout << msg << std::endl;
#define outLogVal(msg, a) std::cout << msg << ": " << a << std::endl;
#define outLogVec(msg, x, y, z) std::cout << msg << ": " << x << " " << y << " " << z << std::endl;
#define outErrMsg(msg) std::cerr << msg << std::endl;

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
const double EPSILON = 0.00001;

enum ENTITY_TYPE
{
    ENTITY_STROKE
    , ENTITY_PHOTO
};

enum APPMODE{
    SCREEN_MAX = 0,
    SCREEN_MIN = 1,
    SCREEN_FULL = 2,
    SCREEN_VIRTUAL = 3,
    SCREEN_DETACHED = 4
};

enum MOUSE_MODE
{     MOUSE_SELECT // for selection and delete
    , MOUSE_SKETCH // to draw strokes and 2D geometric shapes
    , MOUSE_CANVAS_OFFSET
    , MOUSE_CANVAS_ROTATE
    , MOUSE_CANVAS_CLONE
    , MOUSE_ENTITY_MOVE
    , MOUSE_PHOTO_FLIPH
    , MOUSE_PHOTO_FLIPV
    , MOUSE_ENTITY_SCALE
    , MOUSE_ENTITY_ROTATE
    , MOUSE_BOOKMARK // create bookmark (take a snapshot of a selected viewer)
    , MOUSE_ERASE // deletes entyties from scene
    , MOUSE_DELETE // remove the selected element completely (photo or canvas)
    , MOUSE_ORBIT
    , MOUSE_ZOOM
    , MOUSE_PAN
    , MOUSE_FIXEDVIEW
};

enum EVENT {
    EVENT_OFF,
    EVENT_PRESSED,
    EVENT_DRAGGED,
    EVENT_RELEASED
};

const osg::Vec4 BACKGROUND_CLR = solarized::base3;

const osg::Vec3f NORMAL = osg::Vec3f(0.f,0.f,1.f);

const osg::Vec4 CANVAS_CLR_CURRENT = solarized::magenta;
const osg::Vec4 CANVAS_CLR_PREVIOUS = solarized::violet;
const osg::Vec4 CANVAS_CLR_REST = solarized::base1;
const osg::Vec4 CANVAS_CLR_SELECTED = solarized::red;
const osg::Vec4 CANVAS_CLR_EDIT = solarized::cyan;
const osg::Vec4 CANVAS_CLR_ROTAXIS = solarized::orange;

const osg::Vec4 STROKE_CLR_NORMAL = solarized::base03;
const osg::Vec4 STROKE_CLR_SELECTED = solarized::red;
const float STROKE_MINL = 0.05f;
const float STROKE_LINE_WIDTH = 1.5f;

const osg::Vec4 PHOTO_CLR_SELECTED = solarized::red; // texture frame colors
const osg::Vec4f PHOTO_CLR_REST = osg::Vec4f(1.f,1.f,1.f,1.0f); // white filter

const float CANVAS_MINW = 1.2f; // half width
const float CANVAS_MINH = 1.0f; // half height
const float CANVAS_MINB = 0.1f; // bound
const float CANVAS_CORNER = 0.3f; // corner size
const float CANVAS_AXIS = 0.5f; // loxal axis size
const float CANVAS_LINE_WIDTH = 1.5f;

const float PHOTO_MINW = 1; // half width
const float PHOTO_MINH = 1; // half height

const float AXES_SIZE = 0.3f;
const osg::Vec4 AXES_CLR_X = solarized::blue;
const osg::Vec4 AXES_CLR_Y = solarized::cyan;
const osg::Vec4 AXES_CLR_Z = solarized::red;

const float ERASER_MIN = 0.05f; // eraser diameter
const float ERASER_MID = 0.2f;
const float ERASER_MAX = 0.4f;

// entity "names"
const std::string NAME_CANVAS = "Canvas";
const std::string NAME_ENTITY = "Entity";
const std::string NAME_STROKE = "Stroke";
const std::string NAME_GEOMETRY = "Geometry";
const std::string NAME_PHOTO = "Photo";
const std::string NAME_BOOKMARK = "Bookmark";

const float HUD_LEFT = -60.f;
const float HUD_RIGHT = 60.f;
const float HUD_TOP = 40.f;
const float HUD_BOTTOM = -40.f;
const float HUD_TEXT_SIZE = 1.5f;
const float HUD_TEXT_POSX = HUD_LEFT + 2.f;
const float HUD_TEXT_POSY = HUD_BOTTOM + 15.f;
const float HUD_TEXT_POSZ = 0.f;

const size_t APP_SCREENSHOT_HEIGHT = 100;
const size_t APP_WIDGET_BUTTON = 32;

} // namespace dura

#endif // SETTINGS

