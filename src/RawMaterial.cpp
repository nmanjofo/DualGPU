#include "RawMaterial.hpp"

RawMaterial::RawMaterial()
{
	_ambientColor = XMFLOAT3A(0, 0, 0);
	_diffuseColor = XMFLOAT3A(0, 0, 0);
	_specularColor = XMFLOAT3A(0, 0, 0);
	_shininess = 0.0f;
	_isWireframe = false;
	_isTwoSided = false;

	_ambientTexture = nullptr;
	_specularTexture = nullptr;
	_normalTexture = nullptr;
	_displacementTexture = nullptr;
	_emissiveTexture = nullptr;
}


//TODO - is this behaviour correct?
//Mozno pridat pre kazdy typ stack
void RawMaterial::addTexture(RawTexture2D::TextureType type, RawTexture2D::RawTextureData* data)
{
	switch (type)
	{
	case RawTexture2D::TextureType::Ambient:
		if (_ambientTexture != nullptr)
		{
			_ambientTexture = data;
		}
		else
		{
			std::wcerr << "WARNING: Texture of this type already exists!\n";
			delete data;
		}
		break;
	case RawTexture2D::TextureType::Diffuse:
		_diffuseTextures.push_back(data);
		break;
	case RawTexture2D::TextureType::Specular:
		if (_specularTexture != nullptr)
		{
			_specularTexture = data;
		}
		else
		{
			std::wcerr << "WARNING: Texture of this type already exists!\n";
			delete data;
		}
		break;
	case RawTexture2D::TextureType::Displacement:
		if (_displacementTexture != nullptr)
		{
			_displacementTexture = data;
		}
		else
		{
			std::wcerr << "WARNING: Texture of this type already exists!\n";
			delete data;
		}
		break;
	case RawTexture2D::TextureType::Emissive:
		if (_emissiveTexture != nullptr)
		{
			_emissiveTexture = data;
		}
		else
		{
			std::wcerr << "WARNING: Texture of this type already exists!\n";
			delete data;
		}
		break;
	case RawTexture2D::TextureType::Normal:
		if (_normalTexture != nullptr)
		{
			_normalTexture = data;
		}
		else
		{
			std::wcerr << "WARNING: Texture of this type already exists!\n";
			delete data;
		}
		break;
	default:
		break;
	}
}

const RawTexture2D::RawTextureData* RawMaterial::getTextureByName(std::wstring& name) const
{
	if (name.compare(_ambientTexture->name) == 0)
	{
		return _ambientTexture;
	}

	if (name.compare(_specularTexture->name) == 0)
	{
		return _specularTexture;
	}

	if (name.compare(_normalTexture->name) == 0)
	{
		return _normalTexture;
	}

	if (name.compare(_displacementTexture->name) == 0)
	{
		return _displacementTexture;
	}

	if (name.compare(_emissiveTexture->name) == 0)
	{
		return _emissiveTexture;
	}

	auto it = _diffuseTextures.begin();
	const auto end = _diffuseTextures.end();

	for (it; it != end; ++it)
	{
		if (name.compare((*it)->name) == 0)
			return (*it);
	}

	return nullptr;
}

const RawTexture2D::RawTextureData* RawMaterial::getAmbientTexture() const
{
	return _ambientTexture;
}

const RawTexture2D::RawTextureData* RawMaterial::getSpecularTexture() const
{
	return _specularTexture;
}

const RawTexture2D::RawTextureData* RawMaterial::getNormalTexture() const
{
	return _normalTexture;
}

const RawTexture2D::RawTextureData* RawMaterial::getDisplacementTexture() const
{
	return _displacementTexture;
}

const RawTexture2D::RawTextureData* RawMaterial::getEmissiveTexture() const
{
	return _emissiveTexture;
}

const std::vector<RawTexture2D::RawTextureData*>& RawMaterial::getDiffuseTextures() const
{
	return _diffuseTextures;
}

unsigned int RawMaterial::getNumOfDiffuseTextures() const
{
	return static_cast<unsigned int>(_diffuseTextures.size());
}


void RawMaterial::setAmbientColor(XMFLOAT3A& ambient)
{
	_ambientColor = ambient;
}

void  RawMaterial::setDiffuseColor(XMFLOAT3A& diffuse)
{
	_diffuseColor = diffuse;
}

void  RawMaterial::setSpecularColor(XMFLOAT3A& specular)
{
	_specularColor = specular;
}

void  RawMaterial::setColors(XMFLOAT3A& ambient, XMFLOAT3A& diffuse, XMFLOAT3A& specular)
{
	_ambientColor = ambient;
	_diffuseColor = diffuse;
	_specularColor = specular;
}

void  RawMaterial::setShininess(float shininess)
{
	_shininess = shininess;
}

void  RawMaterial::setName(std::wstring& name)
{
	_name = name;
}

void  RawMaterial::setIsWireframe(bool isWireframe)
{
	_isWireframe = isWireframe;
}

void  RawMaterial::setIsTwoSided(bool isTwoSided)
{
	_isTwoSided = isTwoSided;
}
