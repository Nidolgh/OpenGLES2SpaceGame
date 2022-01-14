

#version 100
// very simple and probably only used for debug systems
attribute vec3 a_position;
uniform mat4 MVP; 
void main()
{
	gl_Position = MVP*vec4(a_position, 1);

}
