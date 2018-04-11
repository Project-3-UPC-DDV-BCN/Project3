#include "ModuleTextureImporter.h"
#include "Texture.h"
#include "Application.h"
#include "ModuleFileSystem.h"

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
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
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

Texture * ModuleTextureImporter::LoadTextureFromLibrary(std::string path)
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
			tmp_texture->SetImageData(data);
			tmp_texture->SetFormat(Texture::rgba);
			tmp_texture->LoadToMemory();

			CONSOLE_DEBUG("Image loaded from library: %s", path.c_str());

			//tmp_texture->RecreateTexture();
		}

		ilDeleteImages(1, &ImageInfo.Id);
	}
	else
	{
		CONSOLE_DEBUG("Cannot load image %s. Error: %s", path.c_str(), iluErrorString(ilGetError()));
	}

	return tmp_texture;
}

