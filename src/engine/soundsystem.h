#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include <miniaudio.h>

// WinAPI redifine
#ifdef PlaySound
#undef PlaySound
#endif

#define MAX_SOUNDS 64

typedef int SoundHandle_t;

class ENGINE_API SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	void Init();
	void Shutdown();

	SoundHandle_t LoadSound(const char* filename);

	void PlaySound(SoundHandle_t handle, bool forever);
	void StopSound(SoundHandle_t handle);

	bool IsPlaying(SoundHandle_t handle);

	void SetMasterVolume(float volume);

private:
	ma_sound m_sounds[MAX_SOUNDS];
	ma_engine m_engine;
	int m_numSounds;
	bool m_inited;
};

extern ENGINE_API SoundSystem g_SoundSystem;

#endif // !SOUNDSYSTEM_H
