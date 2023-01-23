#include <iostream>
#include <time.h>
#include <SDL.h>

#include "vanilla.h"
#include "geometry.h"
#include "render.h"
#include "FreeTypeHelper.h"
#include "SDLHelper.h"

int main(int args, char* argsc[])
{
    const int screenWidth = 640;
    const int screenHeight = 640;
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* window = SDL_CreateWindow("Project",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

//SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );

    SDL_StopTextInput();
    SDL_GL_CreateContext(window);

    ViewPort::init(screenWidth,screenHeight);
    //RenderProgram::init(screenWidth,screenHeight);

      //  glEnable(GL_MULTISAMPLE);
//

    //glDisable(GL_DEPTH_TEST);

    Font::init(screenWidth, screenHeight);
    PolyRender::init(screenWidth,screenHeight);

    RenderProgram basicProgram;
    basicProgram.init("../../resources/shaders/vertex/vertexShader.h","../../resources/shaders/fragment/fragmentShader.h",18,{16,1,1});
    Uint32 now = SDL_GetTicks();
    RenderProgram betterProgram;
    betterProgram.init("../../resources/shaders/vertex/betterShader.h","../../resources/shaders/fragment/fragmentShader.h",7,{4,1,1,1}); //rect, angle, z, effect

    SDL_Event e;
    bool quit = false;
    glClearColor(1,1,1,1);
    bool eventsEmpty = true;

    glm::vec2 a = {100,100};
    glm::vec2 b = {200,200};
    glm::vec2 c = {400,100};
    glm::vec2 d = {100,500};

    Sprite sprite("projects/transDiagonal.png");
    Sprite block("projects/block.png");
    Sprite fish ("projects/fish.png");
    Sprite transFish ("projects/trans_fish.png");
    Sprite diag ("projects/diagonal.png");

    int dimen = 1;
    int instances = screenWidth*screenHeight/dimen/dimen;

    Font alef("projects/alef.ttf");

    float* data = new float[instances*7];
    //glDepthMask(GL_FALSE);
    //swarm.addBoid(fish);
    //glEnable(GL_DEPTH_TEST);
    //SDL_GL_SetSwapInterval(0);
    glm::vec2 score = {0,0};
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            eventsEmpty = false;
            KeyManager::update(e);
            MouseManager::update(e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        if (eventsEmpty)
        {
            KeyManager::update(e);
            MouseManager::update(e);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //sprite.draw(RenderProgram::basicProgram,data,instances);
        //PolyRender::requestRect({1,0,64,64},{1,0,0,1},true,0,1);
        //PolyRender::render();
                //Font::tnr.requestWrite({convert(DeltaTime::deltaTime),{0,0,100,100},0,{0,0,0,1},1});
        //SpriteManager::request(transFish,betterProgram,glm::vec4(200,300,100,100));
        SpriteManager::request(sprite,betterProgram,{glm::vec4(200,200,100,100),1});
        SpriteManager::request(block,betterProgram,{glm::vec4(200,200,100,100)});
        SpriteManager::request(transFish,betterProgram,{glm::vec4(300,500,100,100)});
        SpriteManager::request(diag,betterProgram,{glm::vec4(100,100,100,100)});

        alef.requestWrite({"asdf",glm::vec4(100,100,100,100),0,1});

        SpriteManager::render();

        SDL_GL_SwapWindow(window);
        DeltaTime::update();
        //

        eventsEmpty = true;


        //std::cout << DeltaTime::deltaTime << "\n";
    }
    return 0;
}
