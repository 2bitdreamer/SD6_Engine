#include "KeyFrameAnimationBase.hpp"
#include "Assert.hpp"



KeyFrameAnimationBase::KeyFrameAnimationBase()
{
}


KeyFrameAnimationBase::~KeyFrameAnimationBase()
{
}

bool KeyFrameAnimationBase::IsLooping()
{
	FATAL_ERROR("Function should never be called");
}

void KeyFrameAnimationBase::SetAnimationTime(float time)
{
	FATAL_ERROR("Function should never be called");
	(void)time;
}

float KeyFrameAnimationBase::GetAnimationTime() {
	FATAL_ERROR("Function should never be called");
}
