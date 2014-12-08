#include "Loader.hpp"

Loader::Loader()
{

}


SceneGraph* Loader::loadSceneGraphFromFile(std::wstring pathw, MaterialManager* manager)
{
	Assimp::Importer* importer = new Assimp::Importer;

	//TOTO do nejakeho globalu, resp. top klasy
	//convert wstring path to string
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string path = converter.to_bytes(pathw);

	//Load scene
	const aiScene* scene = importer->ReadFile(path, 
												aiProcess_MakeLeftHanded |
												aiProcess_RemoveRedundantMaterials |
												aiProcess_JoinIdenticalVertices |
												aiProcess_GenSmoothNormals |
												aiProcess_ImproveCacheLocality |
												aiProcess_SortByPType |
												aiProcess_FindInstances
												//aiProcess_OptimizeMeshes 
	);

	if (scene == NULL)
	{
		std::wcerr << "Error importing file \"" << pathw << "\"!\n";
		std::wcerr << importer->GetErrorString();
		return NULL;
	}

	//Create and init reference counter
	std::vector< std::pair<unsigned int, LeafNode*> > _meshReferenceCounter;
	_meshReferenceCounter.resize(scene->mNumMeshes, std::make_pair<unsigned int, LeafNode*>(0, nullptr));

	Node* root = nullptr;
	SceneGraph* graph = new SceneGraph();

	//Process meshes, loads data
	if(!_processMeshes(scene->mMeshes, scene->mNumMeshes))
	{
		//!!Cleanup
		return nullptr;
	}

	//Create hierarchy, count references
	root = _processHierarchyRecursive(scene, _meshReferenceCounter, scene->mRootNode, nullptr);


	//clean Assimp
	importer->FreeScene();

	return graph;
}

Node* Loader::_processHierarchyRecursive(const aiScene* scene, std::vector< std::pair<unsigned int, LeafNode*> > &meshReferenceCounter, const aiNode* node, Node* parent)
{
	if (node ==NULL || (node->mNumChildren == 0 && node->mNumMeshes == 0) )
		return NULL;

	GraphNode* gn = new GraphNode;
	gn->setParent(parent);

	//Name
	gn->setName(node->mName.C_Str());

	//Recursion for each child
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		Node* n = _processHierarchyRecursive(scene, meshReferenceCounter, node->mChildren[i], gn);

		gn->addChild(n);
	}

	//Leaves
	//Reference counting - add reference to mesh count array, for instancing
	//Add raw data to it
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		LeafNode* leaf = meshReferenceCounter[node->mMeshes[i]].second;
		unsigned int refcount = meshReferenceCounter[node->mMeshes[i]].first++;

		//Is it an instance
		if (refcount > 1)
		{
			leaf->setRawData(nullptr);
			//DOPLNIT instancovacie veci
		}
		else
		{
			leaf->setParent(gn);
		}

		gn->addChild(leaf);
	}

	return gn;
}

