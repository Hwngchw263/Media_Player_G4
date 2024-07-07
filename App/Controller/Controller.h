#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "UART.h"
#include "Model.h"
#include "View.h"
#include "Player.h"
#include "Message.h"
#include <stack>
#include <string>
#include <condition_variable>
#include <queue>

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
    DATAMCU mcu_data;
    std::stack<Tab> tabHistory;
    static Player* playerptr;
    std::string cur_dir;
    void getInputFromSerial();
    void getInputFromCin();
    void executeTask();
    SerialPort sp;
    std::atomic<bool> serial_command_received;
    std::mutex flag_mutex;
    char serial_command;
    std::mutex mode_mutex;
    std::mutex datafield_mutex;
    std::atomic<bool> is_playing;
    char ParseData(std::string& message);
    std::queue<std::string> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool running = true;
    char mode = '1';
};

#endif // CONTROLLER_H