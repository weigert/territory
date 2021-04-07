#include <TinyEngine/TinyEngine>
#include <TinyEngine/parse>
#include <TinyEngine/log>
#include <TinyEngine/color>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#include "helper/arraymath.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>

#include "render/scene.h"


#define CHUNKSIZE 16
#define CHUNKVOL (CHUNKSIZE*CHUNKSIZE*CHUNKSIZE)
#define WDIM glm::vec3(128,48,128)
#define RDIM glm::vec3(16,16,16)
#define CDIM glm::vec3(CHUNKSIZE)

#include "render/vertexpool.cpp"

namespace fs = boost::filesystem;

fs::path rootdir;                   //Savefile Information
fs::path savedir;



#include "helper/volume.h"
#include "helper/graph.h"

#include "world/world.cpp"

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





  Tiny::view.vsync = false;
  Tiny::view.antialias = 16;

//  Tiny::benchmark = true;
  Tiny::window("Territory", 1200, 800);

  cam::near = -800.0f;
  cam::far = 800.0f;
  cam::rad = 1.0f;


//  cam::look = CDIM*WDIM/2.0f;
  //cam::look.y = CDIM.y*WDIM.y/4.0f;

  cam::moved = true;
  cam::init(5, cam::ORTHO);


   scene::renderdist = vec3(5, 4, 5);
   scene::dproj = ortho<float>(-200,200,-200,200,-300,300);
   scene::dvp = scene::dproj*scene::dview;

   scene::QUAD = 2400;
   scene::MAXBUCKET = 18*18*18*8;



  //Load Shaders
  Shader cubeShader({"source/shader/default.vs", "source/shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});
  Shader depthShader({"source/shader/depth.vs", "source/shader/depth.fs"}, {"in_Position"});
	Shader effectShader({"source/shader/effect.vs", "source/shader/effect.fs"}, {"in_Quad", "in_Tex"});

  Billboard shadow(1000, 1000, false);
  Billboard image(1200, 800);

  Square2D flat;

  logg::debug = true;

	World world((std::string)args[1], SEED);
  world.buffer();
  world.mesh();

  timer::Timer<std::chrono::milliseconds> worldUpdateTimer;
  worldUpdateTimer.set_interval(&world.tickLength, [&](){
    world.time = (world.time+1)%(60*24);
  });

  vec3 prelookpos = floor(cam::look/vec3(world.chunkSize));
  vec3 oldpos;
  vec3 newpos;

  oldpos.x = (cam::pos.x > 0)?1:-1;
  oldpos.y = (cam::pos.y > 0)?1:-1;
  oldpos.z = (cam::pos.z > 0)?1:-1;
  if(cam::pos.x == 0) oldpos.x = 0;
  if(cam::pos.y == 0) oldpos.y = 0;
  if(cam::pos.z == 0) oldpos.z = 0;

	//Add the Event Handler
	Tiny::event.handler = [&](){

    if(cam::moved){

      //Bounds on Camera Movement

      if(cam::roty > 90.0f - cam::turnrate) cam::roty = 90.0f - cam::turnrate;
      if(cam::rad <= 1.5f) cam::rad = 1.5f;

      //Rebuffer Condition

      if(!all(equal(floor(cam::look/vec3(world.chunkSize)), prelookpos))){
        prelookpos = floor(cam::look/vec3(world.chunkSize));
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

	};

	//Set up an ImGUI Interface here
	Tiny::view.interface = [&](){

	};

  auto _old = std::chrono::high_resolution_clock::now();

	//Define the rendering pieeline
	Tiny::view.pipeline = [&](){

    auto _new = std::chrono::high_resolution_clock::now();
    std::cout<<"TIME: "<<std::chrono::duration_cast<std::chrono::microseconds>(_new - _old).count()<<std::endl;
    _old = std::chrono::high_resolution_clock::now();

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

     Tiny::view.target(color::black);

     effectShader.use();
     effectShader.texture("imageTexture", image.texture);
     effectShader.texture("depthTexture", image.depth);
     effectShader.uniform("grain", scene::grain);          //Texture Grain
     effectShader.uniform("fog", scene::fog);              //Fog
     effectShader.uniform("fogcolor", scene::fogcolor);    //

     effectShader.uniform("model", glm::mat4(1));
     flat.render();

	};

	//Execute the render loop
	Tiny::loop([&](){

    scene::daytime = (double)world.time/(60.0*24.0);
    scene::lightpos = vec3(-10.0f, ease::quadratic(0.1)*20.0f+10.0f, -10.0f+0.1*20.0f);

    scene::computelighting(scene::daytime);

	});

	Tiny::quit();

	return 0;
}
