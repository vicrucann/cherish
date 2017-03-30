#version 330

uniform float Thickness;
uniform vec2 Viewport;
uniform float MiterLimit;
uniform int Segments;
uniform bool IsFogged;
uniform float FogMin;
uniform float FogMax;
uniform vec4 CameraEye;

const int SegmentsMax = 30; // max_vertices = (SegmentsMax+1)*4;
const int SegmentsMin = 3; // min number of segments per curve

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 124) out;

in VertexData{
    vec4 mColor;
    vec4 mVertex;
} VertexIn[4];

out VertexData{
    vec2 mTexCoord; // fix before usage
    vec4 mColor;
} VertexOut;

vec2 toScreenSpace(vec4 vertex)
{
    return vec2( vertex.xy / vertex.w ) * Viewport;
}

float toZValue(vec4 vertex)
{
    return vertex.z / vertex.w;
}

vec2 toBezier(float delta, int i, vec2 b0, vec2 b1, vec2 b2, vec2 b3)
{
    float t = delta * float(i);
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    return (b0 * one_minus_t2 * one_minus_t + b1 * 3.0 * t * one_minus_t2 + b2 * 3.0 * t2 * one_minus_t + b3 * t2 * t);
}

vec4 toBezier3D(float delta, int i, vec4 B0, vec4 B1, vec4 B2, vec4 B3)
{
    float t = delta * float(i);
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    return (B0 * one_minus_t2 * one_minus_t + B1 * 3.0 * t * one_minus_t2 + B2 * 3.0 * t2 * one_minus_t + B3 * t2 * t);
}

float getFogFactor(float d)
{
    if (d>=FogMax) return 0;
    if (d<=FogMin) return 1;

    return (FogMax - d) / (FogMax - FogMin);
}

void drawSegment(vec2 points[4], vec4 colors[4], float zValues[4])
{
    vec2 p0 = points[0];
    vec2 p1 = points[1];
    vec2 p2 = points[2];
    vec2 p3 = points[3];

    /* perform naive culling */
    vec2 area = Viewport * 4;
    if( p1.x < -area.x || p1.x > area.x ) return;
    if( p1.y < -area.y || p1.y > area.y ) return;
    if( p2.x < -area.x || p2.x > area.x ) return;
    if( p2.y < -area.y || p2.y > area.y ) return;

    /* determine the direction of each of the 3 segments (previous, current, next) */
    vec2 v0 = normalize( p1 - p0 );
    vec2 v1 = normalize( p2 - p1 );
    vec2 v2 = normalize( p3 - p2 );

    /* determine the normal of each of the 3 segments (previous, current, next) */
    vec2 n0 = vec2( -v0.y, v0.x );
    vec2 n1 = vec2( -v1.y, v1.x );
    vec2 n2 = vec2( -v2.y, v2.x );

    /* determine miter lines by averaging the normals of the 2 segments */
    vec2 miter_a = normalize( n0 + n1 );	// miter at start of current segment
    vec2 miter_b = normalize( n1 + n2 ); // miter at end of current segment

    /* determine the length of the miter by projecting it onto normal and then inverse it */
    float an1 = dot(miter_a, n1);
    float bn1 = dot(miter_b, n2);
    if (an1==0) an1 = 1;
    if (bn1==0) bn1 = 1;
    float length_a = Thickness / an1;
    float length_b = Thickness / bn1;

    /* prevent excessively long miters at sharp corners */
    if( dot( v0, v1 ) < -MiterLimit ) {
        miter_a = n1;
        length_a = Thickness;

        /* close the gap */
        if( dot( v0, n1 ) > 0 ) {
            VertexOut.mTexCoord = vec2( 0, 0 );
            VertexOut.mColor = colors[1];
            gl_Position = vec4( ( p1 + Thickness * n0 ) / Viewport, zValues[1], 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 0 );
            VertexOut.mColor = colors[1];
            gl_Position = vec4( ( p1 + Thickness * n1 ) / Viewport, zValues[1], 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 0.5 );
            VertexOut.mColor = colors[1];
            gl_Position = vec4( p1 / Viewport, zValues[1], 1.0 );
            EmitVertex();

            EndPrimitive();
        }
        else {
            VertexOut.mTexCoord = vec2( 0, 1 );
            VertexOut.mColor = colors[1];
            gl_Position = vec4( ( p1 - Thickness * n1 ) / Viewport, zValues[1], 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 1 );
            VertexOut.mColor = colors[1];
            gl_Position = vec4( ( p1 - Thickness * n0 ) / Viewport, zValues[1], 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 0.5 );
            VertexOut.mColor = colors[1];
            gl_Position = vec4( p1 / Viewport, zValues[1], 1.0 );
            EmitVertex();

            EndPrimitive();
        }
    }
    if( dot( v1, v2 ) < -MiterLimit ) {
        miter_b = n1;
        length_b = Thickness;
    }
    // generate the triangle strip
    VertexOut.mTexCoord = vec2( 0, 0 );
    VertexOut.mColor = colors[1];
    gl_Position = vec4( ( p1 + length_a * miter_a ) / Viewport, zValues[1], 1.0 );
    EmitVertex();

    VertexOut.mTexCoord = vec2( 0, 1 );
    VertexOut.mColor = colors[1];
    gl_Position = vec4( ( p1 - length_a * miter_a ) / Viewport, zValues[1], 1.0 );
    EmitVertex();

    VertexOut.mTexCoord = vec2( 0, 0 );
    VertexOut.mColor = colors[2];
    gl_Position = vec4( ( p2 + length_b * miter_b ) / Viewport, zValues[2], 1.0 );
    EmitVertex();

    VertexOut.mTexCoord = vec2( 0, 1 );
    VertexOut.mColor = colors[2];
    gl_Position = vec4( ( p2 - length_b * miter_b ) / Viewport, zValues[2], 1.0 );
    EmitVertex();

    EndPrimitive();
}

