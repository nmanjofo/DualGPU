#pragma once

#include <string>
#include "BoundingVolume.hpp"

#include <iostream>
#include <codecvt>

class Node
{
public:
	void setParent(Node* parent);
	Node* getParent();

	void setBoundingVolume(BoundingVolume* bv);
	BoundingVolume* getBoundingVolume();

	//Virtual?
	bool isLeaf();
	bool isLight();
	bool isCullable();
	bool isCamera();

	void setName(std::wstring wstr);
	void setName(std::string str);
	void setName(const char* str);

	std::wstring getName();

protected:
	Node();

	std::wstring			_name;
	BoundingVolume*			_boundingVolume;
	Node*					_parent;

	bool					_hasChildren;
	bool					_hasTransform;
	bool					_hasAnimation;
	bool					_isCullable;
	bool					_isLeaf;
	bool					_isLight;
	bool					_isCamera;
};