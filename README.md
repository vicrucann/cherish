# Cherish - Tool for Cultural HERitage data representation by using Image manipulation and Sketching

[![Build Status](https://travis-ci.org/vicrucann/cherish.svg?branch=master)](https://travis-ci.org/vicrucann/cherish)

[![Coverity Status](https://scan.coverity.com/projects/9322/badge.svg)](https://scan.coverity.com/projects/vicrucann-cherish)

**Cherish** is a tool that aids archaeologists and designers to represent historial, urban and archaeologocal structures and sites in 3D by means of image manipulation and sketching. Given a problem of disparate data types such as photographs, diagrams, text annotations, point cloud representation, etc., the main idea is to combine all the data within the same 3D space. 

For the data processing, rather than relying on either purely automatic approaches or human input, the framework supports speculation and reasoning in the form of both. As a result, the software is capable to take as input various raw data sources and produce a labeled, corresponded and augmented data source that can be used in variety of applications.

For more details, examples and other user-oriented information, refer to the [Cherish site](http://vicrucann.github.io/cherish/).

## Binary download

If you are using Windows machine (version>=7), the fastest way is to download the pre-compiled binary and start testing the software right away. For that, go to the [release section](https://github.com/vicrucann/cherish/releases) and download the latest version. The download link will have the form of `cherish-qtX.Y-msvcYEAR.zip`.

## Building from source

**Cherish** can be built on a range of platforms, e.g., Windows, Linux, Mac. This is the list of libaraies and tools that are used for the system compillation and build:

* [CMake](https://cmake.org/) (>=2.8.11)
* [Qt](http://www.qt.io/) (>=5.4; in case if you are planning to use Wacom tablem, the recommended Qt version is 5.4 or 5.5 due to a [tablet bug](https://bugreports.qt.io/browse/QTBUG-53628) in the stable versions older than 5.5.1)
* [OpenSceneGraph](http://www.openscenegraph.org/) (>=3.4.0)
* [Doxygen](www.doxygen.org/) (not mandatory; only used for documentation build)

All tools, but OpenSceneGraph (OSG), have binaries and are easy to install. There are no official binaries for OSG library and it is recommended to install it from source. Use the [official guide](http://trac.openscenegraph.org/projects/osg//wiki/Support/GettingStarted) for detailed steps. Note: the used version of OSG only requires OpenGL and CMake; there is no need to install additional plugins; the minimal install is enough.

Once all the dependencies are installed, you can compile **cherish** on your machine. CMake is used for build and compilation, i.e., you can either perform the build from command line by using `cmake` and `make` commands, or you can conviniently use the [Qt Creator tool](https://www.qt.io/ide/) which normally comes with the Qt library. 

The list of build parameters:

* Build mode: `Debug` by default. Example: `-DCMAKE_BUILD_TYPE=Release`.
* Build unit tests: `OFF` by deafult. Example: `-DCherish_BUILD_TEST=ON`
* Build development documentation (requires Doxygen): `OFF` by default. Example: `-DCherish_BUILD_DOC=ON`

## Contact and authors info

The software is being developed at [Yale Graphics Lab](http://graphics.cs.yale.edu/site/). The main development is performed by Victoria Rudakova under the guidance of [prof. Holly Rushmeier](http://graphics.cs.yale.edu/site/people/holly-rushmeier) and [prof. Julie Dorsey](http://graphics.cs.yale.edu/site/people/julie-dorsey).  

For any questions and requests, feel free to contact Victoria Rudakova - victoria.rudakova(at)yale.edu. Or use [Issues](https://github.com/vicrucann/dura-europos-insitu/issues) for feature requests and bug submission.

Other contributors:

* Goze Akoglu: user testing and feedback
* Eleni Kotoula: user testing and feedback
* Wendy Chen: user testing and feedback 
* Natallia Trayan: icon design
* Yumo Rong: user interface functionality

## References

The list of references will be placed here.
