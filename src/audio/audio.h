#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

typedef enum {
    SFX_STEP = 0,
    SFX_PUSH,
    SFX_GOAL,
    SFX_BLOCKED,
    SFX_RESTART,
    SFX_LEVEL_COMPLETE,
    SFX_UNDO,
    SFX_UI_HOVER,
    SFX_UI_CLICK,
    SFX_UI_DENIED,
    SFX_STAR,
    SFX_COUNT
} SfxId;

void AudioInit(void);

void AudioShutdown(void);

void AudioUpdate(float deltaTime, bool paused, bool solved);

void AudioPlay(SfxId id);

void AudioPlayPitched(SfxId id, float pitch);

void AudioStop(SfxId id);

void AudioSetMasterOn(bool on);
void AudioSetMusicVolume(float volume);
void AudioSetEffectsVolume(float volume);

void AudioSetNonGameplayActive(bool active);

void AudioToggleMute(void);
void AudioUnmute(void);
bool AudioIsMuted(void);

bool AudioMusicIsSilent(void);

void AudioRestartMusic(void);

#endif
