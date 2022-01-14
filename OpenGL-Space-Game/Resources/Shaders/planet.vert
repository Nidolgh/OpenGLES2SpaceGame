#version 100
precision mediump float;
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;

uniform mat4 normal_matrix;
uniform vec3 light_pos;
uniform mat4 M;
uniform mat4 MVP;
uniform bool is_shaded;

varying vec4 v_rand_col;
varying vec2 v_texCoord;
varying vec4 v_diffuse;

// from Bojan lecture
float rand2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
float rand3D(in vec3 co){
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,144.7272))) * 43758.5453);
}

//uniform float time;

const float lightInt = 1.0;

void main()
{
    gl_Position = MVP * vec4(a_position, 1);

    if (is_shaded)
    {
	    vec4 worldpos = M * vec4(a_position, 0.0);
	    vec3 transformed_normal = normalize(vec3(normal_matrix * vec4(a_normal, 1.0)));
	    vec3 lightDir = normalize(light_pos - worldpos.xyz);
	    float diff = max(dot(transformed_normal, lightDir), 0.0);
	    v_diffuse = diff * vec4(1,1,1,1);
    }
    else
    {
	    v_diffuse = 1.0 * vec4(1,1,1,1);
    }

    float rand = rand3D(a_position);
    v_rand_col = vec4(rand, rand, rand, 1.0);

	v_texCoord = a_texCoord;
}