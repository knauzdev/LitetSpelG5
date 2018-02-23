#pragma once

#include <iostream>
#include "particlecomponent.h"
#include "addParticleEvent.h"
#include "removeParticleEvent.h"
#include <entityx\entityx.h>
#include <entityx\Event.h>

class ParticleSystem : public entityx::System<ParticleSystem>, public entityx::Receiver<ParticleSystem> {

	size_t searchIndex = 0;
	std::vector<Particles> pool;

	ComputeShader resetShader;
	ComputeShader trailShader;
	ComputeShader explosionShader;
	ComputeShader engineTrailShader;
	ComputeShader deadTrailShader;
	ComputeShader sparkShader;
	ShaderProgram program;
public:
	void configure(entityx::EventManager &eventManager) {
		eventManager.subscribe<entityx::ComponentAddedEvent<ParticleComponent>>(*this);
		eventManager.subscribe<entityx::EntityDestroyedEvent>(*this);
		eventManager.subscribe<AddParticleEvent>(*this);
		eventManager.subscribe<RemoveParticleEvent>(*this);
		resetShader.create("particlereset.glsl");
		trailShader.create("particletrail.glsl");
		explosionShader.create("explosionCompute.glsl");
		engineTrailShader.create("engineTrail.glsl");
		deadTrailShader.create("deadTrail.glsl");
		sparkShader.create("sparks.glsl");
		program.create("partVert.glsl", "partGeom.glsl", "partFrag.glsl");

		for (int i = 0; i < 200; i++) {
			pool.emplace_back(5000);
		}
	}

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override {
		entityx::ComponentHandle<ParticleComponent> particles;
		for (Entity entity : es.entities_with_components(particles)) {
			for (size_t i = 0; i < particles->systems.size(); i++)
			{
				if (!particles->systems[i]->isAlive())
				{
					particles->systems[i]->used = false;
					particles->systems[i]->t.restart();
					particles->systems.erase(particles->systems.begin() + i);
					i--;
					std::cout << "[DEBUG] - Removed timed particleSystem \n";
				}
			}

			for (auto &p : particles->systems)
			{
				chooseUniforms(p, entity, dt);
				p->update();
			}
		}
	}

	void chooseUniforms(Particles *& p, entityx::Entity &entity, const entityx::TimeDelta &dt)
	{
		auto transform = entity.component<Transform>();
		switch (p->type) {
		case TRAIL:
			p->setComputeShader(trailShader);
			p->setTexture("N/A");
			p->setSize(0.1);
			trailShader.use();
			if (transform) {
				trailShader.uniform("spawn", transform->pos);
				trailShader.uniform("direction", transform->orientation * glm::vec3(0, 0, -1));
			}
			trailShader.uniform("life", 10.f);
			trailShader.uniform("dt", float(dt));
			break;
		case EXPLOSION:
			p->setComputeShader(explosionShader);
			p->setTexture("explosion");
			p->setSize(0.05);
			explosionShader.use();
			if (transform) {
				explosionShader.uniform("spawn", transform->pos);
				explosionShader.uniform("direction", transform->orientation * glm::vec3(0, 0, -1));
			}
			explosionShader.uniform("life", 1.f);
			explosionShader.uniform("dt", float(dt));
			break;
		case SPARKS:
			p->setComputeShader(sparkShader);
			p->setTexture("N/A");
			p->setSize(0.01);
			sparkShader.use();
			if (transform) {
				sparkShader.uniform("spawn", transform->pos);
				sparkShader.uniform("direction", transform->orientation * glm::vec3(0, 1, 0));
			}
			sparkShader.uniform("life", 1.f);
			sparkShader.uniform("dt", float(dt));
			break;
		case ENGINE_TRAIL:
			p->setComputeShader(engineTrailShader);
			p->setTexture("engine_fire");
			p->setSize(0.05);
			engineTrailShader.use();
			if (transform) {
				engineTrailShader.uniform("spawn", transform->pos);
				engineTrailShader.uniform("direction", transform->orientation * glm::vec3(0, 0, -1));
			}
			engineTrailShader.uniform("life", 1.f);
			engineTrailShader.uniform("dt", float(dt));
			break;
		case DEAD_TRAIL:
			p->setComputeShader(deadTrailShader);
			p->setTexture("N/A");
			p->setSize(0.1);
			deadTrailShader.use();
			if (transform) {
				deadTrailShader.uniform("spawn", transform->pos);
				deadTrailShader.uniform("direction", transform->orientation * glm::vec3(0, 0, -1));
			}
			deadTrailShader.uniform("life", 10.f);
			deadTrailShader.uniform("dt", float(dt));
			break;
		default:
			// plz no
			break;
		}
	}

	void receive(const entityx::ComponentAddedEvent<ParticleComponent>& event) {
		std::cout << "[DEBUG] Adding ParticleComponent\n";
	}

	// cancer
	
	void receive(const AddParticleEvent& event) {
		auto handle = event.component;

		Particles* free = nullptr;
		for (int i = 0; i < pool.size(); i++) {
			if (!pool[i].used) {
				pool[i].used = true;
				free = &pool[i];
				break;
			}
			searchIndex = (searchIndex + 1) % pool.size();
		}
		if (free) {
			free->program = &program;
			free->type = event.type;
			free->t.restart();
			free->setTimer(event.effectLength);
			resetShader.use();
			free->setComputeShader(resetShader);
			free->update();
			handle->systems.push_back(free);
		}
		else {
			for (int i = 0; i < 20; i++) {
				pool.emplace_back(5000);
			}
			for (int i = 0; i < pool.size(); i++) {
				if (!pool[i].used) {
					pool[i].used = true;
					free = &pool[i];
					break;
				}
				searchIndex = (searchIndex + 1) % pool.size();
			}
			if (free) {
				free->program = &program;
				free->type = event.type;
				free->t.restart();
				free->setTimer(event.effectLength);
				resetShader.use();
				free->setComputeShader(resetShader);
				free->update();
				handle->systems.push_back(free);
			}
			std::cout << "[WARNING] out of particles in pool, creating new!\n";
		}
	}
	void receive(const RemoveParticleEvent& event)
	{
		auto handle = event.component;
		for (unsigned i = 0; i < handle->systems.size(); i++)
		{
			if (handle->systems[i]->type == event.type)
			{
				handle->systems[i]->used = false;
				handle->systems[i]->t.restart();
				handle->systems.erase(handle->systems.begin() + i);
				i--;
				break;
			}
		}
	}
	void receive(const entityx::EntityDestroyedEvent& event) {
		auto entity = event.entity;
		auto handle = entity.component<ParticleComponent>();
		if (handle) {
			for (auto &s : handle->systems)
			{
				s->used = false;
				s = nullptr;
			}
			//handle->system = nullptr;
		}
	}

};
