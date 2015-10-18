# Introduction #

After [installation and execution](Installation.md) of braingl, the user interface looks like this:

![http://wiki.braingl.googlecode.com/hg/files/main500.jpg](http://wiki.braingl.googlecode.com/hg/files/main500.jpg)

The user interface is split into several windows, the menu bar, and a [toolbar](#Toolbar.md).

**Dataset list** shows all the datasets that are currently loaded. Braingl determines what the data contains strictly by file extension; it can therefore be necessary to rename datasets accordingly. Toggling the checkbox in front of each dataset name switches the visibility of the respective dataset. Selecting a dataset in this list switches the displayed properties in the dataset properties view to the selected datasets' options. Further information about supported datasets and their file formats is described on the [Types of displayable data](DataStructure.md) page.

**Dataset Properties** contains dataset-specific viewing options.

**[Global Properties](Views.md)** contains display options for all datasets (on the picture above, these options are hidden behind the dataset properties, but accessible with the rider on the bottom).

**ROIList and ROIProperties** make interaction with regions of interest possible.

**Main gl** is where the loaded datasets are visualized.


Visibility of these windows as well as additional viewing options can be toggled using the view option in the menu bar.

![http://wiki.braingl.googlecode.com/hg/files/View_menu.png](http://wiki.braingl.googlecode.com/hg/files/View_menu.png)

The views can be arranged flexibly in the main window, stacked, and even moved to a separate independent window by dragging the title bars around. See the [List of GUI views](Views.md) page for detailed information on each option.


# Toolbar #

![http://wiki.braingl.googlecode.com/hg/files/Toolbar_small.png](http://wiki.braingl.googlecode.com/hg/files/Toolbar_small.png)

There are three sections in the toolbar, separated by dotted lines. The first two sections, [Data Options](#DataOptions.md) and [View Options](#ViewOptions.md), are described below. The third section changes depending on the type of dataset loaded and specific options are described on the relevant dataset pages.

### Data Options ###

It is possible to create, load and save a variety of different file types in brainGL. Each of these options are available from the first three buttons in the toolbar.

**New dataset** creates a new dataset.

**Load dataset** allows you to open existing datasets. This option can also be accessed by choosing file->open in the menu bar.

**Save dataset** allows you to save open datasets. This option can also be accessed by choosing file->save in the menu bar.

**Continous rendering** toggles the option to continuously render the graphics. Enabling this option may slow down your computer.


### View Options ###

The first three brain symbols toggle the visibility of the axial, coronal and saggital slices from the loaded anatomical data in the main gl window.

**Show axial slice** toggles visibility of the axial slice.

**Show coronal slice** toggles visibility of the coronal slice.

**Show sagittal slice** toggles visibility of the sagittal slice.


The next three brain symbols center the view on the axial, coronal or saggital slice position.

**Axial view** centers the view on the axial plane. By default the view is from a dorsal perspective (i.e. from above). Shift-click this button for an axial view from the ventral perspective (i.e. from below).

**Coronal view** centers the view on the coronal plane. By default the view is from the posterior perspective (i.e. from the back). Shift-click this button for a coronal view from the anterior perspective (i.e. from the front)

**Sagittal view** centers the view on the sagittal plane. By default this view is from left to right (i.e. centers on the lateral side of the left hemisphere). Shift-click this button for a view from right to left.

To use these buttons for [main gl 2](Views.md), use control-click or shift-control-click.


**New ROI** The blue cube button creates a new ROI.




# Menu bar #


### File menu ###
**Open...** allows you to open existing datasets. This option can also be accessed by pressing the _Load dataset_ toolbar button\_Ctrl+O_on the keyboard_

**Save...** allows you to save open datasets. This option can also be accessed by pressing the _Save dataset_ toolbar button or _Ctrl+S_ on the keyboard

**Save Scene** allows you to save the current braingl session including all settings. When restarting the scence, braingl will try to reload all datasets and settings.

**1**, **2** are shortcuts to load recently used datasets.

**Quit** will finish braingl. This can also be achieved by pressing _Ctrl+Q_ on the keyboard.


### Options menu ###
**Lock Widgets** will hide the widgets of all currently visible GUI windows

**Render crosshair** toggles the visibility of the crosshair in both gls. This can also be achieved by the _Render crosshair_ checkbox in Global Properties.


### View menu ###
This menu allows you to toggle the visibilty of the different components in your GUI. See [of GUI views](List.md) for detailed information.


### Help menu ###
**Screenshot** allows you to save a screenshot of your current view. Adjust the screenshot properties and storage path in the _Screenshot_ tab in Global Properties.

**Reset Settings** will close braingl. Upon restart all settings and paths will be set to the default values.