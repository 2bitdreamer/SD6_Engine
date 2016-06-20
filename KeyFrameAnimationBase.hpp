#pragma once
class KeyFrameAnimationBase
{
public:
	KeyFrameAnimationBase();
	virtual ~KeyFrameAnimationBase();

	virtual bool IsLooping();
	virtual void SetAnimationTime(float time);
	virtual float GetAnimationTime();
};

