#ifndef SETTINGS
#define SETTINGS

#include <iostream>
#include <string>

#include <osg/Vec4>
#include <QColor>
#include <QVariant>
#include <QDebug>
#include <QtGlobal>

/*! \file Settings.h
 * \brief Settings is a configuration variables file for Cherish: enums, consts, defaults.
 * Contains settings such as
 * * default color schemes (based on Solarized scheme)
 * * default entity sizes
 * * cursor modes and entity types
 * * canvas traversal masks
 * * standard naming conventions for entities
*/

/* Debug macros */
#define outLogVec(msg, x, y, z) qDebug() << msg << ": " << x << " " << y << " " << z;

/*! \addtogroup solarized
 * The color scheme settings are based on color scheme solarized.
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

namespace molokai {
const QColor orangeSoft(QString("#EFC090"));
const QColor orange(QString("#FD971F"));
const QColor orangeBright(QString("#EF5939"));

const QColor violetSoft(QString("#D197D9"));
const QColor violet(QString("#AE81FF"));

const QColor greenSoft(QString("#D9E577"));
const QColor green(QString("#A6E22E"));
const QColor greenBright(QString("#66CCB3"));
const QColor greenDark(QString("#465457"));

const QColor blueSoft(QString("#79ABFF"));
const QColor blue(QString("#1E90FF"));
const QColor blueBright(QString("#66D9EF"));
const QColor blueDark(QString("#13354A"));

const QColor cherrySoft(QColor("#BCA3A3"));
const QColor cherry(QColor("#960050"));
const QColor cherryBright(QColor("#F92672"));
const QColor cherryDark(QColor("#1E0010"));

const QColor yellowSoft(QColor("#FFE792"));
} // molokai

/*! Cherish constants and other settings */
namespace cher{

// global constants
const double PI = 3.14159265359;
const double EPSILON = 0.00001;
const osg::Vec3f CENTER = osg::Vec3f(0.f,0.f,0.f);
const osg::Vec3f UP = osg::Vec3f(0.f, 0.f, 1.f); /*!< global up vector */

// main entity types
enum ENTITY_TYPE
{
    ENTITY_STROKE
    , ENTITY_PHOTO
    , ENTITY_POLYGON
};

/*! Enum class for mouse modes */
enum MOUSE_MODE
{
    MOUSE_PEN = 0x000,
    PEN_SKETCH,
    PEN_ERASE,
    PEN_DELETE,
    PEN_POLYGON,

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

    MOUSE_SVM = 0x500,
    SVM_IDLE = 0x510,
    SVM_HOVER_WIRE = 0x520,
    SVM_DRAG_WIRE,
    SVM_HOVER_POINT = 0x530,
    SVM_DRAG_POINT,

    MOUSE_CAMPOSE = 0x600,
    CAMPOSE_IDLE = 0x610,
    CAMPOSE_EYE = 0x620,
    CAMPOSE_CENTER = 0x630,
    CAMPOSE_FOCAL = 0x640,

    maskMouse = 0xff00,
    maskEntity = 0xff0,
    maskAction = 0xf,

