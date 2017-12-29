#include "RenderTextureMSAA.h"
#include "Application.h"
#include "ModuleWindow.h"

RenderTextureMSAA::RenderTextureMSAA()
{
	fbo_id = 0;
	fbo_msaa_id = 0;
	texture_id = 0;
	texture_msaa_id = 0;
	rbo_id = 0;
	max_msaa_samples = 8; //defined at modulewindow line 61 
	SetType(Resource::RenderTextureResource);
}

RenderTextureMSAA::~RenderTextureMSAA()
{
}

bool RenderTextureMSAA::Create(uint width, uint height, int MSAA_level)
{
	bool ret = true;

	if (MSAA_level > max_msaa_samples) MSAA_level = max_msaa_samples;
	current_msaa_samples = MSAA_level;

	//Create MSAA framebufer
	glGenFramebuffers(1, &fbo_msaa_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa_id);

	//Create a multisampled color attachment texture
	glGenTextures(1, &texture_msaa_id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_msaa_id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, current_msaa_samples, GL_RGB, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_msaa_id, 0);

	//Create a renderbuffer for depth and stencil
	glGenRenderbuffers(1, &rbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, current_msaa_samples, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_id);

	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if ( error != GL_FRAMEBUFFER_COMPLETE)
	{
		CONSOLE_ERROR("RenderTextureMSAA: Framebuffer is not complete! %s", gluErrorString(error));
		ret = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//configure post-processing framebuffer
	glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	//create the color attachment texture
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

	error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		CONSOLE_ERROR("RenderTextureMSAA: Intermediate framebuffer is not complete! %s", gluErrorString(error));
		ret = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->width = width;
	this->height = height;

	return ret;
}

void RenderTextureMSAA::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa_id);
	glViewport(0, 0, width, height);
}

void RenderTextureMSAA::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
}

void RenderTextureMSAA::Render()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_msaa_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
	glBlitFramebuffer(0, 0, width, height,  // src rect
		0, 0, width, height,  // dst rect
		GL_COLOR_BUFFER_BIT, // buffer mask
		GL_NEAREST); // scale filter
}

void RenderTextureMSAA::ChangeMSAALevel(int MSAA_level)
{
	Destroy();
	Create(width, height, MSAA_level);
}

void RenderTextureMSAA::Destroy()
{
	glDeleteTextures(1, &texture_id);
	texture_id = 0;
	glDeleteTextures(1, &texture_msaa_id);
	texture_msaa_id = 0;

	glDeleteFramebuffers(1, &fbo_id);
	fbo_id = 0;
	glDeleteFramebuffers(1, &fbo_msaa_id);
	fbo_msaa_id = 0;
	glDeleteRenderbuffers(1, &rbo_id);
	rbo_id = 0;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		CONSOLE_ERROR("RenderTextureMSAA: error at destroy! %s\n", gluErrorString(error));
	}
}

uint RenderTextureMSAA::GetTextureID() const
{
	return texture_id;
}

int RenderTextureMSAA::GetMaxMSAALevel() const
{
	return max_msaa_samples;
}

int RenderTextureMSAA::GetCurrentMSAALevel() const
{
	return current_msaa_samples;
}

void RenderTextureMSAA::Save(Data & data) const
{
}

bool RenderTextureMSAA::Load(Data & data)
{
	return true;
}

void RenderTextureMSAA::CreateMeta() const
{
}

void RenderTextureMSAA::LoadToMemory()
{
}

void RenderTextureMSAA::UnloadFromMemory()
{
}

uint RenderTextureMSAA::GetWidth() const
{
	return width;
}

uint RenderTextureMSAA::GetHeight() const
{
	return height;
}

uint RenderTextureMSAA::GetFrameBufferID() const
{
	return fbo_msaa_id;
}
