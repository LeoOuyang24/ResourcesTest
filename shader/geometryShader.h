
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT
{
    vec2 curPos;
    flat in int height;
    vec2 center;
    vec4 centerColor;
    vec4 edgeColor;
    flat in int time;
} gs_in[];

out VS_OUT
{
    vec2 curPos;
    flat out int height;
    vec2 center;
    vec4 centerColor;
    vec4 edgeColor;
    flat out int time;
} fs_in;

in mat4[] projection_;
in mat4[] view_;

void main() {
    /*fs_in = gs_in[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    fs_in = gs_in[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    fs_in = gs_in[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();*/

       /* static constexpr float basicScreenCoords[12] = { //verticies to render a rectangle the size of the screen
        -1,1, //top left
        1,-1, //bot right
        -1,-1, //bot left
        -1,1, //top left
        1,-1, //bot right
        1,1 //top right*/
    for (int j = 0; j < 2; ++j)
    {
        for (int i = 0; i < 3; ++i)
        {
            fs_in = gs_in[0];
            if (mod(i,3) == 0)
            {
                gl_Position = vec4(-1,1,1,1);
            }
            else if (mod(i,3) == 1)
            {
                gl_Position = vec4(1,-1,1,1);
            }
            else
            {
                gl_Position = vec4(mod(j,2)*2 - 1,mod(j,2)*2 - 1,1,1);
            }
            fs_in.curPos = fs_in.center + gl_Position.xy*vec2(fs_in.height/2,-fs_in.height/2);
            //gl_Position = projection_[0]*view_[0]*gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }

}
