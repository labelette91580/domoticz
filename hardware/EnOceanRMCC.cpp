#include "stdafx.h"
#include "../main/Logger.h"
#include "../main/SQLHelper.h"
#include <string>
#include "EnOceanRMCC.h"
#include <stdarg.h>
using namespace enocean;

#define RMCC_call_with_retry(FUNCTION,RESPONSE_CODE)\
	T_RMCC_RESULT res = { 0 };															\
	for (int retry = 0 ; (retry < RMCC_NB_RETRY) && (res.function == 0)   ; retry ++ )	\
	{																					\
		const int panswer = (destID == BROADCAST_ID ) ? RC_PACKET_RESPONSE : RESPONSE_CODE ;	\
		FUNCTION;																\
		res = waitRemote_man_answer(panswer, RMCC_ACK_TIMEOUT);							\
	}																					\


#define SET_CMD_SIZE(ptc,SIZE) *ptc++  = SIZE>>1 ; *ptc++  =  (SIZE)<<7 | 0x7F;
#define SET_CMD(ptc,CMD)       *ptc++  = 0xF0 | (CMD>>8) ; *ptc++  = CMD & 0xFF  ;

typedef struct {
	uint16_t   index;
	uint8_t    length;
	uint8_t    data[06];
}T_PAYLOD;

// ESP3 Packet types
enum ESP3_PACKET_TYPE : uint8_t
{
	PACKET_RADIO_ERP1 = 0x01,	  // Radio telegram
	PACKET_RESPONSE = 0x02,		  // Response to any packet
	PACKET_RADIO_SUB_TEL = 0x03,	  // Radio subtelegram
	PACKET_EVENT = 0x04,		  // Event message
	PACKET_COMMON_COMMAND = 0x05,	  // Common command"
	PACKET_SMART_ACK_COMMAND = 0x06,  // Smart Acknowledge command
	PACKET_REMOTE_MAN_COMMAND = 0x07, // Remote management command
	PACKET_RADIO_MESSAGE = 0x09,	  // Radio message
	PACKET_RADIO_ERP2 = 0x0A,	  // ERP2 protocol radio telegram
	PACKET_CONFIG_COMMAND = 0x0B,	  // RESERVED
	PACKET_COMMAND_ACCEPTED = 0x0C,	  // For long operations, informs the host the command is accepted
	PACKET_RADIO_802_15_4 = 0x10,	  // 802_15_4 Raw Packet
	PACKET_COMMAND_2_4 = 0x11,	  // 2.4 GHz Command
};
const char* Query_Status_return_codes[] =
{
	"OK                            ",
	"Wrong target ID               ",
	"Wrong unlock code             ",
	"Wrong EEP                     ",
	"Wrong manufacturer ID         ",
	"Wrong data size               ",
	"No code set                   ",
	"Not send                      ",
	"RPC failed                    ",
	"Message time out              ",
	"Too Long Message              ",
	"Message part already received ",
	"Message part not received     ",
	"Address out of range          ",
	"Code data size exceeded       ",
	"Wrong data                    "
};

typedef struct {
int timeout;
int responseCode;
const char * Name;
}T_RMCC_COMMAND_CODE;
static  std::map < uint32_t , T_RMCC_COMMAND_CODE > 	RC_commande_code   =
{
	{ UNLOCK                               ,{ 1 , RC_ACK                              ,    "Unlock                                "} },
	{ LOCK                                 ,{ 1 , RC_ACK                              ,    "Lock                                  "} },
	{ SETCODE                              ,{ 1 , RC_ACK                              ,    "Set code                              "} },
	{ QUERYID                              ,{ 1 , 0                                   ,    "Query ID                              "} },
	{ QUERYID_ANSWER                       ,{ 1 , 0                                   ,    "Query ID answer                       "} },
	{ QUERYID_ANSWER_EXT                   ,{ 1 , 0                                   ,    "! Ext Query Id Answer                 "} },
	{ ACTION                               ,{ 1 , RC_ACK                              ,    "Action                                "} },
	{ PING                                 ,{ 1 , PING_ANSWER                         ,    "Ping                                  "} },
	{ PING_ANSWER                          ,{ 1 , 0                                   ,    "Ping answer                           "} },
	{ QUERY_FUNCTION                       ,{ 1 , QUERY_FUNCTION_ANSWER               ,    "Query function                        "} },
	{ QUERY_FUNCTION_ANSWER                ,{ 1 , 0                                   ,    "Query function answer                 "} },
	{ QUERY_STATUS                         ,{ 1 , QUERY_STATUS_ANSWER                 ,    "Query status                          "} },
	{ QUERY_STATUS_ANSWER                  ,{ 1 , 0                                   ,    "Query status answer                   "} },
	{ REMOTE_LEARNIN                       ,{ 1 , RC_ACK                              ,    "Remote learn in                       "} },
	{ REMOTE_FLASH_WRITE                   ,{ 5 , 0                                   ,    "Remote flash write                    "} },
	{ REMOTE_FLASH_READ                    ,{ 5 , 0                                   ,    "Remote flash read                     "} },
	{ REMOTE_FLASH_READ_ANSWER             ,{ 5 , 0                                   ,    "Remote flash read answer              "} },
	{ SMARTACK_READ                        ,{ 5 , 0                                   ,    "SmartACK read                         "} },
	{ SMARTACK_READ_MAILBOX_ANSWER         ,{ 5 , 0                                   ,    "SmartACK read mailbox answer          "} },
	{ SMARTACK_READ_LEARNED_SENSOR_ANSWER  ,{ 5 , 0                                   ,    "SmartACK read learned sensor answer   "} },
	{ SMARTACK_WRITE                       ,{ 5 , 0                                   ,    "SmartACK write                        "} },
	{ RC_ACK                               ,{ 1 , 0                                   ,    "Remote Commissioning Acknowledge      "} },
	{ RC_GET_METADATA                      ,{ 1 , RC_GET_METADATA_RESPONSE            ,    "Get Link Table Metadata Query         "} },
	{ RC_GET_METADATA_RESPONSE             ,{ 1 , 0                                   ,    "Get Link Table Metadata Response      "} },
	{ RC_GET_TABLE                         ,{ 2 , 0                                   ,    "Get Link Table Query                  "} },
	{ RC_GET_TABLE_RESPONSE                ,{ 2 , RC_GET_TABLE_RESPONSE               ,    "Get Link Table Response               "} },
	{ RC_SET_TABLE                         ,{ 2 , RC_ACK                              ,    "Set Link Table Query                  "} },
	{ RC_GET_GP_TABLE                      ,{ 2 , RC_GET_GP_TABLE_RESPONSE            ,    "Get Link Table GP Entry Query         "} },
	{ RC_GET_GP_TABLE_RESPONSE             ,{ 5 , 0                                   ,    "Get Link Table GP Entry Response      "} },
	{ RC_SET_GP_TABLE                      ,{ 1 , RC_ACK                              ,    "Set Link Table GP Entry Query         "} },
	{ RC_SET_LEARN_MODE                    ,{ 1 , RC_ACK                              ,    "Remote Set Learn Mode                 "} },
	{ RC_TRIG_OUTBOUND_TEACH_REQ           ,{ 5 , 0                                   ,    "Trigger Outbound Remote Teach Request "} },
	{ RC_GET_DEVICE_CONFIG                 ,{ 2 , RC_GET_DEVICE_CONFIG_RESPONSE       ,    "Get Device Configuration Query        "} },
	{ RC_GET_DEVICE_CONFIG_RESPONSE        ,{ 2 , 0                                   ,    "Get Device Configuration Response     "} },
	{ RC_SET_DEVICE_CONFIG                 ,{ 2 , RC_ACK                              ,    "Set Device Configuration Query        "} },
	{ RC_GET_LINK_BASED_CONFIG             ,{ 1 , RC_GET_LINK_BASED_CONFIG_RESPONSE   ,    "Get Link Based Configuration Query    "} },
	{ RC_GET_LINK_BASED_CONFIG_RESPONSE    ,{ 1 , 0                                   ,    "Get Link Based Configuration Response "} },
	{ RC_SET_LINK_BASED_CONFIG             ,{ 1 , RC_ACK                              ,    "Set Link Based Configuration Query    "} },
	{ RC_APPLY_CHANGES                     ,{ 1 , RC_ACK                              ,    "Apply Changes Command                 "} },
	{ RC_RESET_TO_DEFAULTS                 ,{ 1 , RC_ACK                              ,    "Reset to Defaults                     "} },
	{ RC_RADIO_LINK_TEST_CONTROL           ,{ 5 , 0                                   ,    "Radio Link Test Control               "} },
	{ RC_GET_PRODUCT_ID                    ,{ 2 , RC_GET_PRODUCT_RESPONSE             ,    "Get Product ID Query                  "} },
	{ RC_GET_PRODUCT_RESPONSE              ,{ 2 , 0                                   ,    "Get Product ID Response               "} },
	{ RC_GET_REPEATER_FUNCTIONS            ,{ 1 , RC_GET_REPEATER_FUNCTIONS_RESPONSE  ,    "Get Repeater Functions Query          "} },
	{ RC_GET_REPEATER_FUNCTIONS_RESPONSE   ,{ 1 , 0                                   ,    "Get Repeater Functions Response       "} },
	{ RC_SET_REPEATER_FUNCTIONS            ,{ 1 , RC_ACK                              ,    "Set Repeater Functions Query          "} },
	{ RC_SET_REPEATER_FILTER               ,{ 1 , RC_ACK                              ,    "Set Repeater Filter Query             "} },
	{ RC_PACKET_RESPONSE                   ,{ 1 , 0                                   ,    "PACKET RESPONSE                       "} },

};
uint32_t getRCtimeoutSec(int fct)
{
	int timeout = RC_commande_code[fct].timeout;
	if (timeout<1)
		timeout=1;
	return timeout;
};
uint32_t getRCresponseCode(int fct)
{
	int responseCode = RC_commande_code[fct].responseCode;
	if (responseCode<1)
		_log.Log(LOG_ERROR, "Invalid  response Code for function %d",fct);

	return responseCode;
};
const char* RMCC_Cmd_Desc(const uint32_t fct)
{
	const char* name =  RC_commande_code[fct].Name;
	return name;
}

