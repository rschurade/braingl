varying vec3 v_normal;

varying vec3 v_lightDirection;

void main()
{
	vec4 color;
	color = vec4( 1.0, 1.0, 1.0, 1.0 );

	vec3 uAmbientColor = vec3(0.4);
    vec3 uPointLightingDiffuseColor= vec3(0.6);

    vec3 lightWeighting;

    float diffuseLightWeighting = max(dot(v_normal, v_lightDirection), 0.0);
            
    lightWeighting = uAmbientColor + uPointLightingDiffuseColor * diffuseLightWeighting;

    //gl_FragColor = vec4(color.rgb * lightWeighting, color.a);
    gl_FragColor = vec4(abs(v_normal), color.a);


}
