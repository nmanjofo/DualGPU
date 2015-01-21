#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream> 
#include <codecvt>

//Converts char* or string to wstring
std::wstring stringToWstring(const std::string& str);
std::wstring stringToWstring(const char* str);

//Converts wchar_t* or wstring to string
std::string wstringToString(const std::wstring& wstr);
std::string wstringToString(const wchar_t* str);

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

#define SEVERITY_ENUM_SIZE 3

	//Sets file for log output
	static bool setLogFile(std::wstring path = L"log.txt");
	static void closeLogFile();

	//Sets stream for particular message type
	static void setOutputStream(std::istream* stream, Severity severity);

	//Prints error message
	void __ErrorMessage(const std::wstring msg, const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity = Error::Severity::ERROR);
	void __ErrorMessage(const wchar_t* msg,     const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity = Error::Severity::ERROR);
	void __ErrorMessage(const char* msg,		const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity = Error::Severity::ERROR);

	static void enableWriteToConsole(bool mode, Severity severity);

//Expands __VA_ARGS__
//Because MSVC treats it as a single argument, not following C99
#define EXPAND(x) x

#define GET_MACRO(_1,_2, NAME, ...) NAME
#define ERROR_MESSAGE_1(M)     __ErrorMessage(M, __FUNCTION__, __LINE__ , __FILEW__)
#define ERROR_MESSAGE_2(M , S) __ErrorMessage(M, __FUNCTION__, __LINE__ , __FILEW__ , S)

#define ErrorMessage(...) GET_MACRO(EXPAND(__VA_ARGS__), ERROR_MESSAGE_2 , ERROR_MESSAGE_1)(EXPAND(__VA_ARGS__))

protected:
	static bool				_isWriteToConsoleEnabled[SEVERITY_ENUM_SIZE];
	static bool				_isLogFileEnabled[SEVERITY_ENUM_SIZE];

	static std::wofstream	_logFile;
	static std::wostream*	_streams[SEVERITY_ENUM_SIZE];
};