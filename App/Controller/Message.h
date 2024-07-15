#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdint.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <string>
#define Hex_Error 16

// Structure of message
typedef struct
{
    char type;        // 1 byte
    uint16_t data;    // 2 bytes
    uint8_t checksum; // 1 byte
} Message;

class DATAMCU
{
private:
    Message mess;
    
public:
    // constructor
    DATAMCU();
    ~DATAMCU();
    // Function to get message
    Message &getmess();
    // Function to calculate checksum
    uint8_t calculateChecksum(uint16_t data);
    /// Function to create message
    void createMessage(char type, uint16_t data);
    // Function to verify message
    bool VerifyMessage(Message &msg, std::string& message);
    // Function to parse message
    void ParseMessage(std::string &receiver_data);
    // Function parse data to take number;
    int PareNum(int& number, int total_value);
    // Function to convert hexa number to hexa string
    std::string hexToString(uint32_t value);
    // Function to convert one byte to hex string
    std::string byteToHexStr(uint8_t byte);
    // Function to convert Message to string
    void messageToString(const Message &packet, std::string &hexString);
};

#endif