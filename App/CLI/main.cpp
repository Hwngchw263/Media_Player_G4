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

 
    controller.run();
    // Controller controller;
    // std::string directory;

    // std::cout << "Enter the directory to scan for media files: ";
    // std::getline(std::cin, directory);
    // int m = 300000;
    // // controller.handleSetDirectory(directory);

    // //controller.handleInput('h');  // Show help initially
    // SerialPort a(SerialPort::getOpenSDADevicePath().c_str());
    // SerialPort c(SerialPort::getOpenSDADevicePath().c_str());
    // if (!a.configure()) {
    //     std::cerr << "Failed to configure serial port" << std::endl;
    //     //std::exit(1);  // Exit if configuration fails
    //     return 1;
    // }
    // if (!c.configure()) {
    //     std::cerr << "Failed to configure serial port" << std::endl;
    //     //std::exit(1);  // Exit if configuration fails
    //     return 1;
    // }
    // std::cout<<"\ngui data \n";
    // const char *data_to_send = "ABCD";
    // a.sendData(data_to_send);
    // std::cout<<"\nnhan data\n";

    // // std::string b= a.receiveData();
    // // std::cout<< "\ngia tri gui tu mcu len la : "<< b<<"some"<<std::endl;
    // while (true) {
    //     std::string message = c.receiveData();
    //     if (!message.empty()) {
    //         std::cout << "Received message: " << message << std::endl;
    //     }

    //     // Sleep for a short period to avoid busy waiting
    //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // }
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // a.sendData("hellonhom3");
    // std::string b = a.receiveData();
    // std::cout<< "\ngia tri gui tu mcu len la : "<< b<<"some"<<std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // a.sendData("i love you");
    // std::string c = a.receiveData();
    // std::cout<< "\ngia tri gui tu mcu len la : "<< c<<"some"<<std::endl;
    // std::string input;
    // while (true) {
    //     std::cout << "Enter command: ";
    //     char cmd;
    //     std::cin >> cmd;
    //     controller.handleInput(cmd);
    // }
    // char cmd;
    // std::thread input_thread([&]() {
    //     while (true) {
            
    //             std::cout << "Enter command: ";
    //         //EnterCharInput(command);
    //             std::cin >> cmd;
    //         {
    //             std::lock_guard<std::mutex> lock(controller.command_mutex);
    //             if (!controller.serial_command_received) {
    //                 controller.handleInput(cmd);
    //                 //handleHomeTabCommand(command,page_num); // Handle HomeTab specific commands
    //             }
    //         }
            
            
    //     }
    // });

    // // Thread for handling serial input
    // std::thread serial_thread([&]() {
    //     while (true) {
            
    //             std::string message = controller.sp.receiveData() ;
    //             if (!message.empty()) {
    //             std::lock_guard<std::mutex> lock(controller.command_mutex);
    //             if (message == "2"  || message == "3" || message == "4"||message == "5"||message == "6"||message == "+"|| message == "-") {
    //                 controller.serial_command = message[0];
    //                 controller.serial_command_received = true;
    //                 controller.handleInput(controller.serial_command);
    //                 //handleHomeTabCommand(serial_command,page_num); // Handle HomeTab specific commands
    //                 controller.serial_command_received = false;
    //             } else {
    //                 std::cout << "Received message: " << message << std::endl;
    //             }
    //         }
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            
            
    //     }
    // });

    // input_thread.join();
    // serial_thread.join();

    return 0;
}
