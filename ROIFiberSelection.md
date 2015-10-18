# ROIs for fiber / bundled connections selection #

BrainGL provides two views for working with spatial regions of interest to select fibers / bundled connections: The ROI view shows, like the _dataset list_, all currently present ROIs. The ROI properties view makes it possible to influence the ROI which is currently selected in the ROI view.

## ROI view ##

![http://wiki.braingl.googlecode.com/hg/files/rois_view.png](http://wiki.braingl.googlecode.com/hg/files/rois_view.png)

The roi view lists all the ROIs, makes it possible to select one in order to manipulate its parameters, and allows to arrange the ROIs in tree-like fashion.

The following screenshot shows two independent ROIs, which select a subset of a full-brain fiber tracking dataset. The ROIs have been created with the toolbar button circled in red, and manually placed with a right-button mouse drag in the main window:

![http://wiki.braingl.googlecode.com/hg/files/rois1.png](http://wiki.braingl.googlecode.com/hg/files/rois1.png)

Dragging the second ROI onto the first, the two ROIs are logically linked. The result is that only fibers which intersect both ROIs are visible:

![http://wiki.braingl.googlecode.com/hg/files/rois2.png](http://wiki.braingl.googlecode.com/hg/files/rois2.png)

## ROI properties view ##

The ROI properties view allows to influence the parameters of the ROI currently selected in the ROI view.

### General ###

![http://wiki.braingl.googlecode.com/hg/files/rois_properties.png](http://wiki.braingl.googlecode.com/hg/files/rois_properties.png)

**name** allows to name the selected ROI

**active** allows to toggle whether the selected ROI has an effect on the selection of fibers or not

**render** toggles the visibility of the selected ROI in the scene without affecting its impact on fiber selection

**negate** toggles whether the selected ROI selects / deselects all intersecting fibers, or all fibers NOT intersecting the ROI

**shape** allows to choose the shape of the selected ROI
  * ellipsoid with semi-principal axes dx, dy, and dz along the respective directions
  * sphere with radius dx
  * cube with edge length dx
  * box with edge lengths dx, dy, and dz along the respective directions

**x,y,z** spatial position of selected ROI, denoting the center of the shape

**dx,dy,dz** extent of the selected ROI (see above)

**color** color of the selected ROI

**alpha** allows to influence the transparency of the fibers from 0 (invisible) to 1 (opaque)

**stick to crosshair** if this checkbox is enabled, the position of the selected ROI is linked to the intersection point of the anatomical slices

### Description ###

![http://wiki.braingl.googlecode.com/hg/files/rois_properties_description.png](http://wiki.braingl.googlecode.com/hg/files/rois_properties_description.png)

This tab allows to enter a textual description for each ROI; this way, ROIs can be used as labels and annotations.