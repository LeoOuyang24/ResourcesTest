/*

Code here represents the protoype of the chain physics. ChainLink and ChainTest represents different ways of simulating chains.
Jellyfish has renders a jellyfish that follows your mouse click.

*/

#include "physics.h"

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
