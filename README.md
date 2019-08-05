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

  /Applications/Amira-XXX/lib/arch-MacX-Optimize
  /Applications/Amira-XXX/share/resources

respectively.

### Building from source ###
Although everything should work on Linux or Windows, my only experience is with Mac.

#### Mac Setup ####
  * MacOSX 10.12
  * AmiraDev (Tested with 6.7)
  * Apple's Xcode 9.2 (http://developer.apple.com/xcode/)
  * teem >= 1.11.0 (homebrew)

#### Build teem ####
1. `brew install teem`

#### Build hxNrrdIO ####
1. Make an Amira local directory using the Development Wizard.
2. Use git to clone the ``hxNrrdIO`` repository into the ``src`` subdirectory (or extract a tar ball).
3. Edit ``/Applications/Amira-6.7.0/share/devtools/helpers.tcl`` to change line
   ```
   defOption "macsdk" {10.11 10.12} \
   # to 
   defOption "macsdk" {10.11 10.12 10.13 10.14} \
   ```
4. Make the build files using the Development Wizard in Amira.

4'. (For Linux) ensure that lib teem is found in the local build just created  .../lib/arch-LinuxAMD64-Optimize

```
.../lib/arch-LinuxAMD64-Optimize$ls
```
Should return
```
libhxNrrdIO.so libteem.so libteem.so.1 libteem.so.1.12.0
```
If not, in /lib/arch-LinuxAMD64-Optimize:
``` 
ln -s /usr/local/lib/libteem.so*
```

5. (Prob not required) Edit the file further to disable openmp when compiling using clang:
  Change:

  ```
  HX_OPENMP_FLAG = $(shell $(CXX) --help -v 2>&1 | grep -q fopenmp && echo "-fopenmp")
  ```

  to

  ```
  HX_OPENMP_FLAG = ""
  ```

6. Edit the file one more time to ensure that lib teem is found in /usr/local/lib
  
  Add somewhere after the last place in which LDFLAGS is set (i.e. a line beginning `LDFLAGS +=`)

  ```
  LDFLAGS += -L/usr/local/lib
  LDFLAGS += -Wl,-F$(HX_XCODE_DEVELOPER_ROOT)/../../../../SharedFrameworks
  ```

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
