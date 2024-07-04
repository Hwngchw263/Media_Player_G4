#include "Controller.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Player *Controller::playerptr = nullptr;
Controller::Controller():sp(SerialPort::getOpenSDADevicePath().c_str()), serial_command_received(false){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            // Handle SDL initialization error
        }
    // Initialize SDL_Mixer and check for errors
    int mix_flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG;
    int initialized_flags = Mix_Init(mix_flags);
    if ((initialized_flags & mix_flags) != mix_flags)
        {
            std::cerr << "Mix_Init Error: " << Mix_GetError() << std::endl;
            // Handle SDL_Mixer initialization error
        }
    // Initialize SDL_Mixer audio settings
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
        {
            std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
            // Handle SDL_Mixer audio initialization error
        }
     if (!sp.configure()) {
        std::cerr << "Failed to configure serial port" << std::endl;
        std::exit(1);  // Exit if configuration fails
    }
}
Controller::~Controller(){
    // Quit SDL_Mixer
    Mix_CloseAudio();
    Mix_Quit();
    // Quit SDL
    SDL_Quit();
}
//------------------------------Hook for music------------------------------------
void Controller::MusicFinishedCallbackWrapper()
{
    playerptr->FunctionCallback();
}

void Controller::handleInput(const char& input) {
    playerptr = &player;
    Mix_HookMusicFinished(MusicFinishedCallbackWrapper);
    switch (input) {
        case '1':
            handlePlay();
            break;
        case '2':
            handlePause();
            break;
        case '3':
            handleResume();
            break;
        case '4':
            handleStop();
            break;
        case '5':
            handleNext();
            break;
       case '6':
            handlePrevious();
            break;           
        case '+':
            handleVolume('+');
            break;
        case '-':
            handleVolume('-');
            break;
        case 'q':
            handleExit();
            break;
        case 'F':
            handleSwitchTab(HOME);
            break;
        case 'R':
            handleSwitchTab(MUSIC);
            break;
        case 'D':
            handleSwitchTab(VIDEO);
            break;
        case 'x':
            handleEditMetadata();
            break;
        case 'n':
            handleNextPage();
            break;
        case 'l':
            handlePrevPage();
            break;
        case 'm':
            handleRemoveFile();
            break;
        case 'b':
            handleBack();
            break;
        default:
            std::cout << "Unknown command." << std::endl;
            //view.displayHelp();
            view.displayMetadata(parseTabtofiles());
            break;
    }
}
std::vector<MediaFile>& Controller::parseTabtofiles(){
    if(view.gettab() == HOME){
        return model.getMetadataofmedia();
    }
    if(view.gettab() == MUSIC){
        return model.getMetadataofmusic();
    }
    if(view.gettab() == VIDEO){
        return model.getMetadataofvideo();
    }
    // Thêm một giá trị trả về mặc định hoặc xử lý lỗi
    throw std::runtime_error("Invalid tab selected");
}
std::vector<std::string>& Controller::parseTabtofilepaths(){
    if(view.gettab() == HOME){
        return model.getmediafilepath();
    }
    if(view.gettab() == MUSIC){
        return model.getmusicfilepath();
    }
    if(view.gettab() == VIDEO){
        return model.getvideofilepath();
    }
    // Thêm một giá trị trả về mặc định hoặc xử lý lỗi
    throw std::runtime_error("Invalid tab selected");
}
void Controller::handleSetDirectory(const std::string& directory) {
    cur_dir = directory;
    if (model.setDirectory(directory)) {
        view.settab(HOME);
        tabHistory.push(HOME);
        view.setpage(0);
        view.displayMetadata(parseTabtofiles());
    } else { 
        std::cout << "Invalid directory. Please try again." << std::endl;
    }
}

void Controller::handlePlay() {
    std::cout << "Enter song to play:" ;
    int num;
    std::cin >> num; 
    player.setTrack((num - 1));
    player.setSonglist(parseTabtofilepaths());
    player.setMediafile(parseTabtofiles());
    player.play(parseTabtofilepaths()[num - 1]);
}

void Controller::handlePause() {
    player.pause();
}

void Controller::handleResume() {
    player.resume();
}

void Controller::handleStop() {
    player.stop();
}
void Controller::handleNext(){
    player.next();
}
void Controller::handlePrevious(){
    player.previous();
}
void Controller::handleVolume(char volume) {
    player.setVolume(volume);
}
void Controller::handleExit() {
    exit(0);
}