void main(void)
{
    /* cut segments number if larger than allowed */
    int nSegments = (Segments > SegmentsMax)? SegmentsMax : Segments;
    nSegments = (nSegments < SegmentsMin)? SegmentsMin: nSegments;

    /* read the input */
    vec4 B[4], V[4], C[4];
    for (int i=0; i<4; ++i){
        B[i] = gl_in[i].gl_Position; // bezier points
        V[i] = VertexIn[i].mVertex; // vertex format
        C[i] = VertexIn[i].mColor; // attached colors
    }

    /* adjust alpha channels wrt camera eye distance */
    if (IsFogged){
        for (int i=0; i<4; ++i){
            float d = distance(CameraEye, V[0]);
            float alpha = getFogFactor(d);
            C[i] = vec4(C[i].rgb, alpha);
        }
    }

    /* get the 2d bezier control points */
    vec2 b0 = toScreenSpace( B[0] );
    vec2 b1 = toScreenSpace( B[1] );
    vec2 b2 = toScreenSpace( B[2] );
    vec2 b3 = toScreenSpace( B[3] );

    /* use the points to build a bezier line */
    float delta = 1.0 / float(nSegments);
    vec4 P[4]; // interpolated 3D points of Bezier
    vec2 points[4]; // interpolated 2D points of Bezier
    vec4 colors[4]; // interpolated colors
    float zValues[4]; // stroke z-values
    int j = 0; // bezier segment index for color interpolation
    const float ext = 0.00001;
    for (int i=0; i<=nSegments; ++i){
        /* first point */
        if (i==0){
            P[1] = toBezier3D(delta, i, B[0], B[1], B[2], B[3]);
            P[2] = toBezier3D(delta, i+1, B[0], B[1], B[2], B[3]);
            P[3] = toBezier3D(delta, i+2, B[0], B[1], B[2], B[3]);
            vec4 D = normalize(P[1] - P[2]);
            P[0] = P[1] + D * ext;
        }
        else if (i < nSegments-1){
            P[0] = P[1];
            P[1] = P[2];
            P[2] = P[3];
            P[3] = toBezier3D(delta, i+2, B[0], B[1], B[2], B[3]);
        }
        /* last point */
        else {
            P[0] = P[1];
            P[1] = P[2];
            P[2] = P[3];
            vec4 D = normalize(P[2] - P[1]);
            P[3] = P[2] + D * ext;
        }

        // color interpolation
        {
            if (i==0) colors[1] = C[0];
            else colors[1] = colors[2];

            /* fraction p{i} is located between fraction p{j} and p{j+1} */
            float pi = float(i+1) / float(nSegments);
            if (pi >= float(j+1)/3.f) j++;
            float pj = float(j)/3.f; // 4 bezier points means 3 segments between which points are plotted
            float pj1 = float(j+1)/3.f;
            float a = (pi-pj) / (pj1-pj);
            colors[2] = mix(C[j], C[j+1], a); // j <= 3
        }

        /* stroke z-values */
        points[0] = toScreenSpace(P[0]);
        points[1] = toScreenSpace(P[1]);
        points[2] = toScreenSpace(P[2]);
        points[3] = toScreenSpace(P[3]);

        zValues[0] = toZValue(P[0]);
        zValues[1] = toZValue(P[1]);
        zValues[2] = toZValue(P[2]);
        zValues[3] = toZValue(P[3]);

        drawSegment(points, colors, zValues);
    }

}
