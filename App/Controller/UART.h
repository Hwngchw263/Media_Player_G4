#ifndef UART_H
#define UART_H

#include <string>

class SerialPort {
public:
    SerialPort(const char* port);
    ~SerialPort();
    bool configure();
    std::string readData();
    std::string getOpenSDADevicePath();

private:
    int serial_port;
    std::string port_name;
};

#endif // UART_H
