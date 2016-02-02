#include "Engine/Graphics/FrameBuffer.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Utilities/EngineCommon.hpp"
#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "Engine/Graphics/GL/glext.h"


FrameBuffer::FrameBuffer(void)
{
	// Create color framebuffer texture
	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &m_framebufferColorTextureID );
	glBindTexture( GL_TEXTURE_2D, m_framebufferColorTextureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, (int)(SCREEN_WIDTH), (int)(SCREEN_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL ); // NOTE: RGBA and UNSIGNED BYTE

	// Create depth framebuffer texture
	//glActiveTexture( GL_TEXTURE1 );
	glGenTextures( 1, &m_framebufferDepthTextureID );
	glBindTexture( GL_TEXTURE_2D, m_framebufferDepthTextureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)(SCREEN_WIDTH), (int)(SCREEN_HEIGHT), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL ); // NOTE: DEPTH and UNSIGNED INT

	// Create an FBO (Framebuffer Object) and activate it
	glGenFramebuffers( 1, &m_frameBufferObjectID );
	glBindFramebuffer( GL_FRAMEBUFFER, m_frameBufferObjectID );

	// Attach our color and depth textures to the FBO, in the color0 and depth FBO "slots"
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebufferColorTextureID, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_framebufferDepthTextureID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Activate() {
	glBindFramebuffer( GL_FRAMEBUFFER, m_frameBufferObjectID );
}

void FrameBuffer::Deactivate() {
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


FrameBuffer::~FrameBuffer(void)
{
}
