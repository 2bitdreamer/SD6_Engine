#pragma once
#include "../Math/EulerAngles.hpp"
#include "../Math/raw_vector.hpp"

class Camera3D
{
public:
	Camera3D(void);
	~Camera3D(void);
	Vec3 m_position;
	EulerAngles m_orientation;
};

