#pragma once

#include "RawMaterial.hpp"

#include <map>

class MaterialManager
{
public:
	static MaterialManager* getMaterialManager();

	//Adds a material with raw data
	//Returns its index
	unsigned int	addMaterial(RawMaterial* material);
	const RawMaterial* getMaterialByIndex(unsigned int index) const;
	const RawMaterial* getMaterialByName(const std::wstring& name) const;

	void			deleteMaterialByIndex(unsigned int index);
	void			deleteMaterialByName(const std::wstring& name);
	void			deleteAll();

protected:
	MaterialManager();

	std::map<unsigned int, RawMaterial*>	_materials;
	unsigned int							_numStoredMaterials;

	static MaterialManager*					_instance;
};