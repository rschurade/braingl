# Connections / Connexels #

In order to show connexels, two datasets must be loaded into braingl in the following order:

  1. A NIFTI volume with an anatomical background.
  1. A connexel file, containing the connection information between pairs of points

## Creation of Connexels ##

Connexels are connections between pairs of 3D positions with an associated connection strength. There are three ways to create such data in brainGL:

### .CXLS-Files ###

The easiest way to import connexels, is to load a plain text file with the ending .cxls.

Each line has to contain a tupel (px py pz qx qy qz c) of seven values, separated by whitespace. px, py and pz are the three-dimensional coordinates of one termination point P of a connection between P and Q; qx, qy and qz the coordinates of the other termination point Q, and c is the connection strength between the two points. An example of a file containing three connexels looks as follows:

```
125.3 12.0 31.1 145.2 34.3 25.6 0.61
146.1 25.9 54.2 135.3 24.4 25.2 0.12
156.2 32.8 22.7 154.3 34.5 45.5 0.76
```

### .CONS-Files ###

A binary graph can be loaded from a binary vtk file which contains coordinates and one POLYLINE with a length of 2 for each connection. The files have to have the ending .cons in order for braingl to recognize them as connections.

### Correction of translation ###

The coordinates used for geometrical objects are interpreted as voxel coordinates in the first loaded volume file. If this is not the case, they may appear shifted in relation to the anatomy (see screenshot).

![http://wiki.braingl.googlecode.com/hg/transformation.png](http://wiki.braingl.googlecode.com/hg/transformation.png)

It is possible to correct such a shift in brainGL through the application of an arbitrary transformation matrix in the transformation tab of the dataset properties. If the anatomical background volume contains transformations, often the application of the inverse of these transformations transformes geometrical objects to voxel coordinates (i.e., applying the transformation with "qform inverted" selected). It is also possible to input an arbitrary user defined transformation.

### Creation from Surface connectivity data ###

When [connectivity data](SurfaceConnectivity.md) is loaded in brainGL, the button marked in red below appears in the toolbar.

![http://wiki.braingl.googlecode.com/hg/files/toolbar_connection_button.png](http://wiki.braingl.googlecode.com/hg/files/toolbar_connection_button.png)

Clicking this button creates and displays the connexels resulting from the current threshold and minimum length values.

## Tools for Connexels ##

After creating the connexels, two buttons are available in the toolbar (marked in red below):

### Edge Bundling ###

The bundling button performs edge-bundling of the connections, resulting in a new [dataset of fiber bundles](FiberBundles.md). An application example of this technique is described in the [Edge bundling tutorial](ConnectionBundling.md)

![http://wiki.braingl.googlecode.com/hg/files/toolbar_bundling_button.png](http://wiki.braingl.googlecode.com/hg/files/toolbar_bundling_button.png)

### Create Glyphset ###

A second button summarizes the connexels into summary representations of the connectivity of each terminations point. The resulting [glyph set](SurfaceConnectivity.md) can be displayed independently from any surface representation, but using the same properties described [here](SurfaceConnectivity.md).