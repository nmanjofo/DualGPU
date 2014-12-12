#pragma once

#include "Node.hpp"
#include "LeafNode.hpp"
#include "Animation.hpp"

#include <vector>

// Graph Node
// Contains transform
// Is animable
class GraphNode : public Node
{
public:
	GraphNode();
	~GraphNode();

	void addChild(Node* node); //musi tej node pridat seba ako parenta
	bool deleteChild(std::string name);

	bool hasAnimatedChildren();

private:
	std::vector<GraphNode*>	_children;
	std::vector<LeafNode*>	_leaves;

protected:
	Animation*				_animation;
};