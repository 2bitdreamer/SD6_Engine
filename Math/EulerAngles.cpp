#include "Engine/Math/EulerAngles.hpp"


EulerAngles::EulerAngles(void)
{
	rollDegreesAboutX = 0.f;
	pitchDegreesAboutY = 0.f;
	yawDegreesAboutZ = 0.f;
}

EulerAngles::EulerAngles(float pitch, float yaw) {
	rollDegreesAboutX = 0.f;
	pitchDegreesAboutY = pitch;
	yawDegreesAboutZ = yaw;
}


EulerAngles::~EulerAngles(void)
{
}
