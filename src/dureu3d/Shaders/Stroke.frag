#version 150

flat in vec4 fsColor;
out vec4 fragmentColor;

void main()
{
    fragmentColor = fsColor;
    //fragmentColor.a = 0.5;
}
