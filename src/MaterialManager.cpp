#include "MaterialManager.hpp"

MaterialManager* MaterialManager::_instance = nullptr;


MaterialManager::MaterialManager()
{
	_numStoredMaterials = 0;
}

MaterialManager* MaterialManager::getMaterialManager()
{
	if (_instance == nullptr)
	{
		_instance = new MaterialManager();
	}

	return _instance;
}

unsigned int MaterialManager::addMaterial(RawMaterial* material)
{
	_materials[_numStoredMaterials] = material;
	++_numStoredMaterials;
}

const RawMaterial* MaterialManager::getMaterialByIndex(unsigned int index) const
{
	try
	{
		return _materials.at(index);
	}
	catch (std::out_of_range)
	{
		return nullptr;
	}
}

const RawMaterial* MaterialManager::getMaterialByName(const std::wstring& name) const
{
	auto it = _materials.begin();
	const auto end = _materials.end();

	for (it; it != end; ++it)
	{
		if (name.compare(it->second->getName()) == 0)
			return it->second;
	}

	return nullptr;
}

void MaterialManager::deleteMaterialByIndex(unsigned int index)
{
	if (getMaterialByIndex(index) != nullptr)
	{
		delete _materials[index];
		_materials.erase(index);
	}
}

void MaterialManager::deleteMaterialByName(const std::wstring& name)
{
	auto it = _materials.begin();
	const auto end = _materials.end();

	for (it; it != end; ++it)
	{
		if (name.compare(it->second->getName()) == 0)
		{
			delete it->second;
			_materials.erase(it);
			return;
		}
	}
}

void MaterialManager::deleteAll()
{
	auto it = _materials.begin();
	const auto end = _materials.end();

	for (it; it != end; ++it)
	{
		delete it->second;
		_materials.erase(it);
	}
}