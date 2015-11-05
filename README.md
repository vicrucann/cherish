# Dura-Europos In Situ - (dureu3d) 
The **dureu3d** software is a heterogeneous framework for the modeling and representation of historical structures and archaeological sites in 3D. It is designed so that to unify the currently disparate data types available (e.g. 2D images, 3D strokes entered by user, text annotations attached to 3D locations, mesh representations of buildings) and to use the strength of each to support the others. For the data processing, rather than relying on either purely automatic approaches or human input, the framework supports speculation and reasoning in the form of both. As a result, the software is capable to take as input various raw heterogeneous data source and produce a labeled, corresponded and augmented data source that can be used in variety of applications.

###### Development status
Currently converting the prototype version into alpha version, very active development. The current state of the project is not usable yet. Planned characteristics: 
* Near-future steps
    * Version control on github
    * Functional / technical specification - [manual](https://github.com/vicrucann/dura-europos-insitu/blob/master/doc/manual.md) file
    * Reduce the library dependencies by using only Qt, OpenSceneGraph, OpenGL and cmake
    * Improve GUI (replace it by using Qt only)
    * cmake (or qmake) files, installation procedure steps (what technical expertise is required, tutorial-style examples)
    * Better object orientation design, refactoring, graph scene, and introducing new features
    * Unit testing, GUI testing
    * Documentation for user (monolithic) and wiki (software set up, overview of how it works, guides to doing common tasks)
* Far-future steps
    * Bug database, feature requests 
    * History of past releases 
    * Potential developer guidelines (where documentation is incomplete, desired features, known deficiencies, etc.)
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
    * Canvas
    * Strokes - entered by Stylus
    * 2D geometric forms: polylines, rectangles, circles 
    * Photos = textured canvases?
* Basic entities - navigation (**Nov 2015**)
* Building and manipulation of a scene graph (**Nov 2015**)
    * Add / delete a canvas
    * Switch to another canvas
    * Add / delete a stroke to a current canvas
    * Add / delete a geometric shape to a current canvas
    * Select a canvas and all it contains
    * Select two or more canvases and their contents
    * Select number of entities within one canvas
* Advanced entity manipulation (**Dec 2015**)
    * Create a new canvas as a copy of current
    * Push strokes from one canvas to another
    * Move a photo from one canvas to another
* Save and load of a scene graph to disk (**Dec 2015**)
* Viewport entities (**Dec 2015**)
    * Window views
    * Bookmarks
* GUI integration (**Jan 2016**)
* First user tests and feedback (**Jan 2016**)
* Unit tests (**Jan 2016**)
* Configuration management (**Feb 2016**)
* Memory leaks detection, performance measurements, optimization (threading), other improvements (**Feb 2016**)
* Bug fixing, user feedback, tuning, refactoring (**March 2016**)
* Prepare first official `beta` release (**March 2016**)
* Installers and tarballs (**April 2016**)
* User documentation (**Jan-April 2016**)
* Advanced features (**April 2016**)
    * Path animation

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

* (Automated, planned) **3D modeling**: image-based modeling, laser scan data, procedural models.
* **Markup system**, based on 3D sketching (interpretation of 2D sketches as 3D forms): intuitive and interactive annotations, labeling and correspondence.
* **Human input** (crowd sourcing): augmentation, labeling and alignments of the sparse, multi-scale and heterogeneous data sources. 
* (planned) **Machine learning**: improvements of digital computation algorithms and reductions of the dependency on human computation. E.g., usage of semi-supervised learning and active learning to reduce uncertainty in the data.

## Requirements

The software is written on Windows 7, with QT and OpenSceneGraph libraries using OpenGL. It is planned to be compatible with Linux platform (e.g., Ubuntu 14.04). More precise requirements will be described later.


## Installation

Can be installed using commands `qmake` and `make` (possibly `cmake` and `make`). Also project files for QT Creator and Microsoft Visual Studio will be provided. 

If compiling from source, make sure the dependencies are met. So far, the project relies onto the next open-source libraries:
1. cmake
2. Qt SDK (QtWidgets, QtOpenGL)
3. OpenSceneGraph (core libraries, OpenGL support)

After installation of the OpenSceneGraph (OSG for short), update the environment variables so that `cmake` could find the necessary libraries. Here is the example of the set variables on Linux (same works for `cygwin`):
```
export OPENTHREADS_LIB_DIR="/usr/local/lib64:/usr/local/lib"  
export PATH="$OPENTHREADS_LIB_DIR:$PATH"  
export LD_LIBRARY_PATH="/usr/local/lib64:/usr/local/lib:$LD_LIBRARY_PATH"  
export OSG_FILE_PATH="/usr/local/OpenSceneGraph/data:/usr/local/OpenSceneGraph/data/Images"  
```
Refer to the official OSG guide for more details or on how to set up the OSG using other platforms.

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

This is far-future section. It will be filled as a need arises.

## Contact and authors info

The author and lab information will be added here. For the current version, feel free to contact Victoria Rudakova - victoria.rudakova(at)yale.edu ; or use "Issues" (in the menu on right) for feature requests or bug submission.

## References

Here represented the publications that led to the **dureu3d** software:  

1. An integrated image and sketching environment for archaeological sites - Xuejin Chen, Yann Morvan, Yu He, Julie Dorsey, Holly Rushmeier  
2. The Mental Canvas: a tool for conceptual architectural design and analysis - Julie Dorsey, Songhua Xu, Gabe Smedresman, Holly Rushmeier, Leonard McMillan