std::string  GetDeviceNameFromId(unsigned int ID)
{
	char szDeviceID[20];
	sprintf(szDeviceID, "%07X", (unsigned int)ID);
	std::vector<std::vector<std::string> > result;
	result = m_sql.safe_query("SELECT Name  FROM DeviceStatus WHERE ( instr(DeviceID, '%q' ) <> 0) ", szDeviceID);
	if (result.size() != 0)
	{
		return result[0][0];
	}
	return "";
}
std::string IntToString(int val, int nbDigit)
{
	char fmt[16];
	char intStr[32];
	sprintf(fmt, "%%0%dX", nbDigit);
	sprintf(intStr, fmt, val);
	return intStr;
}
std::string string_format(const char* fmt, ...) {
	va_list ap;
	char buf[1024];
	va_start(ap, fmt);
	vsnprintf((char*)buf, sizeof(buf), fmt, ap);
	return buf;
}
std::string  replaceString(const char* str, std::string stringToReplace, std::string stringReplace)
{
	std::string  s = str;
	while (s.find(stringToReplace) != std::string::npos)
		s.replace(s.find(stringToReplace), stringToReplace.size(), stringReplace);
	return s;
}
//extract eep in 21 bit rorg 8bit func 6bit type 7 bit  mask 3 bit
void extractEEP21bit(unsigned char eep[], int* Rorg, int* Func, int* Type)
{
	*Rorg = eep[0];
	int ft = eep[1] * 256 + eep[2];
	//exlude mask 3 bits
	ft >>= 3;
	*Type = ft & 0x7F;
	ft >>= 7;
	*Func = ft & 0x3F;
}
void extractEEP24bit(unsigned char eep[], int* Rorg, int* Func, int* Type)
{
	*Rorg = eep[0];
	*Func = eep[1];
	*Type = eep[2];
}
void CEnOceanRMCC::extractEEP(unsigned char eep[], int* Rorg, int* Func, int* Type)
{
	extractEEP24bit(eep, Rorg, Func, Type);
	if (GetEEP((const int)*Rorg, *Func, *Type) != nullptr)
		return;
	extractEEP21bit(eep, Rorg, Func, Type);
}
CEnOceanRMCC::CEnOceanRMCC() {
	m_Seq = 0;
	m_com_status = COM_OK;
};
void CEnOceanRMCC::setRorg(unsigned char* buff, int idx)
{
	buff[0] = RORG_SYS_EX;
	//if first fragment increment sequence
	if (idx == 0) {
		m_Seq++;
		if (m_Seq > 3) m_Seq = 1;
	}
	buff[1] = (m_Seq << 6) + idx;       //SEQ 40/80/C0
}
void CEnOceanRMCC::parse_PACKET_REMOTE_MAN_COMMAND(unsigned char m_buffer[], int m_DataSize, int m_OptionalDataSize)
{
	char message[1024] = ""; //log message
	std::string  messageStr = ""; //IHM message
	//get function
	int fct = m_buffer[0] * 256 + m_buffer[1];

	unsigned int senderId = 0;
	if (fct == RC_ACK)
	{
		snprintf(message, sizeof(message), "RMC :  function :%03X :%s", fct, RMCC_Cmd_Desc(fct));
		messageStr = message;
		Log(LOG_NORM, message );
	}
	//ping response
	else if (fct == PING_ANSWER)
	{
		// ping
		//	55 00 0F 07 01 2B         C5 80 00 7F F0 06 00 00 00 00 00 00 00 00 8F        03 01 A6 54 28 FF 00 83
		//response
		//	55 00 08 0A 07 C6         06 06 07 FF D2 01 12 2D                             05 01 33 BE 01 A6 54 28 2D 00 34
		int rorg = 0;
		int func = 0;
		int type = 0;
		//EEP are either on 21 bits or 24 bits
		extractEEP(&m_buffer[4], &rorg, &func, &type);
		int profile = RorgFuncTypeToProfile(rorg, func, type);
		senderId = DeviceArrayToInt(&m_buffer[m_DataSize + 4]);
		unsigned int RSSI = m_buffer[7];
		TeachInNodeIfExist(senderId, 0, rorg, func, type, GENERIC_NODE);
		snprintf(message, sizeof(message), "RMC : Ping Answer <BR> SenderId: %08X Profile:%06X : %s : %s", senderId, profile, GetEEPLabel(rorg, func, type), GetEEPDescription(rorg, func, type));
		messageStr = message;
		Log(LOG_NORM, replaceString(message, "<BR>", ""));
	}
	//query  response
	else if ((fct == QUERYID_ANSWER) || (fct == QUERYID_ANSWER_EXT))
	{
		// queryId 
		//		    queryid send cmd EEP : 00000000 Mask : 0
		//			Send :                                  : 55 00 0F 07 01 2B C5 80 01 FF F0 04 00 00 00 00 00 00 00 00 8F 03 FF FF FF FF FF 00 EE
		//			Recv PACKET_REMOTE_MAN_COMMAND(07 / 0A) : 06 04 07 FF D2 05 00 - FF FF FF FF 05 85 87 4A 3D 00 Opt Size : 10
		int rorg = 0;
		int func = 0;
		int type = 0;
		//EEP are either on 21 bits or 24 bits
		extractEEP(&m_buffer[4], &rorg, &func, &type);
		int profile = RorgFuncTypeToProfile(rorg, func, type);
		senderId = DeviceArrayToInt(&m_buffer[m_DataSize + 4]);
		TeachInNodeIfExist(senderId, 0, rorg, func, type, GENERIC_NODE);
		snprintf(message, sizeof(message), "RMC : QueryId Answer SenderId: %08X Profile:%06X ", senderId, profile);
		messageStr = message;
		Log(LOG_NORM, message);
		fct = QUERYID_ANSWER_EXT;
	}
	//product id  response
	else if (fct == RC_GET_PRODUCT_RESPONSE)
	{
		//get product id  cmd 227 
		//55 00 0F 07 01 2B         C5 80 00 7F F2 27 00 00 00 00 00 00 00 00 8F        03 FF FF FF FF FF 00             55
		//reponse  manu 46 procuct ref 00010003
		//55 00 0A 0A 07 10         08 27 07 FF 00 46 00 01 00 03                       FF FF FF FF 01 A6 54 28 2C 00     B3
		unsigned int manuf = m_buffer[4] * 256 + m_buffer[5];
		unsigned int reference = DeviceArrayToInt(&m_buffer[6]);
		senderId = DeviceArrayToInt(&m_buffer[14]);
		snprintf(message, sizeof(message), "RMC : getProductId Answer SenderId: %08X Manufacturer:%s Ref:%08X  ", senderId, GetManufacturerName(manuf), reference);
		messageStr = message;
		Log(LOG_NORM, message);
		TeachInNodeIfExist(senderId, manuf, 0, 0, 0, GENERIC_NODE);
		//set the code
		//unsigned int code = GetLockCode();
		//setcode(senderId, code);
		//ping(senderId);
	}
	//get link table medatadate cmd 0210 : taille current / max  table
	else if (fct == RC_GET_METADATA_RESPONSE)
	{
		//get link table medatadate cmd 0210 : taille current/ max  table
		//55 00 0F 07 01 2B         C5 40 00 7F F2 10 00 00 00 00 00 00 00 00 8F        03 01 A6 54 28 FF 00  AD
		//response curren size 03 max x18=24
		//55 00 09 0A 07 AD         08 10 07 FF 50 00 00 03 18                          FF FF FF FF 01 A6 54 28 2D 00 08
		int currentSize = m_buffer[7];
		int maxSize = m_buffer[8];
		senderId = DeviceArrayToInt(&m_buffer[13]);
		snprintf(message, sizeof(message), "RMC : Get Link table medatadata Answer SenderId: %08X Size:%d Max:%d ", senderId, currentSize, maxSize);
		messageStr = message;
		Log(LOG_NORM, message);
		m_nodes.setLinkTableMedadata(senderId, currentSize, maxSize);
		//if no link content , delete internal base Adresse
/*		if (currentSize == 0)
			if (SensorExist(senderId))
				UpdateSenorBaseAddress(SensorIDToString(senderId),0 );
				*/
	}
	//get all link table
	else if (fct == RC_GET_TABLE_RESPONSE)
	{
		//get all link table
		//55 00 0F 07 01 2B         C5 40 01 FF F2 11 00 00 17 00 00 00 00 00 8F        03 01 A6 54 28 FF 00 56
		//response                                        ID DEVICE ID   PROFILE  CHN
		//                          0  1    2  3     4    5  6  7  8  9  10 11 12 13
		//55 00 20 0A 07 D4         08 11   07 FF    00   00 FF 99 DF 01 D5 00 01 00   01 FF 99 DF 02 F6 02 01 00   02 FF 99 DF 02 F6 02 01 01       FF FF FF FF 01 A6 54 28 2E 00 FB
		//55 00 20 0A 07 D4         08 11   07 FF    00   03 00 00 00 00 FF FF FF 00   04 00 00 00 00 FF FF FF 00   05 00 00 00 00 FF FF FF 00       FF FF FF FF 01 A6 54 28 2E 00 8F
		//55 00 20 0A 07 D4         08 11   07 FF    00   06 00 00 00 00 FF FF FF 00   07 00 00 00 00 FF FF FF 00   08 00 00 00 00 FF FF FF 00       FF FF FF FF 01 A6 54 28 2E 00 DE
		//55 00 20 0A 07 D4         08 11   07 FF    00   09 00 00 00 00 FF FF FF 00   0A 00 00 00 00 FF FF FF 00   0B 00 00 00 00 FF FF FF 00       FF FF FF FF 01 A6 54 28 2E 00 F4
		//55 00 20 0A 07 D4         08 11   07 FF    00   0C 00 00 00 00 FF FF FF 00   0D 00 00 00 00 FF FF FF 00   0E 00 00 00 00 FF FF FF 00       FF FF FF FF 01 A6 54 28 2E 00 E1
		//55 00 20 0A 07 D4         08 11   07 FF    00   0F 00 00 00 00 FF FF FF 00   10 00 00 00 00 FF FF FF 00   11 00 00 00 00 FF FF FF 00       FF FF FF FF 01 A6 54 28 2E 00 BC
		//55 00 20 0A 07 D4         08 11   07 FF    00   15 00 00 00 00 FF FF FF 00   16 00 00 00 00 FF FF FF 00   17 00 00 00 00 FF FF FF 00       FF FF FF FF 01 A6 54 28 2E 00 66
		senderId = DeviceArrayToInt(&m_buffer[m_DataSize + 4]);
		int nb = m_DataSize - 5;
		nb /= 9;
		for (int i = 0; i < nb; i++) {
			int  offs = m_buffer[5 + i * 9];
			uint32_t entryId = DeviceArrayToInt(&m_buffer[6 + i * 9]);
			uint32_t entryProfile = DeviceArrayToInt(&m_buffer[10 + i * 9]);
			int  channel = m_buffer[13 + i * 9];
			entryProfile /= 256;
			m_nodes.addLinkTableEntry(senderId, offs, entryProfile, entryId, channel);
			snprintf(message, sizeof(message), "RMC : ADD Link table Entry SenderId: %08X  entry %02d EntryId: %08X Profile %06X Channel:%d", senderId, offs, entryId, entryProfile, channel);
			messageStr = message;
			Log(LOG_NORM, message);
		}
		m_nodes.printTableLink();
	}
	//query function
	else if (fct == QUERY_FUNCTION_ANSWER)
	{
		//query function
		//55 00 0F 07 01 2B	C5 80 00 7F F0 07 00 00 00 00 00 00 00 00 8F 				03 01 A6 54 28 FF 00     8D  opt 7
		//55 00 34 0A 07 DD 06 07 07 FF 02 24 07 FF 02 27 07 FF 02 20 07 FF 02 10 07 FF 02 11 07 FF 02 12 07 FF 02 30 07 FF 02 31 07 FF 02 32 07 FF 02 33 07 FF 02 26 07 FF 00 00 00 00      FF FF FF FF 01 A6 54 28 2C 00     2E opt 10
		senderId = DeviceArrayToInt(&m_buffer[m_DataSize + 4]);
		Log(LOG_NORM, "RMC : QUERY FUNCTION answer SenderId: %08X  ", senderId);
		messageStr = "Functions list: <BR>";
		int nb = m_DataSize - 4;
		nb /= 4;
		for (int i = 0; i < nb; i++) {
			int  function = m_buffer[4 + i * 4] * 256 + m_buffer[5 + i * 4];
			if (function) {
				snprintf(message, sizeof(message), " Function :%0X = %s ", function, RMCC_Cmd_Desc(function));
				messageStr += message;
				messageStr += "<BR>";
				Log(LOG_NORM, message);
			}
		}
	}
	else if (fct == QUERY_STATUS_ANSWER)
	{
		bool  CodeIsSet = m_buffer[4] & 0x80;
		int   LastSeq = m_buffer[4] & 0x3;
		int lastFunc = m_buffer[5] * 256 + m_buffer[6];
		int lastReturnCode = m_buffer[7];
		senderId = DeviceArrayToInt(&m_buffer[12]);
		snprintf(message, sizeof(message), "RMC : QUERY STATUS ANSWER SenderId: %08X CodeIsSet:%d LastSeq:%d lastFunc:%04X lastReturnCode:%d :%s", senderId, CodeIsSet, LastSeq, lastFunc, lastReturnCode, Query_Status_return_codes[lastReturnCode & 0xF]);
		messageStr = message;
		Log(LOG_NORM, message);
	}
	else if (fct == RC_GET_REPEATER_FUNCTIONS_RESPONSE)
	{
		/*
		Repeater function (2 bit): 		 0b00 - Repeater Off 		 0b01 - Repeater On 		 0b10 - Filtered Repeating On
		Repeater level    (2 bit):		 0b01 - Repeater Level 1     0b10 - Repeater Level 2
		Repeater Filter Structure (1 bit): 		 0b0 - AND for Repeating		 0b1 - OR for Repeating
		 Data structure:
		7  6    5  4   3
		RepFunc RepLev RepStruct
		*/
		bool  RepFunc = m_buffer[4] >> 6;
		int   RepLev = (m_buffer[4] >> 4) & 0x3;
		int   RepStruct = (m_buffer[4] >> 3) & 0x1;
		senderId = DeviceArrayToInt(&m_buffer[12]);
		snprintf(message, sizeof(message), "RMC : GET_REPEATER_FUNCTIONS_RESPONSE SenderId: %08X Repeater function:%d  Repeater level: %d Repeater Filter Structure  : %d", senderId, RepFunc, RepLev, RepStruct);
		messageStr = message;
		Log(LOG_NORM, message);
	}
	else if (fct == RC_GET_LINK_BASED_CONFIG_RESPONSE)
	{
		senderId = DeviceArrayToInt(&m_buffer[m_DataSize + 4]);
		unsigned char* ptb = &m_buffer[4];
		bool  direction = *ptb++;
		int   LinkTableindex = *ptb++;
		//read paylod
		int   index = *ptb++ * 256;
		index += *ptb++;
		int   Length = *ptb++;
		char data[16];
		uint32_t value = 0;
		for (int i = 0; i < Length; i++)
		{
			value <<= 8;
			value += *ptb;
			sprintf(&data[i * 2], "%02X ", *ptb++);
		}
		snprintf(message, sizeof(message), "RMC : GET_LINK_BASED_CONFIG_RESPONSE SenderId:%08X dir:%d Entry:%d Index:%d Len:%d data:%s : %d ", senderId, direction, LinkTableindex, index, Length, data, value);
		m_nodes.updateLinkConfig (senderId , LinkTableindex ,  value );
		messageStr = message;
		Log(LOG_NORM, message);
	}
	else
	{
		if (m_OptionalDataSize < 8)
			Log(LOG_NORM, "Func: Received RMC :%03X :%s", fct, RMCC_Cmd_Desc(fct));
		else
			Log(LOG_NORM, "Func: Received RMC :%03X :%s from %02X%02X%02X%02X", fct, RMCC_Cmd_Desc(fct), m_buffer[m_DataSize + 4], m_buffer[m_DataSize + 5], m_buffer[m_DataSize + 6], m_buffer[m_DataSize + 7]);
	}

	setRemote_man_answer(fct, (char*)messageStr.c_str(), senderId);
}
void CEnOceanRMCC::RemoteSetLearnMode(unsigned int destID, int channel, T_LEARN_MODE Device_LRN_Mode)
{
	RMCC_call_with_retry(remoteSetLearnMode(destID,channel,Device_LRN_Mode) , getRCresponseCode(RC_SET_LEARN_MODE));

}
void CEnOceanRMCC::remoteSetLearnMode(unsigned int destID, int channel, T_LEARN_MODE Device_LRN_Mode)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x01;			//data len = 2
	buff[3] = 0x7F;		//mamanufacturer 7FF
	buff[4] = 0xF2;			//function 220
	buff[5] = 0x20;
	//payload 4 bytes
	buff[6] = Device_LRN_Mode << 6;
	buff[7] = channel;
	buff[14] = 0x8F; //status
	//optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "send remoteLearning to %08X channel %d Mode:%d", destID, channel, Device_LRN_Mode);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::Unlock(unsigned int destID, unsigned int code)
{
	RMCC_call_with_retry(unlock(destID,code),getRCresponseCode(UNLOCK));
}
void CEnOceanRMCC::unlock(unsigned int destID, unsigned int code)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x02;			//data len = 4
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = 0x01;			//function 001
	buff[14] = 0x8F; //status
	DeviceIntToArray(code, &buff[6]);
	//optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: unlock cmd to %08X code:%08X", destID, code);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::Lock(unsigned int destID, unsigned int code)
{
	RMCC_call_with_retry(lock(destID,code),getRCresponseCode(LOCK));
}
void CEnOceanRMCC::lock(unsigned int destID, unsigned int code)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x02;			//data len = 4
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = 0x02;			//function 002
	buff[14] = 0x8F; //status
	DeviceIntToArray(code, &buff[6]);
	//optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: lock  cmd to %08X code:%08X", destID, code);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::Setcode(unsigned int destID, unsigned int code)
{
	RMCC_call_with_retry(setcode(destID,code),getRCresponseCode(SETCODE));
}
void CEnOceanRMCC::setcode(unsigned int destID, unsigned int code)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x02;			//data len = 4
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = 0x03;			//function 003
	buff[14] = 0x8F; //status
	DeviceIntToArray(code, &buff[6]);
	//optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: setcode cmd to %08X , %d", destID, code);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
