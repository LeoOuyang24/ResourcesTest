#include <iostream>
#include <time.h>
#include <SDL.h>

#include <lodepng.h>

#include "vanilla.h"
#include "render.h"
#include "SDLHelper.h"
#include "FreeTypeHelper.h"
#include "physics.h"

int thickness = 1;


void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

void renderGrass(int strands, glm::vec2* seed, int baseX, int baseY, bool lines )
{
        float dimen = 1;
        int increment = 1;
        float now = SDL_GetTicks()/1000.0f;
        bool write = KeyManager::getJustPressed() == SDLK_F6; //whether or not to write to file
        std::vector<unsigned char> image;
        glm::vec2 screenDimen = RenderProgram::getScreenDimen();
        if (write)
        {
            image.resize(4*screenDimen.x*screenDimen.y);
        }
        for (int j =0; j < strands; ++j)
        {
            int length = seed[j].y;
            float customTilt = cos(now - seed[j].x);
            if (lines)
            {
                float angle = cos(now - seed[j].x)/10 - M_PI/2;
                glm::vec2 dist = {baseX + cos(angle)*length, baseY + sin(angle)*length};
                glm::vec2 old = {baseX,baseY};
                for (int i = 0; i < length; i += increment)
                {

                    float x = baseX + i*.25*(customTilt)*pow(3,i/(float)length);
                    float y = baseY - dimen*i;
                    glm::vec4 color = glm::vec4(0,1.0f/length*i,.3,1);

                    PolyRender::requestLine(glm::vec4(old,x,y),color,1,thickness);
                    old ={x,y};
                }
            }
            else
            {
                for (int i=0; i < length; i += increment)
                {

                    float x = floor(baseX + i*.25*(customTilt)*pow(3,i/(float)length));
                    float y = floor(baseY - dimen*i);
                    glm::vec4 color = glm::vec4(0,1.0f/length*i,.3,1);
                    for(int  k = 0; k < thickness ; ++k)
                    {
                    PolyRender::requestRect({x +dimen*k ,
                                                                                  y,
                                                                                  dimen,
                                                                                  dimen},
                                            color,true,0,1);
                        if (write)
                        {
                            image[4*(y*screenDimen.x + (x + dimen*k))] = color.r*255;
                            image[4*(y*screenDimen.x + (x + dimen*k))+1] = color.g*255;
                            image[4*(y*screenDimen.x + (x + dimen*k))+2] = color.b*255;
                            image[4*(y*screenDimen.x + (x + dimen*k))+3] = color.a*255;
                           //std::cout << "INDEX: " << (y*screenDimen.x + x) <<"\n";
                        }
                    }
                }
            }
        }
        if (write)
        {
           // std::cout << image.size() << "\n";
            encodeOneStep("grass.png",image,screenDimen.x,screenDimen.y);
        }
       // std::cout << SDL_GetTicks() -  now *1000<<"\n";
}

