#pragma once

#include <string>
#include "Resource.h"

class Data;

class Texture : public Resource
{
public:

	enum TextureType
	{
		bmp, jpg, png, tga, dds, UnknownType,
	};
	enum TextureFormat
	{
		ColorIndex, alpha, rgb, rgba, bgr, bgra, luminance, luminance_alpha, UnknownFormat
	};
	enum CompressionFormat
	{
		IL_DXTC_FORMAT, IL_DXT1, IL_DXT2, IL_DXT3, IL_DXT4, IL_DXT5, IL_DXT_NO_COMP, 
		IL_KEEP_DXTC_DATA, IL_DXTC_DATA_FORMAT, IL_3DC, IL_RXGB, IL_ATI1N, IL_DXT1A
	};
	enum TextureMode
	{
		clamp, tile
	};
	Texture();
	virtual ~Texture();

	void SetID(uint id);
	uint GetID() const;
	void SetWidth(uint width);
	uint GetWidth() const;
	void SetHeight(uint height);
	uint GetHeight() const;
	void SetSize(uint width, uint height);
	void GetSize(uint& width, uint& height) const;
	void SetTextureType(TextureType type);
	void SetImageData(byte* data);
	byte* GetImageData() const;
	TextureType GetTextureType() const;
	std::string GetTypeString() const;
	void SetFormat(TextureFormat format);
	TextureFormat GetFormat() const;
	void SetCompression(int compression_format);
	CompressionFormat GetCompression() const;
	std::string CompressionToString() const;

	std::string GetFormatString() const;

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	void RecreateTexture();

private:
	uint texture_id;
	uint width;
	uint height;
	TextureFormat format;
	TextureType type;
	CompressionFormat compression;
	TextureMode mode;
	byte* image_data;
};

