# Territory Makefile

CC = g++-10 -std=c++20
CF = -Wall -O3
#CF = -Wfatal -O
LF = -I/usr/local/include -L/usr/local/lib

# TinyEngine Linking Libraries
TINYLINK = -lX11 -lpthread -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGL -lGLEW -lboost_system -lboost_filesystem

# Make Rule
all: source/territory.cpp
			$(CC) source/territory.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o territory
