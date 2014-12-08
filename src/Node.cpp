#include "Node.hpp"

void Node::setName(std::wstring wstr)
{
	_name = wstr;
}

void Node::setName(std::string str)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	_name = converter.from_bytes(str);
}

void Node::setName(const char* name)
{
	this->setName(std::string(name));
}