bool  Loader::_processMeshes(const aiMesh** meshes, const unsigned int count)
{
	if (meshes == NULL || count == 0)
		return false;

	for (unsigned int i = 0; i < count; ++i)
	{
		const aiMesh* mesh = meshes[i];
		
		LeafNode* leaf = new LeafNode();
		RawMesh * data = new RawMesh();

		leaf->setRawData(data);
		leaf->setName(meshes[i]->mName.C_Str());
		
		//MATERIAL INDEX! nejak to sem treba dolepit

		//Get positions
		RawMesh::RawBuffer* positions = new RawMesh::RawBuffer;
		positions->elemType = ElementDataType::FLOAT;
		positions->size = meshes[i]->mNumVertices;
		positions->elemCountPerVector = FLOATS_PER_POSITION; //4 floats per vector
		positions->size_b = FLOATS_PER_POSITION * sizeof(float) * meshes[i]->mNumVertices;

		positions->data = _loadVectors(mesh->mVertices, 3, mesh->mNumVertices, FLOATS_PER_POSITION, 1.0f);

		if (positions->data == NULL)
		{
			//DO SOME ERROR CHECK
		}

		data->addBufferToMesh(positions, RawMesh::AttributeType::POSITIONS);

		//Get indices
		if (mesh->HasFaces())
		{
			RawMesh::RawBuffer* indices = new RawMesh::RawBuffer;
			indices->elemType = ElementDataType::UINT;
			indices->elemCountPerVector = 1;
			
			//ZLUCIT VSETKY FACES???
			//Merge all faces to one buffer
			//Get count of all faces
			unsigned int numFaces = 0;
			for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
			{
				numFaces += mesh->mFaces[j].mNumIndices;
			}

			unsigned int* ind = new unsigned int(numFaces);
			indices->data = ind;
			indices->size = numFaces;

			//Copy indices
			for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
			{
				memcpy(ind, mesh->mFaces[j].mIndices, sizeof(unsigned int)*(mesh->mFaces[j].mNumIndices));
				++ind;
			}
			
			data->addBufferToMesh(indices, RawMesh::AttributeType::INDICES);
		}

		//Get texcoord
		if (mesh->HasTextureCoords())
		{
			RawMesh::RawBuffer* texcoords = new RawMesh::RawBuffer;
			texcoords->elemType = ElementDataType::FLOAT;
			texcoords->size = meshes[i]->mNumVertices;
			texcoords->elemCountPerVector = 2; //2 texcoords per vector
			texcoords->size_b = 2 * sizeof(float) * meshes[i]->mNumVertices;

			//TODO - podpora pre niekolko sad texturovacich koordinatov
			texcoords->data = _loadVectors(mesh->mTextureCoords[0], 3, mesh->mNumVertices, 2, 0.0f);

			if (texcoords->data == NULL)
			{
				//DO SOME ERROR CHECK
			}

			data->addBufferToMesh(texcoords, RawMesh::AttributeType::TEXCOORDS);
		}

		//Get normals
		if (mesh->HasNormals)
		{
			RawMesh::RawBuffer* normals = new RawMesh::RawBuffer;
			normals->elemType = ElementDataType::FLOAT;
			normals->size = meshes[i]->mNumVertices;
			normals->elemCountPerVector = 3;
			normals->size_b = 3 * sizeof(float) * meshes[i]->mNumVertices;

			normals->data = _loadVectors(mesh->mNormals, 3, mesh->mNumVertices, 3, 0.0f);

			if (normals->data == NULL)
			{
				//DO SOME ERROR CHECK
			}

			data->addBufferToMesh(normals, RawMesh::AttributeType::NORMALS);
		}

		//Get tangents and bitangents
		if (mesh->HasTangentsAndBitangents())
		{
			//Tangents
			RawMesh::RawBuffer* tangents = new RawMesh::RawBuffer;
			tangents->elemType = ElementDataType::FLOAT;
			tangents->size = meshes[i]->mNumVertices;
			tangents->elemCountPerVector = 3;
			tangents->size_b = 3 * sizeof(float) * meshes[i]->mNumVertices;

			tangents->data = _loadVectors(mesh->mVertices, 3, mesh->mNumVertices, 3, 0.0f);

			if (tangents->data == NULL)
			{
				//DO SOME ERROR CHECK
			}

			data->addBufferToMesh(tangents, RawMesh::AttributeType::TANGENTS);

			//Bitangents
			RawMesh::RawBuffer* bitangents = new RawMesh::RawBuffer;
			bitangents->elemType = ElementDataType::FLOAT;
			bitangents->size = meshes[i]->mNumVertices;
			bitangents->elemCountPerVector = 3;
			bitangents->size_b = 3 * sizeof(float) * meshes[i]->mNumVertices;

			bitangents->data = _loadVectors(mesh->mVertices, 3, mesh->mNumVertices, 3, 0.0f);

			if (bitangents->data == NULL)
			{
				//DO SOME ERROR CHECK
			}

			data->addBufferToMesh(bitangents, RawMesh::AttributeType::BITANGENTS);
		}

		//Get colors
		if (mesh->HasVertexColors(0))
		{
			RawMesh::RawBuffer* colors = new RawMesh::RawBuffer;
			colors->elemType = ElementDataType::FLOAT;
			colors->size = meshes[i]->mNumVertices;
			colors->elemCountPerVector = 4;
			colors->size_b = 4 * sizeof(float) * meshes[i]->mNumVertices;

			colors->data = _loadVectors(mesh->mColors[0], 4, mesh->mNumVertices, 4, 0.0f);

			if (colors->data == NULL)
			{
				//DO SOME ERROR CHECK
			}

			data->addBufferToMesh(colors, RawMesh::AttributeType::COLOR);
		}
	}//loop through meshes

	return true;
}
