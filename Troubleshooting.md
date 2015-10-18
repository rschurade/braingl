# Troubleshooting #

## Program starts, loaded datasets appear in dataset browser but nothing is visible in the main view ##

Start braingl with -v (verbose)

The output should be similar to this:
```
12:14:05:439 [D] OpenGL initialized. 
12:14:05:439 [D] GL Vendor : "NVIDIA Corporation" 
12:14:05:439 [D] GL Renderer : "GeForce GTX 295/PCIe/SSE2" 
12:14:05:439 [D] GL Version (string) : "3.3.0 NVIDIA 304.88" 
12:14:05:439 [D] GL Version (integer) : 3 . 3 
12:14:05:439 [D] GLSL Version : "3.30 NVIDIA via Cg compiler"
```

Most important is this line

```
12:14:05:439 [D] GL Version (integer) : 3 . 3 
```

The version needs to be 3.3 or higher.

Also, if you are on Linux and have a nVidia graphics card, make sure you have the latest nVidia drivers installed. Some Linux distributions don't install these automatically because they prefer open source drivers, which are neither tested with braingl nor supported.

## It's not working on Mac ##

There is a known problem with Qt 4.x and OpenGL. Due to a bug it currently only supports OpenGL 2.1 We hope that with OS updates this will be fixed any time soon. Other than that, no solution is known yet.