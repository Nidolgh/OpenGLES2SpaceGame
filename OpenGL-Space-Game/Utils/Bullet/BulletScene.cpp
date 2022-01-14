#include "BulletScene.h"

BulletScene::BulletScene() // don't so anything special here, leave it to the init
{

}

BulletScene::~BulletScene()
{
	// shut down the opengl and close any vectors maps and lists currently active so you can exit clean
	delete DynamicPhysicsWorld;
	delete ConstraintSolver;
	delete BroadPhase;
	delete Dispatcher;
	delete CollisionConfiguration;

	// clean the memory
	{
		std::vector< BulletObject* >::iterator it;
		for (it = m_bullet_objects.begin(); it != m_bullet_objects.end(); it++)
		{
			delete (*it);
		}
	}
}

// could do this in constuctor, but better to do it here, set up all the Game stuff needed
bool BulletScene::Init(PlayerBullet* pPlayer_bullet, ResourceManager* pResource_manager)
{
	m_the_player = pPlayer_bullet;

	// sort out our font.
	Shader* default_shader		= pResource_manager->LoadShader("Shaders/default.vert");
	Shader* diff_notex_shader	= pResource_manager->LoadShader("Shaders/diffused_notexture.vert");
	Shader* diffuse_shader		= pResource_manager->LoadShader("Shaders/diffused.vert");
	Shader* planet_shader		= pResource_manager->LoadShader("Shaders/planet.vert");
	
	m_diff_notex_shader = diff_notex_shader;
	m_diff_shader = diffuse_shader;

	// initialise our physics, create the main physics systems
	BroadPhase				= new btDbvtBroadphase();
	CollisionConfiguration	= new btDefaultCollisionConfiguration();
	Dispatcher				= new btCollisionDispatcher(CollisionConfiguration);
	ConstraintSolver		= new btSequentialImpulseConstraintSolver;
	DynamicPhysicsWorld		= new btDiscreteDynamicsWorld(Dispatcher, BroadPhase, ConstraintSolver, CollisionConfiguration);
	
	// set a "normal" gravity level
	DynamicPhysicsWorld->setGravity(btVector3(0, -9.81f, 0));
	
	glm::vec3 scale = m_the_player->GetTransform().GetScale();
	glm::vec3 pos = m_the_player->GetTransform().GetPosition();
	// ThePlayer->TheModelManager->GetBoundingBoxes(ThePlayer); // he's going to be physics based so get his sizes
	glm::vec3 Box = glm::vec3((m_the_player->GetMeshModel().GetMax() - m_the_player->GetMeshModel().GetMin()) / 2.0f);
	m_the_player->m_phys_obj = CreatePhysicsObj(
		new btBoxShape(btVector3(Box.x, Box.y, Box.z)), // shape btVector3(1.2, 0.8, 3)
		2000, // mass
		btVector3(pos.x, pos.y + 1.0f, pos.z), // position
		btQuaternion(0, 90.0f, 0, 1)
	);
	m_the_player->m_phys_obj->GetRigidBody()->getCollisionShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
	m_the_player->m_phys_obj->GetRigidBody()->setUserPointer(m_the_player);

	// put our ground plane into physics
	//AddBulletObject(
	//	nullptr, // pResource_manager->LoadModel("Models/big_floor.obj")
	//	diff_notex_shader,
	//	this->CreatePhysicsObj(
	//		new btBoxShape(btVector3(500, 1.0f, 500.0f)), // shape
	//		0, // mass
	//		btVector3(0.0f, (pos.y - 2.05f), 0.0f), // position based on player as we know his y is close on the floor
	//		btQuaternion(0, 0, 0, 1) // no rotation
	//	),
	//	glm::vec3(1.0f, 1.0f, 1.0f)
	//);

	// race game demo, using temp vert array vector, should be abstracted away
	btTriangleMesh* triangleMeshTerrain = new btTriangleMesh();
	Model* temp_interior_model = pResource_manager->LoadModel("Models/simple_station_interior/SpaceStationScene_CollisonConvex.obj");
	std::vector<btVector3> temp_triangle;
	std::vector< std::vector<float> >::iterator it_ver_obj;
	for (it_ver_obj = pResource_manager->m_temp_vertex_buffer.begin(); it_ver_obj != pResource_manager->m_temp_vertex_buffer.end(); it_ver_obj++) // stride
	{
		std::vector<float>::iterator it_verts;
		for (it_verts = it_ver_obj->begin(); it_verts != it_ver_obj->end(); it_verts += 11)
		{
			// temp_triangle.push_back(btVector3((*it_verts), (*it_verts + 1), (*it_verts + 2))); // not working cause of wrongly set int
			temp_triangle.push_back(btVector3((*(it_verts)), (*(it_verts + 1)), (*(it_verts + 2))));
			if (temp_triangle.size() >= 3)
			{
				triangleMeshTerrain->addTriangle(temp_triangle.at(0), temp_triangle.at(1), temp_triangle.at(2));
				temp_triangle.clear();
			}
		}
	}
	AddBulletObject(
		pResource_manager->LoadModel("Models/simple_station_interior/SpaceStationScene.obj"),
		diffuse_shader,
		this->CreatePhysicsObj(
			new btBvhTriangleMeshShape(triangleMeshTerrain, true), // shape
			0, // mass
			btVector3(0.0f, 0.0f, 0.0f), // position based on player as we know his y is close on the floor
			btQuaternion(0, 0, 0, 1) // no rotation
		),
		glm::vec3(1.0f, 1.0f, 1.0f)
	);

	// spawn a triangle of boxes
	int base_width = 6;
	for (size_t y = 0; y < base_width; y++)
		for (size_t x = base_width - y; x > 0; x--)
		{
			AddBulletObject(
				pResource_manager->LoadModel("Models/container_box.obj"),
				diffuse_shader,
				this->CreatePhysicsObj(
					new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)), // shape
					500, // mass
					btVector3(0.0f * (x * 1.5f), 2.0f + (y * 1.1f), 10.0f), // position
					btQuaternion(0, 0, 0, 1) // no rotation
				),
				glm::vec3(1.0f, 1.0f, 1.0f)
			);
		}

	// load and init t h e models
	char temp[]{ 'O', 'N', 'D', 'I' };
	for (size_t i = 0; i < 4; i++)
	{
		std::string temp_str = "Models/thatone/";
		temp_str.push_back(temp[i]);
		temp_str += ".obj";

		Model* temp_model = pResource_manager->LoadModel(temp_str);
		glm::vec3 temp_box = glm::vec3((temp_model->GetMax() - temp_model->GetMin()) / 2.0f);
		AddBulletObject(
			temp_model,
			diff_notex_shader,
			this->CreatePhysicsObj(
				new btBoxShape(btVector3(temp_box.x, temp_box.y, temp_box.z)), // shape
				500, // mass
				btVector3(0.0f + (i * 2.0f), 0.6f, 20.0f), // position
				btQuaternion(0, 0, 0, 1) // no rotation
			),
			glm::vec3(1.0f)
		);
	}

	m_diff_shader->set_float("light_intensity", 0.0f);

	m_pPhysicsDrawer = new PhysicsDraw();
	m_pPhysicsDrawer->setDebugMode(0);
	DynamicPhysicsWorld->setDebugDrawer(m_pPhysicsDrawer);
	m_pPhysicsDrawer->m_the_shader = pResource_manager->LoadShader("Shaders/simpletri.vert");

	// no crash when this is flagged
	m_pPhysicsDrawer->ToggleDebugFlag(1 + 2);

	return true;
}

