#ifdef _WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <iosfwd>
#include <ostream>

#include <gl/gl.h>
#include "GL/glext.h"

#include "Engine/Utilities/EngineCommon.hpp"

#include "Shader.hpp"
#include "Renderer.hpp"

//Shader::Shader() {
//
//}

// COMMENT: This is a very specific function, and probably belongs in Shader.cpp, with a different name. The more generic a function's name, the wider a scope it ought to cover
void Shader::PrintLog(unsigned int obj, const std::string& shaderFilePath, const std::string& othershaderFilePath/*=std::string()*/)
{
	
	GLboolean isShader = glIsShader(obj);

	char* fnameTemp = (char*)malloc(100);
	_splitpath(shaderFilePath.c_str(), NULL, NULL, fnameTemp, NULL);
	std::stringstream windowTitle;
	std::string fileName = std::string(fnameTemp);
	std::stringstream errorString;
	std::string errorLineNumber;

	int infologLength = 0;
	int maxLength = 0;

	if (isShader) {
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
		errorString << "GLSL shader compile error!" << std::endl;
	}
	else {
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
		errorString << "GLSL program link error!" << std::endl;
	}

	char* infoLog;
	infoLog = (char *)malloc(maxLength + 1);

	if (isShader)
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);

	if (infologLength > 0)
	{
		std::string errorLog(infoLog);

		std::size_t openParen = errorLog.find('(');
		std::size_t closeParen = errorLog.find(')');
		errorLineNumber = errorLog.substr(openParen + 1, closeParen - openParen - 1);

		errorString << fileName << " , " << "line " << errorLineNumber << ":" << std::endl;

		size_t found = errorLog.find("error");
		std::string errorStart = found != std::string::npos ? errorLog.substr(found) : errorLog;

		errorString << errorStart << std::endl;

		errorString << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
		errorString << "GLSL Shading Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		if (isShader) {
			errorString << "File location" << shaderFilePath << std::endl;
		}

		else {
			errorString << "File location of vertex shader: " << shaderFilePath << std::endl;
			errorString << "File location of fragment shader: " << othershaderFilePath << std::endl;
		}

		errorString << std::endl;
		errorString << "RAW ERROR LOG: " << infoLog;

		if (isShader) windowTitle << "GLSL Compile Error in " << fileName << std::endl;
		else windowTitle << "GLSL Link Error " << std::endl;

		std::stringstream consoleOutput;

		consoleOutput << "1>" << shaderFilePath << '(' << errorLineNumber << ')' << ": " << errorLog.substr(errorLog.find("error")) << "in OpenGL version: " << glGetString(GL_VERSION) << " and " << "GLSL Shading Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		OutputDebugStringA(consoleOutput.str().c_str());
		MessageBoxA(nullptr, errorString.str().c_str(), windowTitle.str().c_str(), MB_OK);
	}

	free(fnameTemp);
	free(infoLog);
}

Shader::Shader(int shaderType, std::string shaderFilePath)
{
	m_shaderFilePath = shaderFilePath;
	m_shaderType = shaderType;
	int wasSuccessful;

	char* shaderTextBuffer = NULL;

	// 1. Load the vertex shader code (text file) to a new memory buffer
	bool didFileLoad = LoadFileToNewTextBuffer(m_shaderFilePath, shaderTextBuffer);
	if (!didFileLoad) {
		std::stringstream errormsg;
		errormsg << "Shader file " << m_shaderFilePath << "could not be opened successfully. " << std::endl;
		OutputDebugStringA(errormsg.str().c_str());
		assert(false);
	}


	// 2. Create a new shader ID
	m_shaderID = glCreateShader(m_shaderType); // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER


	// 3. Associate the shader code with the new shader ID
	glShaderSource(m_shaderID, 1, &shaderTextBuffer, nullptr);

	// 4. Compile the shader (the shader compiler is built in to your graphics card driver)
	glCompileShader(m_shaderID);

	// 5. Check for compile errors
	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &wasSuccessful);

	if (wasSuccessful != GL_TRUE) {
		PrintLog(m_shaderID, shaderFilePath);
		exit(1);
	}
	// 7. Delete the shader program text buffer (we don't need the shader source anymore since we've compiled it)
	free(shaderTextBuffer);
}

const int Shader::GetShaderID() const
{
	return m_shaderID;
}

const std::string& Shader::GetShaderFilePath() const
{
	return m_shaderFilePath;
}

Shader::~Shader(void)
{
}
