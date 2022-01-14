#pragma once
#ifdef RASPBERRY
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>
#endif // RASPBERRY

#include "Utils/Math/Transform.h"

#include "Camera.h"

#include <Utils/Bullet/PhysicsObj.h>

/* Object model is a base class to represent the most basic form of models
its update and draw classes are pure virtual so any object derived from this must supply those function
It contains simplified matrices for orientation, position and scaling and the ability to create new model matrix before a draw is done
examples of usage can be found in CubeModel
*/

// A base container class, containing only information needed to position/rotate and create a model.

class BaseObject
{
protected:
	Transform m_transform;

public:
	BaseObject();
	virtual ~BaseObject();

	virtual bool Update(float dt); // we need to have an Update to move and create the ModelView
	virtual bool Draw(const Camera& the_active_camera) = 0; // and we need to draw from the vertices
	virtual void HandleCollision(BaseObject*);

	Transform& GetTransform() { return m_transform; }

	PhysicsObj* m_phys_obj;
};
