---
layout: page
title: "User manual"
description: Canvas functionality description.
bg: orange  #defined in _config.yml, can use html color like '#0fbfcf'
color: black   #text color
fa-icon: user
---

# Canvas

## Defining new canvases
{: .left}

Canvases are the 3D planes that users can sketch on. To define a new canvas, use one of the tools in the "New Canvas" tool:

![1.1]({{ site.baseurl }}/assets/images/user_guide/1.1 ground plane.PNG "New Canvas menu")
{: .align-center}

The four basic definiton tools "Ground plane", "Left plane", "Right plane", and "Front plane" simply define a new plane matching the given description. For example, defining a "Ground plane" will create a plane corresponding to the ground plane, or XY plane:

![1.11]({{ site.baseurl }}/assets/images/user_guide/1.1 ground plane creation.gif "Ground plane")
{: .align-center}

Similarly, defining a "Left plane" will define a plane on the left:

![1.2]({{ site.baseurl }}/assets/images/user_guide/1.2 left plane.PNG "Left plane")
{: .align-center}

In addition, there are several more advanced tools for defining a new plane. For instance, the "Clone Current" tool will create a copy of the current plane you are working with:

![1.3]({{ site.baseurl }}/assets/images/user_guide/1.3 clone current.gif "Clone Current")
{: .align-center}

Finally, you can use the "New Canvas Set" tool to create a "Standard" set of three canvases consisting of one Ground Plane, one Left Plane, and one Front Plane:

![1.4]({{ site.baseurl }}/assets/images/user_guide/1.4 new canvas set.PNG "New Canvas Set")
{: .align-center}

![1.41]({{ site.baseurl }}/assets/images/user_guide/1.41 new canvas setted.PNG "New Canvas Setted")
{: .align-center}


## Interacting with canvases
{: .left}

Once you have defined canvases, there are several tools that you can use to interact with your canvases. 

First of all, it is important to note that you only work with one canvas at a time. To change the canvas that you are currently working on, you can use the "Select canvas" tool. For instance, if we wanted to sketch on one canvas and then another, we would have to use the "Select canvas" tool to change the current canvas:

![1.5]({{ site.baseurl }}/assets/images/user_guide/1.5 change canvas.gif "Change Canvas")
{: .align-center}

In the above example, we first sketched a circle on Canvas 2, and then changed canvases to Canvas 1, where we sketched an X. Also, as mentioned earlier, another way to navigate between canvasses is to use the canvas widget on the right side of the display. 

Furthermore, the "Edit canvas location" tool allows you change the the 3D location of your canvas. You can use the tool to rotate and offset the current canvas:

![1.6]({{ site.baseurl }}/assets/images/user_guide/1.6 edit location.gif "Edit canvas location")
{: .align-center}