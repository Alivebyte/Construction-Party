#include "ilogger.h"
#include "ifilesystem.h"
#include "texture2d.h"
#include "texturesmanager.h"

#include "gl_shared.h"

#include <stb_image.h>

static const char* g_texFileExtensions[] = { ".png", ".jpeg", ".jpg", ".tga", ".bmp" };
const int kTexFileExtensionsSize = sizeof(g_texFileExtensions) / sizeof(g_texFileExtensions[0]);

TexturesManager* g_pTexturesManager = nullptr;

static std::string getFileExtension(const std::string& filename)
{
	size_t whereIsDot = filename.find_last_of('.');
	if (whereIsDot != std::string::npos) {
		return filename.substr(whereIsDot);
	}

	return "";
}

static std::string getFileNameWithoutExtension(const std::string& filename)
{
	size_t lastindex = filename.find_last_of(".");
	if (lastindex != std::string::npos) {
		return filename.substr(0, lastindex);
	}

	return filename;
}

static std::string getFilenameWithoutPathAndExtension(const std::string& filename)
{
	size_t whereIsDot = filename.find_last_of('.');
	size_t whereIsSlash = filename.find_last_of('/');
	if (whereIsSlash == std::string::npos) {
		whereIsSlash = filename.find_last_of('\\');
	}

	if (whereIsDot == std::string::npos && whereIsSlash == std::string::npos) {
		return filename;
	}

	std::string string = filename.substr(whereIsSlash + 1);
	whereIsDot = string.find_last_of('.');
	string = string.substr(0, whereIsDot);

	return string;
}

TexturesManager::TexturesManager()
{
	m_notex = nullptr;
}

TexturesManager::~TexturesManager()
{

}

void TexturesManager::Init()
{
	stbi_set_flip_vertically_on_load(true);

	m_notex = LoadTexture2D("data/textures/notex.png", true);
	if (!m_notex) {
		GetLogger()->Print("TexturesManager::Init: Failed to initialize system texture! 'system/notex.png' is not exist.");
		SDL_assert(0);
	}
}

void TexturesManager::Shutdown()
{
	if (!m_textures.empty()) {
		GetLogger()->Print("--- unfreed textures ---");

		for (std::vector<Texture2D*>::iterator it = m_textures.begin(); it != m_textures.end(); ++it) {
			GetLogger()->Print("%s", (*it)->m_textureFileName.c_str());
			delete* it;
			*it = nullptr;
		}

		m_textures.clear();
	}
}

void TexturesManager::SetTexture(int slot, Texture2D* texture)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture ? texture->GetHandle() : 0);
}

Texture2D* TexturesManager::CreateManual2D(const char* name, uint32_t width, uint32_t height, PixelFormat format, bool useAsRenderTarget)
{
	for (std::vector<Texture2D*>::iterator it = m_textures.begin(); it != m_textures.end(); ++it) {
		if ((*it)->m_textureFileName == name) {
			GetLogger()->Print("TexturesManager::CreateManual2D: texture %s is already created!", name);
			return nullptr;
		}
	}

	// allocate
	Texture2D* texture = Texture2D::Create();
	texture->CreateRaw(nullptr, width, height, format);
	texture->m_textureFileName = name;

	if (useAsRenderTarget)
		GetLogger()->Print("Created rt texture [%s]", name);

	return texture;
}

bool IsSupportedExtension(const char* filename)
{
	std::string ext = getFileExtension(filename);

	for (int i = 0; i < kTexFileExtensionsSize; i++) {
		if (ext == g_texFileExtensions[i]) {
			return true;
		}
	}

	return false;
}

Texture2D* TexturesManager::LoadTexture2D(const char* texturename, bool useMipmaps /*= false*/)
{
	int texturesNbr = m_textures.size();
	for (int i = 0; i < texturesNbr; i++) {
		if (m_textures[i]->m_textureFileName == texturename)
			return m_textures[i];
	}

	if (strcmp(texturename, "$white$") == 0) {
		Texture2D* tex = Texture2D::Create();
		tex->m_textureFileName = "$white$";
		tex->CreateWhiteTexture(16, 16, 3);
		m_textures.push_back(tex);
		return tex;
	}
	if (strcmp(texturename, "$black$") == 0) {
		Texture2D* tex = Texture2D::Create();
		tex->m_textureFileName = "$black$";
		tex->CreateBlackTexture(16, 16, 3);
		m_textures.push_back(tex);
		return tex;
	}
	if (strcmp(texturename, "$gray$") == 0) {
		Texture2D* tex = Texture2D::Create();
		tex->m_textureFileName = "$gray$";
		tex->CreateGrayTexture(16, 16, 3);
		m_textures.push_back(tex);
		return tex;
	}

	if (strlen(texturename) <= 0) {
		return m_notex;
	}

	std::string texnamebuf;

	// find texture from disk
	for (int i = 0; i < kTexFileExtensionsSize; i++)
	{
		std::string textureFilename = getFileNameWithoutExtension(texturename);
		textureFilename += g_texFileExtensions[i];

		if (GetFileSystem()->IsExist(textureFilename.c_str()))
		{
			texnamebuf = textureFilename;
			break;
		}
	}

	if (!texnamebuf.empty()) {
		Texture2D* texture = Texture2D::Create();
		texture->CreateFromFile(texnamebuf.c_str());

		if (useMipmaps)
			texture->GenerateMipmaps();

		GetLogger()->Print("loaded %s", getFilenameWithoutPathAndExtension(texturename).c_str());
		m_textures.push_back(texture);
		return texture;
	}
	else if (texnamebuf.empty() && m_notex) {
		GetLogger()->Print("not found %s", getFilenameWithoutPathAndExtension(texturename).c_str());
		return m_notex;
	}

	return nullptr;
}
