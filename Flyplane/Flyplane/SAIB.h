#pragma once
#include "glm/gtx/projection.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include <iostream>
namespace SAIB {//STANDARD ARTIFICIAL INTELLIGENCE BEHAVIOURS

	glm::vec3 flyTo(glm::vec3 position, glm::quat orientation, glm::vec3 target);
	glm::vec3 fly_to(glm::vec3 position, glm::quat orientation, glm::vec3 target);

	float rollTowards(glm::vec3 position, glm::quat orientation, glm::vec3 target);

	float pitchTowards(glm::vec3 position, glm::quat orientation, glm::vec3 target);

	float yawTowards(glm::vec3 position, glm::quat orientation, glm::vec3 target);

	float testAxis(glm::vec3 position, glm::quat orientation, glm::vec3 target, glm::vec3 axis);

	float testTowards(glm::vec3 v1, glm::vec3 v2, glm::vec3 t);
}