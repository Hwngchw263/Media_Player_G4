#ifndef UART_H
#define UART_H

#include <string>
#define BUFFER_SIZE 8

class SerialPort
{
public:
    SerialPort(std::string port);
    ~SerialPort();
    bool configure();
    std::string receiveData();
    static std::string getOpenSDADevicePath();
    void sendData(const std::string &data);
    static void sendfirstdata(const char *data);

private:
    int serial_port;
    std::string port_name;
};

#endif // UART_H