    maskDrag = 0x001
};

/*! Enum class for cherish events. */
enum EVENT {
    EVENT_OFF,
    EVENT_PRESSED,
    EVENT_DRAGGED,
    EVENT_RELEASED
};

/*! Enum class for canvas traversal masks. */
enum TraversalMask
{
    MASK_CANVAS_IN = 0x110, /* sees all canvas data */
    MASK_CANVAS_OUT = 0x001, /* does not see any of canvas data */
    MASK_CANVASDATA_IN = 0x010, /* sees only geodeData geometries */
    MASK_CANVASFRAME_IN = 0x100, /* sees only canvas frame drawables */
    MASK_SVMDATA_IN = 0x1000, /* sees only entity::SVMData */
    MASK_CAMPOSEDATA_IN = 0x1000,
    MASK_ALL_IN = ~0x0
};

// general widget settings
const osg::Vec4 BACKGROUND_CLR = solarized::base3;
const int CURSOR_SIZE = 18;

// canvas normal settings
const osg::Vec3f NORMAL = osg::Vec3f(0.f,0.f,1.f);

// canvas color settings
const osg::Vec4 CANVAS_CLR_CURRENT = solarized::magenta;
const osg::Vec4 CANVAS_CLR_INTERSECTION = osg::Vec4(solarized::violet.x(), solarized::violet.y(), solarized::violet.z(), 0.5);
const osg::Vec4 CANVAS_CLR_PREVIOUS = solarized::violet;
const osg::Vec4 CANVAS_CLR_REST = solarized::base1;
const osg::Vec4 CANVAS_CLR_SELECTED = solarized::red;
const osg::Vec4 CANVAS_CLR_EDIT = solarized::cyan;
const osg::Vec4 CANVAS_CLR_ROTAXIS = solarized::orange;

// bookmark tool settings
const osg::Vec4 BOOKMARK_CLR = solarized::green;
const float BOOKMARK_X = 10;
const float BOOKMARK_Y = BOOKMARK_X;
const float BOOKMARK_Z = BOOKMARK_X;

// bookmark SVM data settings
const float SVMDATA_HALFWIDTH = 0.25f;
const std::vector<QColor> SVMDATA_CLR_POINTS = {
                                        molokai::orangeSoft, molokai::violetSoft, molokai::greenSoft, molokai::blueSoft };
const QColor SVMDATA_CLR_WIRE =         molokai::cherrySoft;
const QColor SVMDATA_CLR_WIREHOVER =    molokai::cherry;
const std::vector<QColor> SVMDATA_CLR_POINTSHOVER =  {
                                        molokai::orange, molokai::violet, molokai::green, molokai::blue};
const QColor SVMDATA_CLR_DRAG =         molokai::cherryBright;

// bookmark camera pose color settings
const QColor CAMPOSE_CLR_FOCAL =        molokai::blueDark;
const QColor CAMPOSE_CLR_EYE =          molokai::greenDark;
const QColor CAMPOSE_CLR_CENTER =       molokai::cherryDark;

// stroke settings
const osg::Vec4 STROKE_CLR_NORMAL = solarized::base03;
const osg::Vec4 STROKE_CLR_SELECTED = solarized::red;
const float STROKE_MINL = 0.05f;
const float STROKE_LINE_WIDTH = 4.f;
const int STROKE_SEGMENTS_NUMBER = 11;
const float STROKE_FOG_MIN = 4.f;
const float STROKE_FOG_MAX = 30.f;

// polygon settings
const float POLYGON_LINE_WIDTH = 4.f;
const osg::Vec4f POLYGON_CLR_PHANTOM = solarized::base1;
const osg::Vec4f POLYGON_CLR_NORMALFILL = solarized::base2;
const osg::Vec4f POLYGON_CLR_SELECTEDFILL = solarized::green;
const float POLYGON_PROXIMITY_THRESHOLD = 0.1f;

// photo selected colors
const osg::Vec4 PHOTO_CLR_SELECTED = solarized::red; // texture frame colors
const osg::Vec4f PHOTO_CLR_REST = osg::Vec4f(1.f,1.f,1.f,1.0f); // white filter

// canvas settings
const float CANVAS_MINW = 1.2f; // half width
const float CANVAS_MINH = 1.0f; // half height
const float CANVAS_MINB = 0.1f; // bound
const float CANVAS_CORNER = 0.3f; // corner size
const float CANVAS_EDITQUAD = CANVAS_CORNER/5.f;
const float CANVAS_EDITSLACK = CANVAS_CORNER + 0.1f;
const float CANVAS_AXIS = 0.5f; // loxal axis size
const float CANVAS_EDITAXIS = CANVAS_AXIS*0.5;
const float CANVAS_LINE_WIDTH = 1.5f;

// photo settings
const float PHOTO_MINW = 1; // half width
const float PHOTO_MINH = 1; // half height
const float PHOTO_TRANSPARECY_DELTA = 0.2f;

// global axis settings
const float AXES_SIZE = 100.f;
const osg::Vec4 AXES_CLR_X = solarized::blue;
const osg::Vec4 AXES_CLR_Y = solarized::cyan;
const osg::Vec4 AXES_CLR_Z = solarized::red;

// entity "names"
const std::string NAME_CANVAS = "Canvas";
const std::string NAME_ENTITY = "Entity";
const std::string NAME_STROKE = "Stroke";
const std::string NAME_GEOMETRY = "Geometry";
const std::string NAME_PHOTO = "Photo";
const std::string NAME_BOOKMARK = "Bookmark";
const std::string NAME_SVM_WIRE = "SVMWire";
const std::string NAME_SVM_POINTS = "SVMPoints";
const std::string NAME_CAM_EYE = "CamEye";
const std::string NAME_CAM_CENTER = "CamCenter";
const std::string NAME_CAM_FOCAL = "CamFocal";

// variable that helps to adapt to high DPI monitors
extern double DPI_SCALING;

// cherish default sizing
const size_t APP_SCREENSHOT_HEIGHT = 150;
const size_t APP_WIDGET_BUTTON = 16;
const size_t APP_WIDGET_LINEWIDTH = 4;
const size_t APP_WIDGET_GAP = 6;
const size_t APP_WIDGET_WIDTH = APP_SCREENSHOT_HEIGHT*1.5+APP_WIDGET_BUTTON*4;
const size_t APP_WIDGET_ICONSIZE_W = 100;
const size_t APP_WIDGET_ICONSIZE_H = 80;

// photo format, used for drag and drop functionality
const QString MIME_PHOTO = "image/cherish";

// CanvasPhotoWidget roles
const int DelegateVisibilityRole = Qt::UserRole + 1;
const int DelegateChildRole = Qt::UserRole + 2;
const int DelegateBGColor = Qt::UserRole + 3;

// UI icons base path
const QString FA_ICON_PATH = ":/FA-black-svg/FA/black/svg/";
const QString OI_ICON_PATH = ":/OI-svg/OI/svg/";

} // namespace cher

#endif // SETTINGS