struct ChainLink
{
    constexpr static float dimen = 10; //default dimensions of our links
    static float mouseForce;
    static float tension;
    glm::vec2 point;
    glm::vec2 force = {0,0};
    ChainLink* up = 0, *down = 0;
    glm::vec4 color;
    float tilt = 0;
    ChainLink(const glm::vec4& color_, const glm::vec2& rect_, ChainLink* up_, ChainLink* down_, float tilt_ = M_PI/2) : point(rect_), color(color_), tilt(tilt_)
    {
        setLink(up_,up);
        setLink(down_,down);
    }
    void setLink(ChainLink* link, bool doUp) //sets either up or down. doUp is whether or not to set the up link or the down link
    {
        if (link)
        {
            if (doUp)
            {
                up = link;
                link->down = this;
            }
            else
            {
                down = link;
                link->up = this;
            }
        }
    }
    void addForce(const glm::vec2& f)
    {
        force += f;
        if (glm::length(force) > 10)
        {
            force = glm::normalize(force)*10.0f;
        }
    }
    void propagate(const glm::vec2& f, bool goUp)
    {
        ChainLink* next = goUp ? up : down;
        if (next)
        {
            next->addForce(f);
            next->propagate(f,goUp);
        }
    }
    void update()
    {
            if (down)
            {
                //std::cout << mag << "\n";
                glm::vec2 downPoint = down->point;
                if (pointDistance(downPoint,point) > ChainLink::dimen)
                {
                    glm::vec2 chainForce = downPoint - point ;//+ upPoint - point);
                    addForce(tension*(chainForce));
                    glm::vec2 disp = ChainLink::dimen*glm::normalize(downPoint - point);
                    point.x = downPoint.x - disp.x;
                    point.y = downPoint.y - disp.y;
                }
                point.x += force.x;//std::min(std::max(downPoint.x - ChainLink::dimen*sqrt(2.0f),rect.x + finalForce.x),downPoint.x + ChainLink::dimen*sqrt(2.0f));
                //std::cout << finalForce.x << "\n";
                point.y += force.y;//std::min(rect.y + finalForce.y,downPoint.y);
                //propagate(.01f*force,true);
                //propagate(.01f*force,false);
                force = (1.0f - .001f)*force;
                if (glm::length(force) <= .00001f)
                {
                    force = glm::vec2(0);
                }
            }
           // if (!down)
            //std::cout <<force.x << " " << force.y <<"\n";
            //force += .01f*(origin - glm::vec2(rect.x,rect.y));
        //std::cout << glm::length(force) << " " << force.x << " " << force.y<< "\n";
        //printRect(rect);
        //PolyRender::requestRect(rect,color,true,angle,1);
        if(up)
        {
           /* rectPoint= {rect.x + rect.z/2,rect.y + rect.a/2};
            glm::vec2 displacement = glm::vec2(ChainLink::dimen/(sqrt(2.0f)) - ChainLink::dimen/2,-ChainLink::dimen/(sqrt(2.0f)) - ChainLink::dimen/2);
            PolyRender::requestRect(glm::vec4(rectPoint  + displacement,ChainLink::dimen,ChainLink::dimen),
                                            color + glm::vec4(1,0,.4,1),true,angle,1);
            PolyRender::requestRect(glm::vec4(rectPoint + glm::vec2(-ChainLink::dimen/(sqrt(2.0f)) - ChainLink::dimen/2,displacement.y),ChainLink::dimen,ChainLink::dimen),
                                            color + glm::vec4(1,0,.4,1),true,angle,1);
            PolyRender::requestRect(glm::vec4(rectPoint + glm::vec2(0 - ChainLink::dimen/2,-ChainLink::dimen*sqrt(2.0f) -ChainLink::dimen/2),ChainLink::dimen,ChainLink::dimen),color,true,angle,1);*/
            PolyRender::requestLine(glm::vec4(point,up->point ),color,1,1);
        }
        //Font::tnr.requestWrite({convert(tension)),rect});
    }
};

float ChainLink::mouseForce = 1;
float ChainLink::tension = .00001f;

struct ChainTest //linked-list-esque structure that updates an entire chain
{
    ChainLink bottom;
    float tilt;
    ChainTest(glm::vec2 start, glm::vec4 grad1, glm::vec4 grad2, int length, float tilt_ = M_PI/2) : bottom(grad1,glm::vec4(start,ChainLink::dimen,ChainLink::dimen),nullptr,nullptr),
                                                                                                    tilt(tilt_)
    {
        ChainLink* current = &bottom;
        ChainLink* next = nullptr;
        for (int i = 1; i < length; ++i)
        {
            float grad = (float)i/length;
            next = (new ChainLink(grad1 + grad*(grad2 - grad1),{start.x + cos(tilt)*ChainLink::dimen*i,start.y - ChainLink::dimen*i*sin(tilt)},nullptr,current,tilt));
            current->setLink(next,true);
            current = next;
        }
        //bottom.addForce({0,10});
    }
    void addForce(const glm::vec2& force)
    {
        ChainLink* current = &bottom;
        while (current != nullptr)
        {
            current->addForce(force);
            current = current->up;
        }
    }
    void process()
    {

        if (KeyManager::getJustPressed() == SDLK_RIGHT)
        {
            //ChainLink::mouseForce += .1;
            //ChainLink::tension *= 2.0f;
        }
        else if (KeyManager::getJustPressed() == SDLK_LEFT)
        {
            //ChainLink::mouseForce -= .1;
            //ChainLink::tension /= 2.0f;
        }
        ChainLink* current = &bottom;
        while (current != nullptr)
        {
            current->tilt = tilt;
            current->update();
            current = current->up;
        }
        //Font::tnr.requestWrite({convert(ChainLink::tension),{0,0,-1,1}});
    }
};

