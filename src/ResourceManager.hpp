#pragma once

#include <string>

#include <assimp\Importer.hpp>

//Singleton
//Context-addressed resources
//Context can be abstract (void*)

class ResourceManager
{
public:
	bool init();
	static ResourceManager* getResourceManager();

	//addResource

protected:
	static ResourceManager* _manager;
};