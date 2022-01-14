// https://www.haroldserrano.com/blog/how-to-apply-a-skybox-in-opengl
//1. declare attributes
attribute vec4 a_position;

//2. declare varying type which will transfer the texture coordinates to the fragment shader
varying mediump vec3 vTexCoordinates;

//3. declare a uniform that contains the model-View-projection
uniform mat4 MVP;

void main()
{

	//4. Generate the UV coordinates

	vTexCoordinates=normalize(a_position.xyz);

	//5. transform every position vertex by the model-view-projection matrix

	gl_Position = MVP * a_position;

	//6. Trick to place the skybox behind any other 3D model

	gl_Position=gl_Position.xyww;

}