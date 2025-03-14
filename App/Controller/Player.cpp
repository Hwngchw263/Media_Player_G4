#include "Player.h"
#include <iostream>
#include <SDL2/SDL.h>

Player::Player() : isPlaying(false), isPaused(false), volume(MIX_MAX_VOLUME/2) {
    SDL_Init(SDL_INIT_AUDIO);
}

Player::~Player() {
    // if (playThread.joinable()) {
    //     playThread.join();
    // }
    // Clean up resources
    if (music_Data.music != nullptr) {
        Mix_FreeMusic(music_Data.music);
        music_Data.music = nullptr;
    }
    Mix_CloseAudio();
    SDL_Quit();
}

void Player::RepeatOneSong() {
    std::cout << "Mode play all track: ON" << std::endl;
    repeatSingleSong = false;
}

void Player::RepaetAllSong() {
    std::cout << "Mode play single track: ON" << std::endl;
    repeatSingleSong = true;
}

void Player::setSonglist(std::vector<std::string>& songlist){
    this->music_Data.songlist = songlist;
}

void Player::play(const std::string &filepath) {
   if (isPlaying) {
        stop();
    }
    std::cout << mediafile[music_Data.currentTrack].getTitle() << std::endl;
    // playThread = std::thread([this, file]() {
    //     // Implementation to play the media file using SDL2
    //     std::cout << "Playing: " << file << std::endl;
    //     while (isPlaying) {
    //         if (isPaused) {
    //             SDL_Delay(100);
    //             continue;
    //         }
    //         // Play the media file
    //         SDL_Delay(1000); // Simulating playing
    //     }
    //     std::cout << "Stopped playing: " << file << std::endl;
    // });

    // Free current music
    if (music_Data.music != nullptr) {
        Mix_FreeMusic(music_Data.music);
        music_Data.music = nullptr;
    }
    // Load new music
    music_Data.music = Mix_LoadMUS(filepath.c_str());
    if (music_Data.music == nullptr) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    }
    // Start playing music
    if (Mix_PlayMusic(music_Data.music, 1) == -1) {
        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
    }
    isPlaying = true;
    isPaused = false;
    stopflag = false;
    // Set start time
    music_Data.startTime = SDL_GetTicks();
}

void Player::pause() {
    stopflag = false;
    if (isPlaying) {
        Mix_PauseMusic();
        music_Data.pausedTime = SDL_GetTicks() - music_Data.startTime;
        isPaused = true;
    }

}
 void Player::setMediafile(std::vector<MediaFile>& file){
    this->mediafile = file;
 }
void Player::resume() {
    stopflag = false;
    if (isPlaying && isPaused) {
        Mix_ResumeMusic();
        music_Data.startTime = SDL_GetTicks() - music_Data.pausedTime;
        isPaused = false;
    }
}

void Player::stop() {
    // if (isPlaying) {
    //     isPlaying = false;
    //     if (playThread.joinable()) {
    //         playThread.join();
    //     }
    //     std::cout << "Stopped" << std::endl;
    // }
    // Stop if anyway
    if (Mix_PlayingMusic()) {
        stopflag = true;
        Mix_HaltMusic();
    }
}
void Player::next(){
    isPaused = false;  
    music_Data.currentTrack = (music_Data.currentTrack + 1) % (this->music_Data.songlist.size());
    play(this->music_Data.songlist[music_Data.currentTrack]);
    //stopflag = false;
}

void Player::previous(){
    isPaused = false;
    music_Data.currentTrack = (music_Data.currentTrack - 1 + this->music_Data.songlist.size()) % this->music_Data.songlist.size();
    play(this->music_Data.songlist[music_Data.currentTrack]);
    //stopflag = false;
}

void Player::setVolume(char volume) {
    // this->volume = volume;
    // std::cout << "Volume set to " << volume << std::endl;
    if(volume == '+'){
        this->volume = std::min(MIX_MAX_VOLUME, this->volume + Volume_Step);
        std::cout << "Volume up" << std::endl;
    }
    else if(volume == '-'){
        this->volume = std::min(MIX_MAX_VOLUME, this->volume - Volume_Step);
        std::cout << "Volume down" << std::endl;
    }
    Mix_VolumeMusic(this->volume);
} 


void Player::setTrack(int track){
        this->music_Data.currentTrack = track;
    }
bool Player :: init()

{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}
void Player::FunctionCallback() {
    if(!stopflag){
        if (repeatSingleSong) {
            play(this->music_Data.songlist[music_Data.currentTrack]);
        } else {
            next();
        }
    }
}