float tt = 0.0f;
bool BulletScene::Update(float dt)
{
	tt += dt;
	m_the_player->Update(dt);

	// do some shader stuff
	m_diff_shader->use();
	if (m_light_is_on)
		m_diff_shader->set_float("light_intensity", 1.0f);
	else
		m_diff_shader->set_float("light_intensity", 0.0f);
	//m_diff_shader->set_vec3("light_pos", m_the_player->GetTransform().GetPosition());
	
	m_diff_notex_shader->use();
	if (m_light_is_on)
		m_diff_notex_shader->set_float("light_intensity", 1.0f);
	else
		m_diff_notex_shader->set_float("light_intensity", 0.0f);
	//m_diff_notex_shader->set_vec3("light_pos", m_the_player->GetTransform().GetPosition()/* + glm::vec3(0.0f, 6.0f, 0.0f)*/);

	// now time for bullet to do its thing
	DynamicPhysicsWorld->stepSimulation(1 / 60.0f, 5);

	std::vector< BulletObject* >::iterator it;
	for (it = m_bullet_objects.begin(); it != m_bullet_objects.end(); it++)
	{
		(*it)->Update(dt);
	}
	
	CheckForCollision();

	return true;
}

bool BulletScene::Draw(const Camera& the_active_camera)
{
	std::vector< BulletObject* >::iterator it;
	for (it = m_bullet_objects.begin(); it != m_bullet_objects.end(); it++)
	{
		//if (!this->m_pPhysicsDrawer->m_debugMode) // will kill stuff
			(*it)->Draw(the_active_camera);
	}

	DynamicPhysicsWorld->debugDrawWorld();
	m_pPhysicsDrawer->m_the_shader->use();
	glm::mat4 VP = the_active_camera.get_projection_m4() * the_active_camera.get_view_m4();  // Remember order seems backwards
	m_pPhysicsDrawer->m_the_shader->set_mat4("MVP", VP);
	this->m_pPhysicsDrawer->DoDebugDraw(the_active_camera);

	return true;
}

