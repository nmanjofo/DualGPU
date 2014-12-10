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

	//Process materials and textures
	if (manager != nullptr)
	{
		if (_processMaterialsAndRawTextures(scene->mMaterials, scene->mNumMaterials, true, manager) == false)
		{
			//!!Cleanup
			return nullptr;
		}
	}

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
		leaf->setName(mesh->mName.C_Str());
		leaf->setMaterialID(mesh->mMaterialIndex);

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
		if (mesh->HasTextureCoords(0))
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

bool Loader::_processMaterialsAndRawTextures(const aiMaterial** materials, unsigned int numMaterials, bool loadTextures, MaterialManager* manager)
{
	if (materials == nullptr || manager == nullptr)
	{
		return false;
	}

	if (numMaterials == 0)
		return true;

	for (unsigned int i = 0; i < numMaterials; ++i)
	{
		RawMaterial * material = new RawMaterial;

		//Name
		aiString name;
		materials[i]->Get(AI_MATKEY_NAME, name);

		typedef std::codecvt_utf8<wchar_t> convert_type;
		std::wstring_convert<convert_type, wchar_t> converter;

		material->setName(converter.from_bytes(name.C_Str()));

		//Ambient color
		aiColor3D color;
		XMFLOAT3A colorA;
		materials[i]->Get(AI_MATKEY_COLOR_AMBIENT, color);

		colorA.x = color[0];
		colorA.y = color[1];
		colorA.z = color[2];

		material->setAmbientColor(colorA);

		//Diffuse color
		materials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		colorA.x = color[0];
		colorA.y = color[1];
		colorA.z = color[2];

		material->setDiffuseColor(colorA);

		//Specular color
		materials[i]->Get(AI_MATKEY_COLOR_SPECULAR, color);

		colorA.x = color[0];
		colorA.y = color[1];
		colorA.z = color[2];

		material->setSpecularColor(colorA);

		//Shininess
		float shininess;

		materials[i]->Get(AI_MATKEY_SHININESS, shininess);

		material->setShininess(shininess);

		//IsTwoSided, IsWireframe
		int is;

		materials[i]->Get(AI_MATKEY_ENABLE_WIREFRAME, is);
		material->setIsWireframe(is != 0);

		materials[i]->Get(AI_MATKEY_TWOSIDED, is);
		material->setIsTwoSided(is != 0);

		//Textures (raw)
		if (loadTextures)
		{
			if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
			{
				//Wrong DevIL version
				std::wcerr << "Wrong DevIL version!\n";
				return false;
			}

			ilInit();

			const std::pair<aiTextureType, RawTexture2D::TextureType> textureTypes[] = 
			{	
				{ aiTextureType_AMBIENT, RawTexture2D::TextureType::Ambient },
				{ aiTextureType_DIFFUSE, RawTexture2D::TextureType::Diffuse },
				{ aiTextureType_SPECULAR, RawTexture2D::TextureType::Specular },
				{ aiTextureType_EMISSIVE, RawTexture2D::TextureType::Emissive },
				{ aiTextureType_NORMALS, RawTexture2D::TextureType::Normal }
				{ aiTextureType_DISPLACEMENT, RawTexture2D::TextureType::Displacement }
			};

			const unsigned int textureTypesCount = sizeof(textureTypes) / sizeof(aiTextureType);
			
			//Iterate through texture types
			for (unsigned int n = 0; n < textureTypesCount; ++n)
			{
				unsigned int numTextures = materials[i]->GetTextureCount(textureTypes[n].first);

				for (unsigned int j = 0; j < numTextures; ++j)
				{
					RawTexture2D::RawTextureData* texture = new RawTexture2D::RawTextureData;

					//Open file
					ILuint tex = ilGenImage();
					ilBindImage(tex);

					aiString path;
					materials[i]->GetTexture(textureTypes[n].first, j, &path);

					//vyhodit do globalu
					typedef std::codecvt_utf8<wchar_t> convert_type;
					std::wstring_convert<convert_type, wchar_t> converter;

					bool success = ilLoadImage(converter.from_bytes(name.C_Str()).c_str());

					if (!success)
					{
						_printILErrorString(ilGetError());
						return false;
					}

					//Get name (=path)
					texture->name = converter.from_bytes(name.C_Str());

					//Get dimensions
					texture->width = ilGetInteger(IL_IMAGE_WIDTH);
					texture->height = ilGetInteger(IL_IMAGE_HEIGHT);

					//Get and process texture format
					// get number of channels
					texture->vectorComponentNum = ilGetInteger(IL_IMAGE_CHANNELS);

					// get component type
					int typeSize = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) / ilGetInteger(IL_IMAGE_CHANNELS);

					ILenum type;

					switch (typeSize)
					{
					case sizeof(char) :
						texture->vectorComponentType = ElementDataType::UBYTE;
						type = IL_UNSIGNED_BYTE;
						break;
					case sizeof(short):
						texture->vectorComponentType = ElementDataType::USHORT;
						type = IL_UNSIGNED_SHORT;
						break;
					case sizeof(int) :
						texture->vectorComponentType = ElementDataType::UINT;
						type = IL_UNSIGNED_INT;
						break;
					default:
						//Convert to ubyte
						ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
						texture->vectorComponentType = ElementDataType::UBYTE;
						texture->vectorComponentNum = 3;
						type = IL_UNSIGNED_BYTE;
						break;
					}

					// convert reversed formats
					int format = ilGetInteger(IL_IMAGE_FORMAT);

					if (format == IL_BGR)
					{
						ilConvertImage(IL_RGB, type);
					}
					else if (format == IL_BGRA)
					{
						ilConvertImage(IL_RGBA, type);
					}

					//Get data
					//TODO - EXTRA TESTING - treba spravne skopirovat!
					texture->data = new char[typeSize * ilGetInteger(IL_IMAGE_CHANNELS) * texture->width * texture->height];
					ilCopyPixels(0, 0, 0, texture->width, texture->height, 1, ilGetInteger(IL_IMAGE_FORMAT), type, texture->data);

					material->addTexture(textureTypes[n].second, texture);

					ilDeleteImage(tex);

					//TU skontrolovat, ci tam tie data naozaj su
				}
			}
		}

		//Add material to manager
		manager->addMaterial(material);
	}

	return true;
}

