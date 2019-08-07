// Compile : clang++ main.cpp -o Part_03 -lGL -lSDL2
//
#include <iostream>
#include <string>

#include "Shader.h"

#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>

#define PROGRAM_NAME "Tutorial2"

std::string programName = "Headerphile SDL2 - OpenGL thing";

// Our SDL_Window ( just like with SDL2 wihout OpenGL)
SDL_Window *mainWindow;

// Our opengl context handle
SDL_GLContext mainContext;

// Our object has 4 points
const uint32_t points = 12;

// Each poin has three values ( x, y, z)
const uint32_t floatsPerPoint = 3;

// Each color has 4 values ( red, green, blue, alpha )
const uint32_t floatsPerColor = 4;

// This is the object we'll draw ( a simple square
const GLfloat diamond[points][floatsPerPoint] = {
    {  0.2,  0.2,  0.5 }, // Top right
    { -0.2,  0.2,  0.5 }, // Top left
    {  0.0,  0.0,  0.5 }, // Center

    {  0.2,  0.2,  0.5 }, // Top right
    {  0.2, -0.2,  0.5 }, // Bottom right
    {  0.0,  0.0,  0.5 }, // Center

    { -0.2, -0.2,  0.5 }, // Bottom left
    {  0.2, -0.2,  0.5 }, // Bottom right
    {  0.0,  0.0,  0.5 }, // Center

    { -0.2, -0.2,  0.5 }, // Bottom left
    { -0.2,  0.2,  0.5 }, // Top left
    {  0.0,  0.0,  0.5 }, // Center
};

// This is the object we'll draw ( a simple square
const GLfloat colors[points][floatsPerColor] = {
    { 0.5, 0.5, 0.5, 1.0f }, // Top right
    { 0.5, 0.5, 0.5, 1.0f }, // Bottom right
    { 0.0, 0.0, 0.0, 1.0f }, // Center

    { 0.5, 0.5, 0.5, 1.0f }, // Top left
    { 0.5, 0.5, 0.5, 1.0f }, // Top right
    { 0.0, 0.0, 0.0, 1.0f }, // Center

    { 0.5, 0.5, 0.5, 1.0f }, // Bottom left
    { 0.5, 0.5, 0.5, 1.0f }, // Bottom right
    { 0.0, 0.0, 0.0, 1.0f }, // Center

    { 0.5, 0.5, 0.5, 1.0f }, // Bottom left
    { 0.5, 0.5, 0.5, 1.0f }, // Top left
    { 0.0, 0.0, 0.0, 1.0f }, // Center
};

// Create variables for storing the ID of our VAO and VBO
GLuint vbo[2], vao[1];

// The positons of the position and color data within the VAO
const uint32_t positionAttributeIndex = 1, colorAttributeIndex = 0;

// Our wrapper to simplify the shader code
Shader shader;

bool SetOpenGLAttributes();
void PrintSDL_GL_Attributes();
void CheckSDLError(int line);

void Render()
{
    // First, render a square without any colors ( all vertexes will be black )
    // ===================
    // Make our background black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Invoke glDrawArrays telling that our data is a line loop and we want to draw 4 vertexes
    glDrawArrays(GL_TRIANGLES, 0, points);

    // Swap our buffers to make our changes visible
    SDL_GL_SwapWindow(mainWindow);
}
bool SetupBufferObjects()
{
    // Generate and assign two Vertex Buffer Objects to our handle
    glGenBuffers(2, vbo);

    // Generate and assign a Vertex Array Object to our handle
    glGenVertexArrays(1, vao);

    // Bind our Vertex Array Object as the current used object
    glBindVertexArray(vao[0]);

    // Colors
    // =======================
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    // Copy the vertex data from diamond to our buffer
    glBufferData(GL_ARRAY_BUFFER, ( points * floatsPerColor) * sizeof(GLfloat), colors, GL_STATIC_DRAW);

    // Specify that our coordinate data is going into attribute index 0, and contains three floats per vertex
    glVertexAttribPointer(colorAttributeIndex, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // ===================
    // Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    // Copy the vertex data from diamond to our buffer
    glBufferData(GL_ARRAY_BUFFER, ( points * floatsPerPoint) * sizeof(GLfloat), diamond, GL_STATIC_DRAW);

    // Specify that our coordinate data is going into attribute index 0, and contains three floats per vertex
    glVertexAttribPointer(positionAttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable our attributes within the current VAO
    glEnableVertexAttribArray(positionAttributeIndex);
    glEnableVertexAttribArray(colorAttributeIndex);

    // Set up shader ( will be covered in the next part )
    // ===================
    if (!shader.Init())
        return false;

    shader.UseProgram();

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}
bool Init()
{
    // Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to init SDL\n";
        return false;
    }

    // Create our window centered at 512x512 resolution
    mainWindow = SDL_CreateWindow(programName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512, 512, SDL_WINDOW_OPENGL);

    // Check that everything worked out okay
    if (!mainWindow )
    {
        std::cout << "Unable to create window\n"<< std::endl;;
        CheckSDLError(__LINE__);
        return false;
    }

    SetOpenGLAttributes();

    // Create our opengl context and attach it to our window
    mainContext = SDL_GL_CreateContext(mainWindow );

    // This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

    // Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

bool SetOpenGLAttributes()
{
    // Set our OpenGL version.
    // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    return true;
}

void Cleanup()
{
    // Cleanup all the things we bound and allocated
    shader.CleanUp();

    glDisableVertexAttribArray(0);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, vao);

    // Delete our OpengL context
    SDL_GL_DeleteContext(mainContext);

    // Destroy our window
    SDL_DestroyWindow(mainWindow );

    // Shutdown SDL 2
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    if (!Init())
        return -1;

    // Clear our buffer with a grey background
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(mainWindow );

    std::cout << "Seting up VBO + VAO..." << std::endl;

    if (!SetupBufferObjects())
        return -1;

    std::cout << "Rendering..." << std::endl;
    Render();

    std::cout << "Rendering done!\n";
    std::cin.ignore();

    Cleanup();

    return 0;
}

void CheckSDLError(int line = -1)
{
    std::string error = SDL_GetError();

    if (error != "")
    {
        std::cout << "SDL Error : " << error << std::endl;

        if (line != -1)
            std::cout << "\nLine : " << line << std::endl;

        SDL_ClearError();
    }
}

void PrintSDL_GL_Attributes()
{
    int value = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
    std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION : " << value << std::endl;

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
    std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << std::endl;
}
