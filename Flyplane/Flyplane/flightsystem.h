#pragma once

#include <entityx\entityx.h>
#include <entityx\Entity.h>
#include "transform.h"
#include "physics.h"
#include "input.h"
#include "flightcomponent.h"
#include <iostream>

using namespace entityx;

struct FlightSystem : public entityx::System<FlightSystem> {

	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override {

		ComponentHandle<Physics> physics;
		ComponentHandle<Transform> transform;
		ComponentHandle<FlightComponent> flight;
		for (Entity entity : es.entities_with_components(physics, transform, flight)) {
			physics = entity.component<Physics>();
			transform = entity.component<Transform>();
			flight = entity.component<FlightComponent>();

			glm::vec3 angular_vel;

			angular_vel.z = flight->turnrate * flight->roll;
			angular_vel.x = -flight->turnrate * flight->pitch;

			angular_vel = glm::toMat3(transform->orientation)*angular_vel;

			glm::quat avq{
				0,
				angular_vel.x,
				angular_vel.y,
				angular_vel.z
			};
			glm::quat spin = 0.5f*avq*transform->orientation;
			transform->orientation += spin * float(dt);
			transform->orientation = normalize(transform->orientation);

			float power = ((flight->engine_power * flight->throttle) / physics->calcTotalDrag());
			physics->velocity += glm::toMat3(transform->orientation)*glm::vec3(0.0, 0.0, 1.0) * power;
		}
	};
};