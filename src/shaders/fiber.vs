#version 330

#include uniforms_vs
#include textures_vs
#include lighting_vs
#include peel_vs

out float v_discard;

void main()
{
	v_normal = normalize( a_normal );
	
	v_extra = a_extra;
	v_index = a_indexes;
	
	//vec3 tangent = normalize( ( mv_matrixTI * vec4( v_normal, 1.0 ) ).xyz );
	vec3 tangent = normalize( ( vec4( v_normal, 1.0 ) ).xyz );
    vec3 view = normalize( ( mv_matrixInvert * vec4( vec3( 0.0, 0.0, -1.0 ), 1.0 ) ).xyz );
    vec3 offsetNN = cross( tangent, view );
    vec3 offset = normalize( offsetNN );
    
    if ( dot( v_normal, view ) < 0.0 )
    {
        v_normal = cross( v_normal, offset );
    }
    else
    {
        v_normal = cross( v_normal, offset );
        v_normal *= -1.0;
    }

	
	prepareLight();
	
	if ( u_colorMode == 0 )
	{
	   frontColor = u_globalColor;
	}
	else if ( u_colorMode == 1 )
	{
	   frontColor = vec4( abs( a_normal ), 1.0 );
	}
	else
    {
       frontColor =  vec4( u_color.xyz, 1.0 );
    }
    v_discard = 0.0;
    if ( a_position.x <= ( u_cutx - u_cutdx ) || a_position.x >= ( u_cutx + u_cutdx ) || 
         a_position.y <= ( u_cuty - u_cutdy ) || a_position.y >= ( u_cuty + u_cutdy ) ||
         a_position.z <= ( u_cutz - u_cutdz ) || a_position.z >= ( u_cutz + u_cutdz ) )
    {
        v_discard = 1.0;
    } 
    
    calcTexCoords();
	
	if ( u_unfolding )
    {
        float M_PI = 3.1415926535897932384626433832795;
    
        vec3 newPos = a_position;
      
        float x1 = newPos.x - u_unfolding_barycenter.x;
        float y1 = newPos.y - u_unfolding_barycenter.y;
        float z1 = newPos.z - u_unfolding_barycenter.z;

       //float r = sqrt(  ( x1 * x1 ) + ( y1 * y1  ) );
       //float a = atan( y1, x1 );

       //float r = sqrt(  ( x1 * x1 ) + ( z1 * z1  ) );
       //float a = atan( z1, x1 );
       

       //float r = sqrt(  ( y1 * y1 ) + ( x1 * x1  ) );
       //float a = atan( x1, y1 );

       //float r = sqrt(  ( y1 * y1 ) + ( z1 * z1  ) );
       //float a = atan( z1, y1 );

       // toll
       float r = sqrt(  ( z1 * z1 ) + ( x1 * x1  ) );
       float a = atan( x1, z1 );
       if ( u_unfolding_neg )
       {
            a = atan( -x1, -z1 );
       }
       

       //float r = sqrt(  ( z1 * z1 ) + ( y1 * y1  ) );
       //float a = atan( y1, z1 );


        float r1 = r * ( a / M_PI );
        float a1 = ( a / M_PI ) * u_unfolding_theta;
       
        float d1 = 0.0;
        if ( abs( a1 ) > 0.00001 )
        {
            d1 = M_PI * r1 * sin( a1 ) / a1; 
        }
        else
        {
            d1 = M_PI * r1;
        }       
       

        float x2 = r - d1 * sin( a1 );
        float y2 = newPos.y;
        float z2 = d1 * cos( a1 );
          
        v_position = mvp_matrix * userTransformMatrix * vec4( x2, y2, z2, 1.0 );
        gl_Position = v_position;
    }
    else
    {
        v_position = mvp_matrix * userTransformMatrix * vec4( a_position, 1.0 );
        gl_Position = v_position;
    }
}
