#pragma once

//TODO prerobit na presne velkosti

enum class ElementDataType
{
	FLOAT,
	UBYTE,
	USHORT,
	UINT,
	ULONG,
	BYTE,
	SHORT,
	INT,
	LONG
};

unsigned int getElementDataTypeByteSize(ElementDataType type);