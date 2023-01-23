/*

Code here renders a grass simulation. EncodeOneStep writes the pixels to a file.

*/

#include <lodepng.h>

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
