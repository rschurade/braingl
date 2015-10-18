# Surface connectivity and glyph tutorial #

The following is a step-by-step tutorial on how to use brainGL for visualization of surface connectivity and manual cortical parcellation using glyphs.

## Getting started ##

The sample data used in this tutorial can be downloaded for free by clicking the following link:

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

Using the Widgets menu (shown as "View" in the image below,) open _main gl 2_ and _dataset properties 2_. Main gl 2 provides a second view of the dataset for which selected visualization parameters can be adjusted in dataset properties 2. This allows for simultaneous visualization of different aspects of the data in the two views. In this case we will set up the two views so that main gl displays an inflated cortical surface with glyphs, and main gl 2 displays a seed-based connectivity map on the pial brain surface.

NOTE: The default background color for the main gl views is white. The background color of each view can be changed in _global properties_. See [list of GUI views](Views.md) for more information.

![http://wiki.braingl.googlecode.com/hg/files/3-open_second_view.png](http://wiki.braingl.googlecode.com/hg/files/3-open_second_view.png)

Switch off rendering of the volume slices using the lefthand axial, sagittal, and coronal slice icons in the toolbar. Click the righthand sagittal view button in the toolbar to center the main gl view on the lateral surface of the left hemisphere.

In dataset properties 1, switch the **left right visibility** to _left_. This means that only the left hemisphere will be displayed in the main gl view and is useful to reduce the memory load. Set the **color mode** to _per mesh_. This displays the brain surface as a solid color, making the glyphs more visible. The default mesh color is white but can be changed to any color using the _set color_ button. Set the **surface** to _inflated_ for a better view of the glyphs.

NOTE: The options displayed in _dataset properties_ change according to which dataset is selected in the _dataset list_. For this tutorial, make sure that 'avg.glyphset' is selected in the dataset list.

![http://wiki.braingl.googlecode.com/hg/files/4-adjust_view1.png](http://wiki.braingl.googlecode.com/hg/files/4-adjust_view1.png)

In dataset properties 2, adjust the **minimum** and **maximum** values of the colormap to increase contrast of the connectivity map. Ctrl + click the sagittal view button in the toolbar to center the view of main gl 2 on the lateral side of the left hemisphere. This view now displays an interactive seed-based connectivity map, which can be explored by right-clicking an any area of the cortical surface. The crosshairs are locked between the two views to ease navigation. Place the cursor over one of the two views and scroll up and down to zoom in and out.
![http://wiki.braingl.googlecode.com/hg/files/5-adjust_view2.png](http://wiki.braingl.googlecode.com/hg/files/5-adjust_view2.png)

Switch back to dataset properties 1, choose the tab **glyphs** (last tab) and switch the **glyph style** to _pies_. The pie glyphs display a summary of all connections from each node as a pie chart, with colors representing orientation and sorted by hue within the glyphs. Adjusting the glyph **scale** and **pie normalization** allows you to emphasize different features of the connectivity profile. The picture below shows pie glyphs adjusted to emphasize the strong medial-lateral connectivity of the somatomotor cortex, represented by the large size and red color of the glyphs around the central sulcus.
![http://wiki.braingl.googlecode.com/hg/files/6-pie_glyphsCS.png](http://wiki.braingl.googlecode.com/hg/files/6-pie_glyphsCS.png)

Place the crosshairs on the border of the large red area surrounding the central culcus by right-clicking on the surface in either view. Then switch the **glyph style** back to _points_ and zoom in to where you placed the crosshairs. The picture below shows the main gl view zoomed in on the point glyphs at the anterior border of the motor cortex. The border of the motor cortex is clearly distinguishable by observing the differences between the glyphs on either side of the main gl view. The glyphs on the posterior (righthand) side once again display a large amount of red around the central sulcus, representing the strong medial-lateral connectivity, while the glyphs toward the anterior (lefthand) side  instead show a large amount of green around the superior temporal and parietal areas, representing strong connectivity in the anterior-posterior orientation.
![http://wiki.braingl.googlecode.com/hg/files/7-point_glyphs_zoomCS.png](http://wiki.braingl.googlecode.com/hg/files/7-point_glyphs_zoomCS.png)

For descriptions of all available glyph styles and options, see the glyph section on the [SurfaceConnectivity](SurfaceConnectivity.md) page.

## Painting on a surface mesh ##

Click the _paint_ tab in dataset properties. Set the **paint mode** to _paint_ to switch on painting. Select a color to paint with by clicking the _set color_ button and adjust the size of the paintbrush using the **brush size** slider. Then paint an area by right-clicking on the brain surface while holding down the control key. To erase, use control-shift. Setting the paint mode to _paint values_ allows you to assign a value to each painted node by adjusting the **paint value** slider.
NOTE: The surface **color mode** must be set to _per vertex_ when using the paint function.
![http://wiki.braingl.googlecode.com/hg/files/8-paint_glyphsCS.png](http://wiki.braingl.googlecode.com/hg/files/8-paint_glyphsCS.png)

Surface meshes with painted regions can be saved in a number of different file types using the _save_ icon in the toolbar. Name and save your file by typing in the preferred extension. Available extensions are listed under _files of type_.
![http://wiki.braingl.googlecode.com/hg/files/9-save_painted_area.png](http://wiki.braingl.googlecode.com/hg/files/9-save_painted_area.png)

## Displaying anatomical background glyphs ##

Once you have painted an area on the surface mesh, click the _create little brains_ icon in the toolbar. This function renders miniature brain surfaces for the glyphs in the painted area, providing an anatomical background for easier interpretation of the connectivity information contained in the glyphs.
![http://wiki.braingl.googlecode.com/hg/files/10-create_little_brains.png](http://wiki.braingl.googlecode.com/hg/files/10-create_little_brains.png)

The anatomical background glyphs are rendered in whichever surface (pial, inflated, or sphere) is selected in **glyph geometry**. If you wish to change glyph geometry, click the _create little brains_ button again to update the anatomical background glyphs. The glyphs can be rotated in any direction by selecting the **glyph rotation enabled** option and adjusting the x, y, and z rotation sliders.
![http://wiki.braingl.googlecode.com/hg/files/11-little_brains_pial.png](http://wiki.braingl.googlecode.com/hg/files/11-little_brains_pial.png)

## Changing glyph color mode ##

Switch the **colormode for glyphs** to _value_ in the _glyph_ tab of dataset properties. The color scheme used in the glyphs now reflects the strength of connections from each node, rather than the orientation of the connections. The minimum and maximum values of the colormap can be adjusted in the _general_ tab as shown previously for main gl 2. Adjust the **points/vector primitive size** to improve visibility of individual points when zoomed in.
![http://wiki.braingl.googlecode.com/hg/files/12-value_colormode.png](http://wiki.braingl.googlecode.com/hg/files/12-value_colormode.png)

Switch the **colormode for glyphs** back to _orientation_ and then switch the **glyph style** to _vectors_. This combination of settings places the emphasis on the orientation and distribution of connections from each node. Switching the **glyph color** to _sphere_ increases the range of colors displayed, as the differences in spatial orientation are greatest when mapped onto a sphere.
![http://wiki.braingl.googlecode.com/hg/files/13-vector_glyphs_orientation.png](http://wiki.braingl.googlecode.com/hg/files/13-vector_glyphs_orientation.png)