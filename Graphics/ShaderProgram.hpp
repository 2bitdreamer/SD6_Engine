#pragma once

#include "Shader.hpp"
#include "Renderer.hpp"

class ShaderProgram
{
public:
	ShaderProgram::ShaderProgram(const std::string& shaderName);
	~ShaderProgram(void);
	void Activate();
	const unsigned int GetProgramID() const;

	static ShaderProgram* CreateOrGetShaderProgram(const std::string& shaderName);
	std::string GetFragmentShaderPath(const std::string& shaderName);
	std::string GetVertexShaderPath(const std::string& shaderName);
	std::string GetTesselationControlShaderPath(const std::string& shaderName);
	std::string GetTesselationEvaluationShaderPath(const std::string& shaderName);
	static ShaderProgram* GetShaderProgramByName(const std::string& shaderProgramName);
	static std::map< std::string, ShaderProgram* > s_shaderProgramRegistry;
private:
	unsigned int m_programID;
};

