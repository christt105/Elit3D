# Changelog
## [0.3.0] - 2021-01-04
### Added
* Tools to modify the map
	* Brush
	* Eraser
	* Bucket
	* Eye dropper
* Panel tools
* Brush size
* Brush shape (Rectangle and Circle)
* Shader to know where the brush will paint
* Debug mode to paint layer texture without shader
* Panel about ImGui Markdown

### Changed
* JSON map export csv format, now export an array of id
* Assimp x86 compiled in release

### Fixed
* When select a tileset with height different to 32px it always load 32px
* Create and select a tilesetwithour filewatcher

## [0.2.1] - 2020-12-18
### Added
 * base64 coder and decoder library
 * Parse map in csv, base64 and base64-zlib compressed
 * Added properties on map
 * Edit Name and Type of layer property
 * Option to export on XML and JSON, with csv, base64 and base64-zlib
 * x64 support
 * Load map by clicking it
 * Load last map used on start
 * Program Icon
 * OpenGL class helper
 * Inspector display some info about files
 * Pugixml (XML parser)

### Changed
 * Map paint system
	 * Now I save the id of the tile and not the position on the tileset
 * Improved Panel Resources
	* Icons
	* Zoom
	* Navigate through folders
	* Move files in folders
	* Open on explorer
 * CC-BY License to BSD-2 Clause
 * Filewatcher temporarily disabled due to complexity and bugs
 * Updated MathGeoLib

### Fixed
* mmgr memory leak detector thread safe

### Removed
* Library dependency

## [0.2.0] - 2020-09-25
### Added
* Panel Layers
	* Create Layers
	* Delete Layers
	* Reorder layers
	* Select Layers
	* Hide and Lock Layers
* Layer utilities
	* Layer height edit
	* Layer opacity
	* Layer displacement
* Layer properties
	* Create Layer property as Int, Bool, Float or String
	* Each property has name, type and value
	* Export with map
	* Button to delete property
* Paint on selected layer
* Resize Map support more layers

### Fixed
* Random Number Generator uniform interger distribution

## [0.1.0] - 2020-09-22
### Added
* SDL and OpenGL integration, main structure with modules
* Input handler
* ImGui
* Panels structure
* Viewports with multisampling
* Camera controls
* Logger
* Profiler
* File System
* JSON parser
* Random number generator (PCG)
* Memory Leak detector
* Shaders system
* MathGeoLib
* 3D models importer (not usable in release)
* Resource system
* Basic File Watcher structure (not finished)
* Event System
* System information logger (infoware)
* Paint, save and load a map with only one layer
* Create and resize map