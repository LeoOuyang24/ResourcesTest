#version 330 core

//shader for the gravity field of planets
//used with gravityVertexShader

#define M_PI 3.1415926535897932384626433832795




out vec4 FragColor;

in VS_OUT
{
    vec2 curPos;
    flat in int height;
    vec2 center;
    vec4 centerColor;
    vec4 edgeColor;
    flat in int time;
} fs_in;

/*vec4 CENTER_COLOR = vec4(1,1,1,1);
vec4 EDGE_COLOR = vec4(0,1,1,0);*/

float random(float a)
{
   // e^pi (Gelfond's constant)
   // 2^sqrt(2) (Gelfond–Schneider constant)

   //return fract( cos( mod( 12345678., 256. * dot(p,K1) ) ) ); // ver1
   //return fract(cos(dot(p,K1)) * 123456.); // ver2
     return fract(sin(a) * 12345.6789); // ver3
}

float random( vec2 p)
{
     vec2 K1 = vec2( 2.14069263277926, 23.665144142690225 );//vec2( 23.14069263277926, 2.665144142690225 );
    return random(dot(p,K1));
}



void main()
{
    float dist = abs(fs_in.curPos.y - fs_in.center.y);//length((fs_in.curPos - fs_in.center)); //find the distance between interpolated pixel and center
    float angle = atan(fs_in.curPos.y - fs_in.center.y, fs_in.curPos.x - fs_in.center.x) + M_PI;

    float ratio = dist/(fs_in.height);
    float angleRatio = abs(angle)/(2*M_PI)*1.1;


    //FragColor = vec4(random(fs_in.curPos,fs_in.time),random(vec2(fs_in.curPos.y,fs_in.curPos.x),fs_in.time),0,1);
    vec2 moveDist = normalize(vec2(0,1));
    float random = random(fs_in.curPos);
    vec2 target = vec2((fract(random + fs_in.time*0.001)*moveDist.x)*fs_in.height,
                       (fract(random + fs_in.time*0.001)*moveDist.y)*fs_in.height);
    //vec2 target = vec2(fract( + fs_in.time*.001f))*(fs_in.height)*(moveDist) + fs_in.curPos;
    vec2 distance = fs_in.curPos - target - vec2(320,320);
    if (dot(distance,distance) < 100)
    {
        FragColor = vec4(fs_in.curPos.y/fs_in.height,0,0,1);//fs_in.centerColor;
    }
    else
    {
        FragColor = vec4(0,0,0,0);
    }

}
