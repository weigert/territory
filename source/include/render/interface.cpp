#include "../taskbot/population.h"
#include "../world/world.h"
#include "../game/player.h"
#include "../render/sprite.h"
#include "../taskbot/bot.h"
#include "../taskbot/state.h"
#include "../taskbot/memory.h"
#include "../taskbot/task.h"
#include "../render/view.h"
#include "../game/item.h"
#include "interface.h"


void Interface::createItem(Item &item){
  //Edit and Return the Item

}

void Interface::drawBot(Bot &bot){
  //Jump to Bots Location
  if(ImGui::Button("Interrupt")){
    bot.interrupt = true;
  }
  ImGui::SameLine();
  if(ImGui::Button("Kill")){
    //Kill the Bot!!!
    bot.dead = true;
  }

  ImGui::Text("Species: ");
  ImGui::SameLine();
  ImGui::Text(bot.species.c_str(), "%s");

  //Alive Status
  ImGui::Text("Status: ");
  ImGui::SameLine();
  if(bot.dead){
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Dead");
  }
  else{
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Alive");
  }

  //Position
  ImGui::Text("Position: ");
  ImGui::SameLine();
  ImGui::Text("%d", (int)bot.pos.x);
  ImGui::SameLine();
  ImGui::Text("%d", (int)bot.pos.y);
  ImGui::SameLine();
  ImGui::Text("%d", (int)bot.pos.z);

  //Options for Manipulating Data
  if (ImGui::Button("Add Task")){
    ImGui::OpenPopup("TaskAdder");
  }
  ImGui::SameLine();
  if (ImGui::Button("Add Item")){
    ImGui::OpenPopup("ItemAdder");
  }
  ImGui::SameLine();
  if (ImGui::Button("Add Memory")){
    ImGui::OpenPopup("MemoryAdder");
  }

  //Sprite / Animation Tab
  if (ImGui::TreeNode("Sprite")){
    ImGui::Text("Frame: ");
    ImGui::SameLine();
    ImGui::Text("%d", (int)bot.sprite.animation.nframe);
    ImGui::TreePop();
  }

  //Task Tab
  if (ImGui::TreeNode("Task")){
    //Loop over the Number of Tasks
    drawTask(bot.current);
    ImGui::TreePop();
  }

  //Mandates Tab
  if (ImGui::TreeNode("Mandate")){
    ImGui::Text("List of Mandates");
    //Render the Actual Mandates
    ImGui::TreePop();
  }

  //Memories Tab
  if (ImGui::TreeNode("Memory")){
    //Output Memory Size
    ImGui::Text("Number of Memories: "); ImGui::SameLine();
    ImGui::Text("%d", (int)bot.memories.size());

    //Loop over all memories
    for(unsigned int i = 0; i < bot.memories.size(); i++){
      //Add a tree Node, then display its memory, etc.
      drawState(bot.memories[i].state);
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Inventory")){
    //Loop over all Items in the Inventory
    for(unsigned int i = 0; i < bot.inventory.size(); i++){
      ImGui::Text("ID: ");
      ImGui::SameLine();
      ImGui::Text("%d", (int)bot.inventory[i]._type);
      ImGui::SameLine();
      ImGui::Text("%s", bot.inventory[i].name.c_str());
      ImGui::SameLine();
      ImGui::Text("Amount: ");
      ImGui::SameLine();
      ImGui::Text("%d", (int)bot.inventory[i].quantity);
      ImGui::SameLine();

      if(ImGui::Button("-")){
        //Delete the specified item from the guy
        bot.inventory.erase(bot.inventory.begin()+i);
      }
    }

    //Render the Actual Mandates
    ImGui::TreePop();
  }

}

void Interface::drawState(State state){
  //Draw the State Stuff
  ImGui::Text(state.task.c_str(), "%s"); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%d", (int)state.pos.x); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%d", (int)state.pos.y); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%d", (int)state.pos.z); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%d", (int)state.block); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "%d", (int)state.reachable); ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%d", (int)state.time); ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.75f, 0.6f, 1.0f, 1.0f), "%d", (int)state.target); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)state.range.x); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)state.range.y); ImGui::SameLine();
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)state.range.z);
}

void Interface::drawTask(Task *task){
  //Make sure tree is open
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  //Construct a TreeNode
  if (ImGui::TreeNode(task->name.c_str())){
    //Draw its state
    drawState(task->args);

    //Loop
    for(unsigned int i = 0; i < task->queue.size(); i++){
      //Render the Task as a Headline
      drawTask(&(task->queue[i]));
    }
    ImGui::TreePop();
  }
}

