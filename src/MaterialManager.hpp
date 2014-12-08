#pragma once

#include "Material.hpp"

#include <vector>

class MaterialManager
{
public:
	//Adds a material with raw data

	unsigned int addMaterial();
	const Material* getMaterial(unsigned int index) const;

protected:
	std::vector<Material*> _materials;

};