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

	//Generate the World and Chunks
	world.bufferChunks( view );

	//Add Population
	Population population( world );

	//Game Loop
	bool paused = true;

	//Constant Tick-Length Timer for World Updates
	timer::Timer<std::chrono::milliseconds> worldUpdateTimer;
	worldUpdateTimer.set_interval(&world.tickLength, [&](){
		//Perform the World Update (i.e. simply increase the time!)
		world.time = (world.time+1)%(60*24);
	});

	//Event, Input, Render and Audio Handling
	bool quit = false;
	SDL_Event e;

	//Main Game Loop
	while(!quit){
		//Handle User Input
		events.input(&e, quit, paused);
		events.update(world, population, view, audio);
		ImGui_ImplSDL2_ProcessEvent(&e);

		//Process the Audio
		audio.process();

		if(!paused && !world.lock){
			timer::benchmark<std::chrono::microseconds>([&](){
				population.update(world, view, audio);
			});
		}

		//Render the View
		view.updateChunkModels( world );
		view.render(world, population);
		view.calcFrameTime();
	}

	//End the Program Correctly
	worldUpdateTimer.stop();
	view.cleanup();

	return 0;
}
