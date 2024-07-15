#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include "Mediafile.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>
#include <filesystem>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <ctime>
#include "Mediafile.h"
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>

namespace fs = std::filesystem;


struct MusicData {
    int currentTrack = 0;
    Mix_Music* music = nullptr;
    std::vector<std::string> songlist;
    int startTime;
    int pausedTime;
};

class Player {
public:
    Player();
    ~Player();
    void play(const std::string &filepath);
    void pause();
    void resume();
    void stop();
    void next();
    void previous();
    void RepeatOneSong();
    void RepeatAllSong();
    void FunctionCallback();
    void setSonglist(std::vector<std::string>& songlist);
    void setTrack(int track);
    void CalculateCurrentTime();
    void StartTimeThread();
    void StopTimeThread();
    int getcurrenttrack();
    std::atomic<bool> quitTimeThread;
    uint32_t getduration() ;
private: 

    void audioThread(const std::string& filePath);
    std::atomic<bool> isPlaying;
    std::atomic<bool> isPaused;
    bool stopflag = false;
    MusicData music_Data;
    bool repeatSingleSong = false;
    int duration = 0;
    std::condition_variable cv;
    std::mutex cv_m;
    std::thread timeThread;
  
};

#endif // PLAYER_H