#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <unordered_map>
#include <vector>

#include "raylib.h"

class Audio {
private:
    static constexpr size_t SOUND_POOL_SIZE = 15;

private:
    std::unordered_map<std::string, Music> mMusic;
    std::unordered_map<std::string, Sound> mSounds;
    std::unordered_map<std::string, std::vector<Sound>> mSoundPools;

    std::string mActiveMusic = "";

public:
    static Audio& get();

    void loadMusic(const std::string& alias, const char* path, float volume);
    void loadSound(const std::string& alias, const char* path);

    void playMusic(const std::string& alias);
    void playSound(const std::string& alias);

    void stopActiveMusic();

    void update();
    void shutdown();

private:
    Audio() = default;
    ~Audio() = default;
};

#endif