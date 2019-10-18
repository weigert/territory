#pragma once
#include "../forward/interface.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/player.fwd.h"
#include "../forward/sprite.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/state.fwd.h"
#include "../forward/memory.fwd.h"
#include "../forward/task.fwd.h"
#include "../forward/view.fwd.h"
#include "../forward/item.fwd.h"

class Interface{
public:
  int curID;
  //Render the Interface Elements
  void render(View &view, World &world, Population &population, Player &player);

  //Extra Stuff
  void drawBot(Bot &bot);
  void drawTask(Task *task);
  void drawState(State state);
  void drawItem(Item item);

  //I also need creator interfaces for all these guys!!
  void createItem(Item &item);
  void createState(State &state);
  void createTask(Task &task);
};

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
      ImGui::Text("%s", bot.inventory[i].name.c_str());
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
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", (int)state.dist);
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
  //Do some Drawing
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

      if(ImGui::Button("Get Camera Position")){
        //
      }
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

      /*
      Need to be able to add tasks.
      Need to be able to add states quickly.
      Need to be able to construct items somehow.

      The best way to do this:
      Select a specific bot, and create a task using a task creation interface.
      Then push the task onto their current queue.

      There should also be some kind of inventory tab, where items can be deleted and added.

      */

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
          ImGui::EndPopup();
      }

      if (ImGui::BeginPopupModal("TaskAdder", NULL)){
          //Construct a Task
          static int _taskHandle = TASK_NULL;
          const char* handles[] = { "Null", "Look", "Listen", "Think", "Wait", "Move", "Step", "Walk", "Idle", "Follow", "Collect", "Take", "Find", "Search", "Retrieve", "Convert", "Decide", "Request" };
          ImGui::Combo("Task", &_taskHandle, handles, IM_ARRAYSIZE(handles), 4);

          //Do this guy here
          static int _time = 0.0f;
          ImGui::DragInt("Time", &_time, 1, 0, 100);

          static int _block = 0.0f;
          ImGui::DragInt("Block", &_block, 1, 0, 9);

          //Start the Menu
          if (ImGui::Button("Submit")){
            //Add the item to the inventory.
            Task *_task = new Task(TaskName[_taskHandle].c_str(), a, (TaskHandle)_taskHandle);

            //Construct a State
            State state;
            state.task = _task->name;
            state.time = _time;
            if(_taskHandle == TASK_WALK){
              state.pos = view.select+glm::vec3(0, 1, 0);
            }
            else{
              state.pos = view.select;
            }
            state.block = (BlockType)_block;

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
      //Projection Matrix
      static bool projectionMatrix = false;
      ImGui::Checkbox("Projection Matrix", &projectionMatrix);
      if(projectionMatrix){
        view.projection = glm::perspective(glm::radians(50.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
      }
      else{
        view.projection = glm::ortho(-(float)view.SCREEN_WIDTH*view.zoom, (float)view.SCREEN_WIDTH*view.zoom, -(float)view.SCREEN_HEIGHT*view.zoom, (float)view.SCREEN_HEIGHT*view.zoom, -200.0f, 200.0f);
      }


      //Add an FPS Plot
      ImGui::PlotLines("FPS Counter", view.arr, IM_ARRAYSIZE(view.arr));

      //Generate a colorpicker for the light and sky
      static float sky[3] = {view.skyCol.x, view.skyCol.y, view.skyCol.z};
      ImGui::ColorEdit3("Sky Color", sky);
      view.skyCol = glm::vec3(sky[0], sky[1], sky[2]);

      //col2[3] = {view.skyCol.x, view.skyCol.y, view.skyCol.z};
      static float light[3] = {view.lightCol.x, view.lightCol.y, view.lightCol.z};
      ImGui::ColorEdit3("Light Color", light);
      view.lightCol = glm::vec3(light[0], light[1], light[2]);

      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::End();
}
