#include "Controller.h"
#include <iostream>
#include <string>

int main() {
    Controller controller;
    std::string directory;

    std::cout << "Enter the directory to scan for media files: ";
    std::getline(std::cin, directory);

    controller.handleSetDirectory(directory);

    //controller.handleInput('h');  // Show help initially

    std::string input;
    while (true) {
        std::cout << "Enter command: ";
        char cmd;
        std::cin >> cmd;
        controller.handleInput(cmd);
    }

    return 0;
}
