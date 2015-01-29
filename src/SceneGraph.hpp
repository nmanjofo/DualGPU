#pragma once

#include "AlignedAllocator.hpp"
#include "LeafNode.hpp"
#include "GraphNode.hpp"


#include <vector>

//TODO - musi ist prec
#include <DirectXMath.h>

using namespace DirectX;

class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph();

	void setRoot(GraphNode* root);
	const GraphNode* getRoot();

	bool hasAnimation();

	void update(double time);

protected:
	Node*					_rootNode;

	bool					_hasAnimations;

	std::vector<XMMATRIX, AlignedAllocator<XMMATRIX, Alignment::SSE>>	_localTransforms;
	std::vector<XMVECTOR, AlignedAllocator<XMVECTOR, Alignment::SSE>>	_boundingSpheres;
};