#version 330

uniform bool IsFogged;
uniform float FogMin;
uniform float FogMax;
uniform vec4 CameraEye;
uniform vec4 FogColor;

in VertexData{
    vec4 mColor;
    vec4 mVertex; // to calculate distance to camera eye
} VertexIn;

float getFogFactor(float d)
{
    if (d>=FogMax) return 1.f;
    if (d<=FogMin) return 0.f;

    return 1.f - (FogMax - d) / (FogMax - FogMin);
}

void main(void)
{
    vec4 color = VertexIn.mColor;
    if (IsFogged){
        float d = distance(CameraEye, VertexIn.mVertex);
        float alpha = getFogFactor(d);
        color = mix(color, FogColor, alpha);
    }

    gl_FragColor = color;
            //vec4(VertexIn.mColor.rgb, alpha);
}
