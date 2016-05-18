#pragma once
#include <string>

#define ENABLEASSERTS

#define FATAL_ERROR(errorText) FatalError(errorText, __FILE__, __FUNCTION__, __LINE__);
#define RECOVERABLE_ERROR(errorText) RecoverableError(errorText, __FILE__, __FUNCTION__, __LINE__);
#define FATAL_ASSERT(expr) if (!(expr)) FatalAssertFail(#expr, __FILE__, __FUNCTION__, __LINE__);
#define RECOVERABLE_ASSERT(expr) if (!(expr)) RecoverableAssertFail(#expr, __FILE__, __FUNCTION__, __LINE__);

__declspec(noreturn) void FatalError(const std::string& errorText, const std::string& file, const std::string& func, int lineNum);
void RecoverableAssertFail(const std::string& expr, const std::string& file, const std::string& func, int lineNum);
void FatalAssertFail(const std::string& expr, const std::string& file, const std::string& func, int lineNum);
void RecoverableError(const std::string& errorText, const std::string& file, const std::string& func, int lineNum);
