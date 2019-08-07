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
	world.loadChunks();
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
