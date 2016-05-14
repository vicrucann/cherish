#version 150
#extension GL_EXT_geometry_shader4 : enable


layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 32) out;

in vec4 gsColor[];
flat out vec4 fsColor;

uniform mat4 MVPW; // model-view-projection-window
uniform float BaseWidth;
uniform float WidthAmplitude;

const float pi = 3.14159265;
const float epsilon = 0.0001;


void ClipLineSegmentToNearPlane(
    mat4 MVPW,
    vec4 modelP_before,
    vec4 modelP0,
    vec4 modelP1,
    vec4 modelP_after,
    out vec4 clipP_before,
    out vec4 clipP0,
    out vec4 clipP1,
    out vec4 clipP_after,
    out bool culledByNearPlane)
{
    const float nearPlaneDistance = 0.5;
    clipP0 = MVPW * modelP0;
    clipP1 = MVPW * modelP1;
    culledByNearPlane = false;

    float distanceToP0 = clipP0.z + nearPlaneDistance;
    float distanceToP1 = clipP1.z + nearPlaneDistance;

    if ((distanceToP0 * distanceToP1) < 0.0) {
        float t = distanceToP0 / (distanceToP0 - distanceToP1);
        vec3 modelV = vec3(modelP0) + t * (vec3(modelP1) - vec3(modelP0));
        vec4 clipV = MVPW * vec4(modelV, 1);

        if (distanceToP0 < 0.0)
        {
            clipP0 = clipV;
            clipP_before = clipP0;
            clipP_after = MVPW * modelP_after;
        }
        else
        {
            clipP1 = clipV;
            clipP_after = clipP1;
            clipP_before = MVPW * modelP_before;
        }
    }
    else if (distanceToP0 < 0.0) {
        culledByNearPlane = true;
    }
    else {
        clipP_before = MVPW * modelP_before;
        clipP_after = MVPW * modelP_after;
    }
}

void halfDiscStrip(vec2 dir, float sign, vec3 center, float radius, vec4 color)
{
    const int divs = 8;
    const int nps = divs+1;
    vec2 ps[nps];
    float da = sign*pi/divs;
    vec2 rot_x = vec2(cos(da), -sin(da));
    vec2 rot_y = vec2(sin(da), cos(da));

    int i;
    ps[0] = dir;
    for(i=1; i<nps; ++i)
    {
        ps[i] = vec2(dot(ps[i-1], rot_x), dot(ps[i-1], rot_y));
    }

    int j;
    for(i=0, j=(nps-1); i<(nps/2); ++i, --j)
    {
        vec2 this_pos = ps[i];
        this_pos *= radius;
        this_pos += center.xy;
        gl_Position = vec4( this_pos, center.z, 1.0);
        fsColor = color;
        EmitVertex();

        this_pos = ps[j];
        this_pos *= radius;
        this_pos += center.xy;
        gl_Position = vec4(this_pos, center.z, 1.0);
        fsColor = color;
        EmitVertex();

    }
    if(nps % 2 == 1)
    {
        vec2 this_pos = ps[nps/2];
        this_pos *= radius;
        this_pos += center.xy;
        gl_Position = vec4(this_pos, center.z, 1.0);
        fsColor = color;
        EmitVertex();
    }

}

