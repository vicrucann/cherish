#version 330

uniform bool IsFogged;
uniform float FogMin;
uniform float FogMax;
uniform vec4 CameraEye;

in VertexData{
    vec4 mColor;
    vec4 mVertex; // to calculate distance to camera eye
} VertexIn;

float getFogFactor(float d, float a)
{
    if (d>=FogMax) return 0;
    if (d<=FogMin) return a;

    return (FogMax - d) / (FogMax - FogMin) * a;
}

void main(void)
{
    float alpha = VertexIn.mColor.a;
    if (IsFogged){
        float d = distance(CameraEye, VertexIn.mVertex);
        alpha = getFogFactor(d, alpha);
    }
    gl_FragColor = vec4(VertexIn.mColor.rgb, alpha);
}
