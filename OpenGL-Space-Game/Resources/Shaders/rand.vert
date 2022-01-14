precision highp float;
attribute vec3 a_position;
attribute vec3 normal;
//uniform mat3 normalMatrix;
//varying vec3 wPosition;
uniform mat4 MVP;   

varying vec4 rand_col;

// from Bojan lecture

float rand2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
float rand3D(in vec3 co){
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,144.7272))) * 43758.5453);
}

uniform float time;
uniform vec2 resolution;

const vec3 lightDir = vec3(1.0, 1.0, 1.0);
const float lightInt = 1.0;
const float Ka = 0.2;
const float Kd = 0.8;
const float Ks = 1.0;
const float shininess = 60.0;
const vec3 kDiffuse = vec3(0.9, 0.35, 0.8);
const float kFreq = 10.5;
const float kEps = 0.001;
const float W = 1000.0;

void main()
{
    //fNormal = normalize(normalMatrix * normal);
    //vec4 pos = MVP * vec4(position, 1.0);
    //fPosition = pos.xyz;
    //wPosition = position;
    float rand = rand3D(a_position);
    rand_col = vec4(rand, rand, rand, 1.0);
    gl_Position = MVP * vec4(a_position, 1);
}