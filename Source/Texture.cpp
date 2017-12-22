#include "Texture.h"
#include "OpenGL.h"
#include "Data.h"
#include "Application.h"
#include "ModuleTextureImporter.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include <ctime>

Texture::Texture()
{
	texture_id = 0;
	width = 0;
	height = 0;
	format = UnknownFormat;
	type = UnknownType;
	SetType(Resource::TextureResource);
}

Texture::~Texture()
{
	delete image_data;
	UnloadFromMemory();
}

void Texture::SetID(uint id)
{
	texture_id = id;
}

uint Texture::GetID() const
{
	return texture_id;
}

void Texture::SetWidth(uint width)
{
	this->width = width;
}

uint Texture::GetWidth() const
{
	return width;
}

void Texture::SetHeight(uint height)
{
	this->height = height;
}

uint Texture::GetHeight() const
{
	return height;
}

void Texture::SetSize(uint width, uint height)
{
	this->width = width;
	this->height = height;
}

void Texture::GetSize(uint & width, uint & height) const
{
	width = this->width;
	height = this->height;
}

void Texture::SetTextureType(TextureType type)
{
	this->type = type;
}

void Texture::SetImageData(byte * data)
{
	image_data = data;
}

byte * Texture::GetImageData() const
{
	return image_data;
}

Texture::TextureType Texture::GetTextureType() const
{
	return type;
}

std::string Texture::GetTypeString() const
{
	const char* types[] = {"Color Index", "Alpha", "rgb", "rgba", "bgr", "bgra", "Luminance", "Luminance Alpha", "Unknown" };
	return types[type];
}

void Texture::SetFormat(TextureFormat format)
{
	this->format = format;
}

Texture::TextureFormat Texture::GetFormat() const
{
	return format;
}

void Texture::SetCompression(int compression_format)
{
	compression = (CompressionFormat)compression_format;
}

Texture::CompressionFormat Texture::GetCompression() const
{
	return compression;
}

std::string Texture::CompressionToString() const
{
	std::string ret;
	switch (compression)
	{
	case Texture::IL_DXTC_FORMAT:
		ret = "IL_DXTC_FORMAT";
		break;
	case Texture::IL_DXT1:
		ret = "IL_DXT1";
		break;
	case Texture::IL_DXT2:
		ret = "IL_DXT2";
		break;
	case Texture::IL_DXT3:
		ret = "IL_DXT3";
		break;
	case Texture::IL_DXT4:
		ret = "IL_DXT4";
		break;
	case Texture::IL_DXT5:
		ret = "IL_DXT5";
		break;
	case Texture::IL_DXT_NO_COMP:
		ret = "IL_DXT_NO_COMP";
		break;
	case Texture::IL_KEEP_DXTC_DATA:
		ret = "IL_KEEP_DXTC_DATA";
		break;
	case Texture::IL_DXTC_DATA_FORMAT:
		ret = "IL_DXTC_DATA_FORMAT";
		break;
	case Texture::IL_3DC:
		ret = "IL_3DC";
		break;
	case Texture::IL_RXGB:
		ret = "IL_RXGB";
		break;
	case Texture::IL_ATI1N:
		ret = "IL_ATI1N";
		break;
	case Texture::IL_DXT1A:
		ret = "IL_DXT1A";
		break;
	default:
		break;
	}
	return ret;
}

std::string Texture::GetFormatString() const
{
	const char* formats[] = {"bmp", "jpg", "png", "tga", "dds", "Unknown"};
	return formats[format];
}

void Texture::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("texture_name", GetName());
	data.AddUInt("UUID", GetUID());
}

bool Texture::Load(Data & data)
{
	bool ret = true;
	std::string library_path = data.GetString("library_path");

	Texture* text = App->texture_importer->LoadTextureFromLibrary(library_path);
	if (!text)
	{
		std::string assets_path = data.GetString("assets_path");
		if (App->file_system->FileExist(assets_path))
		{
			library_path = App->resources->CreateLibraryFile(Resource::TextureResource, assets_path);
			if (!library_path.empty())
			{
				Load(data);
			}
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		SetID(text->GetID());
		width = text->width;
		height = text->height;
		format = text->format;
		type = text->type;
		SetAssetsPath(data.GetString("assets_path"));
		SetLibraryPath(data.GetString("library_path"));
		SetName(data.GetString("texture_name"));
		SetUID(data.GetUInt("UUID"));
		SetCompression(text->compression);
	}

	return ret;
}

void Texture::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.AddString("Name", GetName());
	data.AddUInt("Width", width);
	data.AddUInt("Height", width);
	data.AddString("Texture_Type", GetTypeString());
	data.AddString("Format", GetFormatString());
	data.AddString("Compression", CompressionToString());
	
	data.SaveAsMeta(GetAssetsPath());
}

void Texture::LoadToMemory()
{
	if (texture_id == 0)
	{
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void Texture::UnloadFromMemory()
{
	DecreaseUsedCount();

	if (GetUsedCount() == 0)
	{
		glDeleteTextures(1, &texture_id);
		texture_id = 0;
	}
}

void Texture::RecreateTexture()
{
	UnloadFromMemory();
	LoadToMemory();
}
