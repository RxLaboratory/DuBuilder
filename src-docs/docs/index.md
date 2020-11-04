# DuBuilder - The Duduf JSX(inc) Builder

*DuBuilder* is a simple tool used at *Rainbox Laboratory* to quickly build *Adobe Scripts* using the *ExtendScript* programing language.

## What it does

It builds a single *.jsx* file from a script split in several `*.jsxinc` files using the `#include` pre-processor instruction.

It is also able to automatically build any associated [*jsdoc*](http://jsdoc.app) from the script if *jsdoc* is available on the system.

It comes with both a nice [**Graphical User Interface**](gui.md), and a [**Command Line Interface**](cli.md) to make it also easy to use in your scripts and automation processes.

![Main Window](img/dubuilder-main.png)

*DuBuilder* is a native app coded in *C++* with [*Qt*](http://qt.io), available for *Windows*, *Mac OS* and even *Linux* for those who want to write their *Adobe* scripts in a better environment (using a virtual machine to run them) - that's what we like to do at *Rainbox Laboratory*, to stay in a free (as in freedom) and safe environment.

## What it doesn't do

*DuBuilder* is not able to obfuscate your script, nor convert it into a `.jsxbin` obfuscated file. At *Rainbox Lab.* we don't need this as we open source all our tools, and we strongly believe that letting the users access the source code is important to make the world better. No matter what, your scripts will be cracked, so why would you prevent everyone from learning, and being able to use their tools as they wish?

*DuBuilder* is not able to minify your scripts nor remove the comments. Minifying scripts and removing comments do not improve their performance in any way. It is true that minifying will make the file much smaller, but it's quicker and easier to just zip the script to make the file as small as possible when distributing it, it's not really useful to minify it.

That said, the file built by *DuBuilder* is a very standard `.jsx` file, so you can use your own tools to obfuscate and minify it if you wish.

## License

### Software

**Copyright (C)  2020 Nicolas Dufresne and Contributors.**  
This program is free software; you can redistribute them and/or modify them under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the *GNU General Public License* along with *DuBuilder*. If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

### This Documentation

**Copyright (C)  2020 Nicolas Dufresne and Contributors.**  
Permission is granted to copy, distribute and/or modify this document under the terms of the GNU Free Documentation License, Version 1.3 or any later version published by the Free Software Foundation;  
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in the section entitled "[Documentation License](licenses/gfdl.md)".

![GNU](img/gnu.png) ![GFDL](img/gfdl-logo.png)