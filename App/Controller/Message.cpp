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
void DATAMCU::uint8ToHexString(uint8_t value, char *str) {
    const char hexDigits[] = "0123456789ABCDEF";
    str[0] = hexDigits[(value >> 4) & 0x0F];
    str[1] = hexDigits[value & 0x0F];
    str[2] = '\0';
}

void DATAMCU::uint16toHexString(uint16_t value , char *str){
	const char hexDigits[] = "0123456789ABCDEF";
	str[0] = hexDigits[(value >> 12) & 0x0F];
	str[1] = hexDigits[(value >> 8) & 0x0F];
	str[2] = hexDigits[(value >> 4) & 0x0F];
	str[3] = hexDigits[value & 0x0F];
	str[4] = '\0'; // Null-terminate the string
}

void DATAMCU::messageToString( char *str) {
	//01
    uint8ToHexString((uint8_t)mess.type,&str[0]);
    //2345
    uint16toHexString(mess.data, &str[2]);
    //67
    uint8ToHexString(mess.checksum, &str[6]);
    str[8] = '\0';
}

std::string  DATAMCU::messageToString() {
    char str[9]; // 2 (type) + 4 (data) + 2 (checksum) + 1 (null-terminator)
    
    // Sử dụng sprintf để chuyển đổi tất cả các trường sang hex string
    sprintf(str, "%02X%04X%02X", mess.type, mess.data, mess.checksum);
    //du 9 char gom \0
    return std::string(str); // Trả về chuỗi hex
}