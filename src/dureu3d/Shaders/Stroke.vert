#version 150
#extension GL_EXT_geometry_shader4 : enable

varying out vec4 gsColor;

void main()
{
    //Transform the vertex (ModelViewProj matrix)
    gsColor = gl_Color;
    //gl_Position = ftransform();
    gl_Position = gl_Vertex;
}
