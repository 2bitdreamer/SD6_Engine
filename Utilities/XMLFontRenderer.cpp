#include "Engine/Utilities/XMLFontRenderer.hpp"
#include <stdlib.h>
#include "Engine/Utilities/EngineCommon.hpp"
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Math/Vertex3D_PCT.hpp"
#include "../Graphics/Renderer.hpp"

XMLFontRenderer::XMLFontRenderer(void)
{
	LoadFont("Data/Fonts/Papyrus.fnt");
	m_fontTexturePath = "Data/Fonts/Papyrus_0.png";
	m_cursor = Vec2(0.f, 0.f);
	m_lastCursor = Vec2(0.f, 0.f);
}


void XMLFontRenderer::LoadFontFromName(const std::string& name) {
	const std::string fontPath = "Data/Fonts/" + name + ".fnt";
	const std::string texturePath = "Data/Fonts/" + name + "_0.png";
	m_fontTexturePath = texturePath;
	LoadFont(fontPath.c_str());
}

void XMLFontRenderer::LoadFont(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();
	if (!loadOkay)
		return;
	else {
		m_charset = Charset();
		m_charset.Chars.resize(256 * 3);

		m_currentFontLoadedName = std::string(pFilename);
		TiXmlElement* docElement = doc.RootElement();

		TiXmlElement* commonTag = docElement->FirstChild( "common" )->ToElement();
		m_charset.LineHeight = (unsigned short)atoi(commonTag->Attribute( "lineHeight"));
		m_charset.Base = (unsigned short)atoi(commonTag->Attribute( "base"));
		m_charset.Width = (unsigned short)atoi(commonTag->Attribute( "scaleW"));
		m_charset.Height = (unsigned short)atoi(commonTag->Attribute( "scaleH"));

		TiXmlElement* pageTag = docElement->FirstChild( "pages" )->FirstChild("page")->ToElement();
		TiXmlElement* charTag = docElement->FirstChild("chars")->FirstChild("char")->ToElement();

		m_fontImage = pageTag->Attribute( "file");
		for( charTag; charTag; charTag = charTag->NextSiblingElement() )
		{
			FontElement currentChar;
			currentChar.id = atoi(charTag->Attribute( "id"));
			currentChar.x = atoi(charTag->Attribute( "x"));
			currentChar.y = atoi(charTag->Attribute( "y"));
			currentChar.width = atoi(charTag->Attribute( "width"));
			currentChar.height = atoi(charTag->Attribute( "height"));
			currentChar.xoffset = atoi(charTag->Attribute( "xoffset"));
			currentChar.yoffset = atoi(charTag->Attribute( "yoffset"));
			currentChar.xadvance = atoi(charTag->Attribute( "xadvance"));
			m_charset.Chars[currentChar.id] = currentChar;
		}

	}
}

float XMLFontRenderer::CalcTextWidth(const std::string& text, float height) {

	Vec2 cursor = Vec2(0.f, 0.f);

	float ratio = height/m_charset.LineHeight;
	for( size_t i = 0; i < text.length(); ++i )
	{
		FontElement& currentFontElement = m_charset.Chars[text[i]];
		cursor.x() += currentFontElement.xadvance;// + currentFontElement.xoffset + currentFontElement);
	}

	return cursor.x() * ratio;
}

void XMLFontRenderer::renderCharacter(char theCharacter) {
	theCharacter = 'i';
}


XMLFontRenderer::~XMLFontRenderer(void)
{

}


void XMLFontRenderer::DrawString(const std::string& Str, const char* fontFileName, const Vec3& screenPosition, float cellHeight, const RGBA& color) {
	if (std::string(fontFileName).compare(m_currentFontLoadedName) != 0)
		LoadFont(fontFileName);
	
	DrawString(Str, color, screenPosition, cellHeight);
}


