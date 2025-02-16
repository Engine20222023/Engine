#include "Headers.h"
#include "FrameBuffer.h"
#include "ModuleWindow.h"

FrameBuffer::FrameBuffer()
{
	width = ModuleWindow::width;
	height = ModuleWindow::height;
}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::SetDimensions(int width, int height)
{
	this->width = width;
	this->height = height;
	RegenerateBuffer();
}

void FrameBuffer::SetBufferInfo()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// generate texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::RegenerateBuffer()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &textureColorbuffer);
	glDeleteRenderbuffers(1, &RBO);

	SetBufferInfo();
}
