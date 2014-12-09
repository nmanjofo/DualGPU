#pragma once

#include "RawTypes.hpp"

namespace RawTexture2D
{
	enum class TextureType
	{
		Ambient,
		Diffuse,
		Specular,
		Displacement,
		Normal,
		Bump,
		Lightmap,
	};

	typedef struct
	{
		void*			data;
		unsigned int	dimX;
		unsigned int	dimY;
		unsigned int	numVectorComponents;
		ElementDataType componentType;
		float			blend;
		std::string		name;
	} RawTextureData;
}
