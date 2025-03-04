#pragma once
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"
#include <memory>
#include <unordered_map>

class Material : public Resource {
public:

	struct Properties {
		glm::vec3 ambient{ 1.0f };
		glm::vec3 diffuse{ 1.0f };
		glm::vec3 specular{ 1.0f };
		float shininess{ 32.0f };
	};

	Material();
	explicit Material(std::shared_ptr<Shader> shader);
	~Material();

	void SetShader(std::shared_ptr<Shader> shader);
	void SetTextures(Texture::TextureType type, std::vector<std::shared_ptr<Texture>> textures);
	void AddTexture(Texture::TextureType type, std::shared_ptr<Texture> texture);
	void SetProperties(const Properties &props);

	void Bind() const;
	void Unbind() const;

	std::shared_ptr<Shader> GetShader() const { return m_shader; }
	std::vector<std::shared_ptr<Texture>> GetTextures(Texture::TextureType type) const;
	const Properties &GetProperties() const { return m_properties; }

private:
	std::shared_ptr<Shader> m_shader;
	std::unordered_map<Texture::TextureType, std::vector<std::shared_ptr<Texture>>> m_textures;
	Properties m_properties;
};