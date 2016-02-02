#pragma once
#include <string>

class Shader
{
public:
	void PrintLog(unsigned int obj, const std::string& shaderFilePath, const std::string& othershaderFilePath = std::string());
	Shader(int shaderType, std::string shaderFilePath);
	Shader();
	const int GetShaderID() const;
	const std::string& GetShaderFilePath() const;
	~Shader(void);

private:
	int m_shaderID;
	unsigned int m_shaderType;
	std::string m_shaderFilePath;
	std::string m_shaderFileName;
};

