Options available in the view menu:
  * [Dataset List](#Dataset_List.md)
  * [Colormap Edit](#Colormap_Edit.md)
  * [ROIs](#ROIs.md)
  * [ROI Properties](#ROI_Properties.md)
  * [Global properties](#Global_Properties.md)
  * [Dataset Info](#Dataset_Info.md)
  * [Script](#Script.md)
  * [Slice navigation Views](#Slice_Navigation_Views.md)
  * [Main GL](#Main_GL.md)
  * [Dataset Properties](#Dataset_Properties.md)
  * [Main GL 2](#Main_GL_2.md)
  * [Dataset Properties 2](#Dataset_Properties_2.md)

![http://wiki.braingl.googlecode.com/hg/files/View_menu.png](http://wiki.braingl.googlecode.com/hg/files/View_menu.png)

# Dataset List #

Lists all the datasets that are currently loaded. The checkbox in front of each dataset name toggles the visibility of the respective dataset (in MainGL 1, see below how to handle data in [GL 2](Main.md)) Selecting a dataset in this list switches the displayed properties in the [dataset properties](#Dataset_Properties.md) view to the selected datasets' options. Further information about supported datasets and their file formats is described on the [types of displayable data](DataStructure.md) page.

# Colormap Edit #

![http://wiki.braingl.googlecode.com/hg/files/ColormapEdit.png](http://wiki.braingl.googlecode.com/hg/files/ColormapEdit.png)

The colormap edit window allows you to create or update customized colormap options. Pick a color scale, set the desired parameters, then name your colormap and click _save new_ or _update_.

# ROIs #

The ROI view lists any created regions of interest. This option is mostly relevant for display of diffusion imaging tractography data. See [ROIFiberSelection](ROIFiberSelection.md) for a description of how to use ROIs for the selection of fibers / bundled connections.

# ROI Properties #

The ROI properties allows manipulation of parameters concerning created regions of interest. This option is mostly relevant for display of diffusion imaging tractography data. See [ROIFiberSelection](ROIFiberSelection.md) for a description of how to use ROIs for the selection of fibers / bundled connections.

# Global Properties #

Global properties contains visualization options that are independent of the type of data loaded and is divided into several tabs containing different options.

## General ##

![http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_general.png](http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_general.png)

**Camera type** selects either arcball interaction (virtual trackball, left mouse drag rotates the brain around its center), or camera-like interaction (a,s,d,w and the cursor keys move the viewer through the space)

**Camera projection** enables orthogonal or perspective projection

**Global sagittal** determines the position of the sagittal slice.

**Global coronal** determines the position of the coronal slice.

**Global axial** determines the position of the axial slice.

**Background color main** allows you to change the background color of the main gl window.

**Background color main 2** allows you to change the background color of the main gl 2 window.

**Background color combined** allows you to change the background color of the combined navigation window (to toggle visibility of this window, click view->combined nav in the menu bar).

**Background color nav1** allows you to change the background color of the axial navigation window (to toggle visibility of this window, click view->axial in the menu bar).

**Background color nav2** allows you to change the background color of the sagittal navigation window (to toggle visibility of this window, click view->sagittal in the menu bar).

**Background color nav3** allows you to change the background color of the coronal navigation window (to toggle visibility of this window, click view->coronal in the menu bar).

**Render crosshairs** toggles visibility of the crosshairs, and the threedimensional selection marker for surface datasets.

**Crosshair color** allows you to change the color of the crosshairs.

**Show nav sliders** toggles visibility of slice sliders in the navigation windows.

**Transparency mode** allows you to choose between _new_ and _classic_.

## Camera ##

![http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_camera.png](http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_camera.png)

The camera tab allows you to adjust the camera's view. The view can also be adjusted by simply dragging around the image in the main GL window (left-click-drag to rotate the view, scroll to zoom in and out) or through the keyboard (a,s,d,w and the cursor keys move the viewer through the space)

## Arcball ##

![http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_arcball.png](http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_arcball.png)

## Screenshot ##

![http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_screenshot.png](http://wiki.braingl.googlecode.com/hg/files/GlobalProperties_screenshot.png)

The screenshot tab allows you to customize options for taking screenshots in BrainGL.

# Dataset Info #

![http://wiki.braingl.googlecode.com/hg/files/dataset_info.png](http://wiki.braingl.googlecode.com/hg/files/dataset_info.png)

The datset info view lists useful information about the loaded datasets.

# Script #

# Slice Navigation Views #

## Axial, Sagittal, and Coronal views ##

![http://wiki.braingl.googlecode.com/hg/files/nav123.png](http://wiki.braingl.googlecode.com/hg/files/nav123.png)

Displays the axial, sagittal, or coronal views of the loaded anatomical volume and allows navigation through slices using a slider or crosshairs.

## Combined Nav ##

![http://wiki.braingl.googlecode.com/hg/files/CombinedNav.png](http://wiki.braingl.googlecode.com/hg/files/CombinedNav.png)

Simultaneously displays the axial, sagittal, and coronal views of the loaded anatomical volume and allows navigation through slices using crosshairs.

# Main GL #

The main GL view is the main window where the loaded datasets are visualized. See the relevant pages describing the [types of displayable data](DataStructure.md) for examples of visualizations.

# Dataset Properties #

The dataset properties view contains dataset-specific viewing options that vary depending on the types of data currently loaded in brainGL. See the relevant pages describing the [types of displayable data](DataStructure.md) for descriptions of specific options.

# Main GL 2 #

The main GL 2 view is a second visualization window, which allows simultaneous display of two different views of the same dataset. When main GL 2 is in use, the visualization parameters for this view can be customized in the dataset properties 2 view described below.

NOTE that toggling checkboxes in the dataset list only affects main GL 1. To change visibility of different datasets in main GL 2, select the respective dataset in the dataset list and uncheck the _active_ checkbox in the dataset properties 2 general tab.

# Dataset Properties 2 #

The dataset properties 2 window allows manipulation of a selections of visualization parameters for main GL 2 (described above). Some of the parameters available in dataset properties are locked between the two views and are therefore not available in datset properties 2. Changing these locked parameters will affect the visualizations presented in both main GL and main GL 2. Locked parameters vary depending on the type of data loaded.