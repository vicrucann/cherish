## Entity-Component tree:
`Qt3D` uses this technology:
* app -> insitu
    * viewport -> view window
    * controllers -> menu bar, tool bar

## MVC 

**Model-View-Controller** - is used to separate the data from how the data is displayed and how it is manipulated.

* a view is responsible for drawing the current model and provide clickable entities
* a model which keeps the current data to be represented
* a controller is to get the events from the view and convert them into proper entities so that to modify the model

An example: the user clicks on an entity and deletes it. The controller will recieve the event from the view, creates an object representing a command (through Command pattern), adds it to a queue for undo capability and executes the command. The command modifies the model. The responsibility of translating view events into the complex machinery that modifies the model is of controller.

[OSG for CAD-like application, discussion](http://forum.openscenegraph.org/viewtopic.php?t=3709) 

## Graphics engine
[Intergrating Ogre into Qt5](http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Integrating+Ogre+into+QT5)

OpenSceneGraph has OpenGL API.

Qt: [graphics and performance OpenGL](https://blog.qt.io/blog/2010/01/06/qt-graphics-and-performance-opengl/) - `QOpenGLWidget`, raster engine. Example tip: Prefer to use power-of-two (2^n) widths & heights for QImages and QPixmaps (128×256, 256×256, 512×512, etc).

## Scene-Graph
OpenSceneGraph library to represent entities and their relationships on the scene. Main components of the library:
* scene graph classes
* graph traversal
* visualizer of the graphic scene
* nodekit for easy intergration with Qt

[Manipulators in CAD-style application](http://forum.openscenegraph.org/viewtopic.php?t=3709&highlight=cad)

If going from scratch, consider using `QGraphicsScene` and `QGraphicsView`, or BSP tree (although it's probably only relevant for 2D graphics since drawing is done by means of Qt, not OpenGL).

## Misc
Ideas on how the project will be divided, the main parts:
1. 3D graphics: rendering (use OpenGL or even new technology Vulkan)
2. GUI: Qt widgets code
3. MVC - model view controller framework and its elements (classes)
4. 3rd parties: any additional libraries needed for a built, e.g., for rendering

## Qt useful classes
* `QPainterPath` - geometrical shapes, includung strokes
* 

## Developer plan
* Build Qt from source so that to run *boxes* examples (OpenGL and 3D manipulations)
    * What window to use for inheritance?
    * How to perform the rendering, and how often
    * Put some mock models, e.g., rectangles to represent canvases and Bezier paths to represent strokes
    * Introduce Wacom tablet and pen event processing
    * Camera rotation and zoom using mouse
* Put an initial project on github which is a custom window that allows to work with OpenGL 3D graphics and reacts on Wacom tablet pen movements
* Scene graph organization and library
    * Build a simple demo scene using scene graph paradigm
* Model View Controllera
    * Put some basic controller buttons
    * Start main project from scratch using developed techniques and wrap them into MVC framework 

## Main Window framework and components
* see reference of `QMainWindow` - it is shown there how the widgets can be located inside the Main Window
* 
