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

unsigned int getElementDataTypeByteSize(ElementDataType type)
{
	switch (type)
	{
	case ElementDataType::FLOAT:
		return sizeof(float);
	case ElementDataType::UBYTE:
	case ElementDataType::BYTE:
		return sizeof(unsigned char);
	case ElementDataType::UINT:
	case ElementDataType::INT:
		return sizeof(unsigned int);
	case ElementDataType::ULONG:
	case ElementDataType::LONG:
		return sizeof(unsigned long);
	case ElementDataType::USHORT:
	case ElementDataType::SHORT:
		return sizeof(unsigned short);
	default:
		return 0;
	}
}
