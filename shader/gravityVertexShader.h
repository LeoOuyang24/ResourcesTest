#version 330 core

//used to render gravity fields, use with gravityFragmentShader.h

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 center;
layout (location = 2) in int height;
layout (location = 3) in vec4 centerColor;
layout (location = 4) in vec4 edgeColor;
layout (location = 5) in int time;

out VS_OUT
{
    vec2 curPos;
    flat out int height;
    vec2 center;
    vec4 centerColor;
    vec4 edgeColor;
    flat out int time;
} gs_in;

out mat4 projection_;
out mat4 view_;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


void main()
{
    gs_in.curPos = center;// + aPos.xy*vec2(radius*2,-radius*2); //vertex positions
    //gl_Position = projection*view*vec4( gs_in.curPos,0,1); //screen coordination

    gs_in.height = height;
    gs_in.center = center;
    gs_in.centerColor = centerColor;
    gs_in.edgeColor = edgeColor;
    gs_in.time = time;

    projection_ = projection;
    view_ = view;

}

