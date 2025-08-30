#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <string>

#include "render_shared.h"
#include "irender.h"

class TexturesManager;

class Texture2D : public ITexture2D
{
	friend class TexturesManager;
public:
	static Texture2D* Create();

public:
	Texture2D();
	~Texture2D();

	void CreateBlackTexture(int width, int height, int channels);
	void CreateWhiteTexture(int width, int height, int channels);
	void CreateGrayTexture(int width, int height, int channels);
	void CreateTexture_Generator(int width, int height, int channels, int color);
	void CreateFromExistedData(void* data, int width, int height, int channels);
	void CreateFromFile(const char* filename);

	void CreateRaw(void* data, int width, int height, PixelFormat pf);

	void GenerateMipmaps();

	void Bind();

	void setWrapS(TextureWrap wrap);
	void setWrapT(TextureWrap wrap);
	void setMin(TextureFilter filter);
	void setMag(TextureFilter filter);

	uint32_t GetHandle() { return m_handle; }

private:
	std::string m_textureFileName;
	PixelFormat m_pf;
	int m_width;
	int m_height;
	int m_channels;
	uint32_t m_handle;
};

#endif // !TEXTURE2D_H
