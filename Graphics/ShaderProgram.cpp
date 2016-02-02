#ifdef _WIN32
#include <windows.h>
#endif

#include <gl/gl.h>
#include <gl/glu.h>

#include "Engine/Graphics/ShaderProgram.hpp"
#include "Engine/Utilities/Time.hpp"
#include "Engine/Utilities/EngineCommon.hpp"

std::map< std::string, ShaderProgram* > ShaderProgram::s_shaderProgramRegistry;

int printOglError()
{
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		DebuggerPrintf("glError : %s\n", gluErrorString(glErr));
		retCode = 1;
		exit(0);
	}
	return retCode;
}


std::string ShaderProgram::GetFragmentShaderPath(const std::string& shaderName) {
	
	return Stringf("Data/Shaders/%s.fragment.glsl", shaderName.c_str());
}


std::string ShaderProgram::GetVertexShaderPath(const std::string& shaderName) {

	return Stringf("Data/Shaders/%s.vertex.glsl", shaderName.c_str());
}



ShaderProgram* ShaderProgram::GetShaderProgramByName(const std::string& shaderProgramName) {
	ShaderProgram* returnShaderProgram = nullptr;
	if (s_shaderProgramRegistry.find(shaderProgramName) != s_shaderProgramRegistry.end())
	{
		returnShaderProgram = s_shaderProgramRegistry[shaderProgramName];
	}
	return returnShaderProgram;
}

ShaderProgram* ShaderProgram::CreateOrGetShaderProgram(const std::string& shaderName) {
	ShaderProgram* returnShaderProgram = GetShaderProgramByName(shaderName);
	if (returnShaderProgram == nullptr) {
		returnShaderProgram = new ShaderProgram(shaderName);
		s_shaderProgramRegistry[shaderName] = returnShaderProgram;
	}
	return returnShaderProgram;
}


#pragma message("Make constructor build Shaders itself from std::string") 
ShaderProgram::ShaderProgram(const std::string& shaderName) {

	Shader vertexShader = Shader(GL_VERTEX_SHADER, GetVertexShaderPath(shaderName));
	Shader fragmentShader = Shader(GL_FRAGMENT_SHADER, GetFragmentShaderPath(shaderName));

	// Create a new shader program ID
	m_programID = glCreateProgram();
	int wasSuccessful;

	// Attach the vertex and fragment shaders to the new shader program
	glAttachShader(m_programID, vertexShader.GetShaderID());
	glAttachShader(m_programID, fragmentShader.GetShaderID());

	//  Do some other advanced stuff we'll do later on (like setting generic vertex attrib locations)
	glBindAttribLocation(m_programID, 0, "a_position");
	glBindAttribLocation(m_programID, 1, "a_color");
	glBindAttribLocation(m_programID, 2, "a_texCoords");
	glBindAttribLocation(m_programID, 3, "a_normal");

	//  Link the program
	glLinkProgram(m_programID);

	// Check for link errors
	glGetProgramiv(m_programID, GL_LINK_STATUS, &wasSuccessful);

	if (wasSuccessful != GL_TRUE) {
		vertexShader.PrintLog(m_programID, vertexShader.GetShaderFilePath(), fragmentShader.GetShaderFilePath());
		exit(1); // COMMENT: exit() should never be called outside of main, throw an exception instead
	}
}

void ShaderProgram::Activate() {
	// 100. Use the shader program ID to "turn it on" for all subsequent drawing
	glUseProgram(m_programID);

	// 102. Get the location # of each named uniform you wish to pass in to the shader
	int timeUniformLocation = glGetUniformLocation(m_programID, "u_time");
	int scale = glGetUniformLocation(m_programID, "Scale");
	int diffuseMapUniformLocation = glGetUniformLocation(m_programID, "u_diffuseMap");
	int normalMapUniformLocation = glGetUniformLocation(m_programID, "u_normalMap");
	int specularMapUniformLocation = glGetUniformLocation(m_programID, "u_specularMap");
	int emissiveMapUniformLocation = glGetUniformLocation(m_programID, "u_emissiveMap");


	// 103. Set the uniform values, including the texture unit numbers for texture (sampler) uniforms
	glUniform1f(timeUniformLocation, (float)GetAbsoluteTimeSeconds());
	glUniform1f(scale, 2.f);
	glUniform1i(diffuseMapUniformLocation, 0);  // for GL_TEXTURE0, texture unit 0
	glUniform1i(normalMapUniformLocation, 1);   // for GL_TEXTURE1, texture unit 1
	glUniform1i(specularMapUniformLocation, 2); // for GL_TEXTURE2, texture unit 2
	glUniform1i(emissiveMapUniformLocation, 3); // for GL_TEXTURE3, texture unit 3
}

const unsigned int ShaderProgram::GetProgramID() const {
	return m_programID;
}


ShaderProgram::~ShaderProgram(void)
{
}