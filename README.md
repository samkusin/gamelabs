#Game Laboratories

A collection of game related projects used as a starting point for game development.

The primary development language used by these projects is C++ (C++11 standard.)

##Dependencies

Beyond the utilities inside the 'external' project, projects are either standalone or rely on source files from other projects in this repository.

Every project should build on its own without needing to build dependent projects.  But to run many of the samples, you'll need to install GLFW version 3 (an OpenGL utility library) in your environment.  

GLFW (v3) - located here: http://www.glfw.org/

Installing GLFW should be straightforward on most platforms (OSX via homebrew and on Linus as a package.)  The GLFW site offers standalone installers for Windows and there's always building from source as a last resort.

##Projects

###External

A collection of utilities used by multiple projects.

* glm - GL Math library.  See http://glm.g-truc.net/0.9.5/index.html for details.
* rapidjson - RapidJSON parsing classes.  See https://code.google.com/p/rapidjson/ for details.

###Voronoi

A C++11 version of a Javascript class for Voronoi computation written by Raymond Hill.
https://github.com/gorhill/Javascript-Voronoi

###Ringbuffer

Implements a simple ring buffer that copies data from one stream to another using pthreads (not C++11 threads yet...)

## License

Unless otherwise specified per project LICENSE.md, the code here is licensed under the MIT license.