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

#define TOTAL_MODE 16
#define USB_TOTAL_MODE 3
class Controller
{
public:
    Controller();
    ~Controller();
    void handleInput(const char &input);
    void handleSetDirectory(const std::string &directory);
    static void MusicFinishedCallbackWrapper();
    void run();
    //void controlThreads(bool state);
    void continuousPlaybackTimeDisplay(Player& player, std::atomic<bool>& displayFlag);

    
private:
    std::vector<MediaFile> &parseTabtofiles();
    std::vector<std::string> &parseTabtofilepaths();
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
    static Player *playerptr;
    std::string cur_dir;

    void getInputFromSerial();
    void getInputFromCin();
    void executeTask();
    void processMessage(const std::string& message);
    void handleModeAndSongSelection();
    void handleTask(const std::string& task);
    std::string getTaskFromQueue();
    
    SerialPort sp;
    std::atomic<bool> serial_command_received;
    std::mutex flag_mutex;
    char serial_command;

    std::mutex mode_mutex;
    std::mutex numsong_mutex;
    std::mutex datafield_mutex;
    std::mutex cout_mutex;
    std::mutex playing_mutex;
    std::mutex condition_mutex;

    std::atomic<bool> is_playing = false;
    char ParseData(std::string &message);
    std::queue<std::string> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool running = true;

    char usb_mode = '1';
    uint8_t usb_num_mode = 0;
    char mode = '1';
    uint8_t num_mode = 0;
    int num_song = 0;
    int current_song = 0;

    
    std::condition_variable cv;
    bool exitFlag = false;
    bool isModePrinted = false;
    std::mutex printMutex;
   
    // std::mutex controlMutex;
    // std::condition_variable controlCondition;
    // bool running_thread = true;
    std::thread displayThread;
    std::atomic<bool> displayFlag;
    void startDisplayThread();
    void stopDisplayThread();
    std::vector<MediaFile> executing_lisfile;
    std::vector<std::string> executing_listfilepath;






};

#endif // CONTROLLER_H