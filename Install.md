

# Installation of Amanita Library #
Here follows instructions on how to compile and install on different platforms. Because of the overly complex and too little documented Autotools, it has been deserted in favour of CMake.

It is not required, but there are optional libraries that you may wish to install. Libamanita will compile without them. These are GtkSourceView and Scintilla.

## Linux ##
### cmake ###
To download, compile and install, open a terminal and run the following sequence of commands:
```
sudo apt-get update
sudo apt-get install build-essential cmake \
   libgtk2.0-dev libwebkit-dev \
   libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev \
   libsqlite3-dev \
   libjpeg-dev libtiff5-dev \
   libavcodec-dev libavformat-dev libavutil-dev libswscale-dev

svn checkout http://libamanita.googlecode.com/svn/trunk/ libamanita
mkdir libamanita/build & cd libamanita/build

cmake ..
make
sudo make install

cd ../.. & rm -r libamanita
```

### Cross Compile on Linux with MinGW for Windows ###
In a terminal install MinGW like so:
```
sudo apt-get update
sudo apt-get install build-essential \
   mingw32 \
   mingw32-binutils \
   mingw32-runtime
```
Now look in your /usr map (terminal: "ls -la /usr"), there should be a map called "i586-mingw32msvc" or similar. This map consists of the cross compile environment with MinGW binaries, DLLs, libraries and headers. Place all cross compile libraries in this map, they can be found as Fedora RPM-packages. All RPM-packages starting with "mingw32-" can be extracted and installed.

Run cmake (adjust for your version of MinGW-cross-compile environment in the file mingw32.cmake):
```
mkdir build & cd build
cmake -DCMAKE_TOOLCHAIN_FILE=mingw32.cmake ..
make
sudo make install
```

## Windows ##
### MSYS ###
All MinGW32 packages in the Fedora repositories can be extracted and used for building in MSYS. Place the files in your MSYS directory.

Run cmake:
```
mkdir build & cd build
cmake ..
make
make install
```

## Mac OSX ##
Not tested for building on mac yet. Please report if you do and succeed.

## Dependencies ##
  * libgtk2.0-dev
  * libwebkit-dev (not required)
  * libsdl1.2-dev
  * libsdl-ttf2.0-dev
  * libsdl-image1.2-dev
  * libsqlite3-dev
  * libjpeg-dev (not required)
  * libtiff5-dev (not required)
  * libavcodec-dev (not required)
  * libavformat-dev (not required)
  * libavutil-dev (not required)
  * libswscale-dev (not required)