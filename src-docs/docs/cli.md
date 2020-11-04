# Command Line Interface

*DuBuilder* can be used headless from the command line, or included in your scripts and automations. For example, we use this extensively at *Rainbox Laboratory* to automatically build all the panels of [*Duik*](https://rainboxlab.org/tools/duik/), its [API](https://duik-api.rainboxlab.org/), *DuAEF* and all the associated documentation in a single click.

`DuBuilder inputFile.jsx [options] [outputFile.jsx]`

!!! Note
    On Windows, don't forget to add the extension `.exe` to the *DuBuilder* command if it's not in your *PATH* variable.

## Examples

- Build `Duik Bassel.2.jsx`:  
    `DuBuilder "src/Duik Bassel.2.jsx" "build/Duik Bassel.2.jsx"`

- Build the Duik API `duik_api.jsxinc` and its documentation:  
    `DuBuilder src/duik_api.jsxinc -d duik_api_jsdoc_conf.json api/duik_api.jsxinc`

- When used without any argument, or if only the input file is provided, the [user interface](gui.md) will be launched and the optional input file will be opened.  
    `DuBuilder` - Launches the GUI  
    `DuBuilder dugr.jsx` - Launches the GUI and opens the file `dugr.jsx`

## Default Include Paths

When used through the command line, *DuBuilder* will also use the default include paths set through the [graphical user interface](gui.md). Don't forget to set them if you need before using the command line!

## Getting help

| Option | Type | Description |
| --- | --- | --- |
| `-h` | | Print basic options without launching DuBuilder. |
| `--help` | | Print basic options without launching DuBuilder |

## Options

| Option | Type | Description |
| --- | --- | --- |
| `--nobanner` | | Does not output the first descriptive paragraphs (name, version, license and copyright notice). |
| `-d jsdoc_conf.json` | | Also build the *jsdoc* using the `jsdoc_conf.json` configuration file. *Jsdoc* must be installed. See https://jsdoc.app for more info. |
| `--jsdoc jsdoc_conf.json` | | Also build the *jsdoc* using the `jsdoc_conf.json` configuration file. *Jsdoc* must be installed. See https://jsdoc.app for more info. |