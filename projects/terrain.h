#include "render.h"
#include "glGame.h"

#include "stb_image.h"

/*
    Terrain and fish swarm simulator. Fish swarming behavior works without terrain, but the pathfinding around the terrain needs work.
     Run by instantiating a TerrainMain (blank constructor) and then running TerrainMain.run() every update loop
*/

struct Terrain
{
    int space = 16; //space between points
    std::vector<GLfloat> triangles; //vector of the points that form the triangles
    std::vector<GLfloat> colors;
    bool* points = 0; //unique vector of points that are collidable
    glm::vec4 region;
    int horizPoints = 0,vertPoints = 0; //number of points in each dimension
    unsigned int VAO;
    unsigned int vertexVBO;
    unsigned int colorVBO;
    int pointsSize() //return number of points in "points" array.
    {
        return (horizPoints + 1)*(vertPoints + 1); // + 1 for points on the borders
    }
    void setup(std::string source)
    {
        int width, height, channels;
        unsigned char* data = stbi_load(source.c_str(),&width, &height, &channels, 0);
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&vertexVBO);
        glGenBuffers(1,&colorVBO);
        region = {0,0,width,height};
        horizPoints = width/space;
        vertPoints = height/space;
        points = new bool[pointsSize()]; //also have to reserve space for points on the borders
        auto helper = [this,width,height,&data](int topLeft){ //topLeft should be the index of the top left corner
            char counter = 0; //use the first 4 bits to represent which points are wall points (topLeft,topRight,botLeft,botRight from most significant to least significant bit). The bits more significant than the first 4 represent how many points are wall points
            int channels = 4;
            int size = width*height*channels; //size of data
            for (int i = 0; i < 4; ++i)
            {
                int index = channels*space*((topLeft%horizPoints)  + (topLeft/horizPoints)*width + (i%2) + width*(i/2)) + channels - 1 ;
                //std::cout << index << "\n";
                bool notTransparent =
                                    data[index] != 0; //is not transparent
                if (notTransparent)      //check each of the 4 corners to see if they are transparent
                {
                    counter += (1 << i); //set the ith least significant bit to 1
                    counter += (1 << 4); //increment our counter for how many points are wall points
                }
                int x = topLeft%horizPoints + i%2;
                int y = (topLeft/horizPoints + i/2)*(horizPoints+1);
                points[y + x] = notTransparent;

            }
            //std::cout << (counter >> 4) << "\n";
            if (counter >> 4 == 4) //4 points
            {
                int index = channels*space*((topLeft%horizPoints)  + (topLeft/horizPoints)*width) + channels - 1 ;
                int x = topLeft%horizPoints*space;
                int y = topLeft/horizPoints*space;
                triangles.push_back(x);
                triangles.push_back(y);
                triangles.push_back(0);

                triangles.push_back(x + space);
                triangles.push_back(y);
                triangles.push_back(0);

                triangles.push_back(x + space);
                triangles.push_back(y + space);
                triangles.push_back(0);

                triangles.push_back(x);
                triangles.push_back(y);
                triangles.push_back(0);

                triangles.push_back(x);
                triangles.push_back(y + space);
                triangles.push_back(0);

                triangles.push_back(x + space);
                triangles.push_back(y + space);
                triangles.push_back(0);
                for (int j = 0; j < 6; ++j)
                {
                    colors.push_back(data[index - 3]/255.0);
                    colors.push_back(data[index - 2]/255.0);
                    colors.push_back(data[index - 1]/255.0);
                    colors.push_back(1);
                }
            }
            else if (counter >> 4 == 3) //3 points
            {
                int base = channels*space*((topLeft%horizPoints)  + (topLeft/horizPoints)*width);
                for (int i = 0; i <4; ++i)
                {
                    if ((counter & (1 << i)) != 0) //if the ith point is a wall
                    {
                        int index = base + channels*space*((i%2) + width*(i/2));
                        int x = space*((topLeft)%(horizPoints) + i%2);
                        int y = space*((topLeft)/horizPoints + (i/2));
                        //PolyRender::request
                        //int w = -space/2*((i)%2*2-1);
                        //int h = -space/2*((i/2)*2 - 1);
                        triangles.push_back(x);
                        triangles.push_back(y);
                        triangles.push_back(0);

                        //std::cout  << base << " " << i << " " << index << " " <<  x << " " << y << "\n";

                        colors.push_back(data[index]/255.0);
                        colors.push_back(data[index+1]/255.0);
                        colors.push_back(data[index+2]/255.0);
                        colors.push_back(1);
                    }
                }

            }
        };
        for (int i = 0; i < horizPoints*vertPoints; ++i)
        {
            helper(i);
        }
    }
    void update()
    {
        if (triangles.size() > 0)
        {
            glDisable(GL_PRIMITIVE_RESTART);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER,vertexVBO);
            glBufferData(GL_ARRAY_BUFFER,triangles.size()*sizeof(GLfloat),&triangles[0],GL_STATIC_DRAW);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
            glEnableVertexAttribArray(0);
            //glVertexAttribDivisor(0,1);

            glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
            glBufferData(GL_ARRAY_BUFFER,colors.size()*sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);
            glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,(void*)0);
            glEnableVertexAttribArray(1);
            //glVertexAttribDivisor(1,1);

            PolyRender::polyRenderer.use();
            glDrawArrays(GL_TRIANGLES,0,triangles.size()/3);

            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindVertexArray(0);

            glEnable(GL_PRIMITIVE_RESTART);

            //renderMesh();
        }

    }
    int pointToIndex(const glm::vec2& point)
    {
        return ((int)point.x)/space + ((int)point.y)/space*(horizPoints + 1);
    }
    glm::vec2 indexToPoint(int index)
    {
        return {index%(horizPoints + 1)*space,index/(horizPoints + 1)*space};
    }
    template<typename Func>
    auto processPoint(const glm::vec2& point, Func f) //given a point, find the 4 verticies  that contain the point and run "f" on the 4 verticies
    {
        //WARNING: Unsafe if point is in the mesh. Will crash
        //point is a point in the real world
        //f = ( const glm::vec2&, int, glm::vec2[]) -> auto
        //point to process, number of wall verticies, array of wall verticies
        int index = pointToIndex(point);
        int sum = points[index] + points[index + 1] + points[index + horizPoints + 1] + points[index + horizPoints + 1 + 1]; //number of points that are actual walls
        glm::vec2* verticies = new glm::vec2[sum];
        int ind = 0;
        for (int i = 0; i < 4; ++i)
        {
            int newIndex = index + i%2 + i/2*(horizPoints + 1);
            if (points[newIndex])
            {
                verticies[ind] = indexToPoint(newIndex);
                ind++;
            }
        }
        if constexpr(std::is_void<Func()>::value)
        {
            f(point, sum, verticies);
            delete[] verticies;
        }
        else
        {
            auto result = f(point,sum,verticies);
            delete[] verticies;
            return result;
        }
    }
    bool inWall(const glm::vec2& point)
    {
        if (points == nullptr || !pointInVec(region,point)) //if no terrain or if point is outside our region, point can't be in wall
        {
            return false;
        }
        int index = pointToIndex(point); //top left index
        int sum = points[index] + points[index + 1] + points[index + horizPoints + 1] + points[index + horizPoints + 1 + 1];
       /* for (int i = 0; i < 4; ++i)
        {
            int newIndex = index + i%2 + i/2*(horizPoints + 1);
           // if (points[newIndex])
            {
                PolyRender::requestNGon(10,{newIndex%(horizPoints + 1)*space,newIndex/(horizPoints + 1)*space},1,{points[newIndex],0,0,1},0,true,1,true);
            }
        }*/
        if (sum == 4)
        {
            return true;
        }
        else if (sum == 3)
        {
            if (points[index] && points[index + horizPoints + 1 + 1])
            {
                glm::vec2 standard = {fmod(point.x,space), fmod(point.y,space)};
                return points[index + 1] ?
                        (standard.y < standard.x) :
                        (standard.y > standard.x);
            }
            else if (points[index + 1] && points[index + horizPoints + 1])
            {
                glm::vec2 standard = {fmod(point.x,space), space - fmod(point.y,space)};
                return points[index] ?
                        (standard.y > standard.x) :
                        (standard.y < standard.x);
            }
        }
        return false;
    }
    bool inWall(const glm::vec2& p1, const glm::vec2& p2) //returns true if the line is in a wall
    {
        if (points == nullptr  || !lineInVec(p1,p2,region))
        {
            return false;
        }
        glm::vec2 slope = betterNormalize(p2 - p1);
        glm::vec2 start = p1; //first point we will consider
        auto lambda = [start,p2](const glm::vec2& point, int sum, glm::vec2 points[]){
                        bool value = false;
                        switch (sum)
                            {
                            case 4: //if all 4 points are wall, there is no way "start" is not in the wall since the whole chunk is wall
                                value = true;
                                break;
                            case 3: //the points form a triangular wall, so check for collision with the triangle
                             value = lineInLine(start,p2,points[0],points[1]) ||
                                    lineInLine(start,p2,points[1],points[2]) ||
                                    lineInLine(start,p2,points[2],points[0]);
                                    break;
                            case 2: //the points form a line (probably a side of another wall) so check collision with the line
                                value = lineInLine(start,p2,points[0],points[1]);
                                break;
                            default: //1 or 0 points, no such wall
                                break;
                            };
                         return value;
                         }; //lambda used to figure out if point is in a wall
        while (true) //process every point along the line every "space" intervals, where "space" is the space between points
        {
            //std::cout << start.x << " " << start.y <<  " " << p2.x << " " << p2.y << " " << glm::distance(start,p2) << "\n";
            if (processPoint(start,lambda)) //if we found a point that collides with a wall, return true
            {
                return true;
            }
            if (start == p2 || glm::distance(start,p2) <= .0001) //reached the final point and didn't collide with a wall so we are done
            {
                break;
            }
            if (glm::distance(start,p2) <= .0001) //if we are already very close to "p2", clamp onto "p2". Setting "start" = "p2" prevents floating point errors
            {
                start = p2;
            }
            else  //otherwise, move onto the next point that is "space" away or clamp onto "p2"
            {
                start += std::min((float)space,glm::distance(start,p2))*slope;
            }
            if (!pointInVec(region,start))
            {
               break;
            }
            //std::cout << start.x << " " << start.y << " " << slope.x << " " << slope.y<< "\n";
        }
        return false; //no points collide with wall, no wall collision
    }
    void renderMesh(const glm::vec4& wall = {1,0,0,1}, const glm::vec4& none = {0,0,0,1}) //renders a small circle at each point with color "wall" if the point is true, "none" if the point is none
    {
        for (int i = 0; i < pointsSize(); ++i)
        {
            PolyRender::requestNGon(10,indexToPoint(i),1,(points[i] ? wall : none),0,true,0);
        }
    }
};

