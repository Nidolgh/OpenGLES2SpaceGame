#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/euler_angles.hpp>
#include "glm/gtx/quaternion.hpp"

#define PI				3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f
#define DEG2RAD(x) (x*PI)/180
#define RAD2DEG(x) x*(180/PI)

// uses methods from the initial Template from Brian and Transform class from Abhishek
class Transform
{
protected:
	// member values for the class hold info that is used for movement and drawing	
	glm::vec3 m_world_position;
	glm::vec3 m_position_offset;

	glm::vec3 m_world_rotations;
	glm::vec3 m_local_rotations;

	glm::vec3 m_euler_rotations;

	glm::vec3 m_scales;

	glm::vec3 m_v3Forward	= glm::vec3(0, 0, 1);
	glm::vec3 m_v3Up		= glm::vec3(0, 1, 0);
	glm::vec3 m_v3Right		= glm::vec3(1, 0, 0);
	glm::quat m_qRotation;
	glm::quat m_qOrientation;

	// matrix
	glm::mat4 m_translation_matrix;
	glm::mat4 m_position_offset_matrix;

	glm::mat4 m_rotation_matrix;
	glm::mat4 m_local_rotation_matrix;

	glm::mat4 m_scale_matrix;

	glm::mat4 m_rotation_matrix_x;
	glm::mat4 m_rotation_matrix_y;
	glm::mat4 m_rotation_matrix_z;

	glm::mat4 m_model; // the model matrix will effectivly hold all the rotation and positional data for the object

	bool m_bDirty;

public:
	Transform();
	~Transform();

	// a few basic utility functions some are provided for the student	
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3);

	glm::vec3 GetEulerAngles();
	void SetEulerAngles(glm::vec3 a_v3Rotation);
	glm::vec3 GetRotations();
	void SetRotation(glm::quat a_v3Rotation);

	glm::quat GetOrientation();

	void Translate(glm::vec3 a_v3Velocity);
	void Rotate(glm::vec3 a_v3Velocity);

	//void SetXRotation(float); // if you need to set indivual rotations
	//void SetYRotation(float);
	//void SetZRotation(float);

	void SetScale(glm::vec3 a_v3Scale);
	glm::vec3 GetScale();

	void RecalculateMatrix(); // makes and returns the model matrix
	void MakeTranslationMatrix();
	void MakeScaleMatrix();
	void MakeWorldRotationMatrix(); // since these get altered a lot we have a function to remake the matrix
	void MakeLocalRotationMatrix();

	glm::vec3 GetRight();
	glm::vec3 GetLeft();
	glm::vec3 GetUp();
	glm::vec3 GetDown();
	glm::vec3 GetForward();
	glm::vec3 GetBackward();

	glm::mat4 GetRotationMatrix();
	glm::mat4 GetModelMatrix();

	// temps
	bool m_space_movement = false;
	bool m_use_offset_pos = false;
};