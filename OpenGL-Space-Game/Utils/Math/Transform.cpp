#include "Transform.h"

Transform::Transform()
{
	m_translation_matrix	= glm::mat4(1.0f);
	m_position_offset_matrix= glm::mat4(1.0f);
	m_rotation_matrix		= glm::mat4(1.0f);
	m_local_rotation_matrix = glm::mat4(1.0f);
	m_scale_matrix			= glm::mat4(1.0f);
	m_model					= glm::mat4(1.0f);

	m_scales			= glm::vec3(1.0f, 1.0f, 1.0f);
	m_world_rotations	= glm::vec3(0, 0, 0);
	m_position_offset	= glm::vec3(0, 0, 0);
	m_local_rotations	= glm::vec3(0, 0, 0);
	m_euler_rotations	= glm::vec3(0, 0, 0);

	RecalculateMatrix();
}

Transform::~Transform()
{

}

void Transform::MakeTranslationMatrix()
{
	m_translation_matrix = glm::mat4(1.0f);
	m_translation_matrix[3][0] = m_world_position.x;
	m_translation_matrix[3][1] = m_world_position.y;
	m_translation_matrix[3][2] = m_world_position.z;
}

// do the rotation matrices
void Transform::MakeWorldRotationMatrix()
{
	// set rotationx
	m_rotation_matrix_x = glm::mat4(1.0f);
	m_rotation_matrix_x[1][1] = cosf(m_world_rotations.x);
	m_rotation_matrix_x[2][1] = -sinf(m_world_rotations.x);
	m_rotation_matrix_x[1][2] = sinf(m_world_rotations.x);
	m_rotation_matrix_x[2][2] = cosf(m_world_rotations.x);
	//set rotationy
	m_rotation_matrix_y = glm::mat4(1.0f);
	m_rotation_matrix_y[0][0] = cosf(m_world_rotations.y);
	m_rotation_matrix_y[2][0] = sinf(m_world_rotations.y);
	m_rotation_matrix_y[0][2] = -sinf(m_world_rotations.y);
	m_rotation_matrix_y[2][2] = cosf(m_world_rotations.y);
	//set rotationz
	m_rotation_matrix_z = glm::mat4(1.0f);
	m_rotation_matrix_z[0][0] = cosf(m_world_rotations.z);
	m_rotation_matrix_z[1][0] = -sinf(m_world_rotations.z);
	m_rotation_matrix_z[0][1] = sinf(m_world_rotations.z);
	m_rotation_matrix_z[1][1] = cosf(m_world_rotations.z);

	// now the combined rotation	
	m_rotation_matrix = m_rotation_matrix_x * m_rotation_matrix_y * m_rotation_matrix_z;
}

void Transform::MakeLocalRotationMatrix()
{
	// set rotationx
	m_rotation_matrix_x = glm::mat4(1.0f);
	m_rotation_matrix_x[1][1] = cosf(m_local_rotations.x);
	m_rotation_matrix_x[2][1] = -sinf(m_local_rotations.x);
	m_rotation_matrix_x[1][2] = sinf(m_local_rotations.x);
	m_rotation_matrix_x[2][2] = cosf(m_local_rotations.x);
	//set rotationy
	m_rotation_matrix_y = glm::mat4(1.0f);
	m_rotation_matrix_y[0][0] = cosf(m_local_rotations.y);
	m_rotation_matrix_y[2][0] = sinf(m_local_rotations.y);
	m_rotation_matrix_y[0][2] = -sinf(m_local_rotations.y);
	m_rotation_matrix_y[2][2] = cosf(m_local_rotations.y);
	//set rotationz
	m_rotation_matrix_z = glm::mat4(1.0f);
	m_rotation_matrix_z[0][0] = cosf(m_local_rotations.z);
	m_rotation_matrix_z[1][0] = -sinf(m_local_rotations.z);
	m_rotation_matrix_z[0][1] = sinf(m_local_rotations.z);
	m_rotation_matrix_z[1][1] = cosf(m_local_rotations.z);

	// now the combined rotation
	m_local_rotation_matrix *= m_rotation_matrix_x * m_rotation_matrix_y * m_rotation_matrix_z;
}

void Transform::MakeScaleMatrix()
{
	m_scale_matrix = glm::mat4(1.0f);
	m_scale_matrix = glm::scale(m_scale_matrix, m_scales);
}

