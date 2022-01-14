#include "UniverseManager.h"
#include <time.h>

UniverseManager::UniverseManager()
{

}

UniverseManager::~UniverseManager()
{
	delete m_camera;
	delete m_player_space;
	delete m_player_bullet;
	
	ClearSpaceObjects();
}

void UniverseManager::Init(ResourceManager* pThe_res_man)
{
	m_pResource_manager = pThe_res_man;

	m_camera = new Camera(
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0));

	m_camera->Init();

	PreparePlayer();
	PrepareSkybox();

	Shader* text_shader = m_pResource_manager->LoadShader("Shaders/textshader.vert");
	m_the_font.the_shader = text_shader;
	m_the_font.Init(m_pResource_manager->GetWindowWidth(), m_pResource_manager->GetWindowHeight(),
		m_pResource_manager->ResourceFileHandling("Fonts/atari-font.bmp").c_str());
	m_the_font.SetColour(glm::vec4(1.0f, 1.0f, 0.7f, 1.0f));
	m_the_font.SetScale(1.0f);

	// moved bullet initialisation to BulletState enter method
	//m_bullet_scene.Init(m_player_bullet, pThe_res_man);

	Load();

	m_state_system.SwitchState("SpaceState"); // "SpaceState", "DockState", "BulletState"
}

