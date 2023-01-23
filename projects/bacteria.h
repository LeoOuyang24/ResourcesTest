#ifndef BACTERIA_H_INCLUDED
#define BACTERIA_H_INCLUDED

#include "render.h"
#include "components.h"



struct BacteriaColor : public RenderComponent, ComponentContainer<BacteriaColor>
{
    glm::vec4 color;
    float randNum() //used to initialize bacteria color
    {
        return std::min(rand()%1000/1000.0,.5);
    }
    BacteriaColor(Entity& entity) :  RenderComponent(entity,nullptr), ComponentContainer<BacteriaColor>(entity), color(randNum(),randNum(),randNum(),1)
    {

    }
    void update()
    {
        if (RectComponent* rect = entity->getComponent<RectComponent>())
        {
            PolyRender::requestRect(rect->getRect(),color,true,rect->getTilt(),0);
           // PolyRender::requestRect(rect->getBoundingRect(),color + glm::vec4(0,1,0,1),false,0,0);
            //PolyRender::requestNGon(3,rect->getPos(),2,{1,0,0,1},0,true,0);
        }
            color = {0,0,0,1};

    }
    void collide(Entity& other)
    {
        color = {1,0,0,1};
        /*if (auto otherColor = other.getComponent<BacteriaColor>())
        {
            if (glm::length(color) > glm::length(otherColor->color) )
            {
                color = .5f*(otherColor->color + color);
            }
        }*/
    }
};

struct BacteriaMove : public MoveComponent, ComponentContainer<BacteriaMove>
{
    constexpr static int dimen = 10;
    static bool doMove;
    glm::vec4 randRect()
    {
        glm::vec2 screenDimen = RenderProgram::getScreenDimen();
        glm::vec2 dimens = {dimen,dimen};
        return glm::vec4(rand()%(int)(screenDimen.x),rand()%(int)(screenDimen.y ),dimens);
    }
    BacteriaMove(Entity& entity) : MoveComponent(.1,randRect(),entity), ComponentContainer<BacteriaMove>(entity)
    {
        tilt = 0;// (rand()%360*M_PI/180);
    }
    BacteriaMove(int x, int y, float tilt, Entity& entity) : MoveComponent(.1,{x,y,dimen,dimen},entity), ComponentContainer<BacteriaMove>(entity)
    {
        this->tilt = tilt;
    }
    void update()
    {
        if (doMove)
        {
        if (atTarget())
        {
            glm::vec2 screenDimen = RenderProgram::getScreenDimen();
            setTarget({rand()%((int)screenDimen.x),rand()%((int)screenDimen.y)});
        }
        MoveComponent::update();
        }
    }
};

bool BacteriaMove::doMove = true;

Entity* bacteriaAssemble()
{
    Entity* ent = new Entity();
    ent->addComponent(*(new BacteriaMove(*ent)));
    ent->addComponent(*(new BacteriaColor(*ent)));
    return ent;
}

Entity* bacteriaAssemble(int x, int y, float tilt)
{
    Entity* ent = new Entity();
    ent->addComponent(*(new BacteriaMove(x,y,tilt,*ent)));
    ent->addComponent(*(new BacteriaColor(*ent)));
    return ent;
}

#endif // BACTERIA_H_INCLUDED
