precision mediump float;
 
uniform sampler2D s_texture;
uniform vec4 ambient;

varying vec3 v_diffuse;
varying vec2 v_texCoord;

void main()                                        
{                             
	vec3 result = (vec3(0.1) + v_diffuse);               
	gl_FragColor = vec4(result, 1.0) * ambient;
}