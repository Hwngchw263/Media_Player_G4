#include "UART.h"
#include <iostream>
#include <fcntl.h>    // Contains file controls like O_RDWR
#include <errno.h>    // Error number definitions
#include <termios.h>  // POSIX terminal control definitions
#include <unistd.h>   // write(), read(), close()
#include <cstring>    // strerror
#include <regex>
#include <filesystem>
SerialPort::SerialPort(const char* port) {
    port_name = std::string(port);
    serial_port = open(port, O_RDWR | O_NONBLOCK);

    if (serial_port < 0) {
        std::cerr << "Error " << errno << " opening " << port_name << ": " << strerror(errno) << std::endl;
    }
}

SerialPort::~SerialPort() {
    close(serial_port);
}
// void SerialPort::sendfirstdata(const char* data){
//     write(serial_port, data, strlen(data));
// }
bool SerialPort::configure() {
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
       
        return false;
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(ICRNL | INLCR);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        std::cerr << "Error " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
        
        return false;
    }

    return true;
}
void SerialPort::sendData(const char* data) {
    write(serial_port, data, strlen(data));
}
std::string SerialPort::receiveData() {
      char buffer[BUFFER_SIZE +1]; // thêm 1 để chứa ký tự null
     int index = 0;

    // Đọc đúng 4 ký tự
    while (index < BUFFER_SIZE) {
        int bytes_read = read(serial_port, &buffer[index], 1);
        if (bytes_read < 0) {
            std::cerr << "Error reading from serial port: " << strerror(errno) << std::endl;
            break;
        } else if (bytes_read > 0) {
            index++;
        }
    }
    buffer[index] = '\0'; // Kết thúc chuỗi bằng ký tự null
    
    return std::string(buffer);
    //buffer[BUFFER_SIZE] = '\0'; // Null-terminate the string
    //std::cout << "Received: " << buffer << std::endl;
    
    // char read_buf[256];
    // memset(&read_buf, '\0', sizeof(read_buf));
    // int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

    // if (num_bytes < 0) {
    //     if (errno != EAGAIN && errno != EWOULDBLOCK) {
    //         std::cerr << "Error reading: " << strerror(errno) << std::endl;
    //     }
    //     return "";
    // } else if (num_bytes > 0) {
    //     return std::string(read_buf, num_bytes);
    // }

    // return "";
}

std::string  SerialPort::getOpenSDADevicePath(){
    std::string openSDADevicePath;
    std::string path = "/dev/";
    std::regex devicePattern("ttyACM[0-9]+");

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string deviceName = entry.path().filename().string();
        if (std::regex_match(deviceName, devicePattern)) {
            openSDADevicePath = entry.path().string();
            break;  // Exit loop after finding the first matching device
        }
    }

    return openSDADevicePath;
}