void UniverseManager::Load()
{
	// make shader objects
	Shader* default_shader	= m_pResource_manager->LoadShader("Shaders/default.vert");
	Shader* def_notex_shader= m_pResource_manager->LoadShader("Shaders/default_notex.vert");
	Shader* diffuse_shader	= m_pResource_manager->LoadShader("Shaders/diffused.vert");
	Shader* planet_shader	= m_pResource_manager->LoadShader("Shaders/planet.vert");
	Shader* perlin_shader	= m_pResource_manager->LoadShader("Shaders/perlinnoise.vert");
	Shader* cheaper_perlin	= m_pResource_manager->LoadShader("Shaders/perlin_in_vert.vert");

	diffuse_shader->use();
	diffuse_shader->set_float("light_intensity", 1.0f);
	//planet_shader->set_vec3("light_pos", glm::vec3(0.0f, 0.0f, 0.0f));

	/*AddSpaceObject(
		new Planet(m_pResource_manager->
			LoadModel("Models/brian_01.obj"), 
			diffuse_shader),
		glm::vec3(10.0f, -1.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f));

	AddSpaceObject(
		new Planet(m_pResource_manager->
			LoadModel("Models/brian_01.obj"),
			random_shader),
		glm::vec3(-10.0f, -1.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f));*/

	AddSpaceObject(
		m_sstation = new SpaceStation(m_pResource_manager->
			LoadModel("Models/simple_station/TheBigTardisStation.obj"),
			diffuse_shader),
		glm::vec3(200.0f, -60.0f, 50.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(100.0f, 100.0f, 100.0f));
	m_sstation->Init(false);

	m_player_space->m_pSpace_station = m_sstation;
	m_player_bullet->GetTransform().GetPosition() = m_sstation->GetTransform().GetPosition();

	// big sun
	Planet* temp_planet;
	AddSpaceObject(
		temp_planet = new Planet(m_pResource_manager->
			LoadModel("Models/planetbase.obj"),
			planet_shader, Planet::PlanetType::PlanetSun),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(40.0f));
	temp_planet->m_main_col[2] = { 0.0f };
	temp_planet->m_is_shaded = false;

	AddSpaceObject(
		temp_planet = new Planet(m_pResource_manager->
			LoadModel("Models/simple_gas_cloud_base.obj"),
			perlin_shader, Planet::PlanetType::PlanetGassCloud),
		glm::vec3(80.0f, -50.0f, 500.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(200.0f, 200.0f, 1.0f));

	// is this even needed?
	srand((unsigned)time(NULL));

	float temp = 100.0f;
	// create rest of the universe
	char the_letters[]{ 'D', 'I', 'O', 'N' };
	int letter_count = 0;
	for (size_t i = 1; i < 8; i++)
	{
		if ((i % 2) == 1 && letter_count < 4)
		{
			std::string temp_str = "Models/thatone/";
			temp_str.push_back(the_letters[letter_count]);
			temp_str += ".obj";

			Model* temp_model = m_pResource_manager->LoadModel(temp_str);
			AddSpaceObject(
				temp_planet = new Planet(temp_model, def_notex_shader, Planet::PlanetType::PlanetChar),
				glm::vec3(temp + (i * 20.0f), 0.0f, (i * 10.0f)),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(20.0f)
			);
			temp_planet->m_the_one = the_letters[letter_count];
			temp_planet->m_the_one_index = letter_count;
			temp_planet->m_mass = 800.0f;
			letter_count++;
		}
		else
		{
			AddSpaceObject(
				temp_planet = new Planet(m_pResource_manager->
					LoadModel("Models/planetbase.obj"),
					planet_shader),
				glm::vec3(temp + (i * 40.0f), 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(((rand() % 8) + 4.0f) * 2.0f)
			);
		}

		temp_planet->Init();

		temp = temp_planet->GetTransform().GetPosition().x;
		temp += temp_planet->GetTransform().GetScale().x;

		temp_planet->m_ambient_col[0] = temp_planet->m_main_col[0] = Rand(1.0f);
		temp_planet->m_ambient_col[1] = temp_planet->m_main_col[1] = Rand(1.0f);
		temp_planet->m_ambient_col[2] = temp_planet->m_main_col[2] = Rand(1.0f);
	}
}

void UniverseManager::Update(const float dt)
{
	m_skybox->Draw(*m_camera);

	m_state_system.m_pCurrent_state->OnUpdate(dt);

	m_active_player->Update(dt);
	m_active_player->Draw(*m_camera);
	
	m_camera->update_view(); // todo only update when camera updates
}

void UniverseManager::UpdateSpaceObjects(const float dt)
{
	m_the_font.PrintText("Hello world!", 0, 500);

	// from Brian race code example, sprintf our artifact collection
	/*std::string textBuf = "Collected artifacts: " +
		m_player_space->m_the_one_coll[0] +
		m_player_space->m_the_one_coll[1] +
		m_player_space->m_the_one_coll[2] +
		m_player_space->m_the_one_coll[3];*/
	char textBuf[50]; // istream
	sprintf(textBuf, "Collected artifacts: %c %c %c %c"
		, m_player_space->m_the_one_coll[0]
		, m_player_space->m_the_one_coll[1]
		, m_player_space->m_the_one_coll[2]
		, m_player_space->m_the_one_coll[3]);
	/*printf("Collected artifacts: %c %c %c %c"
		, m_player_space->m_the_one_coll[0]
		, m_player_space->m_the_one_coll[1]
		, m_player_space->m_the_one_coll[2]
		, m_player_space->m_the_one_coll[3]);*/
	m_the_font.PrintText(textBuf, 10, 20);

	Model a_model = m_player_space->GetMeshModel();
	Model station_trig_model = m_sstation->GetMeshModel();

	a_model.SetMax(a_model.GetMax() + m_player_space->GetTransform().GetPosition());
	a_model.SetMin(a_model.GetMin() + m_player_space->GetTransform().GetPosition());
	
	station_trig_model.SetMax((station_trig_model.GetMax() + 
		m_sstation->GetTransform().GetPosition()) + m_sstation->GetTransform().GetScale());
	station_trig_model.SetMin((station_trig_model.GetMin() + 
		m_sstation->GetTransform().GetPosition()) - m_sstation->GetTransform().GetScale());

	if (AABBIntersect(a_model, station_trig_model))
	{
		if (!m_sstation->m_is_dockable && m_player_space->m_the_one_total >= 4)
		{
			m_sstation->m_is_dockable = true;
		}
		
		if (m_sstation->m_is_dockable)
		{
			m_the_font.PrintText("Great collection, press 'C' to dock", 300, 300);
		}
		else
		{
			m_the_font.PrintText("Dock is closed, collect all the artifacts!", 300, 300);
		}
	}

	for (int i = 0; i < m_space_objects.size(); i++)
	{
		Planet* pPlanet = m_space_objects.at(i);
		if (pPlanet->m_is_alive)
		{
			pPlanet->Update(dt);
			pPlanet->Draw(*m_camera);

			// check if planet is an objective target
			if (pPlanet->m_planet_type == Planet::PlanetType::PlanetChar)
			{
				// setting correct BB, scaling it etc, could be done better
				Model b_model = pPlanet->GetMeshModel();
				
				b_model.SetMax((b_model.GetMax() +
					pPlanet->GetTransform().GetPosition()) + pPlanet->GetTransform().GetScale());
				b_model.SetMin((b_model.GetMin() +
					pPlanet->GetTransform().GetPosition()) - pPlanet->GetTransform().GetScale());

				// check if player is inside of our bb
				if (AABBIntersect(a_model, b_model))
				{
					// fill players collection
					m_player_space->m_the_one_coll[pPlanet->m_the_one_index] = pPlanet->m_the_one;
					m_player_space->m_the_one_total++;
					pPlanet->m_is_alive = false;
					printf("%c is hit\n", pPlanet->m_the_one);
				}
			}
		}
	}

	m_the_font.RenderText(); // do this last
}

void UniverseManager::UpdateBulletObjects(const float dt)
{
	glm::vec3 play_pos = m_active_player->GetTransform().GetPosition();
	if (play_pos.z > -80.0f)
	{
		m_the_font.PrintText("Thank you for delivering the ancient artifacts, enjoy our gift!", 400, 600);
		m_bullet_scene.m_light_is_on = true;
	}
	else
	{
		m_the_font.PrintText("Hurry, come forth", 400, 600);
	}

	//UpdateSpaceObjects(dt);
	m_bullet_scene.Update(dt);

	m_player_space->Draw(*m_camera);
	m_bullet_scene.Draw(*m_camera);

	m_the_font.RenderText(); // do this last
}

void UniverseManager::PreparePlayer()
{
	Shader* default_shader			= m_pResource_manager->LoadShader("Shaders/default.vert");
	Shader* default_notex_shader	= m_pResource_manager->LoadShader("Shaders/default_notex.vert");
	Shader* player_shad				= m_pResource_manager->LoadShader("Shaders/diffused.vert");
	Shader* diff_notex_shader		= m_pResource_manager->LoadShader("Shaders/diffused_notexture.vert");
	// Shader* md2_shader = ;

	PrepareSpaceObject(
		m_player_space = new PlayerSpace(
			m_pResource_manager->LoadModel("Models/SpaceShip.obj"),
			player_shad),
		glm::vec3(80.0f, -40.0f, -10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	m_player_space->Init(m_camera);

	m_player_bullet = new PlayerBullet(
		m_pResource_manager->LoadModel("Models/modelpack0/samourai/Samourai.md2"),
		default_shader,
		m_bullet_scene.GetPointerPhysicsDraw());
	m_player_bullet->Init(m_camera);

	m_active_player = m_player_space;
}

void UniverseManager::PrepareSkybox()
{
	Shader* skybox_shad = m_pResource_manager->LoadShader("Shaders/skyshader.vert");

	m_skybox = new SkyBox(skybox_shad);
	m_skybox->Init(*m_pResource_manager);

	PrepareSpaceObject(
		m_skybox,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(50.0f, 50.0f, 50.0f));
}

void UniverseManager::AddSpaceObject(Planet* the_object, const glm::vec3 world_pos, const glm::vec3 euler_rotation, const glm::vec3 scale)
{
	m_space_objects.push_back(the_object);

	PrepareSpaceObject(the_object, world_pos, euler_rotation, scale);
}

void UniverseManager::PrepareSpaceObject(DrawableObject* the_object, const glm::vec3 world_pos, const glm::vec3 euler_rotation, const glm::vec3 scale)
{
	Transform& the_transform = the_object->GetTransform();
	the_transform.SetPosition(world_pos);
	the_transform.SetEulerAngles(euler_rotation);
	the_transform.SetScale(scale);
}

bool UniverseManager::AABBIntersect(Model& a, Model& b)
{
	return	(a.GetMin().x <= b.GetMax().x && a.GetMax().x >= b.GetMin().x) &&
			(a.GetMin().y <= b.GetMax().y && a.GetMax().y >= b.GetMin().y) &&
			(a.GetMin().z <= b.GetMax().z && a.GetMax().z >= b.GetMin().z);
}

void UniverseManager::ClearSpaceObjects()
{
	return;
	// clean the memory
	{
		std::vector< Planet* >::iterator it;
		for (it = m_space_objects.begin(); it != m_space_objects.end(); it++)
		{
			MeshModel* mesh = dynamic_cast<MeshModel*>(&(*it)->GetMeshModel());

			if (mesh != nullptr)
			{
				std::map<std::string, GLuint>::iterator it_textures;
				for (it_textures = mesh->m_textures.begin(); it_textures != mesh->m_textures.end(); it_textures++)
				{
					glDeleteTextures(1, &it_textures->second);
				}

				std::vector<MeshModel::DrawObject>::iterator it_buffers;
				for (it_buffers = mesh->m_drawObjects.begin(); it_buffers != mesh->m_drawObjects.end(); it_buffers++)
				{
					glDeleteBuffers(1, &it_buffers->vb_id);
				}
			}
			if ((*it) != nullptr)
			delete (*it);
		}
	}
}