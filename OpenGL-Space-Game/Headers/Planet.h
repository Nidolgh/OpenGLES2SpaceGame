#pragma once
#include "DrawableObject.h"
#include "Utils/TriangleCollection/MeshModel.h"

class Planet : public DrawableObject
{
protected:

	MeshModel* m_mesh;

	float tt = 0.0f;

	glm::vec3 m_orbit_speed;
	glm::vec3 m_orbit;
	glm::vec3 m_velocity;

	const float m_GRAVITY = 0.00667406f;

public:
	enum PlanetType {
		PlanetNormal,
		PlanetChar,
		PlanetSun,
		PlanetStation,
		PlanetAsteroid,
		PlanetGassCloud
	};

	Planet(Model* the_mesh, Shader* the_shader, PlanetType = PlanetType::PlanetNormal);
	virtual ~Planet();

	bool Init();

	virtual bool Update(float dt); // we supply an update system, it can be an all in one system
	virtual bool Draw(const Camera& the_active_camera); // and a draw

	virtual void UpdateUniforms(const Camera& the_active_camera);

	Model& GetMeshModel() { return *m_mesh; }

	float MOS(); // Jan-Mathijs, Mean orbital speed
	float NLOG(); 

	bool m_is_shaded		= true;
	float m_light_intensity = 1.0f;
	float m_main_col[4]		= { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_ambient_col[4]	= { 0.02f, 0.02f, 0.02f, 1.0f };

	float m_mass = 1.0f;
	bool m_is_alive = true;

	PlanetType m_planet_type;

	// chars
	char m_the_one		= ' ';
	int m_the_one_index = -1;
};