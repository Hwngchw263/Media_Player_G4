#include "Message.h"
DATAMCU ::DATAMCU(){}
DATAMCU::~DATAMCU(){}

//calculate sum each byte in data
//exx : data = 12 (deca)
//convet 12 to 0x000C (number = 2byte)

uint8_t DATAMCU::calculateChecksum( uint16_t data){
    //data_high = 1100 chuyen thanh 0000
    uint8_t data_high = (data >> 8) & 0xFF;
    //data_low =1100
	uint8_t data_low = data & 0xFF;
	uint8_t sum= data_high + data_low;
    return ~sum +1;
}
//create message
void DATAMCU::createMessage(char type, uint16_t data) {
    //Message msg;
    mess.type =type;
    mess.data = data;
    mess.checksum = calculateChecksum(data);
}



bool DATAMCU::VerifyMessage(Message& msg){
    uint8_t check = msg.checksum;
    if(check == calculateChecksum(msg.data)){
        return true;
    }
    return false;
}

char DATAMCU::ParseMessage(std::string& receiver_data){  
    //1byte 2 ky tu 
    //2 byte 4 ky tu
    if(!receiver_data.empty()){
        //create type 
        std::string first = receiver_data.substr(0,2);
        //change type to hex ( uint_8)
        char type =  (char)StrtoHex(first,2);
     
        std::string second = receiver_data.substr(2,4);
        uint16_t data = (uint16_t)StrtoHex(second,4);

        createMessage(type,data);
        if(VerifyMessage(mess)){
            if(mess.type == 'A'){

            }
            else if(mess.type ==  'E'){
                
            }
        }


    }
    //check true false 
    //chuoi rong bo di 
    //true chi khi checksum =1 
}


uint8_t DATAMCU::Convert_Char_To_Hex(uint8_t ch_in) {
    //input is char 
	uint8_t hex_out = Hex_Error;
	if (('0' <= ch_in) && (ch_in <= '9'))
    
	{//convert 0-9 acsii to 0-9 hex
		hex_out = ch_in - '0';
	}
	else if (('A' <= ch_in) && (ch_in <= 'F'))
	{
    //convert A-F acsii to A-F hex
		hex_out = ch_in - 'A' + 0xA;
	}
	return hex_out;
}
uint32_t DATAMCU::StrtoHex(std::string& ptr, uint8_t len)
{   //input string is char* ptr
	//Syntax_Type state = SYNTAX_TRUE;
    //len : number want to convert
    uint32_t  pHexa =0;
	uint8_t idx,c;
    //output result is pHexa
	
	for (idx = 0; idx < len; idx++)
	{
		c = Convert_Char_To_Hex(ptr[idx]);
        //if can't read break
		if (16 == c)
		{
			//state = SYNTAX_FLASE;
			break;
		}
		pHexa += c * pow(16,(len - 1 - idx));
	}
	//return state;
    return pHexa;
}

