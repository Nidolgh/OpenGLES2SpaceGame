#version 100
precision mediump float;

uniform vec4 ambient;
uniform vec4 main_col;

varying vec4 v_diffuse;
varying vec2 v_texCoord;
varying vec4 v_rand_col;

void main()
{
	//gl_FragColor = main_col; - v_rand_col.x
	gl_FragColor = ambient + (max(main_col - v_rand_col.x, 0.0)) * v_diffuse;
}