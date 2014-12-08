#pragma once

#ifdef _AVX_
#define ALIGNMENT 32
#else
#define ALIGNMENT 16
#endif

#include "RawTexture.hpp"

#include <DirectXMath.h>
#include <string>

using namespace DirectX;

__declspec(align(ALIGNMENT)) class Material
{
public:
	void setColors(XMFLOAT3A& ambient, XMFLOAT3A& diffuse, XMFLOAT3A& specular);
	void setShininess(float shininess);

	//GETy na farby

	void addTexture(RawTexture::TextureType type, RawTexture::RawTextureData* data);

	//Returns the number of color textures
	unsigned int numColorTextures();

	const RawTexture::RawTextureData* getTextureByName(std::wstring& name);

	//TODO - fcie na ostatne typy textur?

protected:
	XMFLOAT3A	_ambientColor;
	XMFLOAT3A	_diffuseColor;
	XMFLOAT3A	_specularColor;
	float		_shininess;

	//Textures
	std::vector< std::pair<RawTexture::TextureType, RawTexture::RawTextureData*> > _textures;
};