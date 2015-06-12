# Introduction #
Amanita Library is a collection of C++ classes.


## Content ##
### Utility classes ###
The library consist of a set of utility classes:
| **[aCollection](aCollection.md)** | An abstract class for collections, including [aDictionary](aDictionary.md), [aHashtable](aHashtable.md) and [aVector](aVector.md). |
|:----------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------|
| **[aDictionary](aDictionary.md)** | quick search an indexed dictionary for a numeric representation.                                                                   |
| **[aHashtable](aHashtable.md)**   | A hashtable that easily handle all types and objects in one table instance, for both keys and values.                              |
| **[aIterator](aIterator.md)**     | Generic abstract parent-class for Collection-iterators.                                                                            |
| **[aFlood](aFlood.md)**           | A simple class for flood-filling a vector, practical when making maps for example.                                                 |
| **[aInteger](aInteger.md)**       | Handle big integer numbers, any size. Runs pretty fast.                                                                            |
| **[aMath](aMath.md)**             | Static mathematical utility functions collected in one class.                                                                      |
| **[aObject](aObject.md)**         | Handle inheritance and a simple introspection interface.                                                                           |
| **[aPath](aPath.md)**             | Generate the nearest path using the AStar-algorithm.                                                                               |
| **[aRandom](aRandom.md)**         | Very good enthropy and one of the fastest pseudorandom number generators there is. Based on the Mersenne twister.                  |
| **[aRegex](aRegex.md)**           | Minimalistic regex engine. Works pretty well.                                                                                      |
| **[aString](aString.md)**         | Generic string class with many string-handling methods.                                                                            |
| **[aThread](aThread.md)**         | Handle threads, using native functions (glib, win32, and also SDL).                                                                |
| **[aVector](aVector.md)**         | Can like Hashtable handle all types and objects in one vector instance.                                                            |
| **[aWiki](aWiki.md)**             | Convert Wiki-text into HTML. Makes use of the strength of the aString-class.                                                       |
| **[aWord](aWord.md)**             | Generate random words for fantasynames or pseudolanguage.                                                                          |

### Database classes ###
[SQLite](http://www.sqlite.org) wrapper classes:
| **[aRecord](aRecord.md)** | Each row is stored in a Record. |
|:--------------------------|:--------------------------------|
| **[aResult](aResult.md)** | Handle query-results.           |
| **[aDatabase](aDatabase.md)** | Easily open SQLite-databases, make queries and handle results with very little overhead. |

### GUI Widget classes ###
The Amanita Toolkit framework is designed to make cross platform development easier, not replace it entirely. It wraps GTK+ on Linux and Win32 API on Windows, utilising native components, and is very lightweight:
| **[aBrowser](aBrowser.md)** | On GTK+ uses WebKitGtk, and on Win32 makes use of the built in Internet Explorer WebControl. See examples/browser.cpp for how to use this class. |
|:----------------------------|:-------------------------------------------------------------------------------------------------------------------------------------------------|
| **[aButton](aButton.md)**   | A button, checkbox or radiobutton.                                                                                                               |
| **[aCairo](aCairo.md)**     | Use Cario 2D-vector-graphics library to draw a component.                                                                                        |
| **[aCanvas](aCanvas.md)**   | Use native graphics functions to draw a component.                                                                                               |
| **[aContainer](aContainer.md)** | Layout management component, on GTK+ uses the GtkBox and GtkTable classes, on Win32 handles layout management internally in a similar manner as GTK+. Creates a frame with header text, if given a label. |
| **[aLabel](aLabel.md)**     | A text label.                                                                                                                                    |
| **[aMenu](aMenu.md)**       | Menu component.                                                                                                                                  |
| **[aNotebook](aNotebook.md)** | Creates a notebook to switch between pages, with optionary tabs. On GTK+ uses GtkNotebook, on Win32 uses the TabControl.                         |
| **[aPanel](aPanel.md)**     | Creates a panel of buttons or tools. On GTK+ uses the GtkToolbar, on Win32 uses the Toolbar control. The choice to name this class aPanel is to avoid double-names in the class names. |
| **[aSelect](aSelect.md)**   | A drop box, also called combo box.                                                                                                               |
| **[aStatus](aStatus.md)**   | Creates a status bar at the bottom of the window.                                                                                                |
| **[aText](aText.md)**       | A text component, depending on settings with different behaviour. On GTK+ it makes use of GtkEntry, GtkTextView, GtkSourceView or Scintilla, on Win32 either the Edit, RichEdit or Scintilla controls. |
| **[aWidget](aWidget.md)**   | Base class for the Amanita GUI framework.                                                                                                        |
| **[aWindow](aWindow.md)**   | Creates windows and dialogs that contains other widgets.                                                                                         |

### HTTP Network classes ###
Classes for client/server HTTP connections:
| **[aClient](aClient.md)** | A client class that can be used for chatting or online gaming. |
|:--------------------------|:---------------------------------------------------------------|
| **[aHttp](aHttp.md)**     | A http-client. Communicate with a webserver, download webpages or files and upload forms including files. |
| **[aServer](aServer.md)** | A multiuser server class to handle any number of clients.      |
| **[aSocket](aSocket.md)** | Platform independent network communication with sockets.       |

### SDL classes ###
[SDL](http://www.libsdl.org) wrapper classes, and a number of GUI-classes:
| **[aButton](aButton.md)** | A button component. |
|:--------------------------|:--------------------|
| **[aGraphics](aGraphics.md)** | Mimicking the java.awt.Graphics class in a way. To initialize SDL and to work with graphics. |
| **[aFont](aFont.md)**     | Load fonts and display. Using the SDL\_ttf library. |
| **[aImage](aImage.md)**   | Load images.        |
| **[aScroll](aScroll.md)** | A scrolling component. |
| **[aTabset](aTabset.md)** | A tabset component for handling a number of tabs. |
| **[aText](aText.md)**     | A buffer class for handling text in a textbox. |
| **[aTextbox](aTextbox.md)** | A multiline textbox for printing text. Uses the [aText](aText.md) component for handling linebreaks and buffering etc. |
| **[aTextfield](aTextfield.md)** | For entering a line of text. |
| **[aWindow](aWindow.md)** | A basic window class for handling windos and dialog boxed. |