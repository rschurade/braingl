# Prerequisites #

Braingl requires a computer with Linux or Windows and uses the graphics hardware extensively for interactive display. The graphics system has to support at least OpenGL 3.3, which means that low end graphics hardware, older graphics boards, or outdated drivers will lead to the software not displaying anything at all, or the interaction being very slow ([Troubleshooting](Troubleshooting.md)).

For complex datasets, sufficient main memory as well as graphics card memory are required. We recommend a computer with at least 8 GB of RAM and 1 GB of graphics memory; although the example datasets also run on machines with less memory.

# Windows #

For Windows, precompiled binaries are available in the [downloads section](https://drive.google.com/folderview?id=0Bxsr7AsjvzXoR3hFVTFaaUVBUkE&usp=sharing&tid=0Bxsr7AsjvzXoUGFPeTBOLUxZNDA).

# Mac #

For Mac, precompiled binaries are available in the [downloads section](https://drive.google.com/folderview?id=0Bxsr7AsjvzXoZWoyWWNXSVlscm8&usp=sharing&tid=0Bxsr7AsjvzXoUGFPeTBOLUxZNDA).


# Linux #

For Linux systems, we offer an experimental [Debian package](http://wiki.braingl.googlecode.com/hg/files/braingl_0.0-1_amd64.deb), which can be installed using the standard package installation mechanisms (for many operating systems, a double click on the .deb-file will start installation). If that is not possible, or you need the latest version of brainGL, it is necessary to compile the software from the source code, as described in the following section.

# Compiling from source #

## Cloning the source repository ##

Get a local copy of the braingl repository by executing the following command in your terminal (if the _hg_ command is not available on your computer this will return an error message and instructions how to install it):

_hg clone https://code.google.com/p/braingl/_

This will create a directory called braingl.

Change into this directory:

_cd braingl_

Then create a directory called build:

_mkdir build_

## Building braingl ##

Change into that directory:

_cd build_

Execute the following command, which should configure everything for building the software (as for the _hg_ command you might receive instructions how to install _cmake_ first):

_cmake ../src/_

Check carefully if any error messages occur regarding missing libraries: BrainGL requires several external libraries to be installed before compilation is possible. These external dependencies are: boost, Qt4, vtk and GLEW. If you find error messages in the output from cmake, these required libraries probably are not installed on the computer. The libraries can be installed from the terminal using a package manager like apt-get or e.g. the software center GUI in ubuntu. You should search for the following libraries:

libboost

libglew

libqt4

libvtk5

Make sure you install the libraries themselves AND the development files for the source code, i.e. choose a package ending on -dev. After installing all required packages check whether you are still in the correct directory (_pwd_ should return  _../braingl/build_), otherwise return to the correct directory (_cd braingl/build_) and retry the cmake command above.

If everything is configured correctly (cmake finishes without error messages, and creates a makefile in the build directory), execute the following command:

_make_

This will compile the software, and result in the executable braingl, which can be started from the braingl/build directory by clicking on the braingl symbol or from the terminal as follows:

_./braingl_

## Updating ##

In the folder braingl, execute the following to get the latest updates to the source code:

_hg pull_

_hg update_

After that, you have to build braingl again (see above).