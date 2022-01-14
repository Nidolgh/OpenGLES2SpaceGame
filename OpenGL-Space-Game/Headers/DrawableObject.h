#pragma once
#include "BaseObject.h"

#include "Utils/Shader/Shader.h"

class DrawableObject :
	public BaseObject
{
public:
	DrawableObject(Shader* shader);
	~DrawableObject();

	virtual bool Update(float dt) = 0;
	virtual bool Draw(const Camera& the_active_camera) = 0;
	virtual void UpdateUniforms(const Camera& the_active_camera) = 0;
	virtual void HandleCollision(BaseObject*) {}

	// used for drawing	
	Shader* m_shader;

	GLint positionLoc; // index handles to important concepts in texture 
	GLint normalLoc;
	GLint texCoordLoc;
	GLint samplerLoc;

	// an ambiant colour or tone shade for the model, leave as white for normal
	glm::vec4 m_ambient_colour; 
};

