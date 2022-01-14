#include "../Headers/BaseObject.h"

using namespace glm;

BaseObject::BaseObject()
{

}


BaseObject::~BaseObject()
{

}

bool BaseObject::Update(float dt)
{
	if (m_phys_obj == nullptr)
		return false;

	btScalar transform[16]; // physics uses a 16 float matrix syste
	m_phys_obj->GetTransform(transform);

	btQuaternion orient = m_phys_obj->GetRigidBody()->getOrientation();

	glm::quat rot = glm::quat(orient.getW(), orient.getX(), orient.getY(), orient.getZ());
	glm::vec3 pos = glm::vec3(transform[12], transform[13], transform[14]);

	GetTransform().SetPosition(pos);
	GetTransform().SetRotation(rot);

	return true;
}

void BaseObject::HandleCollision(BaseObject*)
{
	printf("Collided with an object!\n");
}