// simply create the model matrix, assumes the translations and others have been set
void Transform::RecalculateMatrix()
{
	// m_translation_matrix = glm::translate(glm::mat4(1.0f), m_world_position);
	MakeTranslationMatrix();

	// dirty cheat for local rotations
	if (m_space_movement)
		m_rotation_matrix *= glm::toMat4(m_qRotation);
	else
		m_rotation_matrix = glm::toMat4(m_qRotation);

	MakeScaleMatrix();

	/*glm::quat(glm::vec3(toRad(X),toRad(Y),toRad(Z))
	glm::identity<glm::quat>()*/

	if (m_use_offset_pos)
	{
		m_position_offset_matrix = glm::translate(glm::mat4(1.0f), m_position_offset);
		m_model = m_position_offset_matrix * m_translation_matrix * m_rotation_matrix * m_scale_matrix ;
	}
	else
		m_model = m_translation_matrix * m_rotation_matrix * m_scale_matrix;

	glm::vec4 x(1, 0, 0, 0);
	glm::vec4 y(0, 1, 0, 0);
	glm::vec4 z(0, 0, 1, 0);
	glm::vec4 o(0, 0, 0, 0);

	x = m_model * x;
	y = m_model * y;
	z = m_model * z;
	o = m_model * o;

	m_v3Up = glm::vec3(glm::normalize(y - o));
	m_v3Forward = glm::vec3(glm::normalize(z - o)); //glm::normalize(m_world_rotation_matrix[2])
	m_v3Right = glm::vec3(glm::normalize(x - o));

	m_bDirty = false;
}

// return the position info from worldposition
glm::vec3 Transform::GetPosition()
{
	return m_world_position;
}

//set the world position
void Transform::SetPosition(glm::vec3 a_Pos)
{
	m_world_position = a_Pos;

	m_bDirty = true;
}

glm::vec3 Transform::GetRotations()
{
	return m_world_rotations;
}

void Transform::SetEulerAngles(glm::vec3 a_v3Rotation)
{
	m_euler_rotations = a_v3Rotation;

	/*DEG2RAD(m_euler_rotations.x);
	DEG2RAD(m_euler_rotations.y);
	DEG2RAD(m_euler_rotations.z);*/

	glm::quat qPitch = glm::angleAxis(m_euler_rotations.x, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(m_euler_rotations.y, glm::vec3(0, 1, 0));
	glm::quat qRoll = glm::angleAxis(m_euler_rotations.z, glm::vec3(0, 0, 1));

	m_qRotation = qYaw * qPitch * qRoll;

	m_bDirty = true;
}

glm::vec3 Transform::GetEulerAngles()
{
	return m_euler_rotations;
}

void Transform::SetRotation(glm::quat a_qRotation)
{
	m_qRotation = a_qRotation;
	m_euler_rotations = glm::eulerAngles(m_qRotation);

	m_bDirty = true;
}

glm::quat Transform::GetOrientation()
{
	return m_qRotation;
}

void Transform::Translate(glm::vec3 a_v3Velocity)
{
	SetPosition(GetPosition() + a_v3Velocity);

	m_bDirty = true;
}

void Transform::Rotate(glm::vec3 a_v3Velocity)
{
	m_euler_rotations += a_v3Velocity;

	glm::quat qPitch = glm::angleAxis(m_euler_rotations.x, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(m_euler_rotations.y, glm::vec3(0, 1, 0));
	glm::quat qRoll = glm::angleAxis(m_euler_rotations.z, glm::vec3(0, 0, 1));

	m_qRotation = qYaw * qPitch * qRoll;

	m_bDirty = true;
}

void Transform::SetScale(glm::vec3 a_v3Scale)
{
	m_scales = a_v3Scale;
	m_bDirty = true;
}

glm::vec3 Transform::GetScale()
{
	return m_scales;
}

glm::vec3 Transform::GetRight()
{
	if (m_bDirty)
		RecalculateMatrix();

	return m_v3Right;
}

glm::vec3 Transform::GetLeft()
{
	return -GetRight();
}

glm::vec3 Transform::GetUp()
{
	if (m_bDirty)
		RecalculateMatrix();

	return m_v3Up;
}

glm::vec3 Transform::GetDown()
{
	return -GetUp();
}

glm::vec3 Transform::GetForward()
{
	if (m_bDirty)
		RecalculateMatrix();

	return m_v3Forward;
}

glm::vec3 Transform::GetBackward()
{
	return -GetForward();
}

glm::mat4 Transform::GetRotationMatrix()
{
	if (m_bDirty)
		RecalculateMatrix();

	return m_rotation_matrix;
}

glm::mat4 Transform::GetModelMatrix()
{
	if (m_bDirty)
		RecalculateMatrix();

	return m_model;
}