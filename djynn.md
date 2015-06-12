# Djynn Plugin #

## Introduction ##

Like everyone else, more or less I suppose, I tried using every other editor available for Linux. None except Geany worked the way I wanted, and with Geany everything was perfect — I even liked the name — everything except the project manager. I couldn't get any of the project-manager-plugins to do what I wanted, and I had something similar to Notepad++, Dev-C++, or CodeBlocks in mind. So I decided to see if I could make a plugin of my own.

The TreeBrowser-plugin was a good starting point. It went well, even better than expected and it was so much fun I decided to add more functionality; so when I needed sorting, it was added; then was added commenting, with a slightly different behaviour of the built-in commenting.

The name Djynn is of course a play on words, like the name Geany, both being oriental spirits, but with a 'y' instead of 'ie'/'i'. Having added more functionality than a simple project manager, I couldn't just name the plugin "Project Manager" or similar, and so I thought of the name Geany, and obviously it refers to the three-wish-fulfilling flask-dwelling spirit "Genie" with a modified spelling, and I came to think of the other oriental spirit "Djinn", of which I suppose the name Genie comes, and thought "hey, that could be a good name!".

## Features ##

Djynn is a plugin for the Geany IDE and adds sorting and commenting functionality as well as a simple projectmanager that integrates with the built in project handler of Geany.

Lines can be sorted in various ways, ascending, descending and reverse line order. If there is a selection, only the selected lines are sorted, otherwise the entire document is sorted.

The commenting differs slightly from the built in Geany commenting. It's of course a matter of taste and personal programming style, better to have a few options to choose from. It toggles line- and block-commenting, and adds doxygen comments.

The project manager inserts a tree for the project files. Files and folders are added manually in each project, there can be any number of files, order in any way you like, and it's also possible to read and insert files from a directory including subdirectories (only source files are read, and non-project directories such as '.svn' are excluded).

![http://img35.imageshack.us/img35/7964/geanydjynn.png](http://img35.imageshack.us/img35/7964/geanydjynn.png)

In the project manager you can have any number of workspaces, and each workspace can contain any number of projects. Each project is stored in a separate config-file, and if two workspaces creates two projects with the same name, the project is shared between workspaces. Projects also integrates with the built in Geany projects, and for every Djynn project a Geany project is created also.

![http://img15.imageshack.us/img15/495/geanydjynnwssess.png](http://img15.imageshack.us/img15/495/geanydjynnwssess.png)

To make it easier working with many projects, a session handler has been added to Djynn, and you can easily add more sessions from the menu. Each session maintains its open documents, of course including position. When switching between sessions, all documents of the previous session are closed, and all documents of the new session opened. This way, you can work with one project, then another, then switch back, in the time it takes to close and open the documents.

Sessions are only saved at the moment of switching between one and the other. When closing Geany and on startup no sessions are stored by the Djynn session-handler, but instead by Geany itself. Therefore, if you for example open Geany by double clicking on a file, only this document will be in the session and you've apparently lost your previous session. Don't panic! Simply select in the menu Tools->Djynn->Project Manager->Reload Session and all documents in your session are reopened, that is, the files that were open last time the session was saved, either on its creation or last time you switched between sessions.

Another way of saving an old session is to create a new session. The open documents will be saved in the new session and you can then switch back to the old session to open its documents.

Create new Workspaces and Sessions in the Djynn menu:

![http://img841.imageshack.us/img841/1903/geanydjynnmenu.png](http://img841.imageshack.us/img841/1903/geanydjynnmenu.png)

Deleting workspaces and sessions are always prompted first, and always delete the active workspace/session. Deleting a workspace will not delete its projects, and so creating a new workspace and adding an old project, using the same name, will insert the old project into the workspace. Deleting a session deletes it entirely.

## Installing Djynn ##
Djynn uses CMake, so you will need to install this, and you will of course need to install GTK+ 2.6. To build and install Djynn, open a terminal and enter the following commands:

```
svn checkout http://libamanita.googlecode.com/svn/trunk/ libamanita
cd libamanita/src/plugin/djynn
mkdir build
cd build
cmake ..
make
sudo make install
```

Restart Geany and you should be able to see Djynn in your Plugin Manager (see Tools menu).

![http://img11.imageshack.us/img11/9071/geanydjynnconfig.png](http://img11.imageshack.us/img11/9071/geanydjynnconfig.png)

Please, report any bugs!

Suggestions on improvements are much welcome.