class TerrainMoveComponent : public MoveComponent, public ComponentContainer<TerrainMoveComponent>
{
    Terrain* terrain;
public:
    TerrainMoveComponent(float speed, const glm::vec4& rect, Entity& entity, Terrain& terr) : terrain(&terr),
                                                                                                MoveComponent(speed,rect, entity),
                                                                                                ComponentContainer<TerrainMoveComponent>(entity)
    {

    }
    void setTarget(const glm::vec2& target)
    {
        MoveComponent::setTarget(target);
        setTiltTowardsTarget();
    }
    glm::vec2 getNextPoint()
    {
        glm::vec2 center = getCenter();

        glm::vec2 fourCorners[4] = {rotatePoint(glm::vec2(rect.x,rect.y),center,tilt),
                                    rotatePoint(glm::vec2(rect.x + rect.z,rect.y),center,tilt),
                                    rotatePoint({rect.x + rect.z, rect.y + rect.a},center, tilt),
                                    rotatePoint({rect.x,rect.y + rect.a}, center, tilt)
                                    };
        for (int i = 0; i < 4; ++i)
        {
            if (terrain->inWall(fourCorners[i],fourCorners[(i+1)%4]))
            {
                PolyRender::requestLine(glm::vec4(fourCorners[i],fourCorners[(i+1)%4]),glm::vec4(0,1,0,1),1,1);
                //PolyRender::requestNGon(10,fourCorners[i],5,{1,0,0,1},0,true,1,true);
                return {rect.x,rect.y};
            }
        }
        return MoveComponent::getNextPoint();
    }
};

