# Developers guide

## Commenting

### Header

If you create a new file for the project: the header comment (which should be put in a `.h` file) must include (license?), your name, email and date (year is enough). For example:

```
/* Copyright (C) 2015 Victoria Rudakova <victoria.rudakova(at)yale.edu> */

```

If you introduce some changes in the file created by someone else, add: `edited`, date, your name and email address, for example:
```
/* Copyright (C) 2015 Victoria Rudakova <victoria.rudakova(at)yale.edu>   
   edited 2015 Your Name <your.name(at)example.edu> */
 
```

### Commenting style

The QDoc (or doxygen) is used to generate documentations for the source files (backend). Refer to the doxygens [Documenting the code](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html) section for comment style guidance and opt for Qt style commenting.  

For QML, refer to Qt's [QML documentation style](http://doc.qt.io/qt-5/qtwritingstyle-qml.html) and optionally to [QML documentation examples](http://doc.qt.io/qt-5/qdoc-componentset-example.html)
