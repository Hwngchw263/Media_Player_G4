#include "USB.h"
#include <filesystem>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include "Controller.h"

namespace fs = std::filesystem;


    USB::USB() {
        username = getCurrentUsername();
    }

    std::string USB::getUsername() const {
        return username;
    }

    std::vector<std::string> USB::getMountedUSBNames() const {
        std::vector<std::string> usbNames;
        std::string basePath = "/media/" + username;
        for (const auto &entry : fs::directory_iterator(basePath)) {
            if (entry.is_directory()) {
                usbNames.push_back(entry.path().filename().string());
            }
        }
        return usbNames;
    }
    std::string USB::getUSBPath() const {
    std::vector<std::string> usbNames = getMountedUSBNames();
        if (!usbNames.empty()) {
        // Trả về đường dẫn đến USB đầu tiên trong danh sách
        return "/media/" + username + "/" + usbNames[0];
    } else {
        // Nếu không có USB nào được gắn kết, trả về một chuỗi rỗng
        return "";
    }
    }
    std::vector<std::string> USB::getUSBPaths() const {
    std::vector<std::string> usbPaths;
    std::string basePath = "/media/" + username;
    for (const auto &entry : fs::directory_iterator(basePath)) {
        if (entry.is_directory()) {
            usbPaths.push_back(entry.path().string());
        }
    }
    return usbPaths;
}

void USB :: displayMenu() {
    std::cout << "Menu:\n";
    std::cout << "[u] Play from USB\n";
    std::cout << "[f] Play from Folder\n";
    std::cout << "[e] Exit\n";
    std::cout << "Enter your choice: ";
}

void USB :: playFromUSB() {
    USB usb1;
    std::vector<std::string> usbpaths = usb1.getUSBPaths();

    if (usbpaths.size() == 0) {
        std::cout << "No USB devices found. Returning to main menu.\n";
        return;
    }

    std::cout << "Available USB devices:\n";
    for (size_t i = 0; i < usbpaths.size(); ++i) {
        std::cout << i + 1 << ". " << usbpaths[i] << '\n';
    }

    int choice;
    std::cout << "Enter the number of the USB to use: ";
    std::cin >> choice;

    if (choice > 0 && choice <= usbpaths.size()) {
        Controller controller;
        controller.handleSetDirectory(usbpaths[choice - 1]);
        controller.run();
    } else {
        std::cout << "Invalid choice. Returning to main menu.\n";
    }
}

void USB :: playFromFolder() {
    std::string baseDir;

    std::cout << "Enter path to folder: ";
    std::cin.ignore(); // To clear the newline character left by previous input
    std::getline(std::cin, baseDir);

    Controller controller1;
    controller1.handleSetDirectory(baseDir);
    controller1.run();
}

// private:
//     std::string username;

   
    std::string USB::getCurrentUsername() const {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return std::string(pw->pw_name);
    }
    return "";
}
//input 3 case 
void USB::handleInputUSB(const char &input){
    switch (input) {
            case '1':
            //handle form usb
            handleUSB();
               // playFromUSB();
                break;
            case '2':
            handleFolder();
                //playFromFolder();
                break;
            case '3':
                handleExitUSBmenu()
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
}
void USB::handleExitUSBmenu(){
    std::cout << "Exiting...\n";
    exit(0);
}

void USB::handleUSB(){
    playFromUSB();
}
void USB::handleFolder(){
    playFromFolder() 
}