class BasicMoveComponent : public MoveComponent, public ComponentContainer<BasicMoveComponent>
{
    static constexpr float speedLimit = .5;
protected:
    glm::vec2 vel = {(rand()%10 - 5),(rand()%10 - 5)};
public:
    static bool doMove;

    BasicMoveComponent(float speed, const glm::vec4& rect, Entity& ent) : MoveComponent(speed,rect,ent),
                                        ComponentContainer<BasicMoveComponent>(ent)
    {
        vel = std::min(speedLimit,speed)*betterNormalize(vel);

    }
    glm::vec2 getVel()
    {
        return vel;
    }
    void addVel(const glm::vec2& newVel)
    {
        vel += newVel;
        if (glm::distance(vel,glm::vec2(0,0)) > speedLimit)
        {
            vel = speedLimit*betterNormalize(vel);
        }
        //std::cout << vel.x << " " << vel.y << "\n";

    }
    virtual glm::vec2 getNextPoint()
    {
        glm::vec2 screenDimen = RenderProgram::getScreenDimen();
        return {fmod(fmod(rect.x + DeltaTime::deltaTime*vel.x,screenDimen.x - rect.z/2) + screenDimen.x - rect.z/2,screenDimen.x - rect.z/2),
        fmod(fmod(rect.y + DeltaTime::deltaTime*vel.y,screenDimen.y - rect.a/2) + screenDimen.y - rect.a/2,screenDimen.y - rect.a/2)};
    }
    void update()
    {

        if (doMove)
        {
         glm::vec2 nextTarget = getNextPoint();

         glm::vec2 center = {rect.x + rect.z/2, rect.y + rect.a/2};
         PolyRender::requestLine(glm::vec4(center,center + 10.0f*(nextTarget - glm::vec2(rect.x,rect.y))),glm::vec4(1,0,0,1),0,1,0);

         rect.x = nextTarget.x;
         rect.y = nextTarget.y;
        }

        //tilt = atan2(vel.y,vel.x);
    }
};

