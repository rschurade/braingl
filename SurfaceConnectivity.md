## Data structure ##

In order to show connectivity information on cortical surfaces, two datasets must be loaded into braingl in the following order:

  1. A NIFTI volume with the anatomical background.
  1. A glyphset file, containing references to surfaces and an associated connectivity matrix.

An example dataset is available here and can be used to complete the tutorials and get familiar with brainGL:

https://powerfolder.gwdg.de/dl/fiMbRrEALsSpPNmHebzXLszY/example_glyphset.zip

This dataset is the average connectivity of 65 brains from the enhanced Nathan Kline Institute-Rockland Sample (http://fcon_1000.projects.nitrc.org/indi/enhanced/).

Once you have downloaded the dataset, unzip the archive 'example\_glyphset.zip' and then open brainGL (See the [installation](Installation.md) page for instructions on how to download and install brainGL).

### Anatomical Volume file ###

The anatomical NIFTI volume (file extension .nii or .nii.gz) is used to establish the coordinate system. Note: The coordinates are voxel coordinates in this volume, which means that we do not take rotations in the transformation matrix of the volume into account.

### .glyphset Files ###

In order to load connectivity data in brainGL, you should first create a glyphset file, which is a text file with the extension .glyphset. It simply lists the names of the files that brainGL should load together, namely the anatomical volume, the surface reconstructions, and the connectivity matrix. It should have the following structure:

  * line1: Anatomical NIFTI volume
  * line2: Surface set(s) for one or both hemispheres (see below for how to create a .set file)
  * line3: Connectivity data

Optionally, minimum and maximum threshold values can be added at the end of line 3 of the glyphset file, in order to make loading of complex datasets possible with low memory configurations.

For example, a .glyphset file containing both hemispheres and whole-brain connectivity data with a minimum threshold of 0.5 would look like this:

```
All6.lh.fsaverage5.pial.asc 128 128 128
All6.lh.fsaverage5.inflated.asc 80 128 128
All6.lh.fsaverage5.sphere.asc 20 128 128
```


![http://wiki.braingl.googlecode.com/hg/files/adjust_min-threshold.png](http://wiki.braingl.googlecode.com/hg/files/adjust_min-threshold.png)

### .set Files ###

Similar to the glyphset file, the .set file is another text file listing a set of surface reconstructions that brainGL should load, and should be created prior to loading connectivity data. Each line in a .set file contains the filename of one surface representation and optionally a translation vector for a correction of shifts between coordinate systems.

A surface set consists of a set of cortical segmentation results from freesurfer, consisting of n nodes, and mesh information between these nodes. The surfaces must be in .asc format. Surfaces can be converted from freesurfer native format to .asc format with the command-line converter mris\_convert, included in freesurfer:

```
mris_convert lh.pial lh.pial.asc
```

One or two sets can be included (seperated by a blank), each representing one hemisphere. Each set can contain multiple representations of the surface (i.e. pial, inflated, spherical) for which the number of nodes has to match.

A .set file containing the pial, inflated, and spherical surface representations of the left hemisphere and coordinate shifts would look like this:

```
All6.lh.fsaverage5.pial.asc 128 128 128
All6.lh.fsaverage5.inflated.asc 80 128 128
All6.lh.fsaverage5.sphere.asc 20 128 128
```

### Connectivity matrix ###

The connectivity data file should be a binary file, containing n x n float values, which represent a square matrix of connectivity values between pairs of the n nodes of the surfaces.

A n x n square matrix M in a binary format (float32), can be saved from Matlab as follows:

First, open a file for writing. Pick a filename, e.g. example\_connectivity.bin:

```
>> fileID = fopen('example_connectivity.bin','w');
```

Then, write the matrix M to that file, using the fileID from the previous command:

```
>> fwrite(fileID, M, 'float32')
```


### Loading the data in brainGL ###

Load the NIFTI dataset and then the glyphset by clicking the _open_ button in the toolbar or selecting file -> open in the menu bar.

The image below shows braingl after loading of the anatomical volume and the associated connectivity glyph dataset:

![http://wiki.braingl.googlecode.com/hg/files/screen1.png](http://wiki.braingl.googlecode.com/hg/files/screen1.png)

The connectivity from one seed point, marked with a crosshair, is shown on the surface. To move the seedpoint to explore seed-based connectivity interactively, simply right-click any point on the surface.

In order to create connectivity glyphs, we first have to specify a region for which the glyphs are created. To interactively paint such a ROI, change to the paint-tab, switch paint mode to "paint", increase the brush size to around 100, and paint a region on the brain by simultaneus holding of the CTRL-key and drawing with a right mouse click (erasing during painting can be done using CTRL-SHIFT-right mouse):

![http://wiki.braingl.googlecode.com/hg/files/screen2_a2.png](http://wiki.braingl.googlecode.com/hg/files/screen2_a2.png)

Once the ROI is specified, create connectivity glyphs by changing to the glyphs-tab, switching the glyph mode from "off" to e.g. "points," and pressing the "create glyphs" button in the toolbar:

![http://wiki.braingl.googlecode.com/hg/files/screen3_a.png](http://wiki.braingl.googlecode.com/hg/files/screen3_a.png)

Connectivity glyphs for the selected region appear, and can be influenced with the properties described in the next section. An application of connectivity glyphs for the interactive parcellation of cortical areas is described in the GlyphTutorial.

## Dataset properties ##

Once the datasets are loaded, they appear in the dataset list window, and the options described below are available in the dataset properties window.

The dataset properties window is organized by tabs, namely [general](#General.md), [paint](#Paint.md), [special](#Special.md), [light](#Light.md), and [glyphs](#Glyphs.md). The most important for glyph visualization are the general and glyph tabs.

### General ###

![http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_general_new.png](http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_general_new.png)

**Name** shows the name of the dataset that is currently selected in dataset list, in this case a .glyphset file.

**Color Mode** gives four options to change the appearance of the cortical surface.

  * _per mesh_ provides one color for the whole surface, which can be chosen by pressing the _set color_ button.

  * _mri_ provides the color of the NIFTI data. This is especially useful for fmri datasets because functional data is shown on the surface.

  * _per vertex_ provides a white surface which is suitable for marking and painting regions on the surface. To mark regions the option _paint mode_ must be enabled in the **paint** tab. See below how to do this.

  * _vertex data_ displays data contained in the connectivity matrix as a colormap on the surface.

**Colormap** allows you to choose the color scheme for the displayed data when color mode is set to vertex data. Colormap options can be created and customized using the [colormap edit](Views.md) view. Default colormap options are _grey_, _rainbow_, _rainbow 2_, and _blue white red_.

**Min** lets you set the minimum color value for the selected colormap.

**Max** lets you set the maximum color value for the selected colormap.

**Lower threshold**

**Upper threshold**

**Color** sets the color of the displayed brain surface when the color mode is set to _per mesh_.

**Alpha** allows you to set an alpha value for the cortical surface. Lower values indicate more transparency.

**Threshold** determines the strength of the connections displayed. The lower the threshold, the more connections will be shown.

**Minimum length** sets the minimum length of the connections displayed. Setting a high minimum length will display only long-range connections.

**Surface visibility** toggles visibility of the brain surface.

**Glyph visibility** toggles visibility of the glyphs.

**Copy colors**

**Visibility of little brain glyphs** toggles visibility of the 'little brain' surfaces on the glyphs. This option is further described under [toolbar](#Toolbar.md).

**Surface** allows you to choose three different visualisations of the brain surface. The number of available options can vary depending on the dataset.

  * _Pial_ shows the pial brain surface, useful for observing gyral anatomy.

  * _Inflated_ shows an inflated brain surface, useful for viewing parts of the cortex contained inside sulci.

  * _Sphere_ transforms the brain surface into a sphere.


### Glyphs ###

![http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_glyphs.png](http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_glyphs.png)

**Glyph Style** allows you to choose between four different glyph styles: points, vectors, pies, and diffpoints.

  * _Points_ capture spatial information about connections by representing each connection from a node as one point, placed around the node to create a small spatial map of all connections.

  * _Vectors_ are similar to point glyphs, but additionally display a line between each point and the node itself, resulting in a vector representing all connections.

  * _Pies_ display a summary of all connections from a node as a pie chart, with colors sorted by hue. While the pie chart does not offer spatial information about the location of connections, the emphasis on color offers a broad overview of similarity and difference, as well as the distribution of connections in different directions.

  * _Diffpoints_ display a glyph at the midpoint of neighboring nodes that represents the absolute value of the difference between the two neighboring glyphs.

**Scale** sets the size of the glyphs. Higher values indicate bigger size.

**Pie Normalisation** inflates or deflates the relative sizes of the pie charts to display differences in the total number of connections form each node. Lower values indicate less size difference.

**Points/ vector primitive size** allows you to increase the size of the elements of point and vector glyphs (i.e. the points or lines making up the glyphs). Higher values indicate bigger size.

**Glyph rotation enabled** toggles the ability to rotate the glyphs using the x, y, and z glyph rotation sliders, which appear below when this option is enabled.

  * _glyph x rotation_ rotates the glyphs around the x axis
  * _glyph y rotation_ rotates the glyphs around the y axis
  * _glyph z rotation_ rotates the glyphs around the z axis

**Glyph alpha** allows you to set an alpha value for the glyphs. Lower values indicate more transparency in display primitives of lower strength connections.

**Colormode for glyphs** allows you to choose between two different color representations in the glyphs.

  * _orientation_ colors represent the directions of connections. Blue indicates dorsal-ventral connections, red indicates medial-lateral connections, and green indicates anterior-posterior connections.

  * _value_ colors indicate the strength of connections. The colormap used can be selected in the [general](#General.md) tab and adjusted in the [colormap](#Colormap.md) tab of the dataset properties window. To create a new customized colormap option, use the [colormap edit](Views.md) window, which can be found by clicking view->colormap edit in the menu bar.

**Glyph geometry** transforms the visualisation and the local distribution of the glyphs, onto one of three different surfaces (pial, inflated and sphere). This is independent of the actual visualisation of the surface and only affects the distribution of the glyphs. It is possible for e.g. to visualize a pial distribution of glyphs on a spherical surface.

**Glyph color** When colormode is set to orientation, the glyph colors reflect the directions of the connections, which can change depending on the shape of the surface. Glyph color allows you to set the directional colors according to the chosen surface (pial, inflated, or sphere). This can be useful for example to check if the direction of a connection is dependent on surface/gyral anatomy.


### Paint ###

The paint tab allows you to enable and customize options for painting on the loaded brain surface.

![http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_paint.png](http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_paint.png)

**Paint mode**
  * _off_ disables painting.
  * _paint_ allows you to paint one solid color on the surface.
  * _paint values_ allows you to paint gradients of colors by adjusting the paint value slider below.

**Brush size** adjusts the size of the paintbrush.

**Paint color** allows you to select a color to paint with.

**Paint value** allows you to adjust the gradient of the paint color to paint with weighting values.

To paint on the brain surface, adjust to the desired settings and then paint with the right mouse button while holding down the control key. You may find it helpful to switch off crosshair rendering in the general tab of the [global properties](Views.md) view. To use paint mode, color mode must be set to _per vertex_ in the general tab of the [dataset properties](Views.md) view.

### Light ###

![http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_light.png](http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_light.png)


## Toolbar ##

![http://wiki.braingl.googlecode.com/hg/files/Toolbar_glyphsets_new.png](http://wiki.braingl.googlecode.com/hg/files/Toolbar_glyphsets_new.png)

There are three sections in the toolbar, separated by dotted lines.
The sections are (seen from left to right) data options,  view options (see [Introduction to GUI](Introduction.md)) and [modification options](#ModificationOptions.md). Modification options are specific to the types of data loaded in BrainGL. The following options are relevant to surface connectivity.

### Modification Options ###

**Make new connections** The colored lines button creates connections from thresholded connectivity. Detailed description of this option is provided on the [connections page](Connections.md).

**Create little brains** allows you to display miniature brain surfaces on a selected region of glyphs. To use this option, [paint](#Paint.md) an area on the surface where you would like to display the little brains and then click the _create little brains_ button. If you then change any parameters relating to surfaces (e.g. glyph geometry or large brain surface), click the _create little brains_ button again to match them to the selected parameters.