# Data import and display #

The types of datasets that braingl can display are listed below. Click on the relevant datatype for a description of the file formats and the associated display properties and tools available for that specific kind of data.

brainGL determines the type of dataset strictly by file ending. The necessary file endings are listed behind the data type.

It is always necessary to load a volume dataset first, because brainGL uses the first loaded volume to determine the coordinate system for the display of other data types:

  * [Anatomical volumes](VolumeData.md): .nii, .nii.gz

Additional data types which brainGL can display are:

  * [Connections](Connections.md): .cxls, .cons
  * [Surface connectivity and glyphs](SurfaceConnectivity.md): .glyphset
  * [Fiber/Connection Bundles](FiberBundles.md): .fib, .vtk