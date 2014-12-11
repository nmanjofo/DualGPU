#include "Error.hpp"

std::wstring Error::_logfile = std::wstring();

std::ostream& Error::_infoStream = std::cerr;
std::ostream& Error::_warningStream = std::cerr;
std::ostream& Error::_errorStream = std::cerr;


void Error::__ErrorMessage(const std::wstring msg, const char* functionName, unsigned int line, const wchar_t* file, Error::Severity severity)
{

}