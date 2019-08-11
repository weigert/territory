//Hexelrender Mainfile
#include "hexelrender.h"

/*
Need a logger class that logs errors and stuff.
*/

//Main Function
int main( int argc, char* args[] ) {

	//Handle the Console Arguments
	if(argc<2){
		std::cout<<"[Territory] Error >>Missing World Name"<<std::endl;
		std::cout<<"Usage: ./territory [worldname]"<<std::endl;
		return 0;
	}

	//Setup the World, View, events
	World world((std::string)args[1]);
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
		events.update(world, view);

		//Render the View
		view.render(world);

		//view.calcFPS();
	}

	view.cleanup();

	return 0;
}
