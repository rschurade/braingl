# Creating and bundling connections tutorial #

The following is a step-by-step tutorial on how to create connections and perform fiber bundling from connectivity data.

## Getting started ##

The sample data used in this tutorial is the same data that is used for the GlyphTutorial. The data can be downloaded for free by clicking the following link:

https://powerfolder.gwdg.de/dl/fiMbRrEALsSpPNmHebzXLszY/example_glyphset.zip

This dataset is the average connectivity of 65 brains from the enhanced Nathan Kline Institute-Rockland Sample (http://fcon_1000.projects.nitrc.org/indi/enhanced/).

Once you have downloaded the dataset, unzip the archive 'example\_glyphset.zip' and then open brainGL (See the [installation](Installation.md) page for instructions on how to download and install brainGL).

NOTE: The glyphset for this sample data has been assigned a minimum threshold of 0.5 in order to accomodate the memory and graphics capacity of the average workstation. Setting a lower minimum threshold in the glyphset file increases the amount of detail displayed in the glyphs but also drastically increases the amount of memory required to load the dataset. If you do wish to adjust the minimum threshold, simply open the textfile 'avg.glyphset' and replace 0.5 with a lower value in the third line.

![http://wiki.braingl.googlecode.com/hg/files/adjust_min-threshold.png](http://wiki.braingl.googlecode.com/hg/files/adjust_min-threshold.png)

## Viewing a glyphset ##

Open the anatomical volume nifti file 'orig2std.nii.gz', followed by the glyphset file 'avg.glyphset' by using file>open or clicking the folder icon in the toolbar. (See surface connectivity [data format](SurfaceConnectivity.md) for descriptions of these file types.) Both of these files should now be visible in the _dataset list_.
For this tutorial, the whole brain has been marked as a Region of Interest, and point glyphs have been created as described [here](SurfaceConnectivity#Loading_the_data_in_brainGL.md) in Section 4. This step must be completed before beginning this section of the tutorial.

![http://wiki.braingl.googlecode.com/hg/files/1-file_open.png](http://wiki.braingl.googlecode.com/hg/files/1-file_open.png)
![http://wiki.braingl.googlecode.com/hg/files/2-NKI_nifti&glyphset_loaded_new.png](http://wiki.braingl.googlecode.com/hg/files/2-NKI_nifti&glyphset_loaded_new.png)

**Views: One view, properties...**

NOTE: The default background color for the main gl views is white. The background color of each view can be changed in _global properties_. See [list of GUI views](Views.md) for more information.

Switch off rendering of the volume slices using the lefthand axial, sagittal, and coronal slice icons in the toolbar. Click the righthand sagittal view button in the toolbar to center the main gl view on the lateral surface of the left hemisphere.

Set the **color mode** to _per mesh_. This displays the brain surface as a solid color, making the glyphs more visible. The default mesh color is white but can be changed to any color using the _set color_ button.

NOTE: The options displayed in _dataset properties_ change according to which dataset is selected in the _dataset list_. For this tutorial, make sure that 'avg.glyphset' is selected in the dataset list.

## Creating connections ##

Manipulate the threshold and the minimum length to reduce the number of connections. To get results similar to our example application, make sure the threshold is set to 0.5. Set the minimum length to 130 mm by entering that value in the text field. The result should look like the following screenshot:

![http://wiki.braingl.googlecode.com/hg/files/glyphs_minlength130_a.png](http://wiki.braingl.googlecode.com/hg/files/glyphs_minlength130_a.png)

Change the alpha value in the glyphset property value to 0.2 to make the surface transparent. Press the "Make new connections" button; this button creates a new dataset containing only above-threshold connexels. The connexels are mainly inside the brain, and therefore only visible with a transparent surface.

![http://wiki.braingl.googlecode.com/hg/files/transparent_glyphs130mm_a.png](http://wiki.braingl.googlecode.com/hg/files/transparent_glyphs130mm_a.png)

## Bundling connections ##

![http://wiki.braingl.googlecode.com/hg/files/connections_a.png](http://wiki.braingl.googlecode.com/hg/files/connections_a.png)

A new connection dataset labelled as "new" should have appeared in the _dataset list_. With this dataset selected, press the "Bundle connections" button. In the following dialogue, press the "bundle" button without changing the default parameters.

![http://wiki.braingl.googlecode.com/hg/files/bundling_dialogue.png](http://wiki.braingl.googlecode.com/hg/files/bundling_dialogue.png)

The result are bundles of geometrically similar connections. These become visible by switching the original connections off in the _dataset list_.

![http://wiki.braingl.googlecode.com/hg/files/bundles_a.png](http://wiki.braingl.googlecode.com/hg/files/bundles_a.png)

The bundles are in the same format as the fiber datasets, and can be e.g. saved as .fib-files, or explored using ROIs.