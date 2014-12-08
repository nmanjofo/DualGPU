#pragma once

#include "Node.hpp"
#include "Drawable.hpp"
#include "RawMesh.hpp"

#include <map>

// Graph Leaf
// Info: material, geometry
class LeafNode : public Node
{
public:
	LeafNode();
	~LeafNode();

	void			setDrawable(Drawable* drawable);
	const Drawable* getDrawable();

	void			setMaterialID(unsigned int material);
	unsigned int	getMaterialID();

	void			setRawData(RawMeshData* data);
	RawMeshData*	getRawData();

	//Adds instance to object
	//Will contain updated transforms
	unsigned int	addInstance();
	void			updateInstanceTransform(unsigned int index); //PRIDAT transform

protected:
	//state set? mozno dany materialom

	//Context-addressed drawables
	std::map<void*, Drawable *>		_drawable; //Isto?

	RawMeshData*					_rawData; //ak je instanciou, tak je to NULL
	unsigned int					_materialID;
	
	//Instance data
	unsigned int					_numInstances;
	//std::vector<>					_transforms;				
	
};