bool BasicMoveComponent::doMove = false;

class RayCastMoveComponent : public BasicMoveComponent, public ComponentContainer<RayCastMoveComponent>
{
    constexpr static float maxAngle = M_PI/4;
    Terrain* terrain;
public:
    RayCastMoveComponent(float speed, const glm::vec4& rect, Entity& entity, Terrain& terr) : BasicMoveComponent(speed,rect,entity),
                                                                                                ComponentContainer<RayCastMoveComponent>(entity),
                                                                                                terrain(&terr)
    {
        ignoreTarget = true;
    }
    glm::vec2 getNextPoint()
    {
        float addition = 0;
        glm::vec2 center = getCenter();
        glm::vec2 topLeft = rotatePoint(glm::vec2(rect.x,rect.y),center,tilt);

        while (addition < 2*M_PI)
        {
            glm::vec2 disp = (rect.z + 10.0f)*betterNormalize(vel); //find a point a distance of 10 in front of us in the direction we are moving
            glm::vec2 nextPoint = center + disp;
            if (!terrain->inWall(center,nextPoint)) //send out a ray to see if we collide
            {
                PolyRender::requestLine(glm::vec4(center,center + disp),glm::vec4(1,0,0,1),0,1,0);
                return BasicMoveComponent::getNextPoint();
            }
            float newAngle = tilt + addition; //new angle to consider moving towards
            vel = glm::length(vel)*glm::vec2(cos(newAngle),sin(newAngle)); //new velocity, calculated by taking our current speed and multiplying by the vector of our new direction
            addition += M_PI/360;
        }
        PolyRender::requestRect(rect,glm::vec4(1,0,0,1),true,0,1);
        return {rect.x,rect.y};
    }
};

