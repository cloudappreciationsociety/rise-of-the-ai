#include "audio.h"

Audio& Audio::get() {
    static Audio instance;
    return instance;
}

void Audio::shutdown() {
    for (auto& pair : mMusic) {
        UnloadMusicStream(pair.second);
    }

    for (auto& pair : mSoundPools) {
        for (Sound& alias : pair.second) {
            UnloadSoundAlias(alias);
        }
    }

    for (auto& pair : mSounds) {
        UnloadSound(pair.second);
    }

    CloseAudioDevice();
}

void Audio::loadMusic(const std::string& alias, const char* path, float volume) {
    mMusic.emplace(alias, LoadMusicStream(path));
    SetMusicVolume(mMusic[alias], volume);
}

void Audio::loadSound(const std::string& alias, const char* path) {
    mSounds.emplace(alias, LoadSound(path));

    for (size_t i = 0; i < Audio::SOUND_POOL_SIZE; ++i) {
        mSoundPools[alias].push_back(LoadSoundAlias(mSounds[alias]));
    }
}

void Audio::playMusic(const std::string& alias) {
    if (mActiveMusic == alias) {
        return;
    }

    this->stopActiveMusic();

    mActiveMusic = alias;
    PlayMusicStream(mMusic[mActiveMusic]);
}

void Audio::playSound(const std::string& alias) {
    for (size_t i = 0; i < mSoundPools[alias].size(); ++i) {
        if (!IsSoundPlaying(mSoundPools[alias][i])) {
            PlaySound(mSoundPools[alias][i]);
            break;
        }
    }
}

void Audio::stopActiveMusic() {
    if (mActiveMusic.empty()) {
        return;
    }

    StopMusicStream(mMusic[mActiveMusic]);
    mActiveMusic.clear();
}

void Audio::update() {
    if (!mActiveMusic.empty()) {
        UpdateMusicStream(mMusic[mActiveMusic]);
    }
}