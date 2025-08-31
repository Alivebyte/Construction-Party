#define ENGINE_EXPORTS
#define MINIAUDIO_IMPLEMENTATION
#include "soundsystem.h"
#include "ilogger.h"

#include <SDL.h>

ENGINE_API SoundSystem g_SoundSystem;

SoundSystem::SoundSystem() :
	m_numSounds(0),
	m_inited(true)
{
	memset(&m_engine, 0, sizeof(m_engine));
	memset(&m_sounds, 0, sizeof(m_sounds));
}

SoundSystem::~SoundSystem()
{
}

void SoundSystem::Init()
{
	ma_result result;

	result = ma_engine_init(NULL, &m_engine);
	if (result != MA_SUCCESS) {
		GetLogger()->Print("Failed to initialize the miniaudio engine.");
		return;  // Failed to initialize the engine.
	}

	m_inited = true;
}

void SoundSystem::Shutdown()
{
	if (!m_inited)
		return;

	for (int i = 0; i < m_numSounds; i++)
	{
		ma_sound_uninit(&m_sounds[i]);
	}

	memset(&m_sounds, 0, sizeof(m_sounds));

	ma_engine_uninit(&m_engine);
	memset(&m_engine, 0, sizeof(m_engine));
}

SoundHandle_t SoundSystem::LoadSound(const char* filename)
{
	if (!m_inited)
		return -1;

	ma_result result;
	
	SoundHandle_t handle = m_numSounds;

	result = ma_sound_init_from_file(&m_engine, filename, 0, NULL, NULL, &m_sounds[handle]);
	if (result != MA_SUCCESS) {
		GetLogger()->Print("Failed to load sound %s", filename);
		return -1;
	}

	m_numSounds++;
	return handle;
}

void SoundSystem::PlaySound(SoundHandle_t handle, bool forever)
{
	if (!m_inited)
		return;

	SDL_assert(handle <= -1 || handle <= m_numSounds);

	ma_sound_start(&m_sounds[handle]);
	
	if (forever)
		ma_sound_set_looping(&m_sounds[handle], true);
}

void SoundSystem::StopSound(SoundHandle_t handle)
{
	if (!m_inited)
		return;

	SDL_assert(handle <= -1 || handle <= m_numSounds);
	ma_sound_stop(&m_sounds[handle]);
}

bool SoundSystem::IsPlaying(SoundHandle_t handle)
{
	if (!m_inited)
		return false;

	SDL_assert(handle <= -1 || handle <= m_numSounds);

	return ma_sound_is_playing(&m_sounds[handle]);
}

void SoundSystem::SetMasterVolume(float volume)
{
	if (!m_inited)
		return;

	ma_engine_set_volume(&m_engine, volume);
}
