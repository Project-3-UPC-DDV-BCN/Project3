#include "RenderTextureMSAA.h"
#include "Application.h"
#include "ModuleWindow.h"

RenderTextureMSAA::RenderTextureMSAA()
{
	fbo_id = 0;
	fbo_msaa_id = 0;
	texture_id = 0;
	rbo_id = 0;
	rbo_color_id = 0;
	rbo_depth_id = 0;

	SetType(Resource::RenderTexture);
}

RenderTextureMSAA::~RenderTextureMSAA()
{
}

bool RenderTextureMSAA::Create(uint width, uint height, int MSAA_level)
{
	max_msaa_samples = 0;
	glGetIntegerv(GL_MAX_SAMPLES, &max_msaa_samples);
	if (MSAA_level > max_msaa_samples) MSAA_level = max_msaa_samples;
	current_msaa_samples = MSAA_level;

	// create a texture object
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a MSAA framebuffer object
	glGenFramebuffers(1, &fbo_msaa_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa_id);

	// create a MSAA renderbuffer object to store color info
	glGenRenderbuffers(1, &rbo_color_id);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color_id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_level, GL_RGB8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// create a MSAA renderbuffer object to store depth info
	glGenRenderbuffers(1, &rbo_depth_id);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth_id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_level, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach msaa RBOs to FBO attachment points
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth_id);

	// create a normal (no MSAA) FBO to hold a render-to-texture
	glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	glGenRenderbuffers(1, &rbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach a texture to FBO color attachement point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

	// attach a rbo to FBO depth attachement point
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_id);

	// check FBO status
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->width = width;
	this->height = height;

	return true;
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
		GL_LINEAR); // scale filter
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

	glDeleteFramebuffers(1, &fbo_id);
	fbo_id = 0;
	glDeleteFramebuffers(1, &fbo_msaa_id);
	fbo_msaa_id = 0;

	glDeleteRenderbuffers(1, &rbo_id);
	glDeleteRenderbuffers(1, &rbo_color_id);
	glDeleteRenderbuffers(1, &rbo_depth_id);
	rbo_id = 0;
	rbo_color_id = 0;
	rbo_depth_id = 0;
}

uint RenderTextureMSAA::GetTexture() const
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

void RenderTextureMSAA::Load(Data & data)
{
}

void RenderTextureMSAA::CreateMeta() const
{
}
