precision mediump float;   
varying vec2 v_texCoord;    
uniform sampler2D s_texture;
uniform vec4  ambient;

void main()                                        
{                                                  
  gl_FragColor = ambient;
}                                                 