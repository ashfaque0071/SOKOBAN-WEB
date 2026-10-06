#include <math.h>

#include "raylib.h"
#include "audio.h"

static float ClampVolume(float v)
{
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

#define SFX_MAX_VOICES 4

typedef struct {
    const char *path;
    int   voices;
    float trim;
    float pitch;
    float jitter;
    float cooldown;
} SfxDef;

static const SfxDef SFX_DEFS[SFX_COUNT] = {

    [SFX_STEP]           = {"assets/audio/sfx/step.wav",             4, 0.85f, 1.00f, 0.06f, 0.00f},
    [SFX_PUSH]           = {"assets/audio/sfx/push.wav",             4, 1.00f, 1.00f, 0.05f, 0.00f},
    [SFX_GOAL]           = {"assets/audio/sfx/goal_sound.mp3",       3, 1.00f, 1.00f, 0.00f, 0.00f},
    [SFX_BLOCKED]        = {"assets/audio/sfx/blocked.wav",          2, 0.80f, 1.00f, 0.04f, 0.22f},
    [SFX_RESTART]        = {"assets/audio/sfx/restart_sound.mp3",    1, 1.00f, 1.00f, 0.00f, 0.00f},
    [SFX_LEVEL_COMPLETE] = {"assets/audio/sfx/completion_sound.mp3", 1, 1.00f, 1.00f, 0.00f, 0.00f},

    [SFX_UNDO]           = {"assets/audio/sfx/step.wav",             3, 0.75f, 1.33f, 0.05f, 0.00f},

    [SFX_UI_HOVER]       = {"assets/audio/sfx/ui_hover.wav",         2, 0.28f, 1.00f, 0.10f, 0.04f},
    [SFX_UI_CLICK]       = {"assets/audio/sfx/ui_click.wav",         3, 0.72f, 1.00f, 0.07f, 0.00f},
    [SFX_UI_DENIED]      = {"assets/audio/sfx/ui_denied.wav",        2, 0.85f, 1.00f, 0.02f, 0.10f},
    [SFX_STAR]           = {"assets/audio/sfx/star.wav",             3, 0.75f, 1.00f, 0.00f, 0.00f},
};

#define MUSIC_PATH "assets/audio/music/bgmus.mp3"

/* The browser build ships the menu theme as MP3: 22 seconds of 44.1kHz stereo
   PCM is 3.8MB of download for no audible gain. */
#if defined(PLATFORM_WEB)
#define MENU_MUSIC_PATH "assets/audio/music/finale.mp3"
#else
#define MENU_MUSIC_PATH "assets/audio/music/finale.wav"
#endif

typedef struct {
    Sound voice[SFX_MAX_VOICES];
    int   voiceCount;
    int   next;
    float cooldownLeft;
} SfxSlot;

static SfxSlot sfx[SFX_COUNT];
static Music   music;
static Music   menuMusic;
static bool    menuMusicActive = false;

static float musicVolume = 0.0f;
static float effectsVolume = 0.80f;
static bool  muted = false;

#define MUSIC_PAUSE_DUCK 0.35f
#define MUSIC_SOLVED_DUCK 0.0f
#define MUSIC_DUCK_RATE 6.0f

static float musicDuck = 1.0f;

static float musicMuffle = 0.0f;
static float musicMuffleLeft = 0.0f;
static float musicMuffleRight = 0.0f;

static void MusicRoomFilter(void *buffer, unsigned int frames)
{
    float *sample = (float *)buffer;
    if (musicMuffle <= 0.002f)
    {

        musicMuffleLeft = 0.0f;
        musicMuffleRight = 0.0f;
        return;
    }

    float cut = expf(-musicMuffle * 3.10f);
    for (unsigned int i = 0; i < frames; i++)
    {
        musicMuffleLeft += cut * (sample[i * 2] - musicMuffleLeft);
        musicMuffleRight += cut * (sample[i * 2 + 1] - musicMuffleRight);
        sample[i * 2] = musicMuffleLeft;
        sample[i * 2 + 1] = musicMuffleRight;
    }
}

static void LoadSfx(int id)
{
        const SfxDef *def = &SFX_DEFS[id];
        SfxSlot *slot = &sfx[id];

        slot->voice[0] = LoadSound(def->path);
        slot->voiceCount = 1;
        slot->next = 0;
        slot->cooldownLeft = 0.0f;

        if (slot->voice[0].frameCount == 0)
        {
            TraceLog(LOG_WARNING, "AUDIO: could not load %s", def->path);
            return;
        }

        int wanted = def->voices;
        if (wanted > SFX_MAX_VOICES) wanted = SFX_MAX_VOICES;
        for (int v = 1; v < wanted; v++)
        {

            slot->voice[v] = LoadSoundAlias(slot->voice[0]);
            slot->voiceCount++;
        }
}

void AudioLoadDeferred(void)
{
    for (int id = 0; id < SFX_COUNT; id++)
        if (id != SFX_UI_HOVER && id != SFX_UI_CLICK)
            LoadSfx(id);
    music = LoadMusicStream(MUSIC_PATH);
    if (music.frameCount == 0)
    {
        TraceLog(LOG_WARNING, "AUDIO: could not load %s", MUSIC_PATH);
    }
    else
    {
        music.looping = true;
        AttachAudioStreamProcessor(music.stream, MusicRoomFilter);
    }

}

void AudioInit(void)
{
    InitAudioDevice();
    LoadSfx(SFX_UI_HOVER);
    LoadSfx(SFX_UI_CLICK);
    menuMusic = LoadMusicStream(MENU_MUSIC_PATH);
    if (menuMusic.frameCount == 0)
    {
        TraceLog(LOG_WARNING, "AUDIO: could not load %s", MENU_MUSIC_PATH);
        menuMusicActive = false;
    }
    else
    {
        menuMusic.looping = true;
        menuMusicActive = true;
        SetMusicVolume(menuMusic, musicVolume);
        PlayMusicStream(menuMusic);
    }
#if !defined(PLATFORM_WEB)
    AudioLoadDeferred();
#endif
}

void AudioShutdown(void)
{
    if (music.frameCount > 0)
    {

        DetachAudioStreamProcessor(music.stream, MusicRoomFilter);
        UnloadMusicStream(music);
    }
    if (menuMusic.frameCount > 0)
    {
        StopMusicStream(menuMusic);
        UnloadMusicStream(menuMusic);
    }

    for (int id = 0; id < SFX_COUNT; id++)
    {
        SfxSlot *slot = &sfx[id];

        for (int v = 1; v < slot->voiceCount; v++)
            UnloadSoundAlias(slot->voice[v]);
        if (slot->voiceCount > 0)
            UnloadSound(slot->voice[0]);
        slot->voiceCount = 0;
    }

    CloseAudioDevice();
}

void AudioUpdate(float deltaTime, bool paused, bool solved)
{
    Music *activeMusic = menuMusicActive ? &menuMusic : &music;
    if (activeMusic->frameCount > 0)
        UpdateMusicStream(*activeMusic);

    if (menuMusicActive)
    {
        musicDuck = 1.0f;
        musicMuffle = 0.0f;
        SetMusicVolume(*activeMusic, muted ? 0.0f : musicVolume);
    }
    else
    {
        float duckTarget = 1.0f;
        if (solved)      duckTarget = MUSIC_SOLVED_DUCK;
        else if (paused) duckTarget = MUSIC_PAUSE_DUCK;

        if (solved)
        {
            // Give the level-complete cue exclusive playback immediately.
            musicDuck = duckTarget;
        }
        else
        {
            float ease = deltaTime * MUSIC_DUCK_RATE;
            if (ease > 1.0f) ease = 1.0f;
            musicDuck += (duckTarget - musicDuck) * ease;
        }

        musicMuffle = (1.0f - musicDuck) / (1.0f - MUSIC_PAUSE_DUCK);
        if (musicMuffle < 0.0f) musicMuffle = 0.0f;
        if (musicMuffle > 1.0f) musicMuffle = 1.0f;

        if (activeMusic->frameCount > 0)
            SetMusicVolume(*activeMusic, muted ? 0.0f
                                               : musicVolume * musicDuck);
    }

    for (int id = 0; id < SFX_COUNT; id++)
    {
        if (sfx[id].cooldownLeft > 0.0f)
            sfx[id].cooldownLeft -= deltaTime;
    }
}

static void PlayVoice(SfxId id, float pitch)
{
    if (id < 0 || id >= SFX_COUNT) return;

    const SfxDef *def = &SFX_DEFS[id];
    SfxSlot *slot = &sfx[id];
    if (slot->voiceCount == 0) return;
    if (slot->cooldownLeft > 0.0f) return;

    Sound voice = slot->voice[slot->next];
    slot->next = (slot->next + 1) % slot->voiceCount;

    SetSoundVolume(voice, effectsVolume * def->trim);

    float wander = (def->jitter > 0.0f)
        ? GetRandomValue(-1000, 1000) / 1000.0f * def->jitter : 0.0f;
    SetSoundPitch(voice, def->pitch * pitch * (1.0f + wander));

    PlaySound(voice);
    slot->cooldownLeft = def->cooldown;
}

void AudioPlay(SfxId id) { PlayVoice(id, 1.0f); }
void AudioPlayPitched(SfxId id, float pitch) { PlayVoice(id, pitch); }

void AudioStop(SfxId id)
{
    if (id < 0 || id >= SFX_COUNT) return;
    SfxSlot *slot = &sfx[id];
    for (int v = 0; v < slot->voiceCount; v++)
        StopSound(slot->voice[v]);
    slot->cooldownLeft = 0.0f;
}

void AudioSetMasterOn(bool on)
{
    SetMasterVolume(on ? 1.0f : 0.0f);
}

void AudioSetMusicVolume(float volume)
{
    musicVolume = ClampVolume(volume);
}

void AudioSetEffectsVolume(float volume)
{
    effectsVolume = ClampVolume(volume);
}

void AudioSetNonGameplayActive(bool active)
{
    if (active == menuMusicActive)
        return;

    // A completion cue must never carry over into menu or level-select music.
    if (active)
        AudioStop(SFX_LEVEL_COMPLETE);

    Music *oldMusic = menuMusicActive ? &menuMusic : &music;
    if (oldMusic->frameCount > 0)
        StopMusicStream(*oldMusic);

    menuMusicActive = active && menuMusic.frameCount > 0;
    Music *newMusic = menuMusicActive ? &menuMusic : &music;
    if (newMusic->frameCount > 0)
    {
        newMusic->looping = true;
        PlayMusicStream(*newMusic);
    }

    musicDuck = 1.0f;
    musicMuffle = 0.0f;
}

void AudioToggleMute(void) { muted = !muted; }
void AudioUnmute(void)     { muted = false; }
bool AudioIsMuted(void)    { return muted; }

bool AudioMusicIsSilent(void)
{
    return muted || musicVolume < 0.005f;
}

void AudioRestartMusic(void)
{
    if (menuMusicActive || music.frameCount == 0) return;
    StopMusicStream(music);
    PlayMusicStream(music);
}
