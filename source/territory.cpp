#include "territory.h"

//Main Function
int main( int argc, char* args[] ) {
	//Handle the Console Arguments
	if(argc<2){
		logger::error("Missing Filename.");
		return 0;
	}

	//Extract Seed
	int SEED = (argc == 3)?std::atoi(args[2]):0;

	//Load / Generate a World
	World world((std::string)args[1], SEED);

	//Add a Renderer
	View view;
	Audio audio;

	//Add an Eventhandler
	eventHandler events;

	//Initialize the View
	if(!view.Init()){
		logger::error("View could not be initialized.");
		return 0;
	}

	if(!audio.Init()){
	  logger::debug("Failed to load audio.");
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
	//		timer::benchmark<std::chrono::microseconds>([&](){
				population.update(world, view, audio);
	//		});
		}

		//Render the View
		view.updateChunkModels( world );
	//	timer::benchmark<std::chrono::microseconds>([&](){
			view.render(world, population);
	//	});
		view.calcFrameTime<std::chrono::microseconds>();
	}

	//End the Program Correctly
	worldUpdateTimer.stop();
	view.cleanup();

	return 0;
}
