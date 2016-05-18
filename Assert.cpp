#include "Assert.hpp"
#include <sstream>
#include <windows.h>

__declspec(noreturn) void FatalError(const std::string& errorText, const std::string& file, const std::string& func, int lineNum) {
#ifdef WIN32
	std::stringstream errorMessage;
	errorMessage << errorText << " in file " << file << "in function " << func << "line number " << lineNum;
	const std::string error = errorMessage.str();
	MessageBoxA(NULL, error.c_str(), "Fatal Error", MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
	exit(0);
#endif
}

void RecoverableError(const std::string& errorText, const std::string& file, const std::string& func, int lineNum) {
#ifdef WIN32
	std::stringstream errorMessage;
	errorMessage << errorText << " in" << file << "in function " << func << " line number " << lineNum << std::endl;
	errorMessage << "Continue Execution?" << std::endl;
	const std::string error = errorMessage.str();
	int userClick = MessageBoxA(NULL, error.c_str(), "Recoverable Error", MB_YESNO | MB_SYSTEMMODAL | MB_ICONERROR);
	if (userClick == IDNO)
		exit(0);
#endif
}

void FatalAssertFail(const std::string& expr, const std::string& file, const std::string& func, int lineNum) {
#ifdef WIN32 
#ifdef ENABLEASSERTS
	std::stringstream errorMessage;
	errorMessage << expr << " fatal assertion failed" << std::endl;
	errorMessage << "in file " << file << "in function " << func << " line number " << lineNum << std::endl;
	const std::string error = errorMessage.str();
	__debugbreak();
	MessageBoxA(NULL, error.c_str(), "Fatal Assert Fail", MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
	exit(0);
#else
	(void)expr;
	(void)file;
	(void)func;
	(void)lineNum;
#endif
#endif
}

void RecoverableAssertFail(const std::string& expr, const std::string& file, const std::string& func, int lineNum) {
#ifdef WIN32
#ifdef ENABLEASSERTS
	std::stringstream errorMessage;
	errorMessage << expr << " recoverable assertion failed" << std::endl;
	errorMessage << "in file " << file << " in function " << func << " line number " << lineNum << std::endl;
	errorMessage << "Continue Execution?" << std::endl;
	const std::string error = errorMessage.str();
	int userClick = MessageBoxA(NULL, error.c_str(), "Recoverable Assert Fail", MB_YESNO | MB_SYSTEMMODAL | MB_ICONERROR);
	//__debugbreak();
	if (userClick == IDNO) {
			__debugbreak();
			exit(0);
	}
#else
	(void)expr;
	(void)file;
	(void)func;
	(void)lineNum;
#endif
#endif
}