#pragma once


#include <entityx\entityx.h>
#include <entityx\Entity.h>

#include "modelcomponent.h"
#include "transform.h"

using namespace entityx;

class RenderSystem : public System<RenderSystem> {

	void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
		ComponentHandle<ModelComponent> model;
		ComponentHandle<Transform> transform;
		for (Entity entity : es.entities_with_components(model, transform)) {
			model = entity.component<ModelComponent>();
			transform = entity.component<Transform>();


		}
	}

};