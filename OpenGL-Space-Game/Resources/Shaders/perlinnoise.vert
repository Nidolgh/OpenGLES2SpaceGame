precision mediump float;    

attribute vec3 a_position;  
attribute vec2 a_texCoord;  

uniform mat4 MVP;

varying vec2 v_texCoord;
varying vec3 v_position;

void main()                 
{ 		
    v_position = a_position;
    gl_Position = MVP * vec4(a_position,1);
    v_texCoord = a_texCoord;
}