class FishRenderComponent : public SpriteComponent
{
    glm::vec4 tint = {0,0,0,1};
public:
    FishRenderComponent(SpriteWrapper& sprite, bool animated, Entity& entity) : SpriteComponent(sprite,animated, entity, nullptr)
    {

    }
    void update()
    {
        SpriteParameter s = defaultRender();
        s.tint = tint;
        setParam(s);
        SpriteComponent::update();
    }
};

class Swarm
{
    glm::vec2 avgPos = glm::vec2(0);
    glm::vec2 avgVel = glm::vec2(0); //first element is angle, second is magnitude
    typedef RayCastMoveComponent Boid;
    BiTree bitree;
    SpatialGrid grid;
    std::vector<Boid*> boids;
    glm::vec4 region;
    constexpr static int viewRange = 1000;
    glm::vec2 separation (Boid& boid)
    {
        int sep = 50; //how far boids have to be to interact with one another
        glm::vec2 vec = {0,0};
        /*for (int i = 0; i < boids.size(); ++i)
        {
            if (boids[i] != &boid)
            {
                float distance = boids[i]->distance(boid.getCenter());
                if (distance <= sep )
                {
                    vec += (boid.getCenter() - boids[i]->getCenter());
                }
            }
        }*/
        /*bitree.findNearest(boid,sep,[&vec,pos = boid.getPos()](Positional& p) mutable {
                           vec += (pos - p.getPos());
                           });*/
        grid.findNearest(boid.getCenter(),sep,[&vec,pos = boid.getPos(),&boid](Positional& p) mutable {
                           if (&p != &boid)
                           {
                               vec += (pos - p.getPos());
                           }
                           });
        return vec ;
    }
    glm::vec2 averagePos(Boid& boid)
    {
        if (boids.size() == 1)
        {
            return boid.getCenter();
        }
        glm::vec2 vec = {0,0};
        int counter = 0; //number of birds nearby
        /*for (int i = 0; i < boids.size(); ++i)
        {
            if (boids[i] != &boid && distance(boids[i]->getCenter(),boid.getCenter()) < viewRange)
            {
                vec += boids[i]->getPos();
            }
        }*/

        grid.findNearest(boid.getCenter(),viewRange,[&vec,&counter](Positional& p) mutable {
                         vec += p.getPos();
                         counter ++;
                         });
        return (1.0f/(counter - 1))*(vec);
    }
    glm::vec2 averageVel(Boid& boid)
    {
        if (boids.size() == 1)
        {
            return boid.getVel();
        }
        glm::vec2 vec = {0,0};
        int counter = 0;
        /*for (int i = 0; i < boids.size(); ++i)
        {
            if (boids[i] != &boid && distance(boids[i]->getCenter(),boid.getCenter()) < viewRange)
            {
                vec += boids[i]->getVel();
            }
        }*/
        grid.findNearest(boid.getCenter(),viewRange,[&vec,&counter](Positional& p) mutable {
                 vec += static_cast<Boid&>(p).getVel();
                 counter ++;
                 });
        return (1.0f/(counter - 1))*vec;
    }
public:
    Swarm(const glm::vec4& region_) : bitree(region_), region(region_), grid(50)
    {

    }
    void addBoid(Entity& ent)
    {
        if(Boid* rect= ent.getComponent<Boid>())
        {
            boids.push_back(rect);
            grid.insert(*rect);
            //bitree.insert(*rect);
        }
    }
    void update()
    {
        int size = boids.size();
        glm::vec2 mousePos = pairtoVec(MouseManager::getMousePos());
        bool leftClicked = (MouseManager::isPressed(SDL_BUTTON_LEFT));
        bool rightClicked = MouseManager::isPressed(SDL_BUTTON_RIGHT);
        glm::vec2 newPos = glm::vec2(0), newVel = glm::vec2(0);
        int count = 0;

        auto func = [this,&newPos,&newVel,&count,mousePos,leftClicked,rightClicked](Positional& p) mutable {
                Boid* b = static_cast<Boid*>(&p);
                glm::vec2 moveVec = glm::vec2(0,0);
                moveVec += (leftClicked || rightClicked)*(1 - rightClicked*2)*.0001f*(mousePos - b->getPos()); //if left clicked, move towards mouse, if right clicked, move away from mouse, if neither do nothing

                //moveVec += 0.00005f*averagePos(*boids[i]);
                if (boids.size() > 1)
                {
                    moveVec += .00001f*separation(*b);
                    moveVec += .00005f*(avgPos - b->getPos());
                    moveVec += .00005f*(avgVel - b->getVel());
                  //std::cout << boids.size() << " " << moveVec.x << " " << moveVec.y << " " <<avgPos.x << " " << avgPos.y << " " << avgVel.x <<" " << avgVel.y << "\n";
                }

                b->addVel(moveVec);
                auto oldPos = b->getBoundingRect();
                b->getEntity().update();
                grid.update(*b,oldPos);
                newPos += b->getPos();
                newVel += b->getVel();
                count ++;
                            };
        //bitree.updateAll(func);
        //bitree.selectUpdate(region,func);
        /*for (int i = 0; i < size; ++i)
        {
           // grid.insert(*boids[i]);
         //   bitree.insert(*boids[i]);
        }*/
        //grid.showNodes();
        for (int i = 0; i < size; ++i)
        {
            func(*boids[i]);
            //grid.insert(*boids[i]);
        }
        //grid.clear();
        //bitree.clear();
        avgPos = newPos*(1.0f/size);
        avgVel = newVel*(1.0f/size);
    }
};

