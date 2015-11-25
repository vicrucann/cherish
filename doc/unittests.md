# Unit Tests description

## `libGUI`

### View widget
* Key events Qt to OSG are passed correctly
* Set mouse mode
* Tablet events
    * To be described later


## `libSGControls`

### Event handler
* Mouse pick (for canvases)
    * Pick with mouse
    * Pick with `<Ctrl>`
    * When pick, change the current / previous canvases
* Mouse erase
    * Canvases is erased
    * Change of modes current / previous when one of them is deleted, e.g., call the mouse pick before the deletion
* Set mouse mode

### Manipulator
* Set mouse mode

## `libSGEntities`

### Axes
* Add / delete to the scene
* Set visibility

### Canvas
* Add / delete to the scene
* Set visibility
* Set color
* Set mode (current, previous, rest)
* Set name
* Get level id (the degree of the deepness in the scene graph)

### RootScene
* Add / delete a canvas
* Add / delete a node
* Delete all
* Set visibility of a node
* Get a node by its name (string)
* Get a canvas by its id (id is a unique number that is assigned when canvas is created, e.g., `Canvas1`, `Canvas2`)
* Get entity name
* Setting a state set so that all the children inherit it

## `libSGUtils`

### Node visitor
* Find node / canvas by a name
* Find list of nodes / canvases
* Set name to find

### Observer
* Text structure is equal to what is seen on the scene (Canvases only)


