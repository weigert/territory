OBJS = source/hexelrender.cpp
CC = g++ -std=c++11
COMPILER_FLAGS = -Wall
LINKER_FLAGS = -I/usr/local/include -L/usr/local/lib -lSDL2 -lnoise -lSDL2_image -lSDL2_ttf -lGL -lGLEW -lboost_serialization -lboost_system -lboost_filesystem
OBJ_NAME = hexelrender
all: $(OBJS)
			$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
