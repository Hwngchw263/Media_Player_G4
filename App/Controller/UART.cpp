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

    tty.c_cc[VTIME] = 1;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        std::cerr << "Error " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

std::string SerialPort::readData() {
    char read_buf[256];
    memset(&read_buf, '\0', sizeof(read_buf));
    int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
    
    if (num_bytes < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Error reading: " << strerror(errno) << std::endl;
        }
        return "";
    } else if (num_bytes > 0) {
        return std::string(read_buf, num_bytes);
    }

    return "";
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