struct TerrainMain
{
    Swarm* swarm;
    Terrain terrain;
    SpriteWrapper fishSprite;

    TerrainMain()
    {
        fishSprite.init("projects/fish.png");

        glm::vec2 screenDimen = RenderProgram::getScreenDimen();
        swarm = new Swarm({0,0,screenDimen.x,screenDimen.y});
        terrain.setup("projects/large_cave.png");

        std::vector<Entity*> fishes;
        int amount = 1000;
        int fishDimen = rand()%32 + 32;
        auto getSpawnPoint = [this,fishDimen, &screenDimen](int i)
        {
            glm::vec2 result = {rand()%((int)screenDimen.x - 30) + 30,rand()%((int)screenDimen.y - 30) + 30};
            while (terrain.inWall(result) || terrain.inWall(result + glm::vec2(0,fishDimen)) || terrain.inWall(result + glm::vec2(fishDimen,0)) || terrain.inWall(result + glm::vec2(fishDimen,fishDimen)))
            {
                result = {rand()%((int)screenDimen.x - 30) + 30,rand()%((int)screenDimen.y - 30) + 30};
            }
            return glm::vec4(result,fishDimen,fishDimen);
        }; //generate a point not in terrain
        for (int i = 0; i < amount; ++i)
        {
            Entity* fish = new Entity();
            fish->addComponent(*(new RayCastMoveComponent(1,getSpawnPoint(i),*fish,terrain)));
            //fishes[i]->addComponent(*(new BasicMoveComponent(0,{rand()%(screenWidth - 30) + 30,rand()%(screenHeight - 30) + 30,rand()%56 + 8,rand()%28 + 4},*fishes[i]))); //{i ? 686 : 810, i ? 586 : 939,32,16}
            fish->addComponent(*(new FishRenderComponent(fishSprite,false,*fish)));
            swarm->addBoid(*fish);
        }
    }
    void run()
    {
        swarm->update();
        terrain.update();
    }

};

