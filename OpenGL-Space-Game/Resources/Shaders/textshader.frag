#version 100
precision mediump float;

// simple flat text display frag shader, we can't really be sure our font has alpha set
// so this is rather a clumsy shader testing for 0 and non 0 pixels. If you can be sure
// you have alpha in your image, this can be improved quite a bit.

varying vec2 v_TexCoord;
uniform vec4 u_Colour; // ensure the alpha is 1, it can be cleared by the texture;
uniform sampler2D u_Sampler;
vec4 TheColour;
vec3 Zerotest = vec3(0.0, 0.0, 0.0); // slight speed improvement if pre defined.
vec4 One = vec4(1.0);
void main()
{    
	TheColour = One;

	if (texture2D(u_Sampler, v_TexCoord).rgb == Zerotest) 
	{
		TheColour.a = 0.0;
	}
 
    gl_FragColor = TheColour * u_Colour;

	
}



