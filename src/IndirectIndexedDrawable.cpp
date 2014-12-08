#include "IndirectIndexedDrawable.hpp"

IndirectIndexedDrawable::IndirectIndexedDrawable() : Drawable()
{
	_drawableType = DrawableType::DRAWABLE_TYPE_GEOMETRY_INDIRECT_INDEXED;
}

bool IndirectIndexedDrawable::create(ID3D11DeviceContext* context/*data,...*/)
{
	//nafasujem data
	//vytvorim buffre
}

void IndirectIndexedDrawable::draw(ID3D11DeviceContext* context)
{
	if (!_isValid)
		return;

	//TODO - nejaky debug output

	context->IASetVertexBuffers(VBO_SLOT, 1, &_VBO, &_VBOStride, &_VBOOffset);
	context->IASetIndexBuffer(_EBO, _EBOFormat, _EBOOffset);
	context->DrawIndexedInstancedIndirect(_IBO, 0);
}