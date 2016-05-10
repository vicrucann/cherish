# User manual on how to use dura-europos-insitu: a heterogeneous framework for modeling of archaeological sites in 3D

## Content

1. **Files**: input and output
    * Opening and saving files (`osg` and `osgt` file formats - standard formats of `OpenSceneGraph`)
    * Import and export (import images like `jpg`, `bmp`, `png` and `tif`; export as other 3d formats: `3ds` and `obj`)
    * Print camera view (user has to specify the camera id, for example they can chose it from a camera list)
    * Inputs: project files, stroke model files, photographs, raster diagrams, simplified 3D models (meshes), SfM (future), geo data - Google-map-like data (future)
    * Outputs: project files
2. **Window multiplication tool** (like tmux)
    * Create new viewing window
    * Switch between windows (is done by a mouse, no GUI here)
    * Remove the window (mouse controller)
    * Deactivate window (GUI option, for example, in `Tools` menu there will be `Windows list` where user can chose a window by id and chose to deactivate it)
    * Change window type (e.g., photos, strokes, diagrams, view windows, bookmarks)
    * Create sub-window of specific type (raw data: photographs, raw data: 3D meshes, raw data: diagrams, etc.)
    * (ideas): hide/show dialogue, bookmarks window, outline mode, full screen mode, preview mode, switch window and duplicate window (when using 2 screens or more)
2. **Principal tools**
    * Select tool: add and subtract from a selection set, expanding the selection set using mouse, selecting multiple entities, selecting or deselecting all geometry, selecting a single entity (it should be implemented as a window where you can view the scene graph info and where each entity has its own id)
    * Select within a canvas
    * Select a canvas
    * Eraser tool: for pen and for mouse
    * Paint bucket tool: transparency
5. **Drawing tools** - Geometric shapes and paths
    * Rectangles
    * 3D Objects: boxes (future)
    * Ellipses and arcs
    * Polygons and polylines
    * Bezier curve pen: *strokes* 
2. **Modification tools** (2D and 3D transformations for a selected canvas)
    * Move tool (translation): making copies, moving groups, moving precisely, moving several entities, moving a single entity (e.g., strokes only)
    * Rotate tool: making rotated copies, folding along an axis of rotation (future), rotating precisely
    * Scale tool: scaling photographs, scaling about the geometry center, scaling components, scaling precisely, scaling uniformly
    * Offset tool: like translation but along one dimension
    * Using: mouse, keyboard, tool controls bar, object drop-down menu, transform dialogue (e.g. to push strokes)
2. **Camera tools**
    * Standard views: perspective mode, isometric view, current-previous view
    * Orbit tool: with mouse and with pen
    * Pan tool: panning while in other tools, panning
    * Zoom tool: change focal length with zoom tool, centering the point of view, zooming with mouse or pen, zooming in on portion of the model
    * Bookmark tool (saving camera coordinates): taking the bookmark of the current scene, navigating through the taken bookmarks, opening the saved bookmark
2. **Walkthrough window**
    * Open a walkthrough canvas
    * Draw the walking trajectory on the map
    * Look at the canvas
    * Turn left / right
3. **Editing tools**
    * Undo and redo
    * Selecting objects (mouse, keyboard, find dialogue)
    * Copying, pasting and deleting objects
    * Cloning
    * Groups: unite and split operators
    * Layers (future) 
13. **Entities**: raw data, user input and exported data
    * Photographs
    * Diagrams
    * 3D models
    * Annotations
    * Canvases with strokes by user
    * 2D and 3D geometrical shapes by user: opacity, color
4. **Photograph organization window**
    * Open image data window
    * View only images on a scene
    * Import an image
    * Import set of images
    * SfM from set of images (future)
    * Select a representative image for an image cluster
    * Drag an image from image viewer to a specific 3D space
    * Change image opacity
5. **Diagram window**
    * Open diagram data window
    * Import as an image
    * Import as a set of strokes (potrace algorithm)
5. **Mesh window**
    * Open mesh data window
    * Import a simplified mesh model
    * View only mesh data on scene
6. **Annotation tool**
    * Open annotations window / show only annotations
    * Creating text
    * Selecting text
    * Editing text
    * Formatting text: font size, font color, orientations
    * Tying text to an object (canvas better)
7. **Canvas tool**
    * Select a canvas
    * Select canvases
    * Change opacity
    * Change color
8. **Contruction tools**
    * Axes tool
    * Dimension tool
    * Display properties: perspective grid, canvas intersections
11. **Customization**
    * Preference dialogue
    * Configuration files
15. **Common tasks**
* Appendix A: **Wacom Pen gestures list**
* Appendix AA: **Keyboard hotkeys list**
* Appendix B: **Glossary**
* Appendix C: **Input and output file formats**
* Appendix D: **SfM from a set of images**, etc.
