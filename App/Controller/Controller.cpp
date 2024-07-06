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
char Controller::ParseData(std::string& message){
            serial_command = '\0';
            //std::lock_guard<std::mutex> lock(command_mutex);
            // switch (message)
            // {
            // case "1":
            // case "2":
            // case "3":
            // case "4":
            // case "5":
            // case "6":
            // case "+":
            // case "-":
            //    serial_command = message[0];
            //     break;
            // default:
            //     break;
            // }
            if (message == "2"||message =="1" || message == "3" || message == "4"||message == "5"||message == "6"||message == "+"|| message == "-") {
                serial_command = message[0];           
            } 
            return serial_command;          
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
void Controller::run(){
    // while (true) {
    //     std::cout << "Enter command: ";
    //     char cmd;
    //     std::cin >> cmd;
    //     handleInput(cmd);
    // }
    //sp.sendData("abcd");
    // Thread for handling serial input
   // std::thread serial_thread([&]() {
    //     while (true) {   
    //             std::string message = sp.receiveData() ;
    //         //     if (!message.empty()) {
    //         //     std::lock_guard<std::mutex> lock(command_mutex);
    //         //     if (message == "2"  || message == "3" || message == "4"||message == "5"||message == "6"||message == "+"|| message == "-") {
    //         //         serial_command = message[0];
    //         //         serial_command_received = true;
    //         //         handleInput(serial_command);
    //         //         //handleHomeTabCommand(serial_command,page_num); // Handle HomeTab specific commands
    //         //         serial_command_received = false;
    //         //     } else {
    //         //         std::cout << "Received message: " << message << std::endl;
    //         //     }
    //         // }
    //         if (!message.empty()) {
    //         //serial_command_received = true;
    //         //serial_command = ParseData(message);
    //         handleInput(ParseData(message)); 
    //         //std::cout << "Received message: " << message << std::endl; 
    //         //serial_command_received = false;
    //         }   
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));   
    //     }
    // });
    // serial_thread.join();
      //Send data ro reset in MCU
    sp.sendData("abcd");
   
    std::thread input_thread([&]() {
        Cin_thread = true;
        MCU_thread = false;
        while (true) {
             char cmd;
                std::cout << "Enter command: ";
            //EnterCharInput(command);
                std::cin >> cmd;
            {
                std::lock_guard<std::mutex> lock(command_mutex);
                std::cout<<"\nben cin \n";
                if (!serial_command_received) {
                    handleInput(cmd);
                    //handleHomeTabCommand(command,page_num); // Handle HomeTab specific commands
                }
            }
            
            
        }
    });

    // Thread for handling serial input
    std::thread serial_thread([&]() {
        Cin_thread = false;
        MCU_thread = true;
        while (true) {   
            //Receive data from UART
            std::string message = sp.receiveData();
            if (!message.empty()) {
            std::lock_guard<std::mutex> lock(command_mutex);
            std::cout<<"\nben mcu\n";
            serial_command_received = true;
            handleInput(ParseData(message));
            
            }   

            serial_command_received = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));   
        }
    });
    serial_thread.join();
    input_thread.join();
    
    //Second thread Cin
}
void Controller::handleInput(const char& input) {
    playerptr = &player;
    Mix_HookMusicFinished(MusicFinishedCallbackWrapper);
    switch (input) {
        case '1':
            std::cout<<"\nplay mode\n";
            //is_playing = true;
            handlePlay();
            //is_playing = false;
            break;
        case '2':
            std::cout<<"\npause music\n";
            handlePause();
            break;
        case '3':
            std::cout<<"\nresume music\n";
            handleResume();
            break;
        case '4':
            std::cout<<"\nstop music\n";
            handleStop();
            break;
        case '5':
            std::cout<<"\nnext music\n";
            handleNext();
            break;
       case '6':
            std::cout<<"\nprevious music\n";
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
    if(MCU_thread == true){
        //Receive data from UART
        std::string data;
        std::cout << "MCU_thread." << std::endl;
        while(data[0] == '\0'){
           data = sp.receiveData();
        }
        std::cout << "This:" << data << std::endl;
        num = ParseData(data) - '0'; //edit
        std::cout << num << std::endl;
    }
    else if(Cin_thread == true){
        std::cin >> num; 
    }
    // std::cout << "Enter song to play:" ;
    // int num;
    // std::cin >> num; 
    
    player.setTrack((num - 1));
    player.setSonglist(parseTabtofilepaths());
    player.setMediafile(parseTabtofiles());
    player.play(parseTabtofilepaths()[num - 1]);
    // std::cout << "Enter song to play (or wait for serial command): ";
    
    // int num = -1;
    // bool valid_input = false;
    // auto start = std::chrono::steady_clock::now();

    // while (!valid_input) {
    //     // Check if there's input from std::cin
    //     if (std::cin.rdbuf()->in_avail() > 0) {
    //         std::cin >> num;
    //         valid_input = true;
    //     }
    //     std::string message = sp.receiveData() ;
    //     // Check if there's input from serial_command
    //     if (!message.empty()) {
    //         serial_command = ParseData(message);
    //         try {
    //             num = std::stoi(serial_command);
    //             valid_input = true;
    //         } catch (const std::invalid_argument& e) {
    //             std::cerr << "Invalid serial command input." << std::endl;
    //         }
    //     }

    //     // Timeout after 5 seconds
    //     auto now = std::chrono::steady_clock::now();
    //     if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > 5) {
    //         std::cout << "No input received within 5 seconds." << std::endl;
    //         break;
    //     }
        
    //     // Sleep to avoid busy-waiting
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }

    // if (num > 0) {
    //     player.setTrack(num - 1);
    //     player.setSonglist(parseTabtofilepaths());
    //     player.setMediafile(parseTabtofiles());
    //     player.play(parseTabtofilepaths()[num - 1]);
    // } else {
    //     std::cout << "Invalid song number." << std::endl;
    // }
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




