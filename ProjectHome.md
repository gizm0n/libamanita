

# The Amanita Library #
## About ##
This library is used in several other projects, including [Mantichora Tools](http://code.google.com/p/mantichoratools/) and [Web of Tales](http://code.google.com/p/weboftales/). Amanita Library also hosts a few minor projects, including the [Djynn plugin](djynn.md) for Geany IDE, screensavers, and more.
![http://img27.imageshack.us/img27/2711/redwhitefractal.png](http://img27.imageshack.us/img27/2711/redwhitefractal.png)

## Install ##
Please refer to the [installation](Install.md) page for instructions on how to download, compile and install this library.

On how to compile and install the **Djynn plugin**, see [djynn](djynn.md).

## Documentation ##
It is recommended to generate doxygen documentation manually. To do so, make a checkout of the project. Open a terminal and go to the ./libamanita/data/ directory where you downloaded the project. Enter the command "doxygen Doxyfile" and the documentation will be generated in libamanita/data/api/. To view in a browser, enter "firefox ./api/index.html".

## Packages ##
### Libraries ###
| **[libamanita](libamanita.md)** | C++-classes, a growing set of GUI-widgets, Sockets and Networking, SDL, SQLite, etc. |
|:--------------------------------|:-------------------------------------------------------------------------------------|
| **[libipomoea](libipomoea.md)** | C structs and functions                                                              |
| **[libmandragora](libmandragora.md)** | Esoteric calculations, astrology, tarot and qabalah calculations in C                |

### Plugins & Extensions ###
| **[djynn](djynn.md)** | Plugin for Geany IDE with sorting and commenting functionality, and a small project manager |
|:----------------------|:--------------------------------------------------------------------------------------------|
| **[scintilla](scintilla.md)** | Two lexers for the Scintilla editor, that hilight syntax for [MediaWiki](MediaWiki.md) and [Creole](Creole.md). |

### Screensavers ###
| **[Amanita Fractal Zoom Screensaver](amanita_screensaver.md)** | Screensaver that is zooming into Mandelbrot-fractals. |
|:---------------------------------------------------------------|:------------------------------------------------------|
| **[Morning Glory Screensaver](ipomoea_screensaver.md)**        | Screensaver that is rotating and twisting spiraling lines into beautiful patterns. |