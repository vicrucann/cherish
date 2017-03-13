# Scene graph structure and elements

This is a generic scene / frame graph that is generated every time the **Cherish** application is run: 

![MD format](https://github.com/vicrucann/cherish/blob/master/docs/images/cherish-SG.png)

If the scene graph is not visible, you can [see it directly at github](https://github.com/vicrucann/cherish/blob/master/docs/images/cherish-SG.png).

## Scene graph

The `RootScene` class is a member of the `MainWindow` - the main UI element of **Cherish**.

To understand the nature of each node and for better visual separation, each node at the above diagram is assigned a specific color which is explained below. The node color is only used within the diagram for better visibility and has no meaning withing the code.

Note the **yellow** color nodes are not saved to a disc when the commands "Save" or "Save As..." are used since those nodes are the tools and they are auto generated each time the application is run, or a file is read. It is possible to save the whole scene only in case when performing an "Export" operation.

The **green** color nodes define *grouping* nodes. They can contain more than one child of the same type, for example, a list of bookmarks - `entity::Bookmarks`, or a list of user data - `GroupData` that contains user input such as strokes, photos, etc.

The **red** colored nodes are the visual scene entities: `entity::Canvas`, `entity::Stroke` and `entity::Photo`.

The **gray** colored nodes are the functional nodes that allow different types of control over the entities such as visibility and transformations.

### Other sub-scene graphs

Certain node's scene graphs are not present on the image in order to keep it more readable. For example, the `SVMData` has an underlining scene graph: 

![`SVMData` scene graph figure](https://github.com/vicrucann/cherish/blob/master/docs/images/SVMData-SG.png)

[View it directly on github](https://github.com/vicrucann/cherish/blob/master/docs/images/SVMData-SG.png), if the image is not displayed. 

The other scene graphs that are not explicitly shown are the tools' scenes. Normally, they are the descendants of the `ToolGlobal` node. Refer to the code source for their underlying scene graphs.

## Frame graph

The "frame graph" is responsible for rendering settings along the scene graph. At the [main scene graph figure](https://github.com/vicrucann/cherish/blob/master/docs/images/cherish-SG.png) some rendering settings are displayed in light blue color. For example, `Stroke` node has Bezier and depth cue shaders attached. The `Canvas` node has settings such as line width, blend function, smoothing and lightning turned on. And so on. 

Note, the presented frame graph contains only key settings. For more details it is safer to check with the source code.

