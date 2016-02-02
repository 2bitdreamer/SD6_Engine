#include "Material.hpp"
#include "Texture.hpp"

namespace{
	ShaderProgram* GetDefaultShaderProgram()
	{
		static ShaderProgram* s_defaultShaderProgram = new ShaderProgram("BasicUnlit");
		return s_defaultShaderProgram;
	}
}

Material const* Material::s_currentlyBoundMaterial = nullptr;

Material::Material(void)
	: m_shaderProgram(GetDefaultShaderProgram())
{

}

Material::Material(const std::string& shaderProgramName)
{
	m_shaderProgram = ShaderProgram::CreateOrGetShaderProgram(shaderProgramName);
}

Material::~Material(void)
{

}

const Texture* Material::CreateDefaultWhiteTexture() {
	unsigned char whiteTex[4] = { 255, 255, 255, 255 };
	static Texture* defaultTexture = new Texture(whiteTex, Vec2i(1, 1));
	return defaultTexture;
}

const Texture* Material::CreateDefaultDarkYellowTexture() {
	unsigned char yellowTex[4] = { 128, 128, 0, 255 };
	static Texture* defaultTexture = new Texture(yellowTex, Vec2i(1, 1));
	return defaultTexture;
}


const Texture* Material::CreateDefaultLightBlueTexture() {
	unsigned char blueTex[4] = { 128, 128, 255, 255 };
	static Texture* defaultTexture = new Texture(blueTex, Vec2i(1, 1));
	return defaultTexture;
}

void Material::AddTexture(const std::string& textureFilePath, const std::string& uniformName)
{
	TextureUniform textureUniform;
	textureUniform.texture = Texture::CreateOrGetTexture(textureFilePath);

	if (!textureUniform.texture || textureFilePath.empty()) {
		if (uniformName == "u_normalMap") {
			textureUniform.texture = CreateDefaultLightBlueTexture();
		}
		else if (uniformName == "u_specularMap") {
			textureUniform.texture = CreateDefaultDarkYellowTexture();
		}
		else if (uniformName == "u_diffuseMap") {
			textureUniform.texture = CreateDefaultWhiteTexture();
		}
	}

	textureUniform.uniformLocation = glGetUniformLocation(GetShaderProgramID(), uniformName.c_str());
	textureUniform.uniformName = uniformName;
	m_textureUniforms.push_back(textureUniform);

}

void Material::Activate() const
{
	if (this == s_currentlyBoundMaterial)
		return;

	s_currentlyBoundMaterial = this;

	int shaderID = GetShaderProgramID();
	glUseProgram(shaderID);

	for (unsigned int textureUnit = 0; textureUnit < m_textureUniforms.size(); textureUnit++) {
		const TextureUniform& textureUniform = m_textureUniforms[textureUnit];
		const Texture* theTexture = textureUniform.texture;
		glActiveTexture(textureUnit + GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, theTexture->GetOpenglTextureID());
		glUniform1i(textureUniform.uniformLocation, textureUnit);
	}

	glActiveTexture(GL_TEXTURE0);
}

const int Material::GetShaderProgramID() const
{
	return m_shaderProgram->GetProgramID();
}

const Vec2i& Material::GetSize() const
{
	return m_textureUniforms[0].texture->GetSize();
}