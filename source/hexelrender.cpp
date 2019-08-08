/*
5.9kB for 1/8 of the octree filled with one block type. Summarizing issues right now!

Updated Version:
904 bites for the simplified version -> 6 times less data
184 bytes now for super simplified versoin
For a full block of one type it's only 104 bytes lmao nice

what's the biggest we can go?

10 Random Points -> 2.1kB
100 Random Points -> 12kB
1000 Random Points -> 40-42kB
2000 Random Points -> 45.7kB,46.3,46.2,46.2 etc
Peaks around 46kB for 16^3 chunk, at about ~20 Random Filling
4000 Random Points -> 46 kB
8000 Random Points -> 36.5 kB


8000 Random Points @ 32^3 -> 329.4 kB
*/

/*
If a chunk has been updated, we need to write it to disk before unloading it.
when a new chunk is loaded, we need to update it's corresponding model only.


*/

//Hexelrender Mainfile
#include "hexelrender.h"

//Main Function
int main( int argc, char* args[] ) {
	//Setup the World, View, events
	World world("test_forest");
	View view;
	eventHandler events;

	//Initialize the View
	if(!view.Init()){
		std::cout<<"View could not be initialized."<<std::endl;
		return 0;
	}

	//Generate the World and Chunks
	world.bufferChunks();
	view.loadChunkModels(world);

	//Game Loop
	bool quit = false;
	SDL_Event e;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Main Game Loop
	while(!quit){
		//Handle User Input
		events.input(&e, quit);
		events.handle(world, view);

		//Render the View
		view.render(world);
		//view.calcFPS();
	}

	view.cleanup();

	return 0;
}
