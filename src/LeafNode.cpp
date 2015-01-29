#include "LeafNode.hpp"

LeafNode::LeafNode() : Node()
{
	_isLeaf = true;
}

LeafNode::~LeafNode()
{

}

void LeafNode::setRawData(RawMesh* data)
{

}

RawMesh* LeafNode::getRawData()
{
	return nullptr;
}

void LeafNode::setMaterialID(unsigned int material)
{

}