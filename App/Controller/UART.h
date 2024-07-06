#ifndef UART_H
#define UART_H

#include <string>
#define BUFFER_SIZE 4
class SerialPort {
public:
    SerialPort(const char* port);
    ~SerialPort();
    bool configure();
    std::string receiveData() ;
    static std::string getOpenSDADevicePath();
    void sendData(const char* data);
    static void sendfirstdata(const char* data);
    //ham send o day 


    //ham phan tich ban tin
    //ham tao ban tin 
    //mcu cung vay 

private:
    int serial_port;
    std::string port_name;
};

#endif // UART_H
