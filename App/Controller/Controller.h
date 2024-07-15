#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "UART.h"
#include "Model.h"
#include "View.h"
#include "Player.h"
#include "Message.h"
#include "USB.h"
#include <stack>
#include <string>
#include <condition_variable>
#include <queue>
#include <cctype>
#include <algorithm>
#define TOTAL_MODE 19
#define USB_TOTAL_MODE 3



class Controller
{
public:
    Controller();
    ~Controller();
    void handleActInput(int input);
    void handleUSBInput(int input);
    void handleSetDirectory(const std::string &dir);
    static void MusicFinishedCallbackWrapper();
    void run();

private:
    //Check input
    bool isValidNumber(const std::string& str);
    void handleFolder();
    void handleUSB();
    void handleExitUSBmenu();
    std::vector<MediaFile> &parseTabtofiles();
    std::vector<std::string> &parseTabtofilepaths();
    void handlePlay();
    void handlePause();
    void handleResume();
    void handleStop();
    void handleNext();
    void handlePrevious();
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
    USB  usb_data;
    SerialPort sp;

    std::stack<Tab> tabHistory;
    static Player *playerptr;
    std::string current_Dir;

    void getInputFromSerial();
    void getInputFromCin();
    void executeTask();
    void display();
    void processMessage(const std::string& message);
    void handleSelection();
    void handleTask(const std::string& task);
    std::string getTaskFromQueue();
    
    Flag current_flag = USB_MODE;
    char ParseData(std::string &message);
    std::queue<std::string> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;

    std::vector<std::string> usbpaths;
    int total_path = 0;
    int current_usb_mode = 0;
    int current_usb_path = 0;
    int current_act_mode = 0;
    int current_song = 1;
    int volume = 1;
   
    std::vector<MediaFile> executing_lisfile;
    std::vector<std::string> executing_listfilepath;

};

#endif // CONTROLLER_H