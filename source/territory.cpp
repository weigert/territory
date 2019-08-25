//Hexelrender Mainfile
#include "territory.h"

/*
I need to make loading a vector of chunks quick!
Just like the editBuffer.
Do this for the player, then when it's ultra fast...
Maybe I can specify a list of coordinates for a bot and see if they want to pathfind through a construct of chunks somehow...
*/

//Main Function
int main( int argc, char* args[] ) {
	//Logger
	Logger _log;

	//Handle the Console Arguments
	if(argc<2){
		_log.error("Missing Filename.");
		return 0;
	}

	//Load / Generate a World
	World world((std::string)args[1]);

	//Add a Renderer
	View view;

	//Add an Eventhandler
	eventHandler events;

	//Initialize the View
	if(!view.Init()){
		_log.error("View could not be initialized.");
		return 0;
	}

	//Add a Population
	Player player;
	Population population( world );

	//Generate the World and Chunks
	world.bufferChunks( view );
	view.loadChunkModels( world , player);

	//Game Loop
	bool quit = false;
	SDL_Event e;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Main Game Loop
	while(!quit){
		//Handle User Input
		events.input(&e, quit);
		events.update(world, player, population, view);

		//Update the Population
		if(SDL_GetTicks()%2 == 0){
			population.update(world);
		}

		//Render the View
		view.render(world, player, population);
	}

	view.cleanup();

	return 0;
}
