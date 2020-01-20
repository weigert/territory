# territory
3D rendered proc-gen world test

C++ homebrew voxel engine for intelligent agent-driven (NPC) procedural generation / world simulation.

Blog: https://weigert.vsos.ethz.ch/

## Compiling

Use the make file to compile the binary.

    make all
    
Tested on Ubuntu 18 LTS. I have been informed that on Ubuntu 19, the install folder for libnoise is different and needs to be fixed in the 'territory.h' header file.
    
### Dependencies
- gcc
- SDL2 (core, image, mixer, ttf)
- OpenGL3
- GLEW
- Boost (Serialization, System, Filesystem)
- DearImgui
- libnoise

## Usage
    ./territory [world_name]
If the world name doesn't exist, it will generate a world. If it does, it will load the world.

If you want to use the perlin+forest world generator, edit the function World::generate() to use generatePerlin() instead of generateFlat(), recompile and generate a new world (e.g. by choosing a new world name, or deleting the one you were using).

### Controls
Mouse and Keyboard Inputs:

	- View Control Panel: ESC
	- P: Toggle Pause (**PAUSED BY DEFAULT - BOTS ARE VERY FAST**)
	- M: Play Music
	- Change Camera Position (Move): WASD
	- Move Camera Up / Down: Space / C
	- Change Horizontal View Angle: Scroll Left / Right
	- Cycle Vertical View Angle: UP- / DOWN-ARROW (for flat and top-down view)
	- Zoom: Scroll Up / Down
	- Right-Click: Select Block (and give task at that location using the panel)
		- Drag and Release: Select Block Region
		- V: Toggle See-Through Region (for building debugging)
	- Fn+F11: Toggle Fullscreen

### Panel
In-simulation control panel allows for manipulating the simulation.

	- Tab: Simulation:
		- Control the Simulation Speed
		- View Position
		-	 Toggle Debug Messages
	- Tab: Population
		- Observe individual bots
			- Follow Bots
			- Check their Task Queue
				- Manipulate their Task-Queue
			- Check the Memory
			- Check their Inventory
				- Manipulate their Inventory
			- Check their Sprite and Animation
		- Interrupt Bots
		- Kill Bots
	- Tab: View
		- Change to perspective projection
		- FPS Counter
		- Change Skybox Color
		- Change Lighting Color
		- Change Picker Color
		- Change Render-distance (move out of current chunk to reload chunks)
		- Toggle Rendering Options

## Screenshots
Perlin world generation, example of what the rendering looks like and what the control panel looks like. This is in fullscreen mode.

![Fullscreen](https://github.com/weigert/territory/blob/working_branch/screenshots/screenshot_fullscreen.png)

Dense forest world gen. The leaf colors are varied using bezier curves, parameterized by the hashed block position in world-space (see my blog for more information).

![Forest](https://github.com/weigert/territory/blob/working_branch/screenshots/screenshot_forest.png)

Example of a "village" (which is really just a spot in the desert where I had the bots build a bunch of houses). It wasn't like this at world gen, but rather the bots built this 100% from scratch. Then I took the screenshot. There are tons of bots in this image.

![Village](https://github.com/weigert/territory/blob/working_branch/screenshots/screenshot_village.png)

If you turn the camera just right, your 3D projection becomes this pseudo-isometric projection and looks really nice. Here is an example of a watery world-gen. Again using hashed bezier parameters for color variation.

![Water](https://github.com/weigert/territory/blob/working_branch/screenshots/screenshot_lake.png)

And zoomed out a bit.

![Water Far](https://github.com/weigert/territory/blob/working_branch/screenshots/screenshot_lake2.png)

## Source
### Reading
In the wiki, there is a dedicated page to reading and understanding the source code.

You may not be interested in all aspects of this project. In that case, reading certain portions of the source code can give you insight into the system. This is also listed on the wiki page.

## Features
A description of features and future plans for various elements of the simulation can be found in the wiki also.

I will try to keep the feature list as complete as possible.

## Contributing
If you are interested in contributing to this project, with ideas or source code, please let me know.

I could particularly benefit from experience in the areas of:
- 3D Rendering with OpenGL / Fast Shader Programming
- GUI Programming (I am using ImGUI but would like to switch to a custom HUD!)
- Voxel Data Storage / Large World Storage

There are lots of possibilities to include generative systems for the simulation, so if you have a nice concept I would be happy to see it.

Currently, this is 100% solo.

## License
This software is available under the [Mozilla Public License](https://www.mozilla.org/en-US/MPL/2.0/). Still, I don't want to clutter the source with header text, so I hope that's ok. Feel free to use and modify the source of this project.

The music and artwork used in the project (everything in /resource/) is not open source. That content was made specifically for this version of this project. If you plan on *distributing* your own variant of this software, you must use your own music and artwork, or contact me. Using the artwork and music yourself for personal use is fine of course.

## Articles
For the interested, there are some writeups on the various elements of this project on my blog, explaining the systems and the thought process behind their design:

- [Original Task-Bot Concept](http://weigert.vsos.ethz.ch/2019/03/08/modular-memory-driven-task-bots/)
- [Voxel Engine](http://weigert.vsos.ethz.ch/2019/10/27/homebrew-voxel-engine/)
- [Task-Bots in the Voxel Engine](http://weigert.vsos.ethz.ch/2019/10/28/task-bots-now-in-full-3d/)
- [Markov Chains for Procedural Buildings](http://weigert.vsos.ethz.ch/2019/10/30/markov-chains-for-procedural-buildings/)
