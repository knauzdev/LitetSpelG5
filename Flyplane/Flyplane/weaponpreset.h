#pragma once
#include <string>
#include <vector>
#include "glm\glm.hpp"
#include "weapon.h"

class WeaponPreset {
public:
	std::string name;
	std::string file;
	std::string model;
	std::string projModel;
	unsigned int ammo, lifetime;
	double speed, mass, cooldown, turnRate, scale, projScale;
	bool infAmmo, isMissile, dissappear;
	double detonateRange, explodeRadius, damage, droptime = 1.0;
	glm::vec3 extraOffset = glm::vec3(0,0,0);
	WeaponPreset() {}
	~WeaponPreset() {}
	void load(std::string presetname);
	Weapon* getWeapon();
};