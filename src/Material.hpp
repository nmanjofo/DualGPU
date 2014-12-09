#pragma once

#ifdef _AVX_
#define ALIGNMENT 32
#else
#define ALIGNMENT 16
#endif

#include "RawTexture.hpp"

#include <DirectXMath.h>
#include <string>

//DO BUDUCNA - vlastna matematicka kniznica, platform independant
//TODO nestacila by struktura?
//TODO pridat transparentnost
using namespace DirectX;

__declspec(align(ALIGNMENT)) class Material
{
public:
	void setAmbientColor(XMFLOAT3A& ambient);
	void setDiffuseColor(XMFLOAT3A& ambient);
	void setSpecularColor(XMFLOAT3A& ambient);

	void setColors(XMFLOAT3A& ambient, XMFLOAT3A& diffuse, XMFLOAT3A& specular);
	void setShininess(float shininess);

	void setName(std::wstring& name);

	void setIsWireframe(bool isWireframe);

	void setIsTwoSided(bool isTwoSided);

	//GETy na farby

	void addTexture(RawTexture2D::TextureType type, RawTexture2D::RawTextureData* data);

	//Returns the number of color textures
	unsigned int numColorTextures();

	const RawTexture2D::RawTextureData* getTextureByName(std::wstring& name);

	//TODO - fcie na ostatne typy textur?

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
	std::vector< std::pair<RawTexture2D::TextureType, RawTexture2D::RawTextureData*> > _textures;
};