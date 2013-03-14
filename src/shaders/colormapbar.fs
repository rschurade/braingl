#include colormap_fs

varying vec3 v_texcoord;

uniform float u_min;
uniform float u_max;
uniform float u_lowerThreshold;
uniform float u_upperThreshold;
uniform int u_colormap;

void main( void )
{
    vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 );
    float value = 0.0;
    float min_ = 0.0;
    float max_ = 1.0;
    if ( u_min >= u_lowerThreshold )
    {
        min_ = 0.0;
    }
    else
    {
        min_ = u_min - u_lowerThreshold;
    }
    
    if ( u_max <= u_upperThreshold )
    {
        max_ = 1.0; 
    }
    else
    {
        max_ = 1.0 - ( u_upperThreshold - u_max ) ;
    } 
    value = ( v_texcoord.x - min_ ) / ( max_ - min_ );

    //float value = ( v_texcoord.x - u_min ) / ( u_max - u_min );
    //float value = v_texcoord.x * ( u_upperThreshold - u_lowerThreshold  ) + u_lowerThreshold;
    value = max( 0.0, min( 1.0, value ) );
    gl_FragColor =  colormap( value, u_colormap, 0.0, 1.0, 0.0, 1.0, 1.0, color );
}