void Interface::render(View &view, World &world, Population &population, Player &player){
  ImGui::Begin("Territory Controller");

  //Begin the Menu Bar
  /*
  if (ImGui::BeginMainMenuBar()){
    if (ImGui::BeginMenu("World")){
      if (ImGui::MenuItem("Save to File", "Ayy")){
        ImGui::Text("Ayy");
      }
      ImGui::EndMenu();
    }
    //ImGui::TextUnformatted("abc");
    ImGui::EndMainMenuBar();
  }*/

  ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if(ImGui::BeginTabBar("MyTabBar", tab_bar_flags)){
    if(ImGui::BeginTabItem("Simulation")){
      //Put the Stuff in Here
      ImGui::LabelText(world.saveFile.c_str(), "World: ");

      //Set the Simulation Rate
      static int b = 10;
      ImGui::SliderInt("Speed", &b, 0, 10);
      world.tickLength = 2*(10-b)+1;

      if(view.picked){
        ImGui::Text("Selected: ");
        ImGui::SameLine();
        ImGui::Text("%d", (int)view.select.x);
        ImGui::SameLine();
        ImGui::Text("%d", (int)view.select.y);
        ImGui::SameLine();
        ImGui::Text("%d", (int)view.select.z);
      }

      ImGui::Checkbox("Debug Messages", &(_log._debug));

      ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("Population")){
      //Size
      ImGui::Text("Population Size: ");
      ImGui::SameLine();
      ImGui::Text("%d", (int)population.bots.size());

      static bool follow = false;
      ImGui::Checkbox("Follow", &follow);

      //Bots Slider
      static int a = 0;
      ImGui::Text("Selected Bot: ");
      ImGui::SameLine();
      ImGui::SliderInt("ID", &a, 0, (int)population.bots.size()-1);

      //Draw the Bot
      drawBot(population.bots[a]);

      //Set the view position to the bots positin
      if(follow){
        view.viewPos = population.bots[a].pos;
        view.loadChunkModels( world );
      }

      if (ImGui::BeginPopupModal("ItemAdder", NULL)){
          //Construct an Item
          static Item item;
          item.name = "Test";
          ImGui::InputText("Name:", &item.name[0], IM_ARRAYSIZE(&item.name));
          ImGui::SliderInt("Quantity:", &item.quantity, 0, 99);

          //Start the Menu
          if (ImGui::Button("Submit")){
            //Add the item to the inventory.
            population.bots[a].inventory.push_back(item);
            ImGui::CloseCurrentPopup();
          }
          ImGui::SameLine();
          if (ImGui::Button("Cancel")){
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
      }

      ImGui::SetNextWindowSize(ImVec2(250, 275));
      if (ImGui::BeginPopupModal("TaskAdder", NULL)){
          //Construct a Task
          static int _taskHandle = TASK_NULL;
          const char* handles[] = { "Dummy", "Null", "Look", "Listen", "Think", "Wait", "Move", "Walk", "Idle", "Follow", "Seek", "Destroy", "Place", "Build", "Take", "Convert", "Decide", "Request", "Interrupt", "Tell", "Ask", "Respond", "Converse" };
          ImGui::Combo("Task", &_taskHandle, handles, IM_ARRAYSIZE(handles), 4);

          //Do this guy here
          static int _time = 0.0f;
          ImGui::DragInt("Time", &_time, 1, 0, 100);

          static int _block = 0.0f;
          ImGui::DragInt("Block", &_block, 1, 0, 10);

          static int _target = 0.0f;
          ImGui::DragInt("Bot", &_target, 1, 0, population.bots.size()-1);

          //Start the Menu
          if (ImGui::Button("Submit")){
            //Add the item to the inventory.
            Task *_task = new Task(TaskName[_taskHandle].c_str(), a, (TaskHandle)_taskHandle);

            //Construct a State
            State state;
            state.task = _task->name;
            state.target = _target;
            state.time = _time;
            state.pos = view.select;
            state.block = (BlockType)_block;
            state.range = population.bots[a].range;

            //Add the Task
            _task->args = state;
            population.bots[a].current = _task;
            ImGui::CloseCurrentPopup();
          }
          ImGui::SameLine();
          if (ImGui::Button("Cancel")){
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
      }
      ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("View")){
      //Add an FPS Plot
      ImGui::PlotLines("FPS Counter", view.arr, IM_ARRAYSIZE(view.arr));

      //Generate a colorpicker for the light and sky
      static float sky[3] = {view.skyCol.x, view.skyCol.y, view.skyCol.z};
      ImGui::ColorEdit3("Sky Color", sky);
      view.skyCol = glm::vec3(sky[0], sky[1], sky[2]);

      static float light[3] = {view.lightCol.x, view.lightCol.y, view.lightCol.z};
      ImGui::ColorEdit3("Light Color", light);
      view.lightCol = glm::vec3(light[0], light[1], light[2]);

      static float _fogcolor[3] = {view.fogColor.x, view.fogColor.y, view.fogColor.z};
      ImGui::ColorEdit3("Fog Color", _fogcolor);
      view.fogColor = glm::vec3(_fogcolor[0], _fogcolor[1], _fogcolor[2]);

      static float _clickcolor[3] = {view.clickColorBlock.x, view.clickColorBlock.y, view.clickColorBlock.z};
      ImGui::ColorEdit3("Click Color", _clickcolor);
      view.clickColorBlock = glm::vec3(_clickcolor[0], _clickcolor[1], _clickcolor[2]);

      static int _rd[3] = {(int)view.renderDistance.x, (int)view.renderDistance.y, (int)view.renderDistance.z};
      ImGui::DragInt3("Render Distance", _rd);
      view.renderDistance = glm::vec3(_rd[0], _rd[1], _rd[2]);

      static bool _fog = view.fog;
      ImGui::Checkbox("Distance Fog", &_fog);
      view.fog = _fog;

      static int _blur = view.blur;
      ImGui::DragInt("Depth of Field", &_blur, 1, 0, 50);
      view.blur = _blur;

      static bool _grain = view.grain;
      ImGui::Checkbox("Texture Grain", &_grain);
      view.grain = _grain;

      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::End();
}
