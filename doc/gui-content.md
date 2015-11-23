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
 
### Camera
* Orbit
* Pan
* Zoom
* Zoom Window
* Zoom Extents
* Previous (to go to previous camera position)

### Drawing
* Stylus 
* Polyline 

### Edit
* Move
* Rotate
* Scale
* Offset

### Canvases
* Looks like drop-down list which allows to select a canvas by its id name

### Principal
* Select
* Eraser

### Standard
* --
    * New
    * Open
    * Save
* --
    * Cut
    * Copy
    * Paste
    * Erase
* --
    * Undo
    * Redo
* --
    * Print

### Styles
* All (view all entities)
* Strokes (view strokes only)
* Images (view images only)

### Views
* Iso
* Top
* Front
* Right
* Back
* Left

### Viewers
* Virtual screen mode
* Two screen mode
* Full screen

## Hint window

## Coordinates

## Other panels: other tools

### Scene structure

```
Entities        | Visibility | Current  
----------------------------------------  
Layer1          |     v      |    -  
|__ Canvas1     |     v      |    -  
|__ Canvas2     |     o      |    -  
|__ Group1      |     v      |    -  
    |__ Canvas3 |     v      |    -  
    |__ Canvas4 |     o      |    -  
Layer2          |     o      |    -  
|__ Canvas5     |     o      |    *  
-----------------------------------------  
```

Each `Visibility` property is represented by a `bool` variable. The visibility of a `Group` sets the same visibility for all of its children. The same holds for a `Layer`. It would be great to use an icon of an "eye" to represent the visibility: e.g., an eye would indicate the corresponding entity is visible, while the closed eye (or crossed eye) would indicate an entyty's invisibility (if no icons can be found, just use letters `v` to represent visibility and 'o' to represent invisibility). 

The `Current` property indicates what is the current entity which is the user is editing. The corresponding entity would be selected by a color. It is possible to select an individual canvas or a group of canvases, a layer or a group of layers; it will also be possible to do selection of different entities, e.g.: different canvases and group of canvases, or a layer and canvases (using keyboard buttor `<Ctrl>` as an example).  The current property should be represented by an icon of a pen or a simple asteric character.