void Loader::_printILErrorString(ILenum error)
{
	switch (error)
	{
	case IL_NO_ERROR: 
		return;
	case IL_INVALID_ENUM:
		std::cerr << "IL: Invalid Enum!\n";
		return;
	case IL_OUT_OF_MEMORY:
		std::cerr << "IL: Out of memory!\n";
		return;
	case IL_FORMAT_NOT_SUPPORTED:
		std::cerr << "IL: Format not supported!\n";
		return;
	case IL_INTERNAL_ERROR:
		std::cerr << "IL: Internal error!\n";
		return;
	case IL_INVALID_VALUE:
		std::cerr << "IL: Invalid value!\n";
		return;
	case IL_ILLEGAL_OPERATION:
		std::cerr << "IL: Illegal operation!\n";
		return;
	case IL_ILLEGAL_FILE_VALUE:
		std::cerr << "IL: Illegal value found in file! Possibly corrupted file?\n";
		return;
	case IL_INVALID_FILE_HEADER:
		std::cerr << "IL: File header corrupted\n";
		return;
	case IL_INVALID_PARAM:
		std::cerr << "IL: Invalid parameter!\n";
		return;
	case IL_COULD_NOT_OPEN_FILE:
		std::cerr << "IL: Could not open specified file!\n";
		return;
	case IL_INVALID_EXTENSION:
		std::cerr << "IL: Invalid file extension!\n";
		return;
	case IL_FILE_ALREADY_EXISTS:
		std::cerr << "IL: File already exists!\n";
		return;
	case IL_OUT_FORMAT_SAME:
		std::cerr << "IL: Conversion to the same format!\n";
		return;
	case IL_STACK_OVERFLOW:
		std::cerr << "IL: Stack overflow!\n";
		return;
	case IL_STACK_UNDERFLOW:
		std::cerr << "IL: Stack underflow!\n";
		return;
	case IL_INVALID_CONVERSION:
		std::cerr << "IL: Invalid conversion!\n";
		return;
	case IL_LIB_JPEG_ERROR:
		std::cerr << "IL: Error in libjpeg library!\n";
		return;
	case IL_LIB_PNG_ERROR:
		std::cerr << "IL: Error in libpng library!\n";
		return;
	case IL_UNKNOWN_ERROR:
	default:
		std::cerr << "IL: Unknown error!\n";
		return;
	}
}