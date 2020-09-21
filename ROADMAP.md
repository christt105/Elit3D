# Roadmap
This file contains the contents of every release I will do. This is a simple guide to organize the concepts of the project and will not be 100% true. This file may change along all duration of the development.
## [0.1.0] - Base
* Main loop with a module structure (SDL and OpenGL integration)
*  Logger
* Profiler
* Memory leak detector
* File System
* Input handler
* ImGui integration
* Simple Camera 3D with controls to move around
* System to load and use shaders
* System information log (infoware)
* Json parser
* MathGeoLib integration
* **Paint, save and load a map with a tileset**
* Basic import system 3D Models
* Resource system
* File Watcher
* Event system
* Random number generator library integration (PCG)
## [0.2.0] - Layers and Maps
* Allow more layers
* Allow multiple maps
* Map and layer properties
* Permission Layer (a layer that users can fill with an id)
* Own file format if not implemented
* Export map and layers (JSON, CSV, XML...?)
## [0.3.0] - Terrains
* Brushes to draw more than one tile at the same time
* Semi transparent tile on cursor, where will paint
* Terrain Brush to draw a terrain transition (similar to [Tiled](https://doc.mapeditor.org/en/stable/manual/using-the-terrain-tool/#using-the-terrain-brush))
* Basic Tools (Brush, Eraser, Eyedropper, Bucket, Rectangle Fill...?)
## [0.4.0] - 3D Models
* Improve import 3D models
* Object layers
* Primitives
* 3D terrains
* Textures
* Creation tool of sprites
	* Given an image, create a sprite and place it on the map inclined
## [0.5.0] - Camera
* Orthographic and perspective camera
* Button to focus layers and objects
* Change infinite Grid to a grid in bounds of the map and in every layer (subdivide every tile in 9 quads?)
## [0.6.0] - Export
* Improve Export Map with objects
* Complete export? (Export all map in a .fbx or something like that)
## [0.7.0] - Shortcuts and key remapping
* Shortcuts system
* Key remap by the user
* Ctrl + Z functionality
## [0.8.0] - Scripting integration and console
* Add scripting system to be able to edit maps
* Add a console to write and execute commands
## [0.9.0] - Bug fixing and clear code
* Bug Fixing
* Clear Code
## [1.0.0] - Program and Projects
* Installer
* Create a HUB with all projects and treat the projects independently
## [Unreleased]
Section of improvements that I don't  know when I will do, but I want to.
* x64 bits
* Linux and Mac compatibility
* Make file