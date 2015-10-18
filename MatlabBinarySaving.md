# Saving a connectivity matrix from Matlab in binary format #

The following instructions save a n x n square matrix M in a binary format (float32), which can be loaded from brainGL.

First, open a file for writing. Pick a filename, e.g. example\_connectivity.bin:

```
>> fileID = fopen('example_connectivity.bin','w');
```

Then, write the matrix M to that file, using the fileID from the previous command:

```
>> fwrite(fileID, M, 'float32')
```