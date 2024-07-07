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
#define Hex_Error		16
//Structure of message
typedef struct {
    char type;
    //2byte
    uint16_t data;
    uint8_t checksum;
} Message;



class DATAMCU{
    private:
        Message mess;
        
    public:
        //constructor
       DATAMCU();
       ~DATAMCU();   
        
          
        uint8_t calculateChecksum( uint16_t data);
        //create kieu la message hay string
        void createMessage(char type, uint16_t data);
        //parse kieu la bool hay message , string
        //giup luu vao mess
        char ParseMessage(std::string& receiver_data);
        //check bien mess
        bool VerifyMessage(Message& msg);
        std::string toString() const;

        uint8_t Convert_Char_To_Hex(uint8_t ch_in);
        uint32_t StrtoHex(std::string& ptr, uint8_t len);

        //tao string gui xuong , return string, input msg
        //convert Message to string (nhu mcu)
};


#endif