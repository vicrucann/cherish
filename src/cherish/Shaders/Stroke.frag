#version 330

in VertexData{
    vec2 mTexCoord;
    vec4 mColor;
} VertexIn;
const vec4 testColor = vec4(0.3,1,0,1);
void main(void)
{
    gl_FragColor = vec4(VertexIn.mColor.xyz, 1);
}
