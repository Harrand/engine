#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <memory>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "utility.hpp"

class FrameBuffer
{
public:
	FrameBuffer(unsigned int width = 256, unsigned int height = 256);
	FrameBuffer(const FrameBuffer& copy) = delete;
	FrameBuffer(FrameBuffer&& move) = delete;
	virtual ~FrameBuffer();
	FrameBuffer& operator=(const FrameBuffer& rhs) = delete;
	
	virtual void setRenderTarget() const;
	virtual void bind(unsigned int id) const;
protected:
	unsigned int width, height;
	GLuint framebuffer_handle, texture_handle;
private:
	GLuint depth_render_buffer_handle;
};

class DepthTexture: public FrameBuffer
{
	DepthTexture(unsigned int width = 1024, unsigned int height = 1024);
	DepthTexture(const DepthTexture& copy) = delete;
	DepthTexture(DepthTexture&& move) = delete;
	~DepthTexture() = default;
	DepthTexture& operator=(const DepthTexture& rhs) = delete;
};

class Texture
{
public:
	Texture(std::string filename = "../../../res/runtime/textures/undefined.jpg", bool gamma_corrected = true);
	Texture(TTF_Font* font, const std::string& text, SDL_Color foreground_colour);
	Texture(const Texture& copy);
	Texture(Texture&& move);
	~Texture();
	Texture& operator=(Texture&& rhs);
	
	virtual void bind(GLuint shader_program_handle, unsigned int id);
	const std::string& getFileName() const;
	int getWidth() const;
	int getHeight() const;
	enum class TextureType : unsigned int
	{
		TEXTURE,
		NORMAL_MAP,
		PARALLAX_MAP,
		DISPLACEMENT_MAP,
		TEXTURE_TYPES,
	};
	virtual TextureType getTextureType();
	template<class Tex>
	static Tex* getFromLink(const std::string& texture_link, const std::vector<std::unique_ptr<Tex>>& all_textures)
	{
		for(auto& texture : all_textures)
		{
			if(texture->getFileName() == texture_link)
				return texture.get();
		}
		return nullptr;
	}
protected:
	unsigned char* loadTexture();
	void deleteTexture(unsigned char* imgdata);
	std::string filename;
	GLuint texture_id;
	GLuint texture_handle;
	int width, height, components;
	bool gamma_corrected;
};

class NormalMap: public Texture
{
public:
	NormalMap(std::string filename = "../../../res/runtime/normalmaps/default_normalmap.jpg");
	NormalMap(const NormalMap& copy) = default;
	NormalMap(NormalMap&& move) = default;
	~NormalMap() = default;
	NormalMap& operator=(const NormalMap& rhs) = delete;
	
	virtual void bind(GLuint shader_program_handle, unsigned int id) override;
	TextureType getTextureType();
};

class ParallaxMap: public Texture
{
public:
	ParallaxMap(std::string filename = "../../../res/runtime/parallaxmaps/default_parallax.png");
	ParallaxMap(const ParallaxMap& copy) = default;
	ParallaxMap(ParallaxMap&& move) = default;
	~ParallaxMap() = default;
	ParallaxMap& operator=(const ParallaxMap& rhs) = delete;
	
	virtual void bind(GLuint shader_program_handle, unsigned int id) override;
	TextureType getTextureType();	
};

class DisplacementMap: public Texture
{
public:
	DisplacementMap(std::string filename = "../../../res/runtime/displacementmaps/default_displacement.png");
	DisplacementMap(const DisplacementMap& copy) = default;
	DisplacementMap(DisplacementMap&& move) = default;
	~DisplacementMap() = default;
	DisplacementMap& operator=(const DisplacementMap& rhs) = delete;
	
	virtual void bind(GLuint shader_program_handle, unsigned int id) override;
	TextureType getTextureType();
};

class CubeMap
{
public:
	CubeMap(std::string right_texture, std::string left_texture, std::string top_texture, std::string bottom_texture, std::string back_texture, std::string front_texture);
	CubeMap(std::string texture_directory = "./", std::string skybox_name = "skybox", std::string skybox_image_file_extension = ".png");
	CubeMap(const CubeMap& copy);
	CubeMap(CubeMap&& move);
	~CubeMap();
	CubeMap& operator=(const CubeMap& rhs) = delete;
	
	void bind(GLuint shader_program_handle, unsigned int id);
private:
	std::vector<unsigned char*> loadTextures();
	GLuint texture_handle, texture_id;
	const std::string right_texture, left_texture, top_texture, bottom_texture, back_texture, front_texture;
	static constexpr std::size_t number_of_textures = 6;
	int width[number_of_textures], height[number_of_textures], components[number_of_textures];
};

#endif