void BulletScene::AddBulletObject(Model* the_mesh, Shader* the_shader, PhysicsObj* pPhys_obj, glm::vec3 scale)
{
	BulletObject* foo = new BulletObject(the_mesh, the_shader);
	foo->m_is_shaded = false;
	m_bullet_objects.push_back(foo);
	pPhys_obj->GetRigidBody()->setUserPointer(foo);
	foo->m_phys_obj = pPhys_obj;

	foo->GetTransform().SetScale(scale);
	foo->m_phys_obj->GetRigidBody()->getCollisionShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
}

void BulletScene::CheckForCollision() {
	// keep a list of the collision pairs we finf during the current update
	ManifoldPairs pairsThisFrame;

	// go through all the manifolds in the Dispatchers list
	for (int i = 0; i < Dispatcher->getNumManifolds(); i++)
	{
		btPersistentManifold* Manifold = Dispatcher->getManifoldByIndexInternal(i);
		if (Manifold->getNumContacts() > 0) 	// we only care if they have contact points
		{
			//getBodyx actually returns btCollisionObject, and we really need to get btRigidBody instead for compares
			//so we have to use the C++ styly static cast. (or plant an idvalue in the rigid body if they are unique)
			const btRigidBody* Body0 = static_cast<const btRigidBody*>(Manifold->getBody0());
			const btRigidBody* Body1 = static_cast<const btRigidBody*>(Manifold->getBody1());

			// since they are pointers, they have numerical value, use that to store them lowest value first
			const btRigidBody* FirstRB = (Body0 > Body1) ? Body1 : Body0;
			const btRigidBody* SecondRB = (Body0 > Body1) ? Body0 : Body1;

			// create the pair
			CollisionObjects NewPair = std::make_pair(FirstRB, SecondRB);

			// insert the pair into the current list
			pairsThisFrame.insert(NewPair);

			//this pair definitely are colliding, if they are brand new we can safely say its a collsion event
			if (pairsLastFrame.find(NewPair) == pairsLastFrame.end()) // search the old list for this new pair
			{
				CollisionEvent((btRigidBody*)Body0, (btRigidBody*)Body1); // got through to the end...it wasn't there so this is a new hit
			}
		} // if
	} // for i

	// create another list for pairs that were removed this update, they will be seperated events
	ManifoldPairs removedPairs;
	// compare the set from lset frame with the	set this frame and put the removed one sin the removed Pairs
	std::set_difference(pairsLastFrame.begin(), pairsLastFrame.end(), pairsThisFrame.begin(), pairsThisFrame.end(), std::inserter(removedPairs, removedPairs.begin()));

	// iterate through all of the removed pairs sending separation events for them, can't use an index for this so this is a C++11 iterer
	for (ManifoldPairs::const_iterator iter = removedPairs.begin(); iter != removedPairs.end(); ++iter)
	{
		SeparationEvent((btRigidBody*)iter->first, (btRigidBody*)iter->second);
	}

	// because the names for sets are pointers, we can simply tell the Lart pair it is now the current pair
	pairsLastFrame = pairsThisFrame;

}

void BulletScene::CollisionEvent(btRigidBody* RB0, btRigidBody* RB1)
{
	BulletObject* First = (BulletObject*)RB0->getUserPointer(); // assumes we set up the user pointer with an ObjectModel*
	BulletObject* Second = (BulletObject*)RB1->getUserPointer(); // and there are handling routines in ObjectModel or overrides in derived
	First->HandleCollision(Second);
	Second->HandleCollision(First);
}

void BulletScene::SeparationEvent(btRigidBody* RB0, btRigidBody* RB1)
{
	printf("Help I've been seperated sob \n");
}

PhysicsObj* BulletScene::CreatePhysicsObj(btCollisionShape* pShape,
	const float& mass,
	const btVector3& aPosition,
	const btQuaternion& aRotation)
{
	// create a new game object
	PhysicsObj* pPhysicsObject = new PhysicsObj(pShape, mass, aPosition, aRotation);
	float currentm = pShape->getMargin();
	pPhysicsObject->GetRigidBody()->setUserPointer(this);
	pShape->setMargin(0.09f);

	if (DynamicPhysicsWorld)
	{
		DynamicPhysicsWorld->addRigidBody(pPhysicsObject->GetRigidBody());
	}
	else { printf("Attempting to add to a Dynamic world that does not exist\n"); }

	return pPhysicsObject; // return to place in the objects data or in another list
}