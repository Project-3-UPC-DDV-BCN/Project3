#include "RenderTexture.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

RenderTexture::RenderTexture()
{
	fbo_id = 0;
	color_texture_id = 0;
	depth_texture_id = 0;
	width = 0;
	height = 0;
}

RenderTexture::~RenderTexture()
{
	Destroy();
}

bool RenderTexture::Create(uint width, uint height)
{
	//Creates Framebuffer Object
	/*glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	//Create Color texture
	/*glGenTextures(1, &color_texture_id);
	glBindTexture(GL_TEXTURE_2D, color_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);*/

	//Create Depth Texture
	/*glGenTextures(1, &depth_texture_id);
	glBindTexture(GL_TEXTURE_2D, depth_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	//Attach both textures to fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	this->width = width;
	this->height = height;

	//If the fbo is not created return false
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	return true;
}

void RenderTexture::Resize(uint new_width, uint new_height)
{
	////In case you need to resize the texture. Not used right now.
	//if (width == new_width && height == new_height) {
	//	return;
	//}
	//Destroy();
	//Create(new_width, new_height);
	//width = new_width;
	//height = new_height;
	//App->renderer3D->OnResize(new_width, new_height);
}

void RenderTexture::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
	glViewport(0, 0, width, height);
}

void RenderTexture::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
}

void RenderTexture::Destroy()
{
	glDeleteFramebuffers(1, &fbo_id);
	glDeleteTextures(1, &color_texture_id);
	glDeleteTextures(1, &depth_texture_id);

	fbo_id = 0;
	color_texture_id = 0;
	depth_texture_id = 0;
}

void RenderTexture::Clear(Color clear_color)
{
	glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTexture::Draw()
{
	//Future method to draw all the elements to the texture
}

uint RenderTexture::GetTexture() const
{
	return fbo_id;
}
