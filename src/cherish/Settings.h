#ifndef SETTINGS
#define SETTINGS

#include <iostream>
#include <string>

#include <osg/Vec4>
#include <QVariant>
#include <QDebug>
#include <QtGlobal>

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

#define outLogVec(msg, x, y, z) qDebug() << msg << ": " << x << " " << y << " " << z;

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

namespace cher{

const double PI = 3.14159265359;
const double EPSILON = 0.00001;
const osg::Vec3f CENTER = osg::Vec3f(0.f,0.f,0.f);

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
{
    MOUSE_PEN = 0x000,
    PEN_SKETCH,
    PEN_ERASE,
    PEN_DELETE,

    MOUSE_SELECT = 0x100,
    SELECT_ENTITY = 0x110,
    ENTITY_MOVE,
    ENTITY_SCALE,
    ENTITY_ROTATE,
    ENTITY_FLIPH,
    ENTITY_FLIPV,
    PHOTO_PUSH,
    SELECT_CANVAS = 0x120,

    MOUSE_CAMERA = 0x200,
    CAMERA_ORBIT,
    CAMERA_ZOOM,
    CAMERA_PAN,
    CAMERA_FIXEDVIEW,

    MOUSE_CREATE = 0x300,
    CREATE_CANVASCLONE,
    CREATE_CANVASSEPARATE,

    MOUSE_CANVAS = 0x400,
    CANVAS_OFFSET,
    CANVAS_ROTATE_UPLUS,
    CANVAS_ROTATE_UMINUS,
    CANVAS_ROTATE_VPLUS,
    CANVAS_ROTATE_VMINUS,

    maskMouse = 0xff00,
    maskEntity = 0xff0,
    maskAction = 0xf,
};

enum EVENT {
    EVENT_OFF,
    EVENT_PRESSED,
    EVENT_DRAGGED,
    EVENT_RELEASED
};

enum TraversalMask{
    MASK_CANVAS_IN = 0x110, /* sees all canvas data */
    MASK_CANVAS_OUT = 0x001, /* does not see any of canvas data */
    MASK_CANVASDATA_IN = 0x010, /* sees only geodeData geometries */
    MASK_CANVASFRAME_IN = 0x100, /* sees only canvas frame drawables */
    MASK_ALL_IN = ~0x0
};

const osg::Vec4 BACKGROUND_CLR = solarized::base3;
const int CURSOR_SIZE = 32;

const osg::Vec3f NORMAL = osg::Vec3f(0.f,0.f,1.f);

const osg::Vec4 CANVAS_CLR_CURRENT = solarized::magenta;
const osg::Vec4 CANVAS_CLR_INTERSECTION = osg::Vec4(solarized::violet.x(), solarized::violet.y(), solarized::violet.z(), 0.5);
const osg::Vec4 CANVAS_CLR_PREVIOUS = solarized::violet;
const osg::Vec4 CANVAS_CLR_REST = solarized::base1;
const osg::Vec4 CANVAS_CLR_SELECTED = solarized::red;
const osg::Vec4 CANVAS_CLR_EDIT = solarized::cyan;
const osg::Vec4 CANVAS_CLR_ROTAXIS = solarized::orange;

const osg::Vec4 BOOKMARK_CLR = solarized::green;
const float BOOKMARK_X = 10;
const float BOOKMARK_Y = BOOKMARK_X;
const float BOOKMARK_Z = BOOKMARK_X;

const osg::Vec4 STROKE_CLR_NORMAL = solarized::base03;
const osg::Vec4 STROKE_CLR_SELECTED = solarized::red;
const float STROKE_MINL = 0.05f;
const float STROKE_LINE_WIDTH = 4.f;
const int STROKE_SEGMENTS_NUMBER = 11;
const float STROKE_FOG_MIN = 8.f;
const float STROKE_FOG_MAX = 70.f;

const osg::Vec4 PHOTO_CLR_SELECTED = solarized::red; // texture frame colors
const osg::Vec4f PHOTO_CLR_REST = osg::Vec4f(1.f,1.f,1.f,1.0f); // white filter

const float CANVAS_MINW = 1.2f; // half width
const float CANVAS_MINH = 1.0f; // half height
const float CANVAS_MINB = 0.1f; // bound
const float CANVAS_CORNER = 0.3f; // corner size
const float CANVAS_EDITQUAD = CANVAS_CORNER/5.f;
const float CANVAS_EDITSLACK = CANVAS_CORNER + 0.1f;
const float CANVAS_AXIS = 0.5f; // loxal axis size
const float CANVAS_EDITAXIS = CANVAS_AXIS*0.5;
const float CANVAS_LINE_WIDTH = 1.5f;

const float PHOTO_MINW = 1; // half width
const float PHOTO_MINH = 1; // half height
const float PHOTO_TRANSPARECY_DELTA = 0.2f;

const float AXES_SIZE = 100.f;
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

extern double DPI_SCALING;

const size_t APP_SCREENSHOT_HEIGHT = 150;
const size_t APP_WIDGET_BUTTON = 16;
const size_t APP_WIDGET_LINEWIDTH = 4;
const size_t APP_WIDGET_GAP = 6;
const size_t APP_WIDGET_WIDTH = APP_SCREENSHOT_HEIGHT*1.5+APP_WIDGET_BUTTON*4;
const size_t APP_WIDGET_ICONSIZE_W = 100;
const size_t APP_WIDGET_ICONSIZE_H = 80;
//const size_t APP_MAINWINDOW_ICONSIZE = 24;

const QString MIME_PHOTO = "image/cherish";

const int DelegateVisibilityRole = Qt::UserRole + 1;
const int DelegateChildRole = Qt::UserRole + 2;
const int DelegateBGColor = Qt::UserRole + 3;

// UI icons base path
const QString FA_ICON_PATH = ":/FA-black-svg/FA/black/svg/";
const QString OI_ICON_PATH = ":/OI-svg/OI/svg/";

} // namespace cher

#endif // SETTINGS