void main(void)
{

    //increment variable
    int i;

    vec4 windowP_before;
    vec4 windowP0;
    vec4 windowP1;
    vec4 windowP_after;

    bool culledByNearPlane;

    ClipLineSegmentToNearPlane(MVPW, gl_in[0].gl_Position, gl_in[1].gl_Position,
            gl_in[2].gl_Position, gl_in[3].gl_Position,
            windowP_before, windowP0, windowP1, windowP_after, culledByNearPlane);

    if (culledByNearPlane) return;

    windowP_before.xyz /= windowP_before.w;
    windowP0.xyz /= windowP0.w;
    windowP1.xyz /= windowP1.w;
    windowP_after.xyz /= windowP_after.w;

    const float min_stroke_width = 0.08;

    float u_fillDistance0 = BaseWidth + WidthAmplitude*gsColor[1].a;
    float u_fillDistance1 = BaseWidth + WidthAmplitude*gsColor[2].a;


    //one point case
    if(windowP0 == windowP1) {
        halfDiscStrip(vec2(1.0, 0.0), 1.0, windowP1.xyz, u_fillDistance0, gsColor[1]);
        EndPrimitive();
        halfDiscStrip(vec2(1.0, 0.0), -1.0, windowP1.xyz, u_fillDistance0, gsColor[1]);
    }
    else {
        vec2 direction = normalize(windowP1.xy - windowP0.xy);
        vec2 normal = vec2(direction.y, -direction.x);

        vec2 direction_before, direction_after, normal_after;
        float a_before, sign_before, recul_before, a_after, sign_after, recul_after;
        bool start = false;
        bool end = false;
        bool u_turn_before = false;
        bool u_turn_after = false;

        if(windowP0 != windowP_before)
        {
            direction_before = normalize(windowP0.xy - windowP_before.xy);
            a_before = acos(clamp(dot(direction, direction_before), -1.0, 1.0));
            sign_before = sign(dot(normal, direction_before));
            if(sign_before != -1.0 && sign_before != 1.0) sign_before = 1.0;
            if(a_before < epsilon)
            {
                recul_before = 0.0;
            }
            else if(a_before > pi/2.0)
            {
                recul_before = 0.0;
                u_turn_before = true;
            }
            else
                recul_before = u_fillDistance0/tan((pi-a_before)/2.0);
        }
        else
        {
            direction_before = vec2(0.0, 0.0);
            a_before = 0.0;
            sign_before = 1.0;
            recul_before = 0.0;
            start = true;
        }

        if(windowP1 != windowP_after)
        {
            direction_after = normalize(windowP_after.xy - windowP1.xy);
            normal_after = vec2(direction_after.y, -direction_after.x);
            a_after = acos(clamp(dot(direction, direction_after), -1.0, 1.0));
            sign_after = sign(dot(normal, direction_after));
            if(sign_after != -1.0 && sign_after != 1.0) sign_after = 1.0;

            if(a_after < epsilon)
            {
                recul_after = 0.0;
            }
            else if(a_after > pi/2.0) //u turn
            {
                u_turn_after = true;
                recul_after = 0.0;
            }
            else
                recul_after = u_fillDistance1/tan((pi-a_after)/2.0);
        }
        else
        {
            direction_after = vec2(0.0, 0.0);
            normal_after = vec2(0.0, 0.0);
            a_after = 0.0;
            sign_after = 1.0;
            recul_after = 0.0;
            end = true;
        }

        vec4 debug_color;

        if(start == true)
        {
            halfDiscStrip(normal, -1.0, windowP0.xyz, u_fillDistance0, gsColor[1]);
            EndPrimitive();

            gl_Position = vec4(windowP0.xy - (normal * u_fillDistance0 * sign_after), windowP0.z, 1.0);
            fsColor = gsColor[1];
            EmitVertex();

            gl_Position = vec4(windowP0.xy + (normal * u_fillDistance0 * sign_after), windowP0.z, 1.0);
            fsColor = gsColor[1];
            EmitVertex();
        }
        else if(u_turn_before == true)
        {
            gl_Position = vec4(windowP0.xy - (normal * u_fillDistance0 * sign_after), windowP0.z, 1.0);
            fsColor = gsColor[1];
            EmitVertex();

            gl_Position = vec4(windowP0.xy + (normal * u_fillDistance0 * sign_after), windowP0.z, 1.0);
            fsColor = gsColor[1];
            EmitVertex();
        }
        else
        {
            if(recul_before != 0.0)
            {
                gl_Position = vec4(windowP0.xy, windowP0.z, 1.0);
                fsColor = gsColor[1];
                EmitVertex();
            }
            if(sign_before != sign_after)
            {
                gl_Position = vec4(windowP0.xy + (normal * u_fillDistance0 * sign_before), windowP0.z, 1.0);
                fsColor = gsColor[1];
                EmitVertex();

                gl_Position = vec4(windowP0.xy - (normal * u_fillDistance0 * sign_before) + direction * recul_before, windowP0.z, 1.0);
                fsColor = gsColor[1];
                EmitVertex();
            }
            else
            {
                gl_Position = vec4(windowP0.xy - (normal * u_fillDistance0 * sign_before) + direction * recul_before, windowP0.z, 1.0);
                fsColor = gsColor[1];
                EmitVertex();

                gl_Position = vec4(windowP0.xy + (normal * u_fillDistance0 * sign_before), windowP0.z, 1.0);
                fsColor = gsColor[1];
                EmitVertex();
            }
        }

        if(end == true)
        {
                gl_Position = vec4(windowP1.xy - (normal * u_fillDistance1 * sign_after), windowP1.z, 1.0);
                fsColor = gsColor[2];
                EmitVertex();

                gl_Position = vec4(windowP1.xy + (normal * u_fillDistance1 * sign_after), windowP1.z, 1.0);
                fsColor = gsColor[2];
                EmitVertex();

                EndPrimitive();
                halfDiscStrip(normal, 1.0, windowP1.xyz, u_fillDistance1, gsColor[2]);

        }
        else if(u_turn_after == true)
        {
            gl_Position = vec4(windowP1.xy - (normal * u_fillDistance1 * sign_after), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();

            gl_Position = vec4(windowP1.xy + (normal * u_fillDistance1 * sign_after), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();
        }
        else
        {
            gl_Position = vec4(windowP1.xy - (normal * u_fillDistance1 * sign_after), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();

            gl_Position = vec4(windowP1.xy + (normal * u_fillDistance1 * sign_after) - (direction * recul_after), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();
            if(recul_after != 0.0)
            {
              gl_Position = vec4(windowP1.xy, windowP1.z, 1.0);
              fsColor = gsColor[2];
              EmitVertex();
            }
        }


        if(recul_after != 0.0 || u_turn_after)
        {
            EndPrimitive();

            gl_Position = vec4(windowP1.xy - (normal * u_fillDistance1 * sign_after), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();

            float da = -sign_after*a_after/3.0;

            vec2 rot_x = vec2(cos(da), -sin(da));
            vec2 rot_y = vec2(sin(da), cos(da));

            vec2 new_normal1 = vec2(dot((-normal*sign_after), rot_x), dot((-normal*sign_after), rot_y));

            gl_Position = vec4(windowP1.xy + (new_normal1 * u_fillDistance1), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();

            gl_Position = vec4(windowP1.xy, windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();

            vec2 new_normal2 = vec2(dot(new_normal1, rot_x), dot(new_normal1, rot_y));

            gl_Position = vec4(windowP1.xy + (new_normal2 * u_fillDistance1), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();

            gl_Position = vec4(windowP1.xy - (normal_after * u_fillDistance1 * sign_after), windowP1.z, 1.0);
            fsColor = gsColor[2];
            EmitVertex();
           }
    }
}

