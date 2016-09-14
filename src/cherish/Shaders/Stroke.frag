#version 330

in VertexData{
    vec2 mTexCoord;
    vec4 mColor;
} VertexIn;

void main(void)
{
    gl_FragColor = VertexIn.mColor;
}
