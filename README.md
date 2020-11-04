# DuAEF_DuBuilder

A builder app for extendscript

## Usage

DuBuilder is a simple tool used at *Rainbox Laboratory* to quickly build *Adobe* Scripts using the *ExtendScript* programing language.

It builds a single `.jsx` file from a script split in several `*.jsxinc` files using the `#include` pre-processor instruction.

It is also able to automatically build any associated `jsdoc` from the script if jsdoc is available on the system.

It comes with both a nice Graphical User Interface, and a Command Line Interface to make it also easy to use in your scripts and automation processes.

The Documentation is available there: [dubuilder-docs.rainboxlab.org](http://dubuilder-docs.rainboxlab.org)

## Compile the source code

### From QT Creator

Just open the `Dutranslator.pro` file with Qt Creator and follow the steps.

### From the terminal :v:

1. `cd` into the project directory;
2. run `qmake Dutranslator.pro`, this will generate a `Makefile`;

	You can run this command only when the pro file has been modified.

3. run `make`, this will compile the code;
4. run `./Dutranslator` to launch the project;
5. use `make clean` to remove all build-related files.
