This test harness was built with Visual Studio 2015 against Matlab 2009b libraries. It was tested with several versions of Matlab (2016a 64-bit, 2013b 32-bit, 2009b 32-bit).

It is meant to be used with the VS Visualizer sensors package. The mex file can be loaded and used to query color, depth, and surface normal data from an active sensor.

Source code for the mex file, along with libraries for the sensor DLL files, are included. If you want to build it yourself you can create a Visual Studio project including these files to output the mex file (just change the output from DLL to the appropriate mex file extension, mexw32 or mexw64).

NOTE: If building with 2009 Matlab libraries, you may need to modify Matlab's "matrix.h" to work properly. Older versons of Matlab had a definition of "CHAR16_T" that conflicts with the newer compiler.


Files of Note:
--------------

VsSharedBufferExample.m

    An example script file that uses the included mex file to sample points and generate a surface plot in Matlab.

VsSharedBufferMex.cpp

    C++ source code for the mex file.

VsSharedBufferMex.mexw64 / .mexw32

    Pre-built Mex files used for the VsSharedBufferExample script.