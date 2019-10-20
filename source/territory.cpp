//Hexelrender Mainfile
#include "territory.h"

//Main Function
int main( int argc, char* args[] ) {
	//Handle the Console Arguments
	if(argc<2){
		_log.error("Missing Filename.");
		return 0;
	}

	//Load / Generate a World
	World world((std::string)args[1]);

	//Add a Renderer
	View view;
	Audio audio;

	//Add an Eventhandler
	eventHandler events;

	//Initialize the View
	if(!view.Init()){
		_log.error("View could not be initialized.");
		return 0;
	}

	if(!audio.Init()){
	  _log.debug("Failed to load audio.");
		return 0;
	}

	//Add a Population
	Player player;

	//Generate the World and Chunks
	world.bufferChunks( view );
	view.loadChunkModels( world );

	Population population( world );

	//Game Loop
	bool quit = false;
	bool paused = true;
	SDL_Event e;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Main Game Loop
	while(!quit){
		//Handle User Input
		events.input(&e, quit, paused);
		events.update(world, player, population, view, audio);
		ImGui_ImplSDL2_ProcessEvent(&e);

		//Update the Population
		if(SDL_GetTicks()%world.tickLength == 0 && !paused){
			population.update(world, view, audio);
		}
		audio.process();

		//Render the View
		view.updateChunkModels( world );
		view.render(world, player, population);

		view.calcFPS();
	}

	view.cleanup();

	return 0;
}
