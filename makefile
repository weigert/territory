OBJS = source/territory.cpp source/include/imgui/imgui.cpp source/include/imgui/imgui_demo.cpp source/include/imgui/imgui_draw.cpp source/include/imgui/imgui_widgets.cpp source/include/imgui/imgui_impl_opengl3.cpp source/include/imgui/imgui_impl_sdl.cpp

CC = g++ -std=c++11
COMPILER_FLAGS = -Wall
LINKER_FLAGS = -I/usr/local/include -L/usr/local/lib -lX11 -lpthread -lSDL2 -lnoise -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGL -lGLEW -lboost_serialization -lboost_system -lboost_filesystem
OBJ_NAME = territory
all: $(OBJS)
			$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