void Controller::handleSwitchTab(const Tab tab) {
    if(tab != view.gettab()){
        tabHistory.push(tab);
        if (tab == HOME) {
            view.settab(HOME);
        }
        else if (tab == MUSIC) {
            view.settab(MUSIC);
        }
        else if (tab == VIDEO) {
            view.settab(VIDEO);
        } else {
            view.settab(HOME);
        }
        view.setpage(0);
    }
    view.displayMetadata(parseTabtofiles());
}

void Controller::handleEditMetadata() {
    std::cout << "Choose file to edit: " ;
    int num;
    bool edit = false;
    std::cin >> num;
    MediaFile file;
    file.setPath(parseTabtofiles()[num - 1].getPath());
    file.setType(parseTabtofiles()[num - 1].getType());
    if (!parseTabtofiles().empty())
    {
        if(parseTabtofiles()[num - 1].getType() == ".mp3"){
            //Edit file mp3
            std::string title, artist, album, genre;
            int year;
            char choice;
            while (!edit) {
                std::cout << "Select the metadata field to edit:\n";
                std::cout << "1. Title\n";
                std::cout << "2. Artist\n";
                std::cout << "3. Album\n";
                std::cout << "4. Genre\n";
                std::cout << "5. Year\n";
                std::cout << "6. Done\n";
                std::cout << "Enter choice: ";
                std::cin >> choice;
                switch (choice) {
                    case '1':
                        std::cout << "Enter new title (current: " << parseTabtofiles()[num - 1].getTitle() << "): ";
                        std::getline(std::cin >> std::ws, title);
                        if (!title.empty()) file.setTitle(title);
                        break;
                    case '2':
                        std::cout << "Enter new artist (current: " << parseTabtofiles()[num - 1].getArtist() << "): ";
                        std::getline(std::cin >> std::ws, artist);
                        if (!artist.empty())
                            file.setArtist(artist);
                        break;
                    case '3':
                        std::cout << "Enter new album (current: " << parseTabtofiles()[num - 1].getAlbum() << "): ";
                        std::getline(std::cin >> std::ws, album);
                        if (!album.empty())
                            file.setAlbum(album);
                        break;
                    case '4':
                        std::cout << "Enter new genre (current: " << parseTabtofiles()[num - 1].getGenre() << "): ";
                        std::getline(std::cin >> std::ws, genre);
                        if (!genre.empty())
                            file.setGenre(genre);
                        break;
                    case '5':
                        std::cout << "Enter new year (current: " << parseTabtofiles()[num - 1].getYear() << "): ";
                        std::cin >> year;
                        if (!std::cin.fail()) // Basic validation
                            file.setYear(year);
                        std::cin.clear(); // Clear any error flags
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                        break;
                    case '6':
                        edit = true;
                        //model.savenewMetadata(parseTabtofiles()[num - 1]);
                        break; // Exit edit
                    default:
                        std::cout << "Invalid choice. Please select again." << std::endl;
                        break;
                    }
                }
            }
        else if (parseTabtofiles()[num - 1].getType() == ".mp4")
        {
            //Edit file mp4
            std::string name;
            std::cout << "Enter new name (current: " << parseTabtofiles()[num - 1].getName() << "): ";
            std::getline(std::cin >> std::ws, name);
            if (!name.empty()){
                file.setName(name);
            }

        }
        model.savenewMetadata(file);
        //get path after change
        model.setDirectory(cur_dir);
    }
    view.displayMetadata(parseTabtofiles());
}


void Controller::handleNextPage() {
    int page = view.getpage();
    page++;
    if(page < view.gettotalpage(parseTabtofiles())){
        view.setpage(page);
    }
    view.displayMetadata(parseTabtofiles());

}

void Controller::handlePrevPage() {
    int currentPage = view.getpage();
    if (view.getpage() > 0) {
        currentPage--;
        view.setpage(currentPage);
    }
    view.displayMetadata(parseTabtofiles());
}

void Controller::handleRemoveFile() {
    std::cout << "Choose file to remove: " << std::endl;
    int num;
    std::cin >> num; 
    //Check valid of num
    model.removeFile(parseTabtofilepaths(), num);
    view.displayMetadata(parseTabtofiles());
}

void Controller::handleBack() {
    if (tabHistory.size() > 1) {
        tabHistory.pop();
        view.settab(tabHistory.top());
    }
    view.displayMetadata(parseTabtofiles());
}




