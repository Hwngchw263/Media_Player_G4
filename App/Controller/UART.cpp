#include "UART.h"
#include <iostream>
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()
#include <cstring>   // strerror
#include <regex>
#include <filesystem>

SerialPort::SerialPort(std::string port):serial_port(-1)
{
    port_name = port;
    // serial_port = open(port, O_RDWR | O_NONBLOCK);

    // if (serial_port < 0)
    // {
    //     std::cerr << "Error " << errno << " opening " << port_name << ": " << strerror(errno) << std::endl;
    // }
}

SerialPort::~SerialPort()
{
    //close(serial_port);
    if (serial_port >= 0)
        {
            close(serial_port);
        }
}
bool SerialPort::configure()
{
    serial_port = open(port_name.c_str(), O_RDWR | O_NONBLOCK);

    if (serial_port < 0)
    {
        std::cerr << "Error " << errno << " opening " << port_name << ": " << strerror(errno) << std::endl;
        return false;
    }
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0)
    {
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

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        std::cerr << "Error " << errno << " from tcsetattr: " << strerror(errno) << std::endl;

        return false;
    }

    return true;
}

// Function to write data to the serial port
void SerialPort::sendData(const std::string &data)
{
    write(serial_port, data.c_str(), data.length());
}

// Function to read data from the serial port
std::string SerialPort::receiveData()
{
    char read_buf[BUFFER_SIZE + 1];                           // Buffer to store received data
    int num_bytes = read(serial_port, read_buf, BUFFER_SIZE); // Read data

    if (num_bytes < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            std::cerr << "Error reading: " << strerror(errno) << std::endl;
        }
        return "";
    }
    read_buf[num_bytes] = '\0'; // Null terminate the string
    return std::string(read_buf);
}

std::string SerialPort::getOpenSDADevicePath()
{
    std::string openSDADevicePath;
    std::string path = "/dev/";
    std::regex devicePattern("ttyACM[0-9]+");

    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        std::string deviceName = entry.path().filename().string();
        if (std::regex_match(deviceName, devicePattern))
        {
            openSDADevicePath = entry.path().string();
            break; // Exit loop after finding the first matching device
        }
    }

    return openSDADevicePath;
}