#pragma once

#ifdef _AVX_
#define ALIGNMENT 32
#else
#define ALIGNMENT 16
#endif

#include "RawTexture.hpp"

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <iostream>

//DO BUDUCNA - vlastna matematicka kniznica, platform independant
//TODO pridat transparentnost
using namespace DirectX;

__declspec(align(ALIGNMENT)) class RawMaterial
{
public:
	RawMaterial();

	void setAmbientColor(XMFLOAT3A& ambient);
	void setDiffuseColor(XMFLOAT3A& diffuse);
	void setSpecularColor(XMFLOAT3A& specular);

	void setColors(XMFLOAT3A& ambient, XMFLOAT3A& diffuse, XMFLOAT3A& specular);
	void setShininess(float shininess);

	void setName(std::wstring& name);
	void setIsWireframe(bool isWireframe);
	void setIsTwoSided(bool isTwoSided);

	void addTexture(RawTexture2D::TextureType type, RawTexture2D::RawTextureData* data);

	const std::wstring&	getName() const;
	const XMFLOAT3A&	getAmbientColor() const;
	const XMFLOAT3A&	getDiffuseColor() const;
	const XMFLOAT3A&	getSpecularColor() const;

	float getShininess();

	const RawTexture2D::RawTextureData*					getTextureByName(std::wstring& name) const;

	const RawTexture2D::RawTextureData*					getAmbientTexture() const;
	const RawTexture2D::RawTextureData*					getSpecularTexture() const;
	const RawTexture2D::RawTextureData*					getNormalTexture() const;
	const RawTexture2D::RawTextureData*					getDisplacementTexture() const;
	const RawTexture2D::RawTextureData*					getEmissiveTexture() const;
	const std::vector<RawTexture2D::RawTextureData*>&	getDiffuseTextures() const;
	unsigned int										getNumOfDiffuseTextures() const;

protected:
	XMFLOAT3A		_ambientColor;
	XMFLOAT3A		_diffuseColor;
	XMFLOAT3A		_specularColor;
	float			_shininess;
	bool			_isWireframe;
	bool			_isTwoSided;	//requires backface culling turned off
	std::wstring	_name;

	//Textures
	//NIE nahodou polia podla typu?

	RawTexture2D::RawTextureData*				_ambientTexture;
	RawTexture2D::RawTextureData*				_specularTexture;
	RawTexture2D::RawTextureData*				_normalTexture;
	RawTexture2D::RawTextureData*				_displacementTexture;
	RawTexture2D::RawTextureData*				_emissiveTexture;
	std::vector<RawTexture2D::RawTextureData*>	_diffuseTextures;

	//std::vector< std::pair<RawTexture2D::TextureType, RawTexture2D::RawTextureData*> > _textures;
};