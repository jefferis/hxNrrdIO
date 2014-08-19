## Intro ##
This is a simple library to allow the Amira 3D visualisation software (http://www.vsg3d.com/amira/overview) to read data in the NRRD (Nearly Raw Raster Data) format (http://teem.sourceforge.net/nrrd/format.html).

For installation instructions, see below.

## Limitations ##
  * VERY LIMITED TESTING SO FAR!
  * Currently only reads 3D (scalar) and 4D (vector) fields
  * Only writes 3D data

## History ##
  * v0.2 - Mac OS X 10.9 (clang) compatible release
  * v0.1 - first release

## Installation ##
For installation on non-Mac-OS-X-10.9 systems (i.e. using gcc rather than clang), see INSTALL file.

### Installing built files ###
Copy the library files (``.dylib``) and the resource files (``.rc``) into:

  /Applications/Amira-5.6/lib/arch-MacX-Optimize
  /Applications/Amira-5.6/share/resources

respectively.

### Building from source ###
Although everything should work on Linux or Windows, my only experience is with Mac.

#### Mac Setup ####
  * AmiraDev (Tested with 5.6)
  * Apple's Xcode 5 (http://developer.apple.com/xcode/)
  * Cmake >= 2.4 (http://www.cmake.org)
  * teem >= 1.10.0 (http://teem.sourceforge.net/)

#### Build teem ####
1. Follow teem's cmake installation build instructions. Basically:

  ```
  mkdir build
  cd build
  ccmake .. # to configure, see 2 below
  make
  ```
2. Make sure you build shared libraries (you can choose to in ccmake).
3. Install into ``/usr/local/teem`` (the default).

#### Build hxNrrdIO ####
1. Make an Amira local directory using the Development Wizard.
2. Use git to clone the ``hxNrrdIO`` repository into the ``src`` subdirectory (or extract a tar ball).
3. Make the build files using the Development Wizard in Amira.
4. Edit the ``src/hxNrrdIO/GNUmakefile`` produced such that:
  ```
  HX_SDKROOT = /Developer/SDKs/MacOSX10.6.sdk
  ```
  points to the 10.8 SDK (perhaps via a symlink to ``/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/``), giving a line such as:
  ```
  HX_SDKROOT = /Developer/SDKs/MacOSX10.8.sdk
  ```

5. Edit the file further such that:
  ```
  ifdef HX_COMPILER_gcc42
      LDFLAGS += -fopenmp
      HXFLAGS += -fopenmp
  endif
  ```
becomes:
  ```
  ifdef HX_COMPILER_gcc42
      LDFLAGS += -openmp
      HXFLAGS += -openmp
  endif
  ```
6. Edit ``/Applications/Amira-5.6.0/include/mclib/McMath.h`` and replace all occurances of ``_mm_cvtss_si64x`` with ``_mm_cvtss_si64`` and all occurances of ``_mm_cvttsd_si64x`` with ``_mm_cvttsd_si64``.
7. Go back to your top-level directory and build with:

  ```
  make MAKE_CFG=Optimize
  ```
  It is likely you will receive some errors regarding building documentation, but by this point the necessary ``.dylib`` and ``.rc`` files have been created in ``lib/arch-MacX-optimize`` and ``share/resources``, respectively.
8. Restart Amira to test.

## License ##
  * LGPL 2.1 (see LICENSE file)
  * Note that as of 20091123 Torsten Rohlfing has explicitly waived the GPLv3 license on the CMTK code that I altered.

## Author ##
(c) Gregory Jefferis (jefferis@gmail.com) 2009

Some code fragments were adapted from the open source CMTK library at:
  http://www.nitrc.org/projects/cmtk/
written by Torsten Rohlfing and colleagues and released under GPL >=3.
