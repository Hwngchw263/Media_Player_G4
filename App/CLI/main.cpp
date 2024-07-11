#include "Controller.h"
#include <iostream>
#include <string>

int main() {
    Controller controller;
    std::string directory;
    std::cout << "Enter the directory to scan for media files: ";
    std::getline(std::cin, directory);
    controller.handleSetDirectory(directory);
    controller.run();

    return 0;
}