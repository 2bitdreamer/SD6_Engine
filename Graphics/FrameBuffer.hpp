#pragma once

class FrameBuffer
{
public:
	FrameBuffer(void);
	~FrameBuffer(void);
	void Activate();
	void Deactivate();
public:
	unsigned int m_frameBufferObjectID;
	unsigned int m_framebufferColorTextureID;
	unsigned int m_framebufferDepthTextureID;
};
