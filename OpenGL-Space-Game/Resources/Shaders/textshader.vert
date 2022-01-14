#version 100
precision mediump float;

// simple flat text display vert shader

uniform vec2 u_HalfSize;

attribute vec2 a_Position; // xy coords
attribute vec2 a_texCoord; // tex coords

varying vec2 v_TexCoord;

void main()
{
	// first map the screen coordinates to (-1..1)(-1..1_
	vec2 vert_clipspace = (a_Position/ u_HalfSize) - vec2(1, 1);

	// invert Y coordinate 
	gl_Position = vec4(vert_clipspace.x, -vert_clipspace.y, 0, 1);

	v_TexCoord = a_texCoord;
}

