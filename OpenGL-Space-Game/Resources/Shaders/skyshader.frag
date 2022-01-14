// https://www.haroldserrano.com/blog/how-to-apply-a-skybox-in-opengl
precision highp float;

//1. declare a uniform sampler2d that contains the texture data
uniform samplerCube SkyBoxTexture;

//2. declare varying type which will transfer the texture coordinates from the vertex shader
varying mediump vec3 vTexCoordinates;

void main()
{
	//3. set the final color to the output of the fragment shader
	gl_FragColor = textureCube(SkyBoxTexture,vTexCoordinates);
}