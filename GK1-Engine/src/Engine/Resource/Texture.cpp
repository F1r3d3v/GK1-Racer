#include "Engine/Resource/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <unordered_map>

Texture::Texture() : m_textureID(0), m_width(0), m_height(0), m_channels(0), m_data(nullptr),
m_type(TextureType::Diffuse), m_format(GL_RGB), m_internalFormat(GL_RGB)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureID);
	if (m_data)
	{
		free(m_data);
	}
}

std::shared_ptr<Texture> Texture::LoadFromFile(const std::string &path, Texture::TextureType type, bool generateMipMaps)
{
	auto texture = std::make_shared<Texture>();
	texture->m_type = type;

	stbi_set_flip_vertically_on_load(true);
	if (type == TextureType::Height)
	{
		texture->m_data = (uint8_t*)stbi_load_16(path.c_str(), &texture->m_width, &texture->m_height, &texture->m_channels, 0);
	}
	else
	{
		texture->m_data = stbi_load(path.c_str(), &texture->m_width, &texture->m_height, &texture->m_channels, 0);
	}

	if (!texture->m_data)
	{
		std::cerr << "Failed to load texture: " << path << std::endl;
		return nullptr;
	}

	// unordered map for channel mapping
	std::unordered_map<int, int> channelMap = {
		{1, GL_RED},
		{3, GL_RGB},
		{4, GL_RGBA}
	};
	texture->m_format = channelMap[texture->m_channels];
	texture->m_internalFormat = texture->m_format;
	if (type == TextureType::Height)
	{
		texture->m_internalFormat = GL_R16;
	}

	glGenTextures(1, &texture->m_textureID);
	glBindTexture(GL_TEXTURE_2D, texture->m_textureID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, texture->m_internalFormat, texture->m_width,
				 texture->m_height, 0, texture->m_format, (type != TextureType::Height) ? GL_UNSIGNED_BYTE : GL_SHORT, texture->m_data);

	if (generateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (type != TextureType::Height) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (type != TextureType::Height) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

std::shared_ptr<Texture> Texture::LoadFromData(const uint8_t *data, int width, int height,
											   int channels, Texture::TextureType type, bool generateMipMaps)
{
	auto texture = std::make_shared<Texture>();

	texture->m_width = width;
	texture->m_height = height;
	texture->m_channels = channels;
	std::unordered_map<int, int> channelMap = {
		{1, GL_RED},
		{3, GL_RGB},
		{4, GL_RGBA}
	};
	texture->m_format = channelMap[channels];
	texture->m_internalFormat = texture->m_format;
	texture->m_type = type;
	texture->m_data = (uint8_t*)malloc(width * height * channels);

	glGenTextures(1, &texture->m_textureID);
	glBindTexture(GL_TEXTURE_2D, texture->m_textureID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, texture->m_internalFormat, width, height, 0,
				 texture->m_format, GL_UNSIGNED_BYTE, data);

	if (generateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

std::shared_ptr<Texture> Texture::CreateCubemap(const std::vector<std::string> &faces)
{
	auto texture = std::make_shared<Texture>();
	texture->m_type = TextureType::Cubemap;

	glGenTextures(1, &texture->m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_textureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		int width, height, channels;
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			std::cerr << "Failed to load cubemap texture: " << faces[i] << std::endl;
			continue;
		}

		GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height,
					 0, format, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	if (m_type == TextureType::Cubemap)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}
}

void Texture::Unbind() const
{
	if (m_type == TextureType::Cubemap)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

std::shared_ptr<Texture> Texture::GetDefaultTexture(bool normalmap)
{
	// Create textures on the fly
	static std::shared_ptr<Texture> defaultDiffuse = nullptr;
	static std::shared_ptr<Texture> defaultNormal = nullptr;

	if (!defaultDiffuse)
	{
		uint8_t white[] = { 255, 255, 255 };
		defaultDiffuse = LoadFromData(white, 1, 1, 3, TextureType::Diffuse, false);
	}

	if (!defaultNormal)
	{
		uint8_t normal[] = { 128, 128, 255 };
		defaultNormal = LoadFromData(normal, 1, 1, 3, TextureType::Normal, false);
	}

	return normalmap ? defaultNormal : defaultDiffuse;
}