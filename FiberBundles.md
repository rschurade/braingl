# Fiber Bundles #

BrainGL can load, save, and visualize results of fiber tracking algorithms, as well as connectivity bundling, as a fiber bundle dataset. ROIs allow for the [interactive selection of subsets of these datasets](ROIFiberSelection.md).

## Data formats ##

We support the vtk format for import of fiber and functional connectivity bundles. The file endings associated with these datasets are .vtk or .fib. The latter are binary .vtk files which are used by several programs for the analysis of fiber tracking results.

The image below shows braingl after loading of an anatomical volume, a set of surfaces, and an associated set of fiber bundles in .fib format:

![http://wiki.braingl.googlecode.com/hg/files/braingl_with_fibers.png](http://wiki.braingl.googlecode.com/hg/files/braingl_with_fibers.png)

## Dataset Properties ##

### General ###

![http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_general.png](http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_general.png)

**Name** shows the name of the datset that is currently selected in the dataset list, in this case a .fib file.

**Fiber render mode** allows you to choose between two different renderings of the fiber bundles.
  * _lines_ renders the fibers as lines
  * _tubes_ renders the fibers as tubes

**Color mode** allows you to choose between four different coloring options for the fiber bundles.
  * _global_
  * _local_
  * _user-defined_ allows you to choose a solid color using the color tab described below
  * _mri_ sets the color of the fibers according to the color scheme of the loaded anatomical volume

**Fiber thickness** allows you to change the thickness of the displayed fibers

### Color ###

![http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_color.png](http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_color.png)

**Color** allows you to choose a color to display the fibers with

**Alpha** allows you to change the transparency of the fibers

### Special ###

![http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_special.png](http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_special.png)

The properties on this tab control the clipping of the rendering: The visibility of the data is spatially restricted to a clipping box. Only parts of the fibers inside that box are drawn, parts outside of the clipping box are invisible:

**dx, dy, dz** allows you to change the size of the clipping box along the three principal axes

**nx, ny, nz** allows you to change the position of the clipping box

It is also possible to show only a fractional part of each individual fiber. The fibers are drawn from their beginning to a point along their length, parts beyond that point are invisible:

**Fiber growth** allows you to influence the relative position of the clipping point along each fiber. A value of 0 means that the whole fiber is invisible, while larger values denote the position of the clipping point along the fibers. For values larger than the length of the fibers, the whole fibers are drawn.

### Light ###

![http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_light.png](http://wiki.braingl.googlecode.com/hg/files/fibDatasetProperties_light.png)

**Toggle light** allows you to enable or disable whether the fibers are shaded using illumination, or simply rendered with a constant flat color. The illumination model is following [Phong](http://en.wikipedia.org/wiki/Phong_reflection_model):

**Light ambient, Light diffuse, Light specular** influence the parameters of the virtual light source

**Material ambient, Material diffuse, Material specular, Material shininess** influence the parameters of the material