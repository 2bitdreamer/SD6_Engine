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
	return false;
}

void KeyFrameAnimationBase::SetAnimationTime(float time)
{
	FATAL_ERROR("Function should never be called");
}

float KeyFrameAnimationBase::GetAnimationTime() {
	FATAL_ERROR("Function should never be called");
}
