## Painting on surfaces and in volumes ##

To enable painting, an additional tab "paint" (see below) exists in the respective dataset properties. Clicking on that tab makes it possible to enable painting by setting paint mode to paint or erase. Erasing is equivalent to painting with white.

![http://wiki.braingl.googlecode.com/hg/files/paintmode150.jpg](http://wiki.braingl.googlecode.com/hg/files/paintmode150.jpg)

For the painting, a brush size and color can be chosen. The brush size is in mm (surface painting) or voxels (volumes). When clicking a point on a mesh surface or on the triplanar slices through a volume, the vertex / voxel closest to the selected point together with all its neighbors within the given radius are painted with the current color.