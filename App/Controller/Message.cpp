#include "Message.h"
DATAMCU ::DATAMCU() {}
DATAMCU::~DATAMCU() {}

// Function to calculate checksum
uint8_t DATAMCU::calculateChecksum(uint16_t data)
{
    // data_high
    uint8_t data_high = (data >> 8) & 0xFF;
    // data_low
    uint8_t data_low = data & 0xFF;
    uint8_t sum = data_high + data_low;
    return ~sum + 1;
}

// Function to create message
void DATAMCU::createMessage(char type, uint16_t data)
{
    // Message msg;
    mess.type = type;
    mess.data = data;
    mess.checksum = calculateChecksum(data);
}

// Function to verify message
bool DATAMCU::VerifyMessage(Message &msg)
{
    uint8_t check = msg.checksum;
    if (check == calculateChecksum(msg.data))
    {
        return true;
    }
    return false;
}

// Function to parse message
char DATAMCU::ParseMessage(std::string &receiver_data)
{
    if (!receiver_data.empty())
    {
        // create type
        std::string first = receiver_data.substr(0, 2);
        // change type to hex ( uint_8)
        char type = (char)std::stoi(first, nullptr, 16);
        std::string second = receiver_data.substr(2, 4);
        uint16_t data = (uint16_t)std::stoi(second, nullptr, 16);
        // save message data structure
        createMessage(type, data);
    }
}

// Function to get message
Message &DATAMCU::getmess()
{
    return this->mess;
}
// Function parse data to take mode
char DATAMCU::PareMode(uint16_t value, uint8_t total_mode)
{
    uint8_t mode = value % total_mode;
    if (mode >= 0 && mode <= 9)
    {
        return static_cast<char>('0' + mode);
    }
    else if (mode >= 10 && mode <= 15)
    {
        return static_cast<char>('A' + (mode - 10));
    }
    else
    {
        return '?';
    }
}

// Function to convert hexa number to hexa string
std::string DATAMCU::hexToString(uint32_t value)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << value;
    return ss.str();
}

// Function to convert one byte to a hexadecimal string
std::string DATAMCU::byteToHexStr(uint8_t byte)
{
    const char hexChars[] = "0123456789ABCDEF";
    std::string hex(2, '0');
    hex[0] = hexChars[(byte >> 4) & 0x0F];
    hex[1] = hexChars[byte & 0x0F];
    return hex;
}

// Function to convert Message to string
void DATAMCU::messageToString(const Message &packet, std::string &hexString)
{
    hexString.clear(); // Clear the string before starting

    // Convert and append the 'type' char
    hexString += byteToHexStr(static_cast<uint8_t>(packet.type));

    // Convert and append the 'data' uint16_t (2 bytes)
    hexString += byteToHexStr(static_cast<uint8_t>(packet.data >> 8));   // High byte
    hexString += byteToHexStr(static_cast<uint8_t>(packet.data & 0xFF)); // Low byte

    // Convert and append the 'checksum' uint8_t
    hexString += byteToHexStr(packet.checksum);
}
