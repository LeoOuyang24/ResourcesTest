#include <iostream>
#include <time.h>
#include <SDL.h>

#include "vanilla.h"
#include "geometry.h"
#include "render.h"
#include "FreeTypeHelper.h"
#include "SDLHelper.h"
#include "components.h"

int main(int args, char* argsc[])
{
    const int screenWidth = 640;
    const int screenHeight = 640;
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    GLContext::init(screenWidth,screenHeight);

//SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );

    glEnable(GL_MULTISAMPLE);

    SDL_StopTextInput();
    ViewPort::init(screenWidth,screenHeight);
    //RenderProgram::init(screenWidth,screenHeight);

      //  glEnable(GL_MULTISAMPLE);
//

    //glDisable(GL_DEPTH_TEST);

    Font::init(screenWidth, screenHeight);
    PolyRender::init(screenWidth,screenHeight); //rect, angle, z, effect

    SDL_Event e;
    bool quit = false;
    glClearColor(1,1,1,1);
    bool eventsEmpty = true;


    int dimen = 1;

    //Font alef("projects/alef.ttf");
    //BaseAnimation anime("turtFrog.png",2,5,2, {0,.5,5,1});
    Sprite bunny ("bunny.png");
    //BaseAnimationComponent comp(entity,anime);
    //glDepthMask(GL_FALSE);
    //swarm.addBoid(fish);
    //glEnable(GL_DEPTH_TEST);
    //SDL_GL_SetSwapInterval(0);

    BasicRenderPipeline pipeline;

    pipeline.init("./shader/gravityVertexShader.h","./shader/gravityFragmentShader.h",{2,1,4,4,1},{"./shader/geometryShader.h"},0);
    RenderCamera camera;
    camera.init(screenWidth,screenHeight);

    ViewPort::currentCamera = &camera;
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


        //pipeline.draw(GL_POINTS,glm::vec2(screenWidth/2,screenHeight/2),screenHeight,glm::vec4(1,1,0,1),glm::vec4(1,0,0,0),(SDL_GetTicks()));
        //comp.request(ViewPort::animeProgram,{{0,0,64,64},0});

        //sprite.draw(RenderProgram::basicProgram,data,instances);
        //PolyRender::requestRect({1,0,64,64},{1,0,0,1},true,0,1);
                //Font::tnr.requestWrite({convert(DeltaTime::deltaTime),{0,0,100,100},0,{0,0,0,1},1});
        int dimen = 60;
        for (int i = 0; i < 1; ++i)
        {
            SpriteManager::request(bunny,ViewPort::basicProgram,{{-10,10,-30,-30},(SDL_GetTicks()/100)%102});
        }
        //SpriteManager::request(anime,ViewPort::animeProgram,{glm::vec4(200,300,100,100),1},glm::vec2(5,2));
        ViewPort::update();
        PolyRender::render();
        SpriteManager::render();

        GLContext::update();
        DeltaTime::update();
        //
        //std::cout << DeltaTime::deltaTime << "\n";
        eventsEmpty = true;

    }
    return 0;
}
