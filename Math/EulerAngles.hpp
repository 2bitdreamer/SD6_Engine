#pragma once
class EulerAngles
{
public:
	float rollDegreesAboutX;
	float pitchDegreesAboutY;
	float yawDegreesAboutZ;

	EulerAngles(void);
	EulerAngles(float pitch, float yaw);
	~EulerAngles(void);
};

