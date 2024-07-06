#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "UART.h"
#include "Model.h"
#include "View.h"
#include "Player.h"
#include <stack>
#include <string>


class Controller {
public:
    Controller();
    ~Controller();
    void handleInput(const char& input);
    void handleSetDirectory(const std::string& directory);
    static void MusicFinishedCallbackWrapper();
     
    void run();
private:
    std::vector<MediaFile>& parseTabtofiles();
    std::vector<std::string>& parseTabtofilepaths();
    void handlePlay();
    void handlePause();
    void handleResume();
    void handleStop();
    void handleNext();
    void handlePrevious();
    void handleVolume(char volume);
    void handleExit();
    void handleSwitchTab(const Tab tab);
    void handleEditMetadata();
    void handleNextPage();
    void handlePrevPage();
    void handleRemoveFile();
    void handleBack();
    Model model;
    View view;
    Player player;
    std::stack<Tab> tabHistory;
    static Player* playerptr;
    std::string cur_dir;
    
    SerialPort sp;
    std::atomic<bool> serial_command_received;
    char serial_command;
    std::mutex command_mutex;
    std::atomic<bool> is_playing;
    std::atomic<bool> Cin_thread ;
    std::atomic<bool> MCU_thread;
    char ParseData(std::string& message);
   
};

#endif // CONTROLLER_H
