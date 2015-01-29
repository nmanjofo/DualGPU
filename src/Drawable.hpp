#pragma once

#include <d3d11.h>

//Slots allocation
//Tuna? Nie niekde vyssie?
#define VBO_SLOT 0

class Drawable
{
public:
	Drawable();

	enum class DrawableType
	{
		DRAWABLE_TYPE_GEOMETRY_INDIRECT_INDEXED,
	};

	DrawableType getType()
	{
		return _drawableType;
	}

	bool isValid()
	{
		return _isValid;
	}

	//virtual void draw(ID3D11DeviceContext* context);


protected:

	DrawableType		_drawableType;
	bool				_isValid;
};