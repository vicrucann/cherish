# GUI description

```
_________________________________________    
Menu Bar: File | Edit | View ...  
_________________________________________   
Tool  |                       |Other  
Box:  |                       | panels 
geom  |                       |__________
text  |  
...   |__________________________  
      |  Bookmarks (not visible) | 
__________________________________________  
info and hints text window   | coordinates  
__________________________________________  
      
```

## Menu bar

### File
* `New`
* `Open` - opens an *open dialog window* where user can select a file of specific type, e.g., `*.deu`; then the user can press either `Open` or `Cancel` to exit the window.
* `Save` - opens *save dialog window* if file was never *saved as*.
* `Save as...` - opens save dialog window where user can chose a name under which to save the file; the file extension is the project extention name, e.g., `*.deu` (temporal name, may be changed).
* `Import...` - opens an *open dialog window* where user can select files of different types: `JPEG Image`, `Windows Bitamap`, (more extensions will be added). On the right side place a set of `Radio buttons` with two (could be more in the future) options:
    * `Use as raster` 
    * `Convert to vector`
* `Exit` - exits the application

### Edit
* `Undo`
* `Redo`
* `Cut`
* `Copy`
* `Paste`
* `Delete`
* `Select all`
* `Select none`
* `Hide`
* `Unhide`
    * `Selected`
    * `Last`
    * `All`
* `Lock`
* `Unlock`
    * `Selected`
    * `All`
* `Make group`

### View
 
### Camera
* Bookmarks

### Draw

### Tools

### Window

### Help


## Toolbars

### File
* New
* Open
* Save

### Edit
* --
    * Undo
    * Redo
* --
    * Cut
    * Copy
    * Paste
 
### Camera navigation
* --
    * Orbit
    * Pan
    * Zoom
* --
    * Previous view
    * Next view
* --
    * Custom view: home, iso, left, right, front, top, back
* Camera filter
    * View only strokes and photos (hide contruction tools)

### Modify scene
* --
    * Sketch, polyline, rectangle(?), arc(?)
    * Eraser
    * Delete
* --
    * Push a set of strokes from current canvas to another
    * Move a photo from current canvas to another
    * Move en entity (any 3D object) from current canvas to another
* Canvas modification
    * Rotate
    * Offset
* Photo modification
    * Rotate
    * Scale
    * Move
    * Flip

### Selector
* Select canvas (or photo within the current canvas)
* Select by plane intersection (select a set of strokes, or a set of strokes and photos) within the current canvas

### Viewers
* Virtual screen mode
* Two screen mode
* Full screen

## Hint window

## Coordinates

## Other panels: other tools

### Canvas manager

The scene structure tool can be called from `Window -> Scene structure`.

```
Entities        | Visibility | Current  
----------------------------------------  
Layer1          |     v      |    o  
|__ Canvas1     |     v      |    o  
|__ Canvas2     |     o      |    o  
|__ Group1      |     v      |    o  
    |__ Canvas3 |     v      |    o  
    |__ Canvas4 |     o      |    o  
Layer2          |     o      |    o  
|__ Canvas5     |     o      |    *  
-----------------------------------------  
```

Each `Visibility` property is represented by a `bool` variable. The visibility of a `Group` sets the same visibility for all of its children. The same holds for a `Layer`. The visibility can be represented by a choice flag (see example in Sketchup of `Window` -> `Layers`). 

The `Current` property indicates what is the current entity which is the user is editing. It is possible to select an individual canvas or a group of canvases, a layer  ( for the future: it will also be possible to do selection of different entities, e.g.: different canvases and group of canvases, or a layer and canvases using keyboard buttor `<Ctrl>` as an example). The current property should be represented by a round flag or a simple asteric (`*`) character.
