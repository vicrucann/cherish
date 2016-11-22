# Scene graph structure and elements

This is an example of a generic scene graph that is generated every time the **Cherish** application is run: 

![MD format](/docs/images/cherish-SG.png)

If the scene graph is not visible, you can [see it directly at github](https://github.com/vicrucann/cherish/blob/master/docs/images/cherish-SG.png).

The `RootScene` class is a memeber of the `MainWindow` - the main UI element of **Cherish**.

To understand the nature of each node and for better visual separation, each node at the above diagram is assigned a specific color which is explained below. The node color is only used within the diagram for better visibility and has no meaning withing the code.

Note the **yellow** color nodes are not saved to a disc when the commands "Save" or "Save As..." are used since those nodes are the tools and they are auto generated each time the application is run, or a file is read. It is possible to save the whole scene only in case when performing an "Export" operation.

The **green** color nodes define *grouping* nodes. They can contains more than one child of the same type, for example, a list of bookmarks - `entity::Bookmarks`, or a list of user data - `GroupData` that contains user input such as strokes, photos, etc.

The **red** colored nodes are the visual scene entities: `entity::Canvas`, `entity::Stroke` and `entity::Photo`.

The **gray** colored nodes are the functional nodes that allow different types of control over the entities such as visibility and transformations.
