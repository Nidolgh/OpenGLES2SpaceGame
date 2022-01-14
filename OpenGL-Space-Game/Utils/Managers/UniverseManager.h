#pragma once
#include "Utils/Managers/ResourceManager.h"

#include "Headers/PlayerSpace.h"
#include "Headers/PlayerBullet.h"
#include "Headers/Planet.h"
#include "Headers/SpaceStation.h"
#include "Headers/SkyBox.h"

#include <vector>

#include <Utils/State/StateSystem.h>

#include <Utils/Bullet/BulletScene.h>
#include <Utils/Font/Font3D.h>

inline float Rand(float a_range) { return (static_cast<float>(rand()) / RAND_MAX) * a_range; }

class UniverseManager
{
protected:
	ResourceManager* m_pResource_manager;
	
	Font3D m_the_font;

	PlayerSpace*	m_player_space;
	PlayerBullet*	m_player_bullet;
	Player*			m_active_player;

	Camera*			m_camera;
	SkyBox*			m_skybox;
	SpaceStation*	m_sstation;
	// Planet*			m_sstation_trigger; // acts as out BoundingBox, used a .obj to simplify but should change

	BulletScene		m_bullet_scene;

public:
	UniverseManager();
	~UniverseManager();

	void Init(ResourceManager*);
	void Load();

	void PreparePlayer();
	void PrepareSkybox();

	void Update(const float dt);

	void UpdateSpaceObjects(const float dt);
	void UpdateBulletObjects(const float dt);

	void AddSpaceObject(Planet* the_object, const glm::vec3 world_pos, const glm::vec3 euler_rotation, const glm::vec3 scale);

	void PrepareSpaceObject(DrawableObject* the_object, const glm::vec3 world_pos, const glm::vec3 euler_rotation, const glm::vec3 scale);

	void ClearSpaceObjects();

	void SwapPlayerToBullet() { m_active_player = m_player_bullet; }
	//https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
	bool AABBIntersect(Model&, Model&);

	Player* GetActivePlayer() const { return m_active_player; }
	PlayerSpace* GetSpacePlayer() const { return m_player_space; }
	PlayerBullet* GetBulletPlayer() const { return m_player_bullet; }
	Camera* GetCamera() const { return m_camera; }
	SkyBox* GetSkybox() const { return m_skybox; }
	SpaceStation* GetSStation() const { return m_sstation; }
	BulletScene* GetBulletScene() { return &m_bullet_scene; }
	ResourceManager* GetResManager() const { return m_pResource_manager; }

	StateSystem	m_state_system;

	std::vector<Planet*> m_space_objects;

	// testing member method pointer but no work
	//glm::vec3(SpaceStation::* ptConstMember)();
};