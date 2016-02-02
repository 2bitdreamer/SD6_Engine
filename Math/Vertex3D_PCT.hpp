#pragma once
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Math/raw_vector.hpp"

struct Vertex3D_PCT
{
	Vec3		m_position;
	RGBA		m_color;
	Vec2		m_texCoords;
	Vertex3D_PCT(): m_color(255,255,255,255) {};

	bool operator == ( const Vertex3D_PCT& rhs ) const
	{
		if (m_position == rhs.m_position && m_color == rhs.m_color && m_texCoords == rhs.m_texCoords) {
			return true;
		}
		else {
			return false;
		}
	}

};

