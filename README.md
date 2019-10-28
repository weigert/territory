# territory
3D rendered proc-gen world test

## Compiling

Use the make file to compile the binary.

    make all
    
### Dependencies
- gcc
- SDL2 (core, image, mixer, ttf)
- OpenGL3
- GLEW
- Boost (Serialization, System, Filesystem)
- DearImgui

## Usage
    ./territory [world_name]
If the world name doesn't exist, it will generate a world. If it does, it will load the world. Tested on Ubuntu 18LTS.

If you want to use the perlin+forest world generator, edit the function World::generate() to use generatePerlin() instead of generateFlat(), recompile and generate a new world (e.g. by choosing a new world name, or deleting the one you were using).

### Controls
Mouse and Keyboard Inputs:

	- View Control Panel: ESC
	- P: Toggle Pause (**PAUSED BY DEFAULT - BOTS ARE VERY FAST**)
	- M: Play Music
	- Change Camera Position: WASD
	- Move Camera Up / Down: Space / C
	- Change View Angle: Scroll Left / Right
	- Zoom: Scroll Up / Down
	- Right-Click: Select Block (and give task at that location using the panel)

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

![Example Rendering](https://github.com/weigert/territory/blob/master/resource/test.png)
![Control Panel](https://github.com/weigert/territory/blob/master/resource/test2.png)

## Source
### Reading

	- Dependencies: Read *source/territory.h* and the makefile
	- Structure: Read *source/territory.cpp*

I also make heavy usage of forward declarations to avoid cyclical dependencies in this project. To understand how the forward declaration structure works, read:

	- Forward Declarations: *thoughts/structure.md*

You may not be interested in all aspects of this project. In that case, reading certain portions of the source code can give you insight into the system.

	- Rendering Engine: Read *source/include/render/view.h*
	- Taskbot System: Read *source/include/taskbot/population.h*
	- World Generation / Storage: Read *source/include/world/world.h*
	- 3D Pathfinding: Read *source/include/taskbot/task.cpp* (bottom) and *source/include/astar/**

## Features
A description of features and future plans for various elements of the simulation can be found in the folder *thoughts*.

	- General: Read *milestones.md*
	- Simulation Engine: Read *thoughts/engine.md*
	- World Generator: Read *thoughts/world.md*
	- Population: Read *thoughts/population.md*

A number of other files describe other systems which are either implemented or are planned.
