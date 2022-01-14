precision mediump float;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;

uniform mat4 MVP;  
uniform mat4 M;
uniform mat4 normal_matrix;
uniform vec3 light_pos;

varying vec2 v_texCoord;
varying vec3 v_diffuse;

void main()                 
{ 							
	gl_Position = MVP * vec4(a_position, 1);
	v_texCoord = a_texCoord;

	vec4 worldpos = M * vec4(a_position, 1);
	
	vec3 transformed_normal = normalize(vec3(normal_matrix * vec4(a_normal, 0.0)));
	vec3 lightDir = normalize(light_pos - worldpos.xyz);
	float diff = max(dot(transformed_normal, lightDir), 0.0);
	v_diffuse = diff * vec3(1,1,1);
}