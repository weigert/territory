#include <TinyEngine/TinyEngine>
#include <TinyEngine/parse>
#include <TinyEngine/log>
#include <TinyEngine/color>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#include "helper/arraymath.h"
#include "render/scene.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>

#include "helper/bundle.cpp"

namespace fs = boost::filesystem;

fs::path rootdir;                   //Savefile Information
fs::path savedir;

#define WDIM glm::vec3(128,32,128)
#define RDIM glm::vec3(16,16,16)
#define CDIM glm::vec3(16,16,16)

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

  int SEED = 0;
  if(parse::option.contains("s"))
    SEED = std::stoi(parse::option["s"]);
  logg::deb("SEED: ", SEED);

  //Load the World
  Tiny::view.vsync = false;
  Tiny::benchmark = true;
  Tiny::window("Territory", 1200, 800);

  cam::near = -500.0f;
  cam::far = 500.0f;
  cam::rad = 1.0f;
  cam::look = vec3(32,0,32);
  cam::init(5, cam::ORTHO);

  //Load Shaders
  Shader cubeShader({"source/shader/default.vs", "source/shader/default.fs"}, {"in_Position, in_Normal, in_Color"});
  Shader reflectShader({"source/shader/reflect.vs", "source/shader/reflect.fs"}, {"in_Position, in_Normal, in_Color"});
  Shader depthShader({"source/shader/depth.vs", "source/shader/depth.fs"}, {"in_Position"});
  Shader spriteDepthShader({"source/shader/spritedepth.vs", "source/shader/spritedepth.fs"}, {"in_Quad, in_Tex"});
  Shader spriteShader({"source/shader/sprite.vs", "source/shader/sprite.fs"}, {"in_Quad, in_Tex"});
  Shader itemShader({"source/shader/item.vs", "source/shader/item.fs"}, {"in_Quad, in_Tex"});
  Shader blurShader({"source/shader/dof.vs", "source/shader/dof.fs"}, {"in_Quad, in_Tex"});
  Shader billboardShader({"source/shader/billboard.vs", "source/shader/billboard.fs"}, {"in_Quad, in_Tex"});

  Billboard shadow(2000, 2000, false);

  logg::debug = true;

	World world((std::string)args[1], SEED);
  world.buffer();

  timer::Timer<std::chrono::milliseconds> worldUpdateTimer;
  worldUpdateTimer.set_interval(&world.tickLength, [&](){
    world.time = (world.time+1)%(60*24);
  });

  vec3 prelookpos = floor(cam::look/vec3(world.chunkSize));

  int zoomstate = 0;
  int oldzoomstate = 0;

	//Add the Event Handler
	Tiny::event.handler = [&](){

    cam::handler();

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

    if(cam::rad <= 1.0f) cam::rad = 1.0f;

    if(cam::rad >= 6.0f) zoomstate = 0;
    else if(cam::rad >= 3.0f) zoomstate = 1;
    else if(cam::rad >= 1.5f) zoomstate = 2;

    if(zoomstate != oldzoomstate){
      std::cout<<"Changed Zoom State "<<zoomstate<<std::endl;

      if(zoomstate == 2){
         scene::renderdist = vec3(15, 10, 15);
         scene::dproj = ortho<float>(-400,400,-400,400,-300,300);
         scene::dvp = scene::dproj*scene::dview;
      }
      if(zoomstate == 1){
         scene::renderdist = vec3(10, 5, 10);
         scene::dproj = ortho<float>(-300,300,-300,300,-300,300);
         scene::dvp = scene::dproj*scene::dview;
      }
      if(zoomstate == 0){
         scene::renderdist = vec3(5);
         scene::dproj = ortho<float>(-200,200,-200,200,-300,300);
         scene::dvp = scene::dproj*scene::dview;
      }

      world.fullmodel.construct([](Model* m){});
      for(size_t i = 0; i < world.chunks.size(); i++)
        world.chunks[i].remesh = true;

      scene::LOD = pow(2,zoomstate);
      cam::moverate = scene::LOD;

      world.buffer();
      world.mesh();

      oldzoomstate = zoomstate;
    }

	};

  Square2D flat;

	//Set up an ImGUI Interface here
	Tiny::view.interface = [&](){
	};

	//Define the rendering pieeline
	Tiny::view.pipeline = [&](){

    mat4 back = translate(mat4(1), -cam::look);

    // Shadow Mapping

    shadow.target(color::black);
    depthShader.use();
    depthShader.uniform("dvp", scene::dvp);

  //  depthShader.uniform("model", back*world.fullmodel.model);
  ///  world.fullmodel.render();

   for(size_t i = 0; i < world.models.size(); i++){
      depthShader.uniform("model", back*world.models[i]->model);
      world.models[i]->render();
    }

    // Regular View

    Tiny::view.target(scene::skycol);

    cubeShader.use();
    cubeShader.texture("shadowMap", shadow.depth);

    cubeShader.uniform("vp",  cam::vp);
    cubeShader.uniform("dbmvp", scene::bias*scene::dvp );
    cubeShader.uniform("back", back );

    cubeShader.uniform("lightstrength", scene::lightstrength);
    cubeShader.uniform("lightcol", scene::lightcol);
    cubeShader.uniform("lightpos", scene::lightpos);
    cubeShader.uniform("lookdir", cam::pos);

    cubeShader.uniform("grain", scene::grain);          //Texture Grain
    cubeShader.uniform("fog", scene::fog);              //Fog
    cubeShader.uniform("fogcolor", scene::fogcolor);    //
    cubeShader.uniform("shading", true);                  //Shading

//     cubeShader.uniform("transparent", false); //Fix Later
//     cubeShader.uniform("volPosA", vec3(0));
//     cubeShader.uniform("volPosB", vec3(0));

//     cubeShader.uniform("model", world.fullmodel.model);
//     world.fullmodel.render();

    for(size_t i = 0; i < world.models.size(); i++){
      cubeShader.uniform("model", world.models[i]->model);
      world.models[i]->render();
    }

	};

	//Execute the render loop
	Tiny::loop([&](){

    scene::daytime = (double)world.time/(60.0*24.0);
  //  scene::skycol = color::bezier(ease::cubic(scene::daytime), color::skycolors);
    scene::lightpos = vec3(-10.0f, ease::quadratic(scene::daytime)*20.0f+10.0f, -10.0f+scene::daytime*20.0f);
    scene::dview = lookAt(scene::lightpos, vec3(0), glm::vec3(0,1,0));
    scene::dvp = scene::dproj*scene::dview;
  //  scene::lightstrength = 1.4*ease::quartic(scene::daytime)+0.1;

  //  scene::fogcolor = glm::vec4(ease::quartic(scene::daytime), ease::quartic(scene::daytime), ease::quartic(scene::daytime), 1.0);



	});

	Tiny::quit();

	return 0;
}
