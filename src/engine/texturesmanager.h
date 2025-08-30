#ifndef TEXTURESMANAGER_H
#define TEXTURESMANAGER_H

#include <stdint.h>
#include <vector>

#include "render_shared.h"

class Texture2D;

class TexturesManager
{
public:
	TexturesManager();
	~TexturesManager();

	void Init();
	void Shutdown();

	void SetTexture(int slot, Texture2D* texture);

	Texture2D* CreateManual2D(
		const char* name,
		uint32_t width,
		uint32_t height,
		PixelFormat format,
		void* pData = nullptr,
		bool useAsRenderTarget = false);

	Texture2D* LoadTexture2D(const char* texturename, bool useMipmaps = false);

private:
	std::vector<Texture2D*> m_textures;

	Texture2D* m_notex;
};

extern TexturesManager* g_pTexturesManager;

#endif // !TEXTURESMANAGER_H
