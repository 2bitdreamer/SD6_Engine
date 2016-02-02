#pragma once
#include <vector>
#include <string>
#include "ShaderProgram.hpp"
class Texture;

struct TextureUniform {
	std::string uniformName;
	const Texture* texture;
	int uniformLocation;
};

class Material
{
public:
	Material(void);
	Material(const std::string& shaderProgramName);
	~Material(void);
	void AddTexture(const std::string& textureFilePath, const std::string& uniformName);
	void Activate() const;

	static const Texture* CreateDefaultWhiteTexture();
	static const Texture* CreateDefaultDarkYellowTexture();
	static const Texture* CreateDefaultLightBlueTexture();
	const int GetShaderProgramID() const;
	const Vec2i& GetSize() const;
private:
	static Material const* s_currentlyBoundMaterial;
	ShaderProgram* m_shaderProgram;
	std::vector<TextureUniform> m_textureUniforms;
};

