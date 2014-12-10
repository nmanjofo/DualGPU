#pragma once

#include "RawTypes.hpp"

namespace RawTexture2D
{
	enum class TextureType
	{
		Ambient,
		Diffuse,
		Specular,
		Normal,
		Emissive,
		Lightmap,
		Displacement,
	};

	typedef struct
	{
		void*			data;
		unsigned int	width;
		unsigned int	height;
		unsigned int	vectorComponentNum;
		ElementDataType vectorComponentType;
		std::wstring	name;
	} RawTextureData;
}