/*class JellyFish
{
    glm::vec4 color = {.7,0,.4,1};
    constexpr static int tentNum = 5;
    constexpr static int dimen = 40;
    constexpr static int tentLength = 20;
    bool render = true;
    ChainTest* tentaclesTest[tentNum];
    Chain* tentacles[tentNum];
    glm::vec4 rect;
    float speed = .1;
    float angle = 0;
public:
    JellyFish(int x, int y) : rect({x,y,40,40})
    {
        for (int i = 0; i < tentNum; ++i)
        {
            tentaclesTest[i] = new ChainTest({0,0},color,color,tentLength,M_PI/8*i -M_PI/4);
            tentacles[i] = new Chain([](ChainRenderComponent& render,ChainLinkComponent& link){
                                    if (link.getNextLink())
                                    {
                                        PolyRender::requestLine(glm::vec4(link.getLinkPos(),link.getNextLink()->getLinkPos()),render.color,0,1,nullptr);
                                    }
                                     //PolyRender::requestLine(glm::vec4(link->getLinkPos(),link->getLinkPos() + 1000.0f*link->getFinalForce()),{1,0,0,1},0,1,0);
                                   // printRect(glm::vec4(link->getLinkPos(),link->getNextLink()->getLinkPos()));
                                     },color,glm::vec4(1,1,1,0),ChainLink::dimen,tentLength,{0,0},ChainLink::tension);
        }
    }
    void update()
    {
        for (int i = 0; i < tentNum; ++i)
        {
            if (render)
            {
                //tentaclesTest[i]->process();
                     //   std::cout << "DONE\n";
                tentacles[i]->update();
            }
            glm::vec2 tentacleBase = rotatePoint(glm::vec2(rect.x , rect.y + rect.a/(tentNum+1)*(i+1)),glm::vec2(rect.x + rect.z/2,rect.y + rect.a/2),angle);
            tentaclesTest[i]->bottom.point = tentacleBase;
            tentacles[i]->setBottomPos(tentacleBase);

            float tentAngle = angle + M_PI/8*i - M_PI/4;
            //tentacles[i].addForce(-1.0f*glm::vec2(cos(angle),sin(angle)));
        }
        if (KeyManager::getJustPressed() == SDLK_LEFT)
        {
            speed -= .1f;
            std::cout << "JellyFish Speed: " << speed << "\n";
        }
        else if (KeyManager::getJustPressed() == SDLK_RIGHT)
        {
            speed += .1f;
            std::cout << "Jellyfish Speed: " << speed << "\n";
        }
        else if (KeyManager::getJustPressed() == SDLK_SPACE)
        {
            render = !render;
        }
        if (MouseManager::isPressed(SDL_BUTTON_LEFT))
        {
            glm::vec2 mousePos = pairtoVec(MouseManager::getMousePos());
            angle = atan2(mousePos.y - (rect.y + rect.a/2), mousePos.x - (rect.x + rect.a/2));
        }

        rect.x += cos(angle)*speed;
        rect.y += sin(angle)*speed;

        PolyRender::requestRect(rect,color,true,angle,0);

    }
};*/
int main(int args, char* argsc[])
{
    const int screenWidth = 640;
    const int screenHeight = 640;
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* window = SDL_CreateWindow("Project",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,screenWidth, screenHeight, SDL_WINDOW_OPENGL);

//SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );

    SDL_StopTextInput();
    SDL_GL_CreateContext(window);

    RenderProgram::init(screenWidth,screenHeight);

      //  glEnable(GL_MULTISAMPLE);
//

    //glDisable(GL_DEPTH_TEST);

    Font::init(screenWidth, screenHeight);
    PolyRender::init(screenWidth,screenHeight);
    SDL_Event e;
    bool quit = false;
    glClearColor(0,.5,1,1);
    bool eventsEmpty = true;
    float size = .5;

   // Chain chain({screenWidth/2,screenHeight/4*3},10,M_PI/4);

   // JellyFish jelly(320,320);
    std::vector<int> iVec;
    iVec.reserve(screenWidth*screenHeight);
   // ReusableVector<int> rVec;
    DeltaTime alarm;
    alarm.set();
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            eventsEmpty = false;
            KeyManager::getKeys(e);
            MouseManager::update(e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        if (eventsEmpty)
        {
            KeyManager::getKeys(e);
            MouseManager::update(e);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // chain.process();
        //jelly.update();
        //PolyRender::requestRect({screenWidth/2,screenHeight/2,screenWidth/2,screenHeight/2},{1,0,0,1},true,0,1);
        int part = 1;
       // int time = SDL_GetTicks();
        for (int i = 0; i < screenWidth*screenHeight/part/part;++i)
        {
           PolyRender::requestRect(glm::vec4(i%(screenWidth/part)*part,i/(screenWidth/part)*part,part,part),
                                    glm::vec4(i%screenWidth/(float)screenWidth,i/screenWidth/screenWidth,1,1),
                                    true,0,0);
            //iVec[i] = i;

        }
       // printf("%d\n", SDL_GetTicks() - time);
       // int* i = new int(0);
       // delete i;
        //iVec.clear();
       // rVec.clear();
        //chain2.process();

        //PolyRender::requestRect({100,100,100,100},{0,1,0,1},false,M_PI/4,1);
        PolyRender::render();
        SpriteManager::render();
       // Font::alef.write(Font::wordProgram,"asdf",320,320,0,1,{0,0,0});
        SDL_GL_SwapWindow(window);
        DeltaTime::update();
        eventsEmpty = true;
        Font::tnr.requestWrite({convert(DeltaTime::deltaTime),{0,0,100,100},0,{0,0,0,1},1});

        //std::cout << DeltaTime::deltaTime << std::endl;
    }
    return 0;
}
