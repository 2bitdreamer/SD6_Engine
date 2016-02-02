#include "Camera3D.hpp"


Camera3D::Camera3D(void)
{
	m_orientation = EulerAngles();
	m_position = Vec3();
}


Camera3D::~Camera3D(void)
{
}
