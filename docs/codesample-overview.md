# Software overview

`Cherish` is the CAD software for cultural heritage 3D representation by means of image manipulation and sketching. Implementation-wise, it is built using *Qt* library for GUI functionality and *OpenSceneGraph* as an OpenGL wrap-up library. Broadly speaking, the main idea of `Cherish` is to provide an interface so that the user can create, arrange and manipulate data in 3D space. 

Every time when a new scene is created, the following graph is built within `Cherish`: 

![Cherish scene graph](https://github.com/vicrucann/cherish/blob/develop/docs/images/cherish-SG.png)

Within the software we need to make sure all the GUI and graphics reflect the current state of the graph, and the opposite - whenever user changes parameters using UI or interactions, it is reflected correctly within the graph.

# Code structure

The code is located in the folder `src` and is divided between several folders:

* `cherish` contains `main.cpp`, settings file, utilities and resources (images, shaders).
* `libGUI` contains UI classes and their associated files such as delegates and actions.
* `libNumerics` contains numerical algorithms, e.g., curve fitting, mesh generation, homography calculation. Most of them are git submodules.
* `libSGControls` contains utility classes that are used to control the scene graph, e.g., geometry intersectors, scene graph callbacks, scene manipulator, shader programs. 
* `libSGEntities` contains all the classes of geometry entities that appear on the scene, e.g., strokes, photos, polygons, canvases, bookmarks, tools, etc.
* `tests` contains unit tests for GUI, scene graph elements and interactions of the two.
* `third_party` contains third party libraries (as git submodule). At the moment only Eigen is used.

# Improvement suggestions

The following non-exhaustive list is to provide a sense of possible changes and improvements within the code:

* There are few occurrences when an abstract class API should have been used instead of the inherited class API. For example, when selecting a stroke or a polygon (which both derive from entity class), it should be: `setEntityCurrent(entity::Entity2D* entity)` instead of having two functions `setPolygonCurrent()` and `setStrokeCurrent()` performing the same thing to the stroke or polygon entities. 
* Class `RootScene` to have unified interface to the whole scene graph as in *Facade* design pattern (currently the interface is split between `RootScene`, `UserScene` and `Canvas` interfaces). It would decrease the dependencies on the sub-graph nodes and also make scene graph manipulation easier (using one class API vs. three).
* The management of mouse modes is complicated and difficult to extend (i.e., certain mouse mode dependent classes are tightly coupled). Every time the mouse goes from one state to another (e.g., from sketching to manipulator's zoom), the changed mode has to be reflected upon certain UI elements (change of mouse cursor) and internal scene graph structure (e.g., setting up traversal masks for graph nodes). The more optimal way to do it would be creating a mouse mode class based off OpenSceneGraph `Object` class (so that to be able to use OSG smart pointers), and make the classes that rely / change mouse mode information to *observe* the mode instead.
* Some bad naming conventions of the classes, files and certain methods are present; their editing would improve code readability. 
* For some UI elements, create the corresponding `*.ui` form and put all the elements there, and then connect the form with the implementation file. This would make code readability easier since the final look of the UI form will be viewable from Qt Designer.
