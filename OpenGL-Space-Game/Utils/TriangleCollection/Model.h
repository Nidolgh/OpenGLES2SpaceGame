#pragma once
#include <glm/vec3.hpp>

class Model
{
protected:
	glm::vec3 m_bmin;
	glm::vec3 m_bmax;

public:
	Model();
	virtual ~Model();

	void Init(glm::vec3 bmin, glm::vec3 bmax) { m_bmin = bmin; m_bmax = bmax; }

	void SetMin(glm::vec3 bmin) { m_bmin = bmin; }
	void SetMax(glm::vec3 bmax) { m_bmax = bmax; }
	glm::vec3 GetMin() const { return m_bmin; }
	glm::vec3 GetMax() const { return m_bmax; }
};