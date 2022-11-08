#include <TinyEngine/TinyEngine>
#include <TinyEngine/parse>
#include <TinyEngine/log>
#include <TinyEngine/color>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;
fs::path rootdir;
fs::path savedir;

#include "include/libmorton/morton.h"

#include "math.h"

#include "render/scene.h"
#include "render/vertexpool.cpp"


#include "world/voxel.h"
#include "world/blueprint.h"
#include "world/world.h"


#include "bot/task.h"
#include "bot/bot.h"
#include "bot/tasks.h"


int main( int argc, char* args[] ) {

  //Parse Commandline Options





  parse::get(argc, args);

  std::string worldname;
  if(parse::val.empty()){
    logg::err("Missing Filename");
    return 0;
  }
  else worldname = parse::val[0];
  logg::deb("Worldname: ", worldname);

  int SEED = time(NULL);
  if(parse::option.contains("s"))
    SEED = std::stoi(parse::option["s"]);
  logg::deb("SEED: ", SEED);

  //Tiny::view.vsync = false;
  Tiny::view.antialias = 1;
  Tiny::window("Territory", 1200, 800);

  cam::near = -800.0f;
  cam::far = 800.0f;
  cam::rad = 1.0f;
  cam::moved = true;
  cam::look = vec3(0, 0, 0);
  cam::init(5, cam::ORTHO);

  scene::renderdist = vec3(5, 4, 5);
  scene::dproj = ortho<float>(-200,200,-200,200,-300,300);
  scene::dvp = scene::dproj*scene::dview;

  scene::QUAD = 2400;
  scene::MAXBUCKET = 18*18*18*8;

  //Load Shaders
  Shader cubeShader({"source/shader/world/default.vs", "source/shader/world/default.fs"}, {"in_Position", "in_Normal", "in_Color"});
  Shader depthShader({"source/shader/world/depth.vs", "source/shader/world/depth.fs"}, {"in_Position"});
	Shader effectShader({"source/shader/effect/effect.vs", "source/shader/effect/effect.fs"}, {"in_Quad", "in_Tex"});

  Shader spriteShader({"source/shader/sprite/sprite.vs", "source/shader/sprite/sprite.fs"}, {"in_Quad", "in_Tex"});
  Texture cowboyfull(image::load("resource/sprite/cowboyfull.png"));

  Billboard shadow(3000, 3000, false);
  Billboard image(1200, 800);

  Square2D flat;
  Square3D flat3;

  logg::debug = true;

	World world((std::string)args[1], SEED);
  world.buffer();
  world.mesh();

  //voxel::compress(world.chunks[0].data);

  timer::Timer<std::chrono::milliseconds> worldUpdateTimer;
  worldUpdateTimer.set_interval(&world.tickLength, [&](){
    world.time = (world.time+1)%(60*24);
  });

  vec3 prelookpos = floor(cam::look/vec3(CHUNKSIZE));
  vec3 oldpos;
  vec3 newpos;

  oldpos.x = (cam::pos.x > 0)?1:-1;
  oldpos.y = (cam::pos.y > 0)?1:-1;
  oldpos.z = (cam::pos.z > 0)?1:-1;
  if(cam::pos.x == 0) oldpos.x = 0;
  if(cam::pos.y == 0) oldpos.y = 0;
  if(cam::pos.z == 0) oldpos.z = 0;


  vector<Bot> bots;
  bots.emplace_back(world.top(ivec2(1, 0))); //Place a Bot

  cam::look = bots.back().pos;
  cam::update();

  scene::daytime = (double)world.time/(60.0*24.0);
  scene::lightpos = vec3(-10.0f, 15.0f+0.0f, -10.0f+0.1*20.0f);

  scene::computelighting(scene::daytime);

  Task::world = &world;

  cout<<(int)(world.get(ivec3(0, 75, 0)))<<endl;

  bool paused = true;

	//Add the Event Handler
	Tiny::event.handler = [&](){

    if(cam::moved){

      //Bounds on Camera Movement

      if(cam::roty > 90.0f - cam::turnrate) cam::roty = 90.0f - cam::turnrate;
      if(cam::rad <= 1.5f) cam::rad = 1.5f;

      //Rebuffer Condition

      if(!all(equal(floor(cam::look/vec3(CHUNKSIZE)), prelookpos))){
        prelookpos = floor(cam::look/vec3(CHUNKSIZE));
        std::cout<<"Buffering ";
        timer::benchmark<std::chrono::milliseconds>([&](){
          world.buffer();
        });
        std::cout<<"Meshing ";
        timer::benchmark<std::chrono::milliseconds>([&](){
          world.mesh();
        });
      }

      //Bound the Zoom-State

      if(scene::changezoomstate(cam::rad)){

        world.vertexpool.clear();

        for(size_t i = 0; i < world.chunks.size(); i++){
          world.chunks[i].remesh = true;
          world.chunks[i].firstmesh = true;
        }

        cam::moverate = scene::LOD;

        world.buffer();
        world.mesh();

      }

			newpos.x = (cam::pos.x > 0)?1:-1;
			newpos.y = (cam::pos.y > 0)?1:-1;
			newpos.z = (cam::pos.z > 0)?1:-1;
			if(cam::pos.x == 0) newpos.x = 0;
			if(cam::pos.y == 0) newpos.y = 0;
			if(cam::pos.z == 0) newpos.z = 0;

			if(!all(equal(oldpos, newpos))){

        world.mask();

			}

			oldpos = newpos;

    }

    cam::handler();

    if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
      paused = !paused;

	};

	//Set up an ImGUI Interface here
	Tiny::view.interface = [&](){

	};

  //auto _old = std::chrono::high_resolution_clock::now();

	//Define the rendering pieeline
	Tiny::view.pipeline = [&](){

    mat4 back = translate(mat4(1), -cam::look);

    // Shadow Mapping

    shadow.target(color::black);
    depthShader.use();
    depthShader.uniform("model", back);
    depthShader.uniform("dvp", scene::dvp);
    world.vertexpool.render();

    // Regular View

    image.target(scene::skycol);

    cubeShader.use();
    cubeShader.texture("shadowMap", shadow.depth);

    cubeShader.uniform("vp",  cam::vp);
    cubeShader.uniform("dbmvp", scene::bias*scene::dvp );
    cubeShader.uniform("back", back );

    cubeShader.uniform("lightstrength", scene::lightstrength);
    cubeShader.uniform("lightcol", scene::lightcol);
    cubeShader.uniform("lightpos", scene::lightpos);
    cubeShader.uniform("lookdir", cam::pos);

    cubeShader.uniform("shading", true);                  //Shading

//     cubeShader.uniform("transparent", false); //Fix Later
//     cubeShader.uniform("volPosA", vec3(0));
//     cubeShader.uniform("volPosB", vec3(0));

     cubeShader.uniform("model", glm::mat4(1));
     world.vertexpool.render();

     //Draw the Bots



     spriteShader.use();
     for(auto& bot: bots){

       spriteShader.uniform("width", 4.0f);
       spriteShader.uniform("height", 4.0f);
       spriteShader.uniform("nframe", 0.0f);
       spriteShader.uniform("animation", 0.0f);
       spriteShader.texture("spriteTexture", cowboyfull);

       glm::mat4 model = glm::mat4(1.0f);
       model = glm::translate(model, bot.pos + vec3(0,0.5,0));
       model = glm::scale(model, glm::vec3(0.5,1,0.5));
       model = glm::rotate(model, glm::radians(180.0f-cam::rot), glm::vec3(0,1,0));

       spriteShader.uniform("model", model);
       spriteShader.uniform("vp", cam::vp);

       flat3.render();

     }




     Tiny::view.target(color::black);

     effectShader.use();
     effectShader.texture("imageTexture", image.texture);
     effectShader.texture("depthTexture", image.depth);
     effectShader.uniform("grain", scene::grain);          //Texture Grain
     effectShader.uniform("fog", scene::fog);              //Fog
     effectShader.uniform("fogcolor", scene::fogcolor);    //
     effectShader.uniform("lookdir", cam::pos);    //

     effectShader.uniform("model", glm::mat4(1));
     flat.render();

	};

  Bot bot(vec3(0));

	//Execute the render loop
	Tiny::loop([&](){

    if(paused)
      return;

    scene::daytime = (double)world.time/(60.0*24.0);
    scene::lightpos = vec3(-10.0f, 15.0f+0.0f, -10.0f+0.1*20.0f);

    scene::computelighting(scene::daytime);

    for(auto& bot: bots){

      if(bot.execute()){

        /*
        ivec2 npos = math::rand2(ivec2(CHUNKSIZE)*ivec2(world.maxchunk.x-world.minchunk.x, world.maxchunk.z-world.minchunk.z)) + ivec2(CHUNKSIZE)*ivec2(world.minchunk.x, world.minchunk.z);
        ivec3 wpos = world.top(npos);
        while(wpos.y == -1){
          npos = math::rand2(ivec2(CHUNKSIZE)*ivec2(world.maxchunk.x-world.minchunk.x, world.maxchunk.z-world.minchunk.z)) + ivec2(CHUNKSIZE)*ivec2(world.minchunk.x, world.minchunk.z);
          wpos = world.top(npos);
        }
        bot.task = new task::Path(wpos);
      */


        bot.task = new task::Fell(bot.pos);
        //bot.task = new task::Fell(ivec3(10,0,10));
        //bot.task = new task::Fell();

      }

    }

	});

	Tiny::quit();

	return 0;
}
