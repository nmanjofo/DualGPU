#pragma once

#include "Material.hpp"

#include <vector>

class MaterialManager
{
public:
	//Adds a material with raw data

	unsigned int addMaterial(Material* material);
	const Material* getMaterial(unsigned int index) const;

protected:
	std::vector<Material*> _materials;

};