# Volume Data #

It is always necessary to load an anatomical volume dataset as a first step, since braingl uses the first loaded volume to determine its coordinate system.

## File Format ##

The supported file format is NIFTI, with the endings .nii or .nii.gz.

## Dataset Properties ##

When an anatomical NIFTI volume is selected in the [dataset list](Views.md), the following options become available in the _general_ tab of the [dataset properties](Views.md) window. Manipulating these options affects the visualization of the volume slices in the [main gl](Views.md) view as well as the axial, coronal, and sagittal [navigation](Views.md) view.

![http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_general_nifti.png](http://wiki.braingl.googlecode.com/hg/files/DatasetProperties_general_nifti.png)

**Interpolation** smoothens the visualisation of the NIFTI-levels.

**Alpha** allows you to set an alpha-value for the NIFTI-levels. Lower values indicate more transparency.

**Min** changes the contrast of the representation of the brain and the surrounding area. Higher values indicate less contrast and result in a complete grey area.

**Max** changes the contrast by  brightening white areas. Lower values indicate more brightness in whiter areas.

**Lower Threshold** changes the visibility of darker areas. Higher values indicate less visualisation of darker areas.

**Upper Treshold** changes the visibility of brighter areas. Lower values indicate less visualisation of brighter areas.

**Colormap** allows you to select a colormap for the volume data. You can create and customize colormaps using the [colormap edit](Views.md) view.