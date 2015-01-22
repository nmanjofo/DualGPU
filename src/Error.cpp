#include "Error.hpp"

std::wofstream Error::_logFile;
bool Error::_isLogFileEnabled[SEVERITY_ENUM_SIZE] = { false, false, false };
bool Error::_isWriteToConsoleEnabled[SEVERITY_ENUM_SIZE] = { true, true, true };
std::wostream* Error::_streams[3] = { &std::wcerr, &std::wcerr, &std::wcerr };

void Error::__ErrorMessage(const std::wstring msg, const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity)
{
	std::wstringstream wstr;

	wstr << L"ERROR in file " << file << L" line " << line << L" function \"" << functionName << L"\":" << std::endl;
	wstr << msg;
	wstr << L"-----" << std::endl;
	
	//Write to console
	if (_isWriteToConsoleEnabled[static_cast<unsigned int>(severity)])
	{	
		(*_streams[static_cast<unsigned int>(severity)]) << wstr.str();
	}

	//Write to file
	if (_isLogFileEnabled[static_cast<unsigned int>(severity)])
	{
		_logFile << wstr.str();
	}
}

void Error::__ErrorMessage(const wchar_t* msg, const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity)
{
	const std::wstring wstr(msg);
	__ErrorMessage(wstr, functionName, line, file, severity);
}

void Error::__ErrorMessage(const char* msg, const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity)
{
	__ErrorMessage(stringToWstring(msg), functionName, line, file, severity);
}

bool Error::setLogFile(std::wstring path)
{
	_logFile.open(path);
	return _logFile.is_open();
}

void Error::closeLogFile()
{
	_logFile.close();
}

void Error::enableWriteToConsole(bool mode, Severity severity)
{
	_isWriteToConsoleEnabled[static_cast<unsigned int>(severity)] = mode;
}

//String conversion functions
std::wstring stringToWstring(const std::string& str)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.from_bytes(str);
}

std::wstring stringToWstring(const char* str)
{
	std::string s(str);
	return stringToWstring(s);
}

std::string wstringToString(const std::wstring& wstr)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.to_bytes(wstr);
}

std::string wstringToString(const wchar_t* str)
{
	std::wstring wstr(str);
	return wstringToString(wstr);
}

