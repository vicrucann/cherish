# Dura-Europos In Situ - (dureu3d) 

The **dureu3d** (name is subject to change) software is a heterogeneous framework for the modeling, design and representation of historical structures and archaeological sites in 3D. It is designed so that to unify the currently disparate data types available (e.g. 2D images, 3D strokes entered by user, text annotations attached to 3D locations, mesh representations of buildings) and to use the strength of each to support the others. 

For the data processing, rather than relying on either purely automatic approaches or human input, the framework supports speculation and reasoning in the form of both. As a result, the software is capable to take as input various raw heterogeneous data source and produce a labeled, corresponded and augmented data source that can be used in variety of applications.

###### Development status

Currently converting the prototype version into alpha version, very active development. **The current state of the project is not usable yet** (first official relseases are planned on end of spring - beginning of summer). Planned features:
 
* Near-future steps
    * Functional / technical specification - [manual](https://github.com/vicrucann/dura-europos-insitu/blob/master/doc/manual.md) file
    * Installation procedure steps
    * Unit testing, GUI testing, improved GUI design (from user feedback)
    * Documentation for user (monolithic): software set up, overview of how it works, guides to doing common tasks
    * Project site so that to publish announcements and status updates
* Far-future steps
    * Developer guideline
    * FAQ section or file
    * Demos, screen shots, videos, example outputs

Implementation steps:
* (done) Make QT5.5 and OSG-3.4.0 to work together (**October 2015**)
    * (done) On demand rendering
    * (done) Read / display a 3D demo scene
    * (done) Mouse navigation: cathing Qt events and passing them to OSG for processing
    * (done) Stylus navigation
    * (done) Stylus buttons programmation: cathing tablet events in QT and passing them to OSG
* Basic entities - display (**Nov 2015**)
    * (done) Canvas
    * (done) Strokes - entered by Stylus
    * 2D geometric forms: polylines, rectangles, circles 
    * (done) Photos = textured quads (Drawables)
* (done) Camera navigation: zoom, pan, rotate and different mouse modes (**Nov 2015**)
* Building and manipulation of a scene graph (**Nov 2015**)
    * (done)Add / delete a canvas
    * (done) Switch to another canvas
    * (done) Add / delete a stroke to a current canvas
    * Add / delete a geometric shape to a current canvas
    * (done) Select a canvas and all it contains
    * Select two or more canvases and their contents
    * Select number of entities within one canvas
    * (done) Select number of strokes within one canvas
* Advanced entity manipulation (**Dec 2015**)
    * (done) Create a new canvas as a copy of current
    * (done) Push strokes from one canvas to another
    * Move a photo from one canvas to another
* (done) Save and load of a scene graph to disk (**Dec 2015**)
* Viewer entities (**Dec 2015**)
    * (done) Model views: built-in, second screen, detached menu, viewer splitters, viewer views setup (front, top, iso)
    * Bookmarks
* (done) GUI integration (**Jan 2016**)
* (in process) First user tests and feedback (**Jan 2016**)
* Unit tests (**Jan 2016**)
* Memory leaks detection, performance measurements, optimization (threading), other improvements (**Feb 2016**)
* (in process) Bug fixing, user feedback, tuning, refactoring (**March 2016**)
* (in process) Prepare first official `beta` release (**March 2016**)
* Develop virtual touring methods (**March 2016**)
* (in process) Installers and tarballs (**April 2016**)
* (in process) User documentation (**Jan-April 2016**)
* Advanced features (**April 2016**)
    * Path animation
    * Geographic coordinate system matching

![Alt text](https://github.com/vicrucann/dura-europos-insitu/blob/master/doc/gui-orig.png "Prototype GUI screenshot")  
The initial prototype screen shot.

## Content
[Motivation](https://github.com/vicrucann/dura-europos-insitu/#motivation)  
[Features](https://github.com/vicrucann/dura-europos-insitu/#features)  
[Requirements](https://github.com/vicrucann/dura-europos-insitu/#requirements)  
[Installation](https://github.com/vicrucann/dura-europos-insitu/#installation)  
[Quick start](https://github.com/vicrucann/dura-europos-insitu/#quick-start)  
[Development and contribution](https://github.com/vicrucann/dura-europos-insitu/#development-and-contribution)  
[License](https://github.com/vicrucann/dura-europos-insitu/#license)  
[Contact and authors info](https://github.com/vicrucann/dura-europos-insitu/#contact-and-authors-info)  
[References](https://github.com/vicrucann/dura-europos-insitu/#references)  

## Motivation

Modeling historical sites is important in many fields, including cultural heritage preservation, architectural design, urban planning, etc. For any given site, there may exist many different representations such as photos, drawings, maps, laser scans, 3D renderings. Yet it remains challenging to exploit the full range of the data types, especially when given data source is sparse and incomplete.

The presented **dureu3d** software creates a heterogeneous 3D representation of archaeological sites that improve our understanding of the past while supporting novel applications in the present. It is capable of annotation the collected site and artifacts with evidence (e.g., geo-location, scan data accuracy) and confidence estimation (e.g., for text annotations), and use this information to inform the reconstruction process. In the future, it is possible to expand the types of applications for which such data can be used.

## Features

* **3D modeling**: manipulation of images in 3D space, sketch-based human input, labelling and alignment of sparse multi-scale data sources
* **Virtual touring**
* (far-future?) **Machine learning**: improvements of digital computation algorithms and reductions of the dependency on human computation. E.g., usage of semi-supervised learning and active learning to reduce uncertainty in the data.

## Requirements

The software is written on Windows 7, with QT-5.4 and OpenSceneGraph-3.4.0 libraries using minimal CMake-2.8.11. It is compatible with Linux platform. More precise requirements will be described later.

## Installation

Binaries and/or installers are going to be provided. It is also possible to download and compile from source. When compiling from source, you need to install the following tools and libraries:

1. CMake, version 2.8.11 or higher
2. Qt, version 5.4 or higher
3. OpenSceneGraph, version 3.4.0 or higher

Refer to each tool's official guide on how to install it. While there are binaries exist for Qt and CMake, it will, probably, be necessary to compile the OpenSceneGraph from source.

To build *dureu3d*, you can either use CMake commands: `cmake` and then `make`, or you can compile using CMake GUI tool, or you can compile using Qt Designer framework. We tested compilation on Windows 7 and Ubuntu 14.04. It is unknown how it behaves on Mac machines. 

## Quick start

The following sections will be added (each section will be a link inside a separate file): 
* User interface: window anatomy, toolbars, viewing axes, dialogue boxes
* Using the Wacom pen
* Using the mouse 
* Sketching in 3D
* View a model in 3D space
* Creating geometry with push strokes tool
* Draw 2D and 3D shapes
* Import and work with photographs
* Import and trace the diagrams
* Other features: importing meshed 3D data, incorporation of geo data, navigation and data manipulation, annotations. 

For many more details on the software features, check [User Manual](https://github.com/vicrucann/dura-europos-insitu/blob/master/doc/manual.md).

## Development and contribution 

This is far-future section. It will be filled as need arises.

## Contact and authors info

The software is being developed at [Yale Graphics Lab](http://graphics.cs.yale.edu/site/). The main development is performed by Victoria Rudakova under the guidance of [prof. Holly Rushmeier](http://graphics.cs.yale.edu/site/people/holly-rushmeier) and [prof. Julie Dorsey](http://graphics.cs.yale.edu/site/people/julie-dorsey).  

For any questions and requests, feel free to contact Victoria Rudakova - victoria.rudakova(at)yale.edu. Or use [Issues](https://github.com/vicrucann/dura-europos-insitu/issues) for feature requests and bug submission.

Other contributors (in no particular order):

* Yumo Rong: user interface functionality
* Natallia Trayan: icons designer
* Wendy Chan: user testing and feedback, GUI design 

## References

Here represented the publications that led to the **dureu3d** software:  

1. An integrated image and sketching environment for archaeological sites - Xuejin Chen, Yann Morvan, Yu He, Julie Dorsey, Holly Rushmeier  
2. The Mental Canvas: a tool for conceptual architectural design and analysis - Julie Dorsey, Songhua Xu, Gabe Smedresman, Holly Rushmeier, Leonard McMillan
