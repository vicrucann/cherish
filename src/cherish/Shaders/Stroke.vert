#version 330

uniform mat4 ModelViewProjectionMatrix;

layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec4 Color;

out VertexData{
    vec4 mColor;
} VertexOut;

void main(void)
{
    VertexOut.mColor = Color;
    gl_Position = ModelViewProjectionMatrix * Vertex;
}
