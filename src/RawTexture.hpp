#pragma once

#include "RawTypes.hpp"

namespace RawTexture
{
	enum class TextureType
	{
		Color,
		Displacement,
		Normal,
		Bump,
	};

	typedef struct
	{
		void*			data;
		unsigned int	dimX;
		unsigned int	dimY;
		unsigned int	numVectorComponents;
		ElementDataType componentType;
		float			blend;
	} RawTextureData;
}
