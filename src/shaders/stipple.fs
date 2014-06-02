#version 330

#include peel_fs

in vec3 v_dir;
in vec4 frontColor;
in vec3 v_textPos;
in vec3 v_texCoord;

uniform vec3 u_aVec;
uniform vec3 u_bVec;
uniform float u_glyphThickness;


/**
 * Computes the distance from a line segment and a point.
 *
 * \param start Defining the start of the line segment.
 * \param end Defining the end of the line segment.
 * \param point For which the distance should be computed.
 *
 * \return Distance from given line segment (start,end) and point.
 */
float distancePointLineSegment( vec3 point, vec3 start, vec3 end )
{
    float segmentLength = length( start - end );
    if( segmentLength == 0.0 )
    {
        return distance( point, start );
    }

    // Consider the line extending the segment, parameterized as start + t (end - start):
    // We find projection of point p onto the line: t = [(p-v) . (w-v)] / |w-v|^2
    float t = dot( point - start, end - start ) / ( segmentLength * segmentLength );

    if( t < 0.0 )      // Beyond the 'v' end of the segment
    {
        return distance( point, start );
    }
    else if( t > 1.0 ) // Beyond the 'w' end of the segment
    {
        return distance( point, end );
    }
    else
    {
        vec3 projection = start + t * ( end - start );  // Projection falls on the segment
        return distance( point, projection );
    }
}

void main( void )
{
    float scale = 0.8;

    vec3 diffusionDirection = v_dir;
    
    // project into plane (given by two vectors aVec and bVec)
    vec3 aVecNorm = normalize( u_aVec );
    vec3 bVecNorm = normalize( u_bVec );
    vec3 projectedDirectionTextCoords = 0.5 * vec3( dot( aVecNorm, diffusionDirection ), dot( bVecNorm, diffusionDirection ), 0.0 );

    vec3 middlePoint_tex = vec3( 0.5, 0.5, 0.0 );
    vec3 scaledFocalPoint1 = middlePoint_tex + scale * projectedDirectionTextCoords;
    vec3 scaledFocalPoint2 = middlePoint_tex - scale * projectedDirectionTextCoords;
    vec3 focalPoint1 = middlePoint_tex + projectedDirectionTextCoords;
    vec3 focalPoint2 = middlePoint_tex - projectedDirectionTextCoords;
    
    float area =  0.1 * u_glyphThickness; // this is arbitrarily set
    float l = distance( scaledFocalPoint1, scaledFocalPoint2 );
    float p2 = -l / 3.14159265;
    float q = area / 3.14159265;
    float r1 = p2 + sqrt( p2 * p2 + q );
    float r2 = p2 - sqrt( p2 * p2 + q );
    //float radius = 0.05 * u_glyphThickness; //max( r1, r2 );// - 0.05;
    float radius = max( r1, r2 );// - 0.05;
    
    
    float dist = distancePointLineSegment( v_texCoord, scaledFocalPoint1, scaledFocalPoint2 );
    
    if ( dist < radius )
    {
        writePeel( frontColor );
    }
    else
    {
        discard;
        //writePeel( vec4( 0.0, 0.0, 1.0, 1.0 ) );
    }
}
