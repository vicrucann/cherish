#version 330

uniform float Thickness;
uniform vec2 Viewport;
uniform float MiterLimit;

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 7) out;

in VertexData{
    vec4 mColor;
} VertexIn[4];

out VertexData{
    vec2 mTexCoord;
    vec4 mColor;
} VertexOut;

vec2 toScreenSpace(vec4 vertex)
{
return vec2( vertex.xy / vertex.w ) * Viewport;
}

void main(void)
{
    // 4 points
    vec4 P0 = gl_in[0].gl_Position;
    vec4 P1 = gl_in[1].gl_Position;
    vec4 P2 = gl_in[2].gl_Position;
    vec4 P3 = gl_in[3].gl_Position;

    // get the four vertices passed to the shader:
    vec2 p0 = toScreenSpace( P0 );	// start of previous segment
    vec2 p1 = toScreenSpace( P1 );	// end of previous segment, start of current segment
    vec2 p2 = toScreenSpace( P2 );	// end of current segment, start of next segment
    vec2 p3 = toScreenSpace( P3 ); // end of next segment

    // perform naive culling
    vec2 area = Viewport * 1.8;
    if( p1.x < -area.x || p1.x > area.x ) return;
    if( p1.y < -area.y || p1.y > area.y ) return;
    if( p2.x < -area.x || p2.x > area.x ) return;
    if( p2.y < -area.y || p2.y > area.y ) return;

    // determine the direction of each of the 3 segments (previous, current, next)
    vec2 v0 = normalize( p1 - p0 );
    vec2 v1 = normalize( p2 - p1 );
    vec2 v2 = normalize( p3 - p2 );

    // determine the normal of each of the 3 segments (previous, current, next)
    vec2 n0 = vec2( -v0.y, v0.x );
    vec2 n1 = vec2( -v1.y, v1.x );
    vec2 n2 = vec2( -v2.y, v2.x );

    // determine miter lines by averaging the normals of the 2 segments
    vec2 miter_a = normalize( n0 + n1 );	// miter at start of current segment
    vec2 miter_b = normalize( n1 + n2 ); // miter at end of current segment

    // determine the length of the miter by projecting it onto normal and then inverse it
    float an1 = dot(miter_a, n1);
    float bn1 = dot(miter_b, n2);
    if (an1==0) an1 = 1;
    if (bn1==0) bn1 = 1;
    float length_a = Thickness / an1;
    float length_b = Thickness / bn1;

    // prevent excessively long miters at sharp corners
    if( dot( v0, v1 ) < -MiterLimit ) {
        miter_a = n1;
        length_a = Thickness;

        // close the gap
        if( dot( v0, n1 ) > 0 ) {
            VertexOut.mTexCoord = vec2( 0, 0 );
            VertexOut.mColor = VertexIn[1].mColor;
            gl_Position = vec4( ( p1 + Thickness * n0 ) / Viewport, 0.0, 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 0 );
            VertexOut.mColor = VertexIn[1].mColor;
            gl_Position = vec4( ( p1 + Thickness * n1 ) / Viewport, 0.0, 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 0.5 );
            VertexOut.mColor = VertexIn[1].mColor;
            gl_Position = vec4( p1 / Viewport, 0.0, 1.0 );
            EmitVertex();

            EndPrimitive();
        }
        else {
            VertexOut.mTexCoord = vec2( 0, 1 );
            VertexOut.mColor = VertexIn[1].mColor;
            gl_Position = vec4( ( p1 - Thickness * n1 ) / Viewport, 0.0, 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 1 );
            VertexOut.mColor = VertexIn[1].mColor;
            gl_Position = vec4( ( p1 - Thickness * n0 ) / Viewport, 0.0, 1.0 );
            EmitVertex();

            VertexOut.mTexCoord = vec2( 0, 0.5 );
            VertexOut.mColor = VertexIn[1].mColor;
            gl_Position = vec4( p1 / Viewport, 0.0, 1.0 );
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
    VertexOut.mColor = VertexIn[1].mColor;
    gl_Position = vec4( ( p1 + length_a * miter_a ) / Viewport, 0.0, 1.0 );
    EmitVertex();

    VertexOut.mTexCoord = vec2( 0, 1 );
    VertexOut.mColor = VertexIn[1].mColor;
    gl_Position = vec4( ( p1 - length_a * miter_a ) / Viewport, 0.0, 1.0 );
    EmitVertex();

    VertexOut.mTexCoord = vec2( 0, 0 );
    VertexOut.mColor = VertexIn[2].mColor;
    gl_Position = vec4( ( p2 + length_b * miter_b ) / Viewport, 0.0, 1.0 );
    EmitVertex();

    VertexOut.mTexCoord = vec2( 0, 1 );
    VertexOut.mColor = VertexIn[2].mColor;
    gl_Position = vec4( ( p2 - length_b * miter_b ) / Viewport, 0.0, 1.0 );
    EmitVertex();

    EndPrimitive();
}
