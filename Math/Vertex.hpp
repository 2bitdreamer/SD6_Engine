#pragma once
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Math/raw_vector.hpp"
#include <cstring>


struct Vertex
{
	Vec3 m_position;
	RGBA m_color;
	Vec2 m_texCoords;
	Vec3 m_normal;
	Vec3 m_tangent;
	Vec3 m_bitangent;

	Vertex( const Vec3& position, const Vec3& normal, const Vec2& texCoords, const RGBA& color, const Vec3& tangent, const Vec3& bitangent );
	bool operator == (const Vertex& rhs) const;
};



inline Vertex::Vertex(
	const Vec3& position = Vec3(0.f, 0.f, 0.f), 
	const Vec3& normal = Vec3(0.f, 0.f, 1.f),
	const Vec2& texCoord = Vec2(0.f, 0.f), 
	const RGBA& color = RGBA(255, 255, 255, 255),
	const Vec3& tangent = Vec3(0.f, 0.f, 0.f), 
	const Vec3& bitangent = Vec3(0.f, 0.f, 0.f))
	: m_color(color)
	, m_texCoords(texCoord)
	, m_normal(normal)
	, m_position(position)
	, m_tangent(tangent)
	, m_bitangent(bitangent)
	{};



inline bool Vertex::operator == ( const Vertex& rhs ) const
{
	return memcmp(this, &rhs, sizeof(Vertex)) == 0;
};

