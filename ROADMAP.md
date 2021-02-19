# Roadmap

This file contains the contents of every release I will do. This is a simple guide to organize the concepts of the project and is subject to change. This file may change along all duration of the development. For more information you can go to the public complete board of ClickUp [here](https://share.clickup.com/l/h/4-4678676-1/7e2410f745d66ec) or individual board in its version.

## [0.1.0] - Base

ClickUp - [List](https://share.clickup.com/l/h/6-29893194-1/73ddf853ca49c05) - [Board](https://share.clickup.com/b/h/6-29893194-2/4fe33cef4e300df)

- [x] Main loop with a module structure (SDL and OpenGL integration)
- [x] Logger
- [x] Profiler
- [x] Memory leak detector
- [x] File System
- [x] Input handler
- [x] ImGui integration
- [x] Simple Camera 3D with controls to move around
- [x] System to load and use shaders
- [x] System information log (infoware)
- [x] Json parser
- [x] MathGeoLib integration
- [x] **Paint, save and load a map with a tileset**
- [x] Basic import system 3D Models
- [x] Resource system
- [ ] File Watcher
- [x] Event system
- [x] Random number generator library integration (PCG)

## [0.2.0] - Layers and Maps

ClickUp - [List](https://share.clickup.com/l/h/6-29893174-1/65d87bc58cf3a7f) - [Board](https://share.clickup.com/b/h/6-29893174-2/9ac37763d464daa)

- [x] Allow more layers
- [x] Allow multiple maps
- [x] Map and layer properties
- [ ] Permission Layer (a layer that users can fill with an id)
- [ ] Own file format if not implemented
- [ ] Export map and layers
  - [x] JSON
  - [x] XML
  - [x] Parse CSV
  - [x] Base64
  - [x] zlib
  - [ ] To Tiled
  - [ ] Own File format(?)

## [0.3.0] - Tools and Terrains

ClickUp - [List](https://share.clickup.com/l/h/6-29893176-1/771a92a6e746703) - [Board](https://share.clickup.com/b/h/6-29893176-2/77ed457c20e375e)

- [ ] Basic Tools
  - [x] Brush
  - [x] Eraser
  - [x] Eyedropper
  - [x] Bucket
  - [ ] Rectangle Fill
- [x] Brush size
- [x] Brush shape (circle and rectangle)
- [x] Semi transparent tile on cursor, where will paint
- [x] Terrain Brush to draw a terrain transition (similar to [Tiled](https://doc.mapeditor.org/en/stable/manual/using-the-terrain-tool/#using-the-terrain-brush))

## [0.4.0] - 3D Models

ClickUp - [List](https://share.clickup.com/l/h/6-29893177-1/f84816f5bd5b0d7) - [Board](https://share.clickup.com/b/h/6-29893177-2/f9f548d0e90c676)

- [ ] Import 3D models
- [x] Object layers
- [ ] Load 3D models on scene
- [ ] Primitives
- [ ] 3D terrains
- [ ] Textures
- [ ] Creation tool of sprites
  - Given an image, create a sprite and place it on the map inclined

## [0.5.0] - Camera

ClickUp - [List](https://share.clickup.com/l/h/6-29893179-1/62627478f184048) - [Board](https://share.clickup.com/b/h/6-29893179-2/51111142eee3b9c)

- [ ] Orthographic and perspective camera
- [ ] Button to focus layers and objects
- [ ] Change infinite Grid to a grid in bounds of the map and in every layer (subdivide every tile in 9 quads?)

## [0.6.0] - Export

ClickUp - [List](https://share.clickup.com/l/h/6-29893180-1/07e0452cd8ad4d2) - [Board](https://share.clickup.com/b/h/6-29893180-2/8c60faf2395afcf)

- [ ] Export Map with objects
- [ ] Research what we want to export map with 3D objects and terrains
- [ ] Export all map in a .fbx

## [0.7.0] - Shortcuts and key remapping

ClickUp - [List](https://share.clickup.com/l/h/6-29893181-1/42103d9974675f4) - [Board](https://share.clickup.com/b/h/6-29893181-2/659f655e77b1e9e)

- [ ] Shortcuts system
- [ ] Key remap by the user
- [ ] Ctrl + Z functionality

## [0.8.0] - Scripting integration and console

ClickUp - [List](https://share.clickup.com/l/h/6-29893182-1/0a746bbe98f66a7) - [Board](https://share.clickup.com/b/h/6-29893182-2/aeca2fdc703b35b)

- [ ] Add scripting system to be able to edit maps
- [ ] Add a console to write and execute commands

## [0.9.0] - Bug fixing and clear code

ClickUp - [List](https://share.clickup.com/l/h/6-29893183-1/e9941ab8c039bc1) - [Board](https://share.clickup.com/b/h/6-29893183-2/98cb883986639a2)

- [ ] Bug Fixing
- [ ] Clear Code

## [1.0.0] - Program and Projects

ClickUp - [List](https://share.clickup.com/l/h/6-29893184-1/33aa08ceb8caca7) - [Board](https://share.clickup.com/b/h/6-29893184-2/d9fec099499b45f)

- [ ] Installer
- [ ] Create a HUB with all projects and treat the projects independently

## [Unversioned]

Section of improvements that I don't know when I will do, but I want to.

ClickUp - [List](https://share.clickup.com/l/h/6-29893187-1/9bcfae4a1e56f0c) - [Board](https://share.clickup.com/b/h/6-29893187-2/0229c44f42d73ef)

- [x] x64 bits
- [ ] Linux and Mac compatibility
- [x] premake5
- [x] Program Icon
