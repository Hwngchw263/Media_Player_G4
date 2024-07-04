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
    // while (true) {
    //     std::cout << "Enter command: ";
    //     char cmd;
    //     std::cin >> cmd;
    //     controller.handleInput(cmd);
    // }
    char cmd;
    std::thread input_thread([&]() {
        while (true) {
            std::cout << "Enter command: ";
            //EnterCharInput(command);
                std::cin >> cmd;
            {
                std::lock_guard<std::mutex> lock(controller.command_mutex);
                if (!controller.serial_command_received) {
                    controller.handleInput(cmd);
                    //handleHomeTabCommand(command,page_num); // Handle HomeTab specific commands
                }
            }
        }
    });

    // Thread for handling serial input
    std::thread serial_thread([&]() {
        while (true) {
            std::string message = controller.sp.readData();
            if (!message.empty()) {
                std::lock_guard<std::mutex> lock(controller.command_mutex);
                if (message == "2\n"  || message == "3\n" || message == "4\n"||message == "5\n"||message == "6\n"||message == "7\n") {
                    controller.serial_command = message[0];
                    controller.serial_command_received = true;
                    controller.handleInput(controller.serial_command);
                    //handleHomeTabCommand(serial_command,page_num); // Handle HomeTab specific commands
                    controller.serial_command_received = false;
                } else {
                    std::cout << "Received message: " << message << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    input_thread.join();
    serial_thread.join();

    return 0;
}
