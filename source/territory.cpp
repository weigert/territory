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

  int SEED = 0;
  if(parse::option.contains("s"))
    SEED = std::stoi(parse::option["s"]);
  logg::deb("SEED: ", SEED);

  //Load the World
  Tiny::view.vsync = false;
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
      timer::benchmark<std::chrono::milliseconds>([&](){
        world.buffer();
      });
    }

    if(cam::rad <= 1.0f) cam::rad = 1.0f;

    if(cam::rad >= 6.0f) zoomstate = 0;
    else if(cam::rad >= 3.0f) zoomstate = 1;
    else if(cam::rad >= 1.5f) zoomstate = 2;
//    else if(cam::rad >= 0.0f) zoomstate = 3;

    if(zoomstate != oldzoomstate){
      std::cout<<"Changed Zoom State "<<zoomstate<<std::endl;

      if(zoomstate == 2){
         scene::renderdist = vec3(20);
         scene::dproj = ortho<float>(-250,250,-250,250,-300,300);
         scene::dvp = scene::dproj*scene::dview;
      }
      if(zoomstate == 1){
         scene::renderdist = vec3(10);
         scene::dproj = ortho<float>(-150,150,-150,150,-200,200);
         scene::dvp = scene::dproj*scene::dview;
      }
      if(zoomstate == 0){
         scene::renderdist = vec3(5);
         scene::dproj = ortho<float>(-100,100,-100,100,-300,300);
         scene::dvp = scene::dproj*scene::dview;
      }

      for(size_t i = 0; i < world.chunks.size(); i++)
        world.chunks[i].remesh = true;

      scene::LOD = pow(2,zoomstate);
      cam::moverate = scene::LOD;

      world.buffer();
      world.remesh();

      oldzoomstate = zoomstate;
    }



  //  scene::depthCamera = lookAt(scene::lightpos+cam::look, cam::look, vec3(0,1,0));

	};

  Square2D flat;

	//Set up an ImGUI Interface here
	Tiny::view.interface = [&](){
	};

	//Define the rendering pieeline
	Tiny::view.pipeline = [&](){

    timer::benchmark<std::chrono::milliseconds>([&](){

    mat4 back = translate(mat4(1), -cam::look);

    // Shadow Mapping

    shadow.target();

    depthShader.use();
    depthShader.uniform("dvp", scene::dvp);

  //  cubeShader.uniform("model", world.fullmodel.model);
  //  world.fullmodel.render();


    for(unsigned int i = 0; i < world.models.size(); i++){
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

    cubeShader.uniform("grain", scene::grain);  //Texture Grain
    cubeShader.uniform("fog", scene::fog);  //Texture Grain
    cubeShader.uniform("fogcolor", scene::fogcolor);  //Texture Grain

    //    cubeShader.uniform("transparent", false); //Fix Later
    //    cubeShader.uniform("volPosA", vec3(0));
    //    cubeShader.uniform("volPosB", vec3(0));

//    cubeShader.uniform("model", world.fullmodel.model);
//    world.fullmodel.render();

    for(unsigned int i = 0; i < world.models.size(); i++){
      cubeShader.uniform("model", glm::mat4(1));
      world.models[i]->render();
    }


    glFlush();

  });

/*
    billboardShader.use();
    billboardShader.texture("imageTexture", shadow.depth);
    billboardShader.uniform("model", flat.model);
    flat.render();
*/



/*


    //Render the Sprites to the Depthmap
    spriteDepthShader.useProgram();
    spriteDepthShader.setInt("spriteTexture", 0);

    //Loop over all Sprites
    for(unsigned int i = 0; i < population.bots.size(); i++){
      //Check for No-Render Condition
      if(population.bots[i].dead) continue;
      if(!helper::inModRange(population.bots[i].pos, viewPos, renderDistance, world.chunkSize)) continue;

      //Set the Position of the Sprite relative to the sun
      population.bots[i].sprite.resetModel();
      population.bots[i].sprite.model = glm::translate(population.bots[i].sprite.model, population.bots[i].pos-viewPos);
      population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
      float rot = 0;
      if(lightPos.z < 0){
        rot = -acos(glm::dot(glm::vec3(-1, 0, 0), glm::normalize(glm::vec3(lightPos.x, 0, lightPos.z))));
      }
      else{
        rot = acos(glm::dot(glm::vec3(-1, 0, 0), glm::normalize(glm::vec3(lightPos.x, 0, lightPos.z))));
      }
      population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, rot, glm::vec3(0, 1, 0));

      //Setup the Uniforms
      spriteShader.setMat4("mvp", depthProjection*depthCamera*population.bots[i].sprite.model);
      spriteShader.setFloat("nframe", (float)(population.bots[i].sprite.animation.nframe % population.bots[i].sprite.animation.frames));
      spriteShader.setFloat("animation", (float)population.bots[i].sprite.animation.ID);
      spriteShader.setFloat("width", (float)population.bots[i].sprite.animation.w);
      spriteShader.setFloat("height", (float)population.bots[i].sprite.animation.h);

      //Render the Sprite Billboard
      population.bots[i].sprite.render();
    }

    glBindVertexArray(0);

    */


/*
    //Picker Cube

    if(picked){
      picker.shaderColorPick.useProgram();
      if(picked2){
        //Get the Volume Selection Better
        glm::vec3 a = select2-viewPos-(select2-select)/glm::vec3(2.0);
        glm::mat4 trans = glm::translate(glm::mat4(1.0), a);
        picker.model = glm::scale(trans, glm::abs(select2-select)+glm::vec3(1.0));
        picker.shaderColorPick.setVec3("un_Color", hoverColorBlock);
        picker.shaderColorPick.setMat4("mvp", projection*camera*picker.model);
      }
      else{
        picker.model = glm::translate(glm::mat4(1.0f), select-viewPos);
        picker.shaderColorPick.setVec3("un_Color", clickColorBlock);
        picker.shaderColorPick.setMat4("mvp", projection*camera*picker.model);
      }
      glBindVertexArray(picker.vao);
      glDrawArrays(GL_LINE_STRIP, 0, 16);
    }

    //Character Sprites
    spriteShader.useProgram();
    spriteShader.setInt("spriteTexture", 0);

    //Loop over all Sprites
    for(unsigned int i = 0; i < population.bots.size(); i++){
      //Check for No-Render Condition
      if(population.bots[i].dead) continue;
      if(!helper::inModRange(population.bots[i].pos, viewPos, renderDistance, world.chunkSize)) continue;

      //Set the Position of the Sprite relative to the player
      population.bots[i].sprite.resetModel();
      population.bots[i].sprite.model = glm::translate(population.bots[i].sprite.model, population.bots[i].pos-viewPos);
      population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, glm::radians(45.0f), glm::vec3(0, 1, 0));
      population.bots[i].sprite.model = glm::rotate(population.bots[i].sprite.model, glm::radians(-rotation), glm::vec3(0, 1, 0));

      //Setup the Uniforms
      spriteShader.setMat4("mvp", projection*camera*population.bots[i].sprite.model);
      spriteShader.setFloat("nframe", (float)(population.bots[i].sprite.animation.nframe % population.bots[i].sprite.animation.frames));
      spriteShader.setFloat("animation", (float)population.bots[i].sprite.animation.ID);
      spriteShader.setFloat("width", (float)population.bots[i].sprite.animation.w);
      spriteShader.setFloat("height", (float)population.bots[i].sprite.animation.h);
      spriteShader.setFloat("lightStrength", lightStrength);

      //Render the Sprite Billboard
      population.bots[i].sprite.render();
    }

    //Item Sprites
    itemShader.useProgram();
    itemShader.setInt("spriteTexture", 0);

    //Loop over all Sprites
    for(unsigned int i = 0; i < world.drops.size(); i++){
      //Check for No-Render Condition
      if(!helper::inModRange(world.drops[i].pos, viewPos, renderDistance, world.chunkSize)) continue;

      //Set the Position of the Sprite relative to the player
      world.drops[i].sprite.resetModel();
      world.drops[i].sprite.model = glm::translate(world.drops[i].sprite.model, world.drops[i].pos-viewPos);
      world.drops[i].sprite.model = glm::rotate(world.drops[i].sprite.model, glm::radians(45.0f), glm::vec3(0, 1, 0));
      world.drops[i].sprite.model = glm::rotate(world.drops[i].sprite.model, glm::radians(-rotation), glm::vec3(0, 1, 0));

      //Setup the Shader
      itemShader.setFloat("index", (float)world.drops[i]._type);
      itemShader.setMat4("mvp", projection*camera*world.drops[i].sprite.model);

      //Draw the drops!
      world.drops[i].sprite.render();
    }


    //Render Temp to Screen with a horizontal blur shader
    glBindFramebuffer(GL_FRAMEBUFFER, temp1.fbo);
    blurShader.useProgram();
    blurShader.setFloat("mousex", focus.x);
    blurShader.setFloat("mousey", focus.y);
    blurShader.setFloat("width", SCREEN_WIDTH);
    blurShader.setFloat("height", SCREEN_HEIGHT);
    blurShader.setBool("vert", false);
    blurShader.setBool("_fog", fog);
    blurShader.setInt("_blur", blur);
    blurShader.setVec3("fogColor", fogColor);
    glActiveTexture(GL_TEXTURE0+0);
    glBindTexture(GL_TEXTURE_2D, image.texture);
    blurShader.setInt("imageTexture", 0);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, image.depthTexture);
    blurShader.setInt("depthTexture", 1);
    glBindVertexArray(image.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //Render screen to monitor with vertical blur shader
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0+0);
    glBindTexture(GL_TEXTURE_2D, temp1.texture);
    blurShader.setInt("imageTexture", 0);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, image.depthTexture);
    blurShader.setInt("depthTexture", 1);
    blurShader.setBool("vert", true);
    glBindVertexArray(temp1.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


*/

	};

	//Execute the render loop
	Tiny::loop([&](){

    scene::daytime = (double)world.time/(60.0*24.0);
  //  scene::skycol = color::bezier(ease::cubic(scene::daytime), color::skycolors);
    scene::lightpos = vec3(-10.0f, ease::quadratic(scene::daytime)*20.0f+10.0f, -10.0f+scene::daytime*20.0f);
    scene::dview = lookAt(scene::lightpos, vec3(0), glm::vec3(0,1,0));
    scene::dvp = scene::dproj*scene::dview;
  //  scene::lightstrength = 1.4*ease::quartic(scene::daytime)+0.1;

    scene::fogcolor = glm::vec4(ease::quartic(scene::daytime), ease::quartic(scene::daytime), ease::quartic(scene::daytime), 1.0);



	});

	Tiny::quit();

	return 0;
}
