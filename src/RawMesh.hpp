#pragma once

#include "RawTypes.hpp"

#include <map>

class RawMesh
{
public:
	RawMesh();

	enum class AttributeType
	{
		POSITIONS,
		INDICES,
		NORMALS,
		TEXCOORDS,
		TANGENTS,
		BITANGENTS,
		COLOR
	};

	typedef struct
	{
		unsigned long		size_b;					//size of the buffer in bytes
		void*				data;					//data pointer
		unsigned int		elemCountPerVector;		//coponents per ty, e.g. vec4 = 4
		unsigned int		size;					//number of elements in the buffer (num of vec4 f.e.)
		ElementDataType		elemType;				//data type inside
	} RawBuffer;

	//Checks if there is a buffer of that attrib type already present
	//Returns false if buffer of specieifed type is already present and no
	//data is added
	//Adds buffer and return true otherwise
	bool				addBufferToMesh(RawBuffer* buffer, AttributeType type);

	//Returns buffer descriptor based on requested attribute
	//Returns nullptr if not present
	const RawBuffer*	getBufferByType(AttributeType type);

protected:
	std::map<AttributeType, RawBuffer*> _buffers;
	bool								_dirty;
};