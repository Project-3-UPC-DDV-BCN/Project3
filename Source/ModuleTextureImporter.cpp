#include "ModuleTextureImporter.h"
#include "Texture.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "OpenGL.h"

#include "Devil/include/il.h"
#include "Devil/include/ilut.h"
#pragma comment ( lib, "Devil/libx86/DevIL.lib" )
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )

ModuleTextureImporter::ModuleTextureImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Texture_Importer";

	ilInit();
	iluInit();
	//ilutInit();
	//ilutRenderer(ILUT_OPENGL);
}

ModuleTextureImporter::~ModuleTextureImporter()
{
}

bool ModuleTextureImporter::CleanUp()
{
	ilShutDown();
	return true;
}

std::string ModuleTextureImporter::ImportTexture(std::string path)
{
	std::string ret;

	ILuint image_id;
	ilGenImages(1, &image_id);
	ilBindImage(image_id);

	if (ilLoad(IL_TYPE_UNKNOWN, path.c_str()))
	{
		//save texture library
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		if (ilSave(IL_DDS, (LIBRARY_TEXTURES_FOLDER + App->file_system->GetFileNameWithoutExtension(path) + ".dds").c_str()))
		{
			CONSOLE_DEBUG("%s library file created.", App->file_system->GetFileNameWithoutExtension(path).c_str());
			ret = LIBRARY_TEXTURES_FOLDER + App->file_system->GetFileNameWithoutExtension(path) + ".dds";
		}
		else
		{
			CONSOLE_DEBUG("%s library file cannot be created.", App->file_system->GetFileNameWithoutExtension(path).c_str());
		}

		ilDeleteImage(ilGetInteger(IL_ACTIVE_IMAGE));
		CONSOLE_DEBUG("Texture Loaded: %s", path.c_str());
	}
	else
	{
		CONSOLE_DEBUG("Cannot load image %s. Error: %s", path.c_str(), iluErrorString(ilGetError()));
	}

	return ret;
}

Texture * ModuleTextureImporter::LoadTextureFromLibrary(std::string path, bool on_mem)
{
	Texture* tmp_texture = nullptr;



	if (ilLoad(IL_TYPE_UNKNOWN, path.c_str()))
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			CONSOLE_LOG("DeviL: Failed to convert image %s. Error: %s", path.c_str(), iluErrorString(ilGetError()));
		}

		ILubyte *data = ilGetData();
		int width = ilGetInteger(IL_IMAGE_WIDTH);
		int height = ilGetInteger(IL_IMAGE_HEIGHT);
		int format = ilGetInteger(IL_IMAGE_FORMAT);
		int type = ilGetInteger(IL_IMAGE_TYPE);
		int file_size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		if (file_size > 0)
		{
			tmp_texture = new Texture();
			tmp_texture->SetWidth(width);
			tmp_texture->SetHeight(height);
			tmp_texture->SetLibraryPath(path);
			tmp_texture->SetName(App->file_system->GetFileNameWithoutExtension(path).c_str());
			tmp_texture->SetCompression(ilGetInteger(IL_DXTC_FORMAT));
			//tmp_texture->SetImageData(data);
			tmp_texture->SetFormat(Texture::rgba);
			//if(on_mem) 
			//tmp_texture->LoadToMemory();

			uint texture_id;
			glGenTextures(1, &texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			tmp_texture->SetID(texture_id);
		
			CONSOLE_DEBUG("Image loaded from library: %s", path.c_str());
		}

		ilDeleteImages(1, &ImageInfo.Id);
	}
	else
	{
		CONSOLE_DEBUG("Cannot load image %s. Error: %s", path.c_str(), iluErrorString(ilGetError()));
	}

	return tmp_texture;
}

void ModuleTextureImporter::CreateCubeMapTexture(std::string textures_path[6], uint& cube_map_id)
{
	glGenTextures(1, &cube_map_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_id);

	for (int i = 0; i < 6; i++)
	{
		if (ilLoad(IL_BMP, textures_path[i].c_str()))
		{
			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);

			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}

			if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
			{
				CONSOLE_LOG("DeviL: Failed to convert image %s. Error: %s", textures_path[i].c_str(), iluErrorString(ilGetError()));
			}

			ILubyte *data = ilGetData();
			int width = ilGetInteger(IL_IMAGE_WIDTH);
			int height = ilGetInteger(IL_IMAGE_HEIGHT);
			int file_size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
			if (file_size > 0)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				CONSOLE_DEBUG("CubeMap Image loaded from library: %s", textures_path[i].c_str());
			}

			ilDeleteImages(1, &ImageInfo.Id);
		}
		else
		{
			CONSOLE_DEBUG("Cannot load image %s. Error: %s", textures_path[i].c_str(), iluErrorString(ilGetError()));
		}
	}
}

