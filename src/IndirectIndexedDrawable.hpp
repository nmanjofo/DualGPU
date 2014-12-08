#pragma once

#include "Drawable.hpp"

#include <d3d11.h>

// V buducnu urobit klasy pre buffre?
class IndirectIndexedDrawable : public Drawable
{
public:
	IndirectIndexedDrawable();

	virtual void		draw(ID3D11DeviceContext* context);
	bool				create(ID3D11DeviceContext* context);

	bool				isValid();

private:
	ID3D11Buffer*		_VBO;
	ID3D11Buffer*		_EBO;
	ID3D11Buffer*		_IBO;

	DXGI_FORMAT			_EBOFormat;
	unsigned int		_EBOOffset;

	unsigned int		_VBOStride;
	unsigned int		_VBOOffset;
};