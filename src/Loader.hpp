#pragma once

#include "Error.hpp"
#include "SceneGraph.hpp"
#include "MaterialManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> 
#include <assimp/scene.h>

#include <IL\il.h>

#include <vector>

#define FLOATS_PER_POSITION 4

//TODO
//dorobit flagy typu nacitat/nagenerovat normaly/tangenty atd
class Loader : public Error
{
public:
	Loader();

	//Creates a scene graph from file or a set of files
	SceneGraph*	loadSceneGraphFromFile(std::wstring path, MaterialManager* manager);
	SceneGraph*	loadSceneGraphFromFiles(std::vector<std::wstring> paths, MaterialManager* manager);

protected:
	//Goes through scene hierarchy
	//Builds scene graph, but does not add geometry
	//Calculates references for each leaf, in order to handle instancing
	Node* _processHierarchyRecursive(const aiScene* scene, std::vector< std::pair<unsigned int, LeafNode*> > &_meshReferenceCounter, const aiNode* node, Node* parent); //ked sa vola rekurzivne, moze pretiect zasobnik pri velkych scenach
	
	//Loads mesh raw data
	//Fills in _meshReferenceCounter array, initializes it to zero
	//Returns false if anything goes wrong
	bool  _processMeshes(aiMesh** const meshes, unsigned int count);

	//Loads materials from file
	//Creates also raw textures
	//Respects original order of materials from Assimp
	bool _processMaterialsAndRawTextures(aiMaterial** const materials, unsigned int numMaterials, bool loadTextures, MaterialManager* manager);


	std::wstring _getILErrorString(ILenum error);
	//Loads an array of aiVector3D into a linear array
	//Array is allocated
	//The vertices are loaded and then expanded if necessary (vec3->vec4 f.e.)
	//Expand value is copied to expand components
	//Returns nullptr if something goes wrong
	//VEC_T is a vector type supporting [] operator
	template <typename T, typename VEC_T>
	T*	_loadVectors(VEC_T* vertices, unsigned int vecTypeLength, unsigned int vectorCount, unsigned int expandTo, T	expandValue)
	{
		if (expandTo == 0)
			expandTo = vecTypeLength;

		T* data = new T[expandTo*vectorCount];
		T* d = data;

		if (data == NULL)
			return nullptr;

		//Keeping size & shrinking
		if (expandTo <= vecTypeLength)
		{
			for (unsigned int i = 0; i < vectorCount; ++i)
			{
				for (unsigned int j = 0; j < expandTo; ++j)
				{
					*d = vertices[i][j];
					d++;
				}
			}
		}
		//Expanding
		else if (expandTo > vecTypeLength)
		{
			for (unsigned int i = 0; i < vectorCount; ++i)
			{
				for (unsigned int j = 0; j < vecTypeLength; ++j)
				{
					*d = vertices[i][j];
					d++;
				}

				const unsigned int diff = expandTo - vecTypeLength;
				for (unsigned int j = 0; j < diff; ++j)
				{
					*d = expandValue;
					d++;
				}
			}
		}
		//else just return nullptr

		return data;
	}
};