void XMLFontRenderer::DrawString(const std::string& Str, const RGBA& color, const Vec3& position, float cellHeight) {

	std::vector<Vertex3D_PCT> m_vertexData;
	Renderer& myRenderer = Renderer::GetInstance();

	myRenderer.PushMatrix();
	myRenderer.TranslateMatrix(position.x(), position.y(), position.z());
	float ratio = cellHeight / (float)m_charset.LineHeight;
	myRenderer.SetScale(ratio, ratio, 1.f);

	float lineTopY = (float)m_charset.LineHeight;

	for (size_t i = 0; i < Str.size(); ++i)
	{
		char currChar = Str[i];

		FontElement element = m_charset.Chars[currChar];

		int CharX = element.x;
		int CharY = element.y;
		int Width = element.width;
		int Height = element.height;


		int OffsetX = element.xoffset;
		int OffsetY = element.yoffset;

		float charTopY = lineTopY - (float)OffsetY;
		float charBottomY = charTopY - (float)Height;


		//upper left
		Vertex3D_PCT upLeft, upRight, lowRight, lowLeft;
		Vec2 upLeftTexCoord = Vec2(0.f, 0.f);
		upLeftTexCoord.x() = (float)CharX / (float)m_charset.Width;
		upLeftTexCoord.y() = (float)CharY / (float)m_charset.Height;

		Vec2 upLeftCoord = Vec2(0.f, 0.f);
		upLeftCoord.x() = (float)m_cursor.x() + OffsetX;
		upLeftCoord.y() = charTopY;

		upLeft.m_color = color;
		upLeft.m_position = Vec3(upLeftCoord.x(), upLeftCoord.y(), 0.f);
		upLeft.m_texCoords = upLeftTexCoord;

		//upper right
		Vec2 upRightTexCoord = Vec2(0.f, 0.f);
		upRightTexCoord.x() = (float)(CharX + Width) / (float)m_charset.Width;
		upRightTexCoord.y() = (float)CharY / (float)m_charset.Height;

		Vec2 upRightCoord = Vec2(0.f, 0.f);
		upRightCoord.x() = (float)Width + m_cursor.x() + OffsetX;
		upRightCoord.y() = charTopY;

		upRight.m_color = color;
		upRight.m_position = Vec3(upRightCoord.x(), upRightCoord.y(), 0.f);
		upRight.m_texCoords = upRightTexCoord;

		//lower right
		Vec2 lowRightTexCoord = Vec2(0.f, 0.f);
		lowRightTexCoord.x() = (float)(CharX + Width) / (float)m_charset.Width;
		lowRightTexCoord.y() = (float)(CharY + Height) / (float)m_charset.Height;

		Vec2 lowRightCoord = Vec2(0.f, 0.f);
		lowRightCoord.x() = (float)Width + m_cursor.x() + OffsetX;
		lowRightCoord.y() = charBottomY;

		lowRight.m_color = color;
		lowRight.m_position = Vec3(lowRightCoord.x(), lowRightCoord.y(), 0.f);
		lowRight.m_texCoords = lowRightTexCoord;

		//lower left
		Vec2 lowLeftTexCoord = Vec2(0.f, 0.f);
		lowLeftTexCoord.x() = (float)CharX / (float)m_charset.Width;
		lowLeftTexCoord.y() = (float)(CharY + Height) / (float)m_charset.Height;

		Vec2 lowLeftCoord = Vec2(0.f, 0.f);
		lowLeftCoord.x() = (float)m_cursor.x() + OffsetX;
		lowLeftCoord.y() = charBottomY;

		lowLeft.m_color = color;
		lowLeft.m_position = Vec3(lowLeftCoord.x(), lowLeftCoord.y(), 0.f);
		lowLeft.m_texCoords = lowLeftTexCoord;

		m_vertexData.push_back(upLeft);
		m_vertexData.push_back(upRight);
		m_vertexData.push_back(lowRight);
		m_vertexData.push_back(lowLeft);

		m_lastCursor = m_cursor;
		m_cursor.x() += m_charset.Chars[currChar].xadvance;

	}

	m_cursor = Vec2(0.f, 0.f);

	myRenderer.RenderTexturedQuadList(m_vertexData, m_fontTexturePath);
	myRenderer.PopMatrix();
}


