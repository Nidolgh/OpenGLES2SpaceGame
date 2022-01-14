#pragma once
// #include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <algorithm> 
#include <iterator>
#include <map>
#include <set>
#include <vector>

#include <Headers/BulletObject.h>
#include <Headers/PlayerBullet.h>

#include <Utils/Managers/ResourceManager.h>

// Largely based on the example car project from Brian
class BulletScene
{
protected:
	typedef std::pair<const btRigidBody*, const btRigidBody*> CollisionObjects;
	typedef std::set<CollisionObjects> ManifoldPairs;

	std::vector<BulletObject*> m_bullet_objects;

	std::map<std::string, btCollisionShape*> ExistingColShapes;
	ManifoldPairs pairsLastFrame; // the pairs from last frame need to be stored

	// bullet 
	btBroadphaseInterface* BroadPhase;
	btDefaultCollisionConfiguration* CollisionConfiguration;
	btCollisionDispatcher* Dispatcher;
	btSequentialImpulseConstraintSolver* ConstraintSolver;
	btDiscreteDynamicsWorld* DynamicPhysicsWorld;

	PlayerBullet* m_the_player;
	// groundplane: GP?
	// TrackManager TM;

	PhysicsDraw* m_pPhysicsDrawer;

	// temp shader
	Shader* m_diff_notex_shader;
	Shader* m_diff_shader;

public:
	BulletScene();
	~BulletScene();

	bool	Init(PlayerBullet* pPlayer_bullet, ResourceManager* pResource_manager);
	bool	Update(float dt);
	bool	Draw(const Camera& the_active_camera);

	void AddBulletObject(Model* the_mesh, Shader* the_shader, PhysicsObj* pPhys_obj, glm::vec3 scale);
	
	// we need a simple means to create the physics data	
	PhysicsObj* CreatePhysicsObj(btCollisionShape* pShape,
		const float& mass,
		const btVector3& Position = btVector3(0, 0, 0),
		const btQuaternion& Rotation = btQuaternion(0, 0, 0, 1)); // default no rotation using w 1

	void CheckForCollision();
	void CollisionEvent(btRigidBody* RB0, btRigidBody* RB1);
	void SeparationEvent(btRigidBody* RB0, btRigidBody* RB1);

	PhysicsDraw* GetPointerPhysicsDraw() { return m_pPhysicsDrawer; }

	bool m_light_is_on = false;
};