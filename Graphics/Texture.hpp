#pragma once

#include <iostream>
#include <map>
#include <string>

#include "Engine/Math/raw_vector.hpp"

class Texture
{
public:
	Texture(const std::string& imageFilePath);
	Texture(unsigned char* imageData, const Vec2i& imageSize);
	Texture(void);
	~Texture(void);

	static Texture* GetTextureByName(const std::string& imageFilePath); // COMMENT: Maybe textures should be passed by value, since they are Plain-Old-Data (POD)
	static Texture* CreateOrGetTexture(const std::string& imageFilePath);
	unsigned int GetOpenglTextureID() const;
	const Vec2i& GetSize() const;
private:
	static std::map< std::string, Texture* > s_textureRegistry; // COMMENT: Technically this is a memory leak

	unsigned int m_openglTextureID;
	Vec2i m_size;
	bool IsValid();
};