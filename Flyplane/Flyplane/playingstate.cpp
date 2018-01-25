#include "playingstate.h"
#include <glm\gtc\constants.hpp>

#include "physicssystem.h"
#include "transform.h"
#include "weaponsystem.h"
#include "rendersystem.h"
#include "heightmap.h"

Model m;
void PlayingState::init()
{
	m.load("assets/MIG-212A.fbx");
	/*
	* add systems
	* 
	* example

	ex.systems.add<System class here>();
	*/
	ex.systems.add<PhysicsSystem>();
	ex.systems.add<WeaponSystem>();
	ex.systems.add<RenderSystem>();
	ex.systems.configure();

	/*
	* add entities
	*
	* example

	auto entity = ex.entities.create();
	entity.assign<Component class here>(Component constructor params);
	*/
	


	for (int i = 0; i < 200; i++) {
		auto entity = ex.entities.create();
		glm::vec3 pos(rand() % 100, rand() % 100, rand() % 100);
		glm::quat orien(rand() % 100, rand() % 100, rand() % 100, rand() % 100);
		entity.assign<Transform>(pos, normalize(orien));
		//entity.assign<Physics>(1000.0, 1.0, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0));
		entity.assign <ModelComponent>(&m);
	}



	entityx::Entity wep1 = ex.entities.create();

	Model* weaponmodel;

	WeaponStats* stats = new WeaponStats(100, 100, 100, 0.2);

	wep1.assign<Weapon>(stats, weaponmodel);
}

void PlayingState::update(double dt)
{
	double time = t.elapsed();

	double r = (1 + sin(time))*0.5;
	double g = (1 + sin(time + glm::two_pi<double>() / 3.0))*0.5;
	double b = (1 + sin(time + 2.0*glm::two_pi<double>() / 3.0))*0.5;
	glClearColor(r,g,b, 1.0);

	/*
	ex.systems.update<System class here>(dt);
	*/
	

	ex.systems.update<PhysicsSystem>(dt);
	ex.systems.update<WeaponSystem>(dt);
	ex.systems.update<RenderSystem>(dt);
}
