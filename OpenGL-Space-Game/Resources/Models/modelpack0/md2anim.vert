#version 330

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;                                                                           
	mat4 normalMatrix;
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inNextPosition;
layout (location = 4) in vec3 inNextNormal;

smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;

smooth out vec4 vEyeSpacePos;

uniform float fInterpolation;

void main()
{
  mat4 mMV = matrices.viewMatrix*matrices.modelMatrix;  
  mat4 mMVP = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix;
  
  vTexCoord = inCoord;
  
  vec3 vInterpolatedPosition = inPosition;
  if(fInterpolation >= 0.0f)vInterpolatedPosition += (inNextPosition - inPosition)*fInterpolation;

  vEyeSpacePos = mMV*vec4(vInterpolatedPosition, 1.0);
	gl_Position = mMVP*vec4(vInterpolatedPosition, 1.0);

  vec3 vInterpolatedNormal = inNormal;
  if(fInterpolation >= 0.0f)vInterpolatedNormal += (inNextNormal - inNormal)*fInterpolation;

  vNormal = (matrices.normalMatrix*vec4(vInterpolatedNormal, 1.0)).xyz;
  vWorldPos = (matrices.modelMatrix*vec4(vInterpolatedPosition, 1.0)).xyz;
}