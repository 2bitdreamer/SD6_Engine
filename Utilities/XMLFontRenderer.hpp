#pragma once
#include "Engine/Libraries/tinyxml.h"
#include "Engine/Libraries/tinystr.h"
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Graphics/Texture.hpp"
#include <vector>

struct Rect {
	int left, right, top, bottom;
};

struct FontElement {
	int id, x, y, width, height, xoffset, yoffset, xadvance;
};

struct Charset
{
	unsigned short LineHeight;
	unsigned short Base;
	unsigned short Width, Height;
	std::vector<FontElement> Chars;
};

class XMLFontRenderer
{
public:
	XMLFontRenderer();
	void LoadFontFromName(const std::string& name);
	~XMLFontRenderer(void);
	void LoadFont(const char* pFilename);
	void renderCharacter(char theCharacter);
	void DrawString(const std::string& Str, const RGBA& color, const Vec3& position, float cellHeight=50.f);
	void DrawString(const std::string& Str, const char* fontFileName="Fonts/Calibri.fnt", const Vec3& screenPosition=Vec3(600.f,450.f,0.f), float cellHeight=50.f, const RGBA& color= RGBA(0, 255, 255, 255));
	float CalcTextWidth(const std::string& text, float height);
public:
	const char* m_fontImage;
	Vec2 m_cursor;
	Vec2 m_lastCursor;
	Charset m_charset;
	std::string m_fontTexturePath;
	std::string m_currentFontLoadedName;
};