/*
 always as broadcast telegram
Query ID Command
Query ID is sent always as broadcast telegram. All unlocked devices respond to the Query
ID with their ID and their EEP.
The EEP is a 21 bit and it is defined as following: ORG-FUNC-TYPE. For more information
about the EEP be sure to read EEP2.1 specification.
The Query ID command contains an EEP definition and mask bits. When the mask bits are
set to 0x01 only Remote Devices with the matching EEP will process the remote command. If
the query ID with mask bit 0x00 is transmitted, the EEP bytes in this command will be
ignored and every Remote Device will answer to this command. If a Remote Device has no
EEP, then it will only respond to the Query ID command where the mask bits are set to 0x00.
The mask bits in the Query ID answer telegrams are set to 0x00.
Query ID Answer Extended (0x704) was defined in later reviews and should replace the
original Query ID Answer (0x604). The usage of Query ID Answer (0x604) is depreciated.
Query ID Answer Extended contains the information about the device being managed by
other manager. Locked by other manager (0 – false, 1 - true).
*/
void CEnOceanRMCC::queryid(unsigned int EEP, unsigned int mask)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x01;			//data len = 3
	buff[3] = 0xFF;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = 0x04;			//function 004
	buff[6] = 0;
	buff[7] = 0;
	buff[8] = 0;
	buff[14] = 0x8F; //status
	//optionnal data : alway broadcast
	setDestination(opt, 0xFFFFFFFF);
	Log(LOG_NORM, "SEND: queryid send cmd EEP: %08X Mask: %d", EEP, mask);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
