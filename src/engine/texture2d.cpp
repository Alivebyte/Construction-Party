#include "ilogger.h"
#include "ifilesystem.h"
#include "texture2d.h"
#include "texturesmanager.h"
#include "gl_shared.h"

Texture2D* Texture2D::Create()
{
	return new Texture2D;
}

Texture2D::Texture2D()
{
	m_pf = PF_UNKNOWN;
	m_width = 0;
	m_height = 0;
	m_channels = 0;
	m_handle = -1;
}

Texture2D::~Texture2D()
{
	m_pf = PF_UNKNOWN;
	m_width = 0;
	m_height = 0;
	m_channels = 0;
	m_handle = -1;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// STB Image loading code
void Texture2D::CreateFromFile(const char* filename)
{
	int width, height, channels;
	m_textureFileName = filename;

	FileHandle_t file = GetFileSystem()->OpenFile(filename, "rb");

	size_t imageSize = GetFileSystem()->GetFileLength(file);

	uint8_t* fileData = (uint8_t*)malloc(imageSize);
	GetFileSystem()->ReadFile(file, fileData, imageSize);

	GetFileSystem()->CloseFile(file);

	uint8_t* imageData = stbi_load_from_memory(fileData, int(imageSize), &width, &height, &channels, 0);

	if (imageData == NULL) {
		free(fileData);
		GetLogger()->Print("Texture loading error: %s (%s)", filename, stbi_failure_reason());
		SDL_assert(imageData);
	}

	CreateFromExistedData(imageData, width, height, channels);
	m_textureFileName = filename;

	free(fileData);

	stbi_image_free(imageData);
}

void Texture2D::CreateBlackTexture(int width, int height, int channels)
{
	size_t textureSize = width * height * channels;
	uint8_t* data = new uint8_t[textureSize];
	assert(data);

	for (int i = 0; i < (int)textureSize; i++) {
		data[i] = 0;
	}

	CreateFromExistedData(data, width, height, channels);

	delete[] data;
}

void Texture2D::CreateWhiteTexture(int width, int height, int channels)
{
	size_t textureSize = width * height * channels;
	uint8_t* data = new uint8_t[textureSize];
	assert(data);

	for (int i = 0; i < (int)textureSize; i++) {
		data[i] = 255;
	}

	CreateFromExistedData(data, width, height, channels);

	delete[] data;
}

void Texture2D::CreateGrayTexture(int width, int height, int channels)
{
	size_t textureSize = width * height * channels;
	uint8_t* data = new uint8_t[textureSize];
	assert(data);

	for (int i = 0; i < (int)textureSize; i++) {
		data[i] = 255 / 2;
	}

	CreateFromExistedData(data, width, height, channels);

	delete[] data;
}

void Texture2D::CreateTexture_Generator(int width, int height, int channels, int color)
{
	size_t textureSize = width * height * channels;
	uint8_t* data = new uint8_t[textureSize];
	assert(data);

	m_textureFileName = "$generator_texture$";

	for (int i = 0; i < (int)textureSize; i++) {
		data[i] = color;
	}

	CreateFromExistedData(data, width, height, channels);

	delete[] data;
}

void Texture2D::CreateFromExistedData(void* data, int width, int height, int channels)
{
	//assert(data);

	m_width = width;
	m_height = height;
	m_channels = channels;

	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexImage2D(GL_TEXTURE_2D, 0, (channels == 3) ? GL_RGB : GL_RGBA, width, height, 0, (channels == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::CreateRaw(void* data, int width, int height, PixelFormat pf)
{
	m_width = width;
	m_height = height;
	m_channels =  (pf == PF_R8G8B8) ? 3 : 4;
	m_pf = pf;

	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalPF(pf), width, height, 0, GetGLInternalPF(pf), GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

void Texture2D::GenerateMipmaps()
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//if (g_texAnisoFilter.getValueB()) {
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, g_texAnisoLevel.getValueI());
	//}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture2D::setWrapS(TextureWrap wrap)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getGlWrap(wrap));
}

void Texture2D::setWrapT(TextureWrap wrap)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getGlWrap(wrap));
}

void Texture2D::setMin(TextureFilter filter)
{
	GLint param = 0;
	param = getGlTexFilter(filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
}

void Texture2D::setMag(TextureFilter filter)
{
	GLint param = 0;
	param = getGlTexFilter(filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
}
