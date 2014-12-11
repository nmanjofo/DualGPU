#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <ostream>

//Provides interface for error handling
//and printing error messages
class Error
{
public:

	enum class Severity : unsigned int
	{
		INFO = 0,
		WARNING,
		ERROR
	};

	//Sets file for log output
	static void setLogFile(std::wstring path);
	static void closeLogFile();

	//Sets stream for particular message type
	static void setOutputStream(std::istream stream, Severity severity);

	//Prints error message
	void __ErrorMessage(const std::wstring msg, const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity = Error::Severity::ERROR);
	void __ErrorMessage(const wchar_t* msg,     const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity = Error::Severity::ERROR);
	void __ErrorMessage(const char* msg,		const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity = Error::Severity::ERROR);

//Expands __VA_ARGS__
//Because MSVC treats it as a single argument, not following C99
#define EXPAND(x) x

#define GET_MACRO(_1,_2, NAME, ...) NAME
#define ERROR_MESSAGE_1(M)     __ErrorMessage(M, __FUNCTION__, __LINE__ , __FILE__)
#define ERROR_MESSAGE_2(M , S) __ErrorMessage(M, __FUNCTION__, __LINE__ , __FILE__ , S)

#define ErrorMessage(...) GET_MACRO(EXPAND(__VA_ARGS__), ERROR_MESSAGE_2 , ERROR_MESSAGE_1)(EXPAND(__VA_ARGS__))

protected:
	static std::wstring _logfile;

	static std::ostream& _infoStream;
	static std::ostream& _warningStream;
	static std::ostream& _errorStream;
};