T_RMCC_RESULT CEnOceanRMCC::Ping(unsigned int destID)
{
	RMCC_call_with_retry(ping(destID),getRCresponseCode(PING));
	return res;
}
void CEnOceanRMCC::ping(unsigned int destID)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 0
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = 0x06;			//function 006
	buff[14] = 0x8F; //status
	//optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: Ping cmd send to %08X ", destID);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::Action(unsigned int destID)
{
	RMCC_call_with_retry(action(destID),getRCresponseCode(ACTION));
}
void CEnOceanRMCC::action(unsigned int destID)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 0
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = 0x05;			//function 005
	buff[14] = 0x8F;		//status
					 //optionnal data
	setDestination(opt, destID);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	Log(LOG_NORM, "SEND: action cmd %08X ", destID);
}
/*
broadcast oe unicast
Get Product ID Query & Response
The Product ID Query returns the Product ID of a device. This manufacture and device specific
ID can be used as a key to lookup additional information about an EnOcean device via the Device
Description file.
The Product ID is the combination of the Manufacturer ID and a 4 byte Product Reference that
is unique per a device’s firmware and is managed by the manufacturer. The Product ID is 6 bytes
in length.
The Get Product ID Response is transmitted from the target device in a beaconing mode, when
the Get Product ID Request is transmitted broadcast. Beaconing mode represents the repeated
transmission of a response until the device is acknowledged by the commissioning device with
any addressed  Remote  Management message  to  the  target  device.  After  receiving  any
addressed Remote Management message the beaconing stops.
The period to repeat the beacon is semirandom specific 1..5 sec  for every end device 10 times within
one minute. At every retransmission of the beacon the period is randomly determined again.
The Get Product ID and Get Product ID Selective may be processed only in locked state if the
default factory code is set or not set. Get Product ID Selective shall not be processed in locked
status of the managed devices with specific code set [1]
*/
void CEnOceanRMCC::getProductId(unsigned int destination)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 0
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x27;			//function 227
	buff[14] = 0x8F;		//status
							//optionnal data
	setDestination(opt, destination);
	Log(LOG_NORM, "SEND: getProductId cmd send to %08X", destination);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::getLinkTableMedadata(uint32_t destID)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 0
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x10;			//function 210
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: getLinkTableMedadata %08X ", destID);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::GetLinkTableMedadata(uint32_t destID)
{
	RMCC_call_with_retry(getLinkTableMedadata(destID),getRCresponseCode(RC_GET_METADATA));
}
T_RMCC_RESULT CEnOceanRMCC::QueryFunction(uint32_t destID)
{
	RMCC_call_with_retry(queryFunction(destID),getRCresponseCode(QUERY_FUNCTION));
	return res;
}
void CEnOceanRMCC::queryFunction(uint32_t destID)
{
	unsigned char buff[16];
	unsigned char opt[16];
	//C5 80 00 7F F0 07 00 00 00 00 00 00 00 00 8F
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 0
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = 0x07;			//function 007
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: queryFunction %08X ", destID);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::queryStatus(uint32_t destID)
{
	unsigned char buff[16];
	unsigned char opt[16];
	//C5 80 00 7F F0 08 00 00 00 00 00 00 00 00 8F
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 0
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF0;
	buff[5] = QUERY_STATUS;			//function 008
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: queryStatus %08X ", destID);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::getDeviceConfiguration(uint32_t SensorId, int begin, int end, int length)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	unsigned char* ptc = &buff[2];
	*ptc++ = 0x02;			//data len = 5
	*ptc++ = 0xFF;			//mamanufacturer 7FF
	*ptc++ = 0xF2;
	*ptc++ = 0x30;			//function 230
	*ptc++ = begin >> 8;
	*ptc++ = begin & 0xFF;
	*ptc++ = end >> 8;
	*ptc++ = end & 0xFF;
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, SensorId);
	Log(LOG_NORM, "SEND: getDeviceConfiguration %08X begin :%d End:%d ", SensorId, begin, end);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	memset(buff, 0, sizeof(buff));
	setRorg(buff, 1);
	ptc = &buff[2];
	*ptc++ = length & 0xFF;
	buff[14] = 0x8F; //status
	//optionnal data
	setDestination(opt, SensorId);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::getDeviceLinkBaseConfiguration(uint32_t SensorId, int Linkindex, int begin, int end, int length)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	unsigned char* ptc = &buff[2];
	*ptc++ = 0x03;			//2 //data len = 7
	*ptc++ = 0xFF;			//3 //mamanufacturer 7FF
	*ptc++ = 0xF2;         //4 
	*ptc++ = 0x32;			//5 //function 232
	*ptc++ = 0;          //6 //0:inbound 0x80 outbound
	*ptc++ = Linkindex & 0xFF;//7 
	*ptc++ = begin >> 8;     //8 Start	Index
	*ptc++ = begin & 0xFF;//9 
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, SensorId);
	Log(LOG_NORM, "SEND: getDeviceLinkBaseConfiguration  %08X entry:%d begin :%d End:%d len:%d", SensorId, Linkindex, begin, end, length);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	memset(buff, 0, sizeof(buff));
	setRorg(buff, 1);
	ptc = &buff[2];
	*ptc++ = end >> 8;         //2 End Index
	*ptc++ = end & 0xFF;    //3  End Index
	*ptc++ = length & 0xFF;  //   Length
	buff[14] = 0x8F; //status
	//optionnal data
	setDestination(opt, SensorId);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::GetDeviceLinkBaseConfiguration(uint32_t destID, int Linkindex, int begin, int end, int length)
{
	RMCC_call_with_retry(getDeviceLinkBaseConfiguration(destID,Linkindex,begin,end,length) , getRCresponseCode(RC_GET_LINK_BASED_CONFIG));
}
void CEnOceanRMCC::sendSysExMessage(uint32_t destSensorId, uint8_t data[], uint8_t len)
{
	unsigned char buff[16];
	unsigned char opt[16];
	int fragment = 0;
	while (len > 0)
	{
		memset(buff, 0, sizeof(buff));
		setRorg(buff, fragment);
		int currenLen = len;
		if (currenLen > 8)
			currenLen = 8;
		//copy up to 8
		memcpy(&buff[2], data, currenLen);
		buff[14] = 0x8F; //status
		//optionnal data
		setDestination(opt, destSensorId);
		SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
		fragment++;
		data += 8;
		len -= currenLen;
	}
}
void CEnOceanRMCC::setDeviceLinkBaseConfiguration(uint32_t SensorId, int Linkindex, int indexParam, int NbParam, int length, uint8_t data[])
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	Log(LOG_NORM, "SEND: setDeviceLinkBaseConfiguration  %08X entry:%d begin :%d End:%d len:%d", SensorId, Linkindex, indexParam, NbParam, length);
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	unsigned char* ptc = &buff[2];
	NbParam = 1;
	int len = 2 + (length + 3) * NbParam;
	SET_CMD_SIZE(ptc, len); //set datalen & //manufacturer 7FF
	SET_CMD(ptc, 0x233); //manufacturer 7FF & function code
	*ptc++ = 0;          //6 //0:inbound 0x80 outbound
	*ptc++ = Linkindex & 0xFF;//7 
	*ptc++ = indexParam >> 8;     //8 Start	Index
	*ptc++ = indexParam & 0xFF;//9 
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, SensorId);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	memset(buff, 0, sizeof(buff));
	setRorg(buff, 1);
	ptc = &buff[2];
	*ptc++ = length & 0xFF;  //   Length
	for (int i = 0; i < length; i++)
		*ptc++ = data[i];
	buff[14] = 0x8F; //status
	//optionnal data
	setDestination(opt, SensorId);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::setDeviceLinkBaseConfiguration2(uint32_t SensorId, int Linkindex, int indexParam, int NbParam, int length, uint8_t data[])
{
	unsigned char buff[256];
	memset(buff, 0, sizeof(buff));
	Log(LOG_NORM, "SEND: setDeviceLinkBaseConfiguration  %08X entry:%d begin :%d End:%d len:%d", SensorId, Linkindex, indexParam, NbParam, length);
	unsigned char* ptc = buff;
	NbParam = 1;
	int len = 2 + (length + 3) * NbParam;
	SET_CMD_SIZE(ptc, len); //set datalen & //manufacturer 7FF
	SET_CMD(ptc, 0x233); //manufacturer 7FF & function code
	*ptc++ = 0;          //6 //0:inbound 0x80 outbound
	*ptc++ = Linkindex & 0xFF;//7 
	*ptc++ = indexParam >> 8;     //8 Start	Index
	*ptc++ = indexParam & 0xFF;//9 
	*ptc++ = length & 0xFF;  //   Length
	for (int i = 0; i < length; i++)
		*ptc++ = data[i];
	sendSysExMessage(SensorId, buff, len + 4);
}
void CEnOceanRMCC::getallLinkTable(uint32_t SensorId, int begin, int end)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x01;			//data len = 3
	buff[3] = 0xFF;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x11;			//function 211
	buff[7] = begin;		//end offset table 
	buff[8] = end;		//end offset table 
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, SensorId);
	Log(LOG_NORM, "SEND: getallLinkTable %08X begin :%d End:%d ", SensorId, begin, end);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	//Number of table entry to received
	//3 entry by response datagramm
	int NbAnswer = ((end - begin + 1) + 2) / 3;
}
void CEnOceanRMCC::GetallLinkTable(uint32_t destID, int begin, int end)
{
	RMCC_call_with_retry(getallLinkTable(destID,begin,end) , getRCresponseCode(RC_GET_TABLE_RESPONSE));
}
void CEnOceanRMCC::getGPTable(uint32_t SensorId, int index)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x01;			//data len = 2
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x13;			//function 213
	buff[6] = 0;		//0:inbound 0x80 outbound
	buff[7] = index;		//end offset table 
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, SensorId);
	Log(LOG_NORM, "SEND: getGPTable %08X index :%d ", SensorId, index);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	waitRemote_man_answer(RC_GET_GP_TABLE_RESPONSE, RMCC_ACK_TIMEOUT);
}
void CEnOceanRMCC::getLinkTable(uint32_t DeviceId)
{
	unlockDevice(DeviceId);
	if (!isCommStatusOk())
		return;
	int PreviousTableSize = m_nodes.getTableLinkCurrentSize(DeviceId);
	GetLinkTableMedadata(DeviceId);
	if (isCommStatusOk())
	{
		sleep_milliseconds(1000);
		int TableSize = m_nodes.getTableLinkCurrentSize(DeviceId);
		int begin = 0;
		if (TableSize != PreviousTableSize)
			while (TableSize > m_nodes.getTableLinkValidSensorIdSize(DeviceId))
			{
				GetallLinkTable(DeviceId, begin, begin + 2);
				begin += 3;
				if (!isCommStatusOk())
					break;
				if (begin > m_nodes.getTableLinkMaxSize(DeviceId))
					break;
			}
	}
	if (!isCommStatusOk())
	{
		//clear entry table on error
		m_nodes.setLinkTableMedadata(DeviceId, 0, 0);
	}
}
void CEnOceanRMCC::SetLinkEntryTable(uint32_t destID, int begin, uint32_t ID, int EEP, int channel)
{
	RMCC_call_with_retry(setLinkEntryTable(destID,begin,ID,EEP,channel),getRCresponseCode(RC_SET_TABLE));
}
void CEnOceanRMCC::setLinkEntryTable(uint32_t SensorId, int begin, uint32_t ID, int EEP, int channel)
{
	unsigned char buff[16];
	unsigned char opt[16];
	unsigned char sdid[16];
	Log(LOG_NORM, "SEND: setLinkTable %08X begin :%d ID:%08X EEP:%06X Channel : %d", SensorId, begin, ID, EEP, channel);
	DeviceIntToArray(ID, sdid);
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x05;			//data len = 10
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x12;			//function 212
	buff[6] = 0x00;			//direction InBound bit 8 = 0 
	unsigned char* ptc = &buff[7];
	*ptc++ = begin;		//end offset table 
	*ptc++ = sdid[0];
	*ptc++ = sdid[1];
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, SensorId);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	memset(buff, 0, sizeof(buff));
	setRorg(buff, 1);
	ptc = &buff[2];
	*ptc++ = sdid[2];
	*ptc++ = sdid[3];
	*ptc++ = getRorg(EEP);
	*ptc++ = getFunc(EEP);
	*ptc++ = getType(EEP);
	*ptc++ = channel;
	buff[14] = 0x8F; //status
	//optionnal data
	setDestination(opt, SensorId);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
	//wait for all the table response
	//waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
}
void CEnOceanRMCC::ResetToDefaults(uint32_t destID, int resetAction)
{
	RMCC_call_with_retry(resetToDefaults(destID,resetAction),getRCresponseCode(RC_RESET_TO_DEFAULTS));
}
void CEnOceanRMCC::resetToDefaults(uint32_t destID, int resetAction)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 1
	buff[3] = 0xFF;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x24;			//function 224 RC_RESET_TO_DEFAULTS
	buff[6] = resetAction;
	buff[14] = 0x8F; //status
					 //optionnal data
	setDestination(opt, destID);
	Log(LOG_NORM, "SEND: resetToDefaults %08X ", destID);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
