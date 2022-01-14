uniform vec4 ambient;

varying vec2 v_texCoord;
varying vec3 v_position;
varying vec4 v_perlin_col;

void main()                                        
{
    gl_FragColor = v_perlin_col;
}