T_RMCC_RESULT CEnOceanRMCC::GetRepeaterQuery(unsigned int destID)
{
	RMCC_call_with_retry(getRepeaterQuery(destID),getRCresponseCode(RC_GET_REPEATER_FUNCTIONS));
	return res;
}
void CEnOceanRMCC::getRepeaterQuery(unsigned int destination)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 0
	buff[3] = 0x7F;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x50;			//function x250 RC_GET_REPEATER_FUNCTIONS
	buff[14] = 0x8F;		//status
							//optionnal data
	setDestination(opt, destination);
	Log(LOG_NORM, "SEND: geRepeaterFunctionsQuery cmd send to %08X", destination);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::SetRepeaterQuery(unsigned int destID, int Repeaterfunction, int Repeaterlevel, int RepeaterFilter)
{
	RMCC_call_with_retry(setRepeaterQuery(destID,Repeaterfunction,Repeaterlevel,RepeaterFilter),getRCresponseCode(RC_SET_REPEATER_FUNCTIONS));
}
void CEnOceanRMCC::setRepeaterQuery(unsigned int destination, int Repeaterfunction, int Repeaterlevel, int RepeaterFilter)
{
	unsigned char buff[16];
	unsigned char opt[16];
	memset(buff, 0, sizeof(buff));
	setRorg(buff);
	buff[2] = 0x00;			//data len = 1
	buff[3] = 0xFF;			//mamanufacturer 7FF
	buff[4] = 0xF2;
	buff[5] = 0x51;			//function x251 RC_SET_REPEATER_FUNCTIONS
	buff[14] = 0x8F;		//status
		/*
		Repeater function (2 bit): 		 0b00 - Repeater Off 		 0b01 - Repeater On 		 0b10 - Filtered Repeating On
		Repeater level    (2 bit):		 0b01 - Repeater Level 1     0b10 - Repeater Level 2
		Repeater Filter Structure (1 bit): 		 0b0 - AND for Repeating		 0b1 - OR for Repeating
		 Data structure:
		7  6    5  4   3
		RepFunc RepLev RepStruct
		*/
	buff[6] = (Repeaterfunction << 6) & (Repeaterlevel << 4) & (RepeaterFilter << 3);
	//optionnal data
	setDestination(opt, destination);
	Log(LOG_NORM, "SEND: setRepeaterFunctionsQuery cmd send to %08X  Repeaterfunction:%d  Repeaterlevel:%d  RepeaterFilter:%d ", destination, Repeaterfunction, Repeaterlevel, RepeaterFilter);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 15, opt, 7);
}
void CEnOceanRMCC::setNodonRepeaterLevel(unsigned int source, unsigned int destination, int Repeaterlevel)
{
	unsigned char buff[16];
	unsigned char opt[16];
	// repeater level 0 55 00 0B 06 01 95   D1 00 46 08 00 00   05 01 33 BE  00     03 04 11 EA 38 FF     09
	// reponse          55 00 0A 07 01 EB   D1 46 00 0A 00      04 11 EA 38  00     01 FF FF FF FF 2D 00  58
	// 
	// repeater level 1 55 00 0B 06 01 95   D1 00 46 08 01 01   05 01 33 BE  00     03 04 11 EA 38 FF     C0
	// reponse          55 00 0A 07 01 EB   D1 46 00 0A 01      04 11 EA 38  00     01 FF FF FF FF 2D 00  CC
	// 
	// repeater level 2 55 00 0B 06 01 95   D1 00 46 08 01 02   05 01 33 BE  00     03 04 11 EA 38 FF     27
	// reponse          55 00 0B 07 01 80   D1 00 46 08 01 02   05 01 33 BE  81     01 04 11 EA 38 2D 00  70 -->repeat 1
	//                  55 00 0A 07 01 EB   D1 46 00 0A 02      04 11 EA 38  00     01 FF FF FF FF 2D 00  77
	memset(buff, 0, sizeof(buff));
	buff[0] = RORG_MSC;
	buff[1] = 0x00;
	buff[2] = 0x46;
	buff[3] = 0x08;
	if (Repeaterlevel >= 1)
	{
		buff[4] = 0x01;
		buff[5] = Repeaterlevel;
	}
	DeviceIntToArray(source, &buff[6]);
	//optionnal data
	setDestination(opt, destination);
	Log(LOG_NORM, "SEND: SetNodonRepeaterLevel cmd send to %08X  Repeaterlevel:%d  ", destination, Repeaterlevel);
	SendESP3PacketQueued(PACKET_RADIO_ERP1, buff, 11, opt, 7);
}
void CEnOceanRMCC::TeachIn(std::string& sidx, T_LEARN_MODE Device_LRN_Mode)
{
	//teachin from ID database
	std::vector<std::vector<std::string> > result;
	result = m_sql.safe_query("SELECT DeviceID,Unit  FROM DeviceStatus WHERE (ID='%s')  ", sidx.c_str());
	if (result.size() > 0)
	{
		TeachIn(result[0][0], result[0][1], Device_LRN_Mode);
	}
}
void CEnOceanRMCC::TeachIn(std::string& deviceId, std::string& unit, T_LEARN_MODE Device_LRN_Mode)
{
	//teachin from senderId / unit 
	int channel = atoi(unit.c_str());
	//get sender adress from db
	unsigned int SenderAdress = DeviceIdStringToUInt(deviceId);
	Log(LOG_NORM, "SEND: remoteLearning to device %s channel:%d Mode:%d", deviceId.c_str(), channel, Device_LRN_Mode);
	unlockDevice(SenderAdress);
	if (!isCommStatusOk())
		return;
	RemoteSetLearnMode(SenderAdress, channel - 1, Device_LRN_Mode);
}
static void CopyLine(Json::Value& root, int ii)
{
	root["result"][ii]["DeviceID"] = root["result"][ii - 1]["DeviceID"];
	root["result"][ii]["Profile"] = root["result"][ii - 1]["Profile"];
	root["result"][ii]["Manufacturer"] = root["result"][ii - 1]["Manufacturer"];
	root["result"][ii]["Manufacturer_name"] = root["result"][ii - 1]["Manufacturer_name"];
	root["result"][ii]["TeachInStatus"] = root["result"][ii - 1]["TeachInStatus"];
	root["result"][ii]["EnoTypeName"] = root["result"][ii - 1]["EnoTypeName"];
	root["result"][ii]["Description"] = root["result"][ii - 1]["Description"];
	root["result"][ii]["Unit"] = root["result"][ii - 1]["Unit"];
	root["result"][ii]["asLinkTable"] = root["result"][ii - 1]["asLinkTable"];
}
static int addNodeToList(Json::Value& root, std::string& Name, std::string& Type, std::string& SubType, std::string& SwitchType, int ii)
{
//	if (!Name.empty())
	{
		root["result"][ii]["Name"] = Name;
		root["result"][ii]["Type"] = Type;
		root["result"][ii]["SubType"] = SubType;
		root["result"][ii]["SwitchType"] = SwitchType;
		//		root["result"][ii]["TypeName"]   = RFX_Type_SubType_Desc(atoi(Type.c_str()), atoi(SubType.c_str()));
		root["result"][ii]["TypeName"] = RFX_Type_Desc(atoi(Type.c_str()), 1);
		//if new line 
		if (!root["result"][ii]["DeviceID"].isString())
			CopyLine(root, ii);
		ii++;
	}
	return ii;
}
static int addMayBeaNode(int devId, int devUnit, int devType, int ii, std::string& HardwareID, Json::Value& root, char* DeviceIdfmt)
{
	std::vector<std::vector<std::string> > result2;
	char sId[32];
	snprintf(sId, sizeof(sId), DeviceIdfmt, devId);
	result2 = m_sql.safe_query("SELECT  Name, Type, SubType, SwitchType, Unit FROM DeviceStatus    where ( DeviceId ==  '%s') and ( Unit ==  %d ) and (HardwareID == %s ) and ( Type == %d )", sId, devUnit, HardwareID.c_str(), devType);
	if (result2.size() > 0) {
		ii = addNodeToList(root, result2[0][0], result2[0][1], result2[0][2], result2[0][3], ii);
	}
	return ii;
}
void CEnOceanRMCC::GetNodeList(std::string& HardwareID, Json::Value& root)
{
	root["status"] = "OK";
	root["title"] = "EnOceanNodes";
	std::vector<std::vector<std::string> > result;
	//	result = m_sql.safe_query("SELECT printf(\"%%.08X\", e.NodeId  ) AS HexNodeId  , E.Rorg, E.Func   , E.Type, E.ManufacturerId, E.nValue , D.Name, D.Type, d.SubType, d.SwitchType, d.Unit, length(D.DeviceId) as len FROM EnOceanNodes AS E LEFT OUTER JOIN DeviceStatus   as d ON( (instr(HexNodeId , D.DeviceId) + len ) == 9  ) and (D.HardwareID == E.HardwareID) ");
	result = m_sql.safe_query("SELECT printf(\"%%.08X\", e.NodeId  ) AS HexNodeId  , E.Rorg, E.Func   , E.Type, E.ManufacturerId, E.nValue , D.Name, D.Type, d.SubType, d.SwitchType, d.Unit, length(D.DeviceId) as len FROM EnOceanNodes AS E LEFT OUTER JOIN DeviceStatus   as d ON( (instr(HexNodeId , D.DeviceId) == 1) and (len==8) or (instr(HexNodeId , D.DeviceId) == 2) and (len==7 ) ) and (D.HardwareID == E.HardwareID) ");
	//                                                                  0            1       2          3        4                5          6       7         8        9            10
	if (result.size() > 0)
	{
		std::vector<std::vector<std::string> >::const_iterator itt;
		int ii = 0;
		for (itt = result.begin(); itt != result.end(); ++itt)
		{
			std::vector<std::string> sd = *itt;
			{
				int lastii = ii ;
				root["result"][ii]["DeviceID"] = sd[0];
				int rorg = atoi(sd[1].c_str());
				int func = atoi(sd[2].c_str());
				int type = atoi(sd[3].c_str());
				uint32_t profil = RorgFuncTypeToProfile(rorg, func, type);
				root["result"][ii]["Profile"] = IntToString(rorg, 2) + IntToString(func, 2) + IntToString(type, 2);
				root["result"][ii]["Manufacturer"] = sd[4];
				std::string man = GetManufacturerName(atoi(sd[4].c_str()));
				if (man[0] == '>') man = "Unkown";
				root["result"][ii]["Manufacturer_name"] = man;
				root["result"][ii]["TeachInStatus"] = NodeInfo::teachin_mode_string((TeachinMode)std::stoi(sd[5]));
				std::string typ = GetEEPLabel(rorg, func, type);
				if (typ[0] == '>') typ = "Unkown";
				root["result"][ii]["EnoTypeName"] = typ;
				root["result"][ii]["Description"] = GetEEPDescription(rorg, func, type);
				if (sd[10].empty())       sd[10] = "1";
				root["result"][ii]["Unit"] = sd[10];
				root["result"][ii]["asLinkTable"] = NodeInfo::asLinkTable(profil);
				root["result"][ii]["Name"] = "";
				root["result"][ii]["Type"] = "";
				root["result"][ii]["SubType"] = "";
				root["result"][ii]["SwitchType"] = "";
				root["result"][ii]["TypeName"] = "";
				if (!sd[6].empty())
					ii = addNodeToList(root, sd[6], sd[7], sd[8], sd[9], ii);
				uint32_t    NodeId = std::stoul(sd[00].c_str(), 0, 16);
				uint8_t ID0 = (NodeId) & 0xFF;
				uint8_t ID1 = (NodeId >> 8) & 0xFF;
				uint8_t ID2 = (NodeId >> 16) & 0xFF;
				uint8_t ID3 = (NodeId >> 24) & 0xFF;
				//for temp --> deviceId = BYTE2 BYTE1 Unit=BYTE0 
				ii = addMayBeaNode((NodeId >> 8) & 0xFFFF, ID0, pTypeTEMP, ii, HardwareID, root, "%d");
				//for tempHum   --> deviceId = BYTE2 BYTE1 Unit=0 
				ii = addMayBeaNode((NodeId >> 8) & 0xFFFF, 0, pTypeTEMP_HUM, ii, HardwareID, root, "%d");
				//pTypeHUM
				ii = addMayBeaNode((NodeId >> 8) & 0xFFFF, 1, pTypeHUM, ii, HardwareID, root, "%d");
				//RFXSENSOR : ID1.ID0 :	sprintf(szTmp, "%d", pResponse->RFXSENSOR.id); = ID1 // Unit = (pResponse->RFXSENSOR.rssi << 4) | pResponse->RFXSENSOR.filler;  ID0
				ii = addMayBeaNode(ID1, ID0, pTypeRFXSensor, ii, HardwareID, root, "%d");
				//pTypeFan : ID3.ID2.ID1.% // sprintf(szTmp, "%02X%02X%02X", pResponse->FAN.id1, pResponse->FAN.id2, pResponse->FAN.id3);
				ii = addMayBeaNode((NodeId >> 8), 0, pTypeFan, ii, HardwareID, root, "%06X");
				//%.ID2.ID1.% : pTypeRFXMeter	sprintf(szTmp, "%d", (pResponse->RFXMETER.id1 * 256) + pResponse->RFXMETER.id2);
				ii = addMayBeaNode((NodeId >> 8) & 0xFFFF, 0, pTypeRFXMeter, ii, HardwareID, root, "%d");
				//pTypeAirQuality device = ID2 unit=id1
				ii = addMayBeaNode(ID2, ID1, pTypeAirQuality, ii, HardwareID, root, "%d");
				//no device found
				if(ii == lastii)
					ii = addNodeToList(root, sd[6], sd[7], sd[8], sd[9], ii);

			}
		}
	}
}
void CEnOceanRMCC::GetLinkTableList(Json::Value& root, std::string& DeviceIds, unsigned int id_chip)
{
	root["status"] = "OK";
	root["title"] = "EnOceanLinkTable";
	unsigned int  DeviceId = DeviceIdStringToUInt(DeviceIds);
	/*
	addLinkTableEntry(0x1a65428, 0, 0xD0500, 0xABCDEF, 1);
		addLinkTableEntry(0x1a65428, 1, 0xD0500, 0x1a65428, 1);
		addLinkTableEntry(0x1a65428, 2, 0xD0500, 0x1234567, 1);
		addLinkTableEntry(0x1a65428, 3, 0xD0500, 0x2345678, 1);
	*/
	NodeInfo* sensors = m_nodes.search(DeviceId);
	if (sensors) {
		//read link table if not readed
		if ((sensors->asLinkTable()) && (sensors->getTableLinkMaxSize() == 0))
			getLinkTable(DeviceId);
		for (int entry = 0; entry < sensors->MaxSize; entry++)
		{
			root["result"][entry]["Profile"] = string_format("%06X", sensors->LinkTable[entry].Profile);
			uint32_t SenderId = sensors->LinkTable[entry].SenderId;
			root["result"][entry]["SenderId"] = string_format("%08X", SenderId);
			root["result"][entry]["Channel"] = string_format("%d", sensors->LinkTable[entry].Channel);
			root["result"][entry]["Config"] = string_format("%d", sensors->LinkTable[entry].Config);
			/*if (CheckIsGatewayAdress(SenderId))
			{
				int unitCode = GetOffsetAdress(SenderId);
				SenderId = GetSenderIdFromAddress(unitCode);
			}*/
			if (sensors->LinkTable[entry].Profile != 0xFFFFFF)
			{
				if (SenderId == id_chip)
					root["result"][entry]["Name"] = GetDeviceNameFromId(DeviceId);
				else
					root["result"][entry]["Name"] = GetDeviceNameFromId(SenderId);
			}
			else
				root["result"][entry]["Name"] = "";
		}
	}
}
unsigned int CEnOceanRMCC::GetLockCode()
{
	std::string scode;
	unsigned int code = 0;
	bool found = m_sql.GetPreferencesVar("EnOceanLockCode", scode);
	if (found)
		code = DeviceIdStringToUInt(scode);
	return code;
}
void  CEnOceanRMCC::SetLockCode(std::string scode)
{
	m_sql.UpdatePreferencesVar("EnOceanLockCode", scode);
}
void CEnOceanRMCC::setRemote_man_answer(int premote_man_answer, char* message, unsigned int senderId)
{
	T_RMCC_RESULT remote_man_answer;
	remote_man_answer.function = premote_man_answer;
	remote_man_answer.message = message;
	remote_man_answer.senderId = senderId;
	std::lock_guard<std::mutex> l(m_RMCC_Mutex);
	m_RMCC_queue.push_back(remote_man_answer);
};
T_RMCC_RESULT CEnOceanRMCC::getRemote_man_answer()
{
	T_RMCC_RESULT remote_man_answer;
	remote_man_answer.function = 0;
	//if a response as been received
	if (m_RMCC_queue.size() > 0)
	{
		std::lock_guard<std::mutex> l(m_RMCC_Mutex);
		remote_man_answer = m_RMCC_queue.front();
		m_RMCC_queue.erase(m_RMCC_queue.begin());
	}
	return remote_man_answer;
};
int  CEnOceanRMCC::getRemote_man_answer_queue_size()
{
	//if a response as been received
	return  m_RMCC_queue.size();
};
void CEnOceanRMCC::clearRemote_man_answer()
{
	//if a response as been received
	if (m_RMCC_queue.size() > 0)
	{
		std::lock_guard<std::mutex> l(m_RMCC_Mutex);
		m_RMCC_queue.clear();
	}
};
T_RMCC_RESULT CEnOceanRMCC::waitRemote_man_answer(int premote_man_answer, int timeout) //return true if time out
{
	//	clearRemote_man_answer();
	timeout= getRCtimeoutSec(premote_man_answer);
	std::string logStr;
	T_RMCC_RESULT remote_man_answer;
	//    Log(LOG_NORM, "Wait: Waiting ,%02X:%s ",premote_man_answer, RMCC_Cmd_Desc(premote_man_answer) );
	logStr = std_format("Wait: %02X:%s / ", premote_man_answer, RMCC_Cmd_Desc(premote_man_answer));
	remote_man_answer.function = 0;
	setCommStatus(COM_OK);
	timeout *= 10;
	sleep_milliseconds(100);
	while ((remote_man_answer.function != premote_man_answer) && (timeout > 0))
	{
		if (getRemote_man_answer_queue_size() > 0)
		{
			remote_man_answer = getRemote_man_answer();
			//            Log(LOG_NORM, "Wait: Reading ,%02X:%s ,%d Time:%d ms",remote_man_answer.function, RMCC_Cmd_Desc(remote_man_answer.function),getRemote_man_answer_queue_size(), timeout*100);
			if (remote_man_answer.function != 0xFF)
				logStr += std_format("Read: %02X:%s ,%d Time:%d ms", remote_man_answer.function, RMCC_Cmd_Desc(remote_man_answer.function), getRemote_man_answer_queue_size(), timeout * 100);
		}
		else
		{
			sleep_milliseconds(100);
			timeout--;
			remote_man_answer.function = 0;
		}
	}
	if ((remote_man_answer.function == 0) || (timeout == 0)) {
		setCommStatus(COM_TIMEOUT);
		//		Log(LOG_NORM, "Wait: TIMEOUT waiting answer %04X :%s ", premote_man_answer, RMCC_Cmd_Desc(premote_man_answer));
//		logStr += std_format(": TIMEOUT waiting answer %04X :%s ", premote_man_answer, RMCC_Cmd_Desc(premote_man_answer));
		logStr += std_format(": TIMEOUT");
	}
	else
		//        Log(LOG_NORM, "Wait: Recving OK " );
		logStr += std_format(": OK ");
	Log(LOG_NORM, logStr.c_str());
	return remote_man_answer;
}
void CEnOceanRMCC::setCommStatus(T_COM_STATUS status)
{
	m_com_status = status;
}
T_COM_STATUS CEnOceanRMCC::getCommStatus()
{
	return m_com_status;
}
bool  CEnOceanRMCC::isCommStatusOk()//return true if comm status = ok
{
	return (m_com_status == COM_OK);
}
//return true if ok 
bool CEnOceanRMCC::unlockDevice(unsigned int deviceId, bool testUnLockTimeoutBeforeSend)
{
	T_RMCC_RESULT res;
	res.function = 0;
	unsigned int code = GetLockCode();
	if (deviceId == BROADCAST_ID) {
		Unlock(BROADCAST_ID, code);
	}
	else {
		NodeInfo* sensors = m_nodes.search(deviceId);
		if ((testUnLockTimeoutBeforeSend && sensors->UnLockTimeout())
			|| (!testUnLockTimeoutBeforeSend)
			)
		{
				Unlock((deviceId), code);
				if (isCommStatusOk()){
					//answer received : some device need time after unlock ??
					sleep_milliseconds(1000);
					sensors->SetUnLockTimeout();
				}
		}
		else {
			Log(LOG_NORM, "unlock device %08X timeout:%d sec ", deviceId, (GetClockTicks() - sensors->TimeLastUnlockInMs) / 1000);
		}
	}
	return isCommStatusOk();
}
