#include "stdafx.h"


#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <ctime>

#include "../main/Logger.h"
#include "../main/mainworker.h"

#include "hardwaretypes.h"
#include "EnOceanESP3.h"
#include "EnOceanRawValue.h"

#include "EnOceanRMCC.h"
//#include "eep-d2.h"
// #include "EnOceanXmlReader.h"

//#define USE_PROFIL
#ifdef USE_PROFIL
#include "EnOceanProfil.h"
#endif
//#include "EnOceanEepXml.h"

using namespace enocean;


//---------------------------------------------------------------------------
// ConvertHexaStringToArray    : Convertit une chaîne hexadécimale ascii en un tableau binaire
// Input  Arg.  : Chaîne hexadécimale ascii
//                Adresse du tableau
//                Adresse de rangement de la longueur du tableau
// Output       : true  -> Conversion réussie
//                false -> Echec
// Remark       : Le tableau de destination doit être suffisement dimensionné
//                La conversion s'arrête sur tout caractère non hexa
//---------------------------------------------------------------------------
int HexToBin(char c)
{
	int h = (unsigned char)(c - '0');
	if (h > 9)
		h = (unsigned char)(h - 7);
	if (h > 15)
		h = (unsigned char)(h - 32);
	if (h > 15)
		return 0;
	return h;

}
bool ConvertHexaStringToArray(const char * st, unsigned char bin[], int  *trame_len)
{
	std::string x;
	int  h, l;
	int i = 0;
	int index = 0;
	/* -------------- */
	*trame_len = 0;

	while (st[index] != 0)
	{
		if (st[index] != ' '){

		    if (st[index] == 'b') {
			    index++;
			    h = 0;
			    while ((st[index] == '0') || (st[index] == '1'))
			    {
				    h = h << 1;
				    if ((st[index] == '1'))
					    h = h + 1;
				    index++;
			    }

			    bin[i++] = (unsigned char)(h);
		    }

		    else
		    {
			    h = HexToBin(st[index]);

			    index++;
			    if (st[index] != ' ')
			    {

				    l = HexToBin(st[index]);
				    bin[i++] = (unsigned char)((h << 4) + l);
			    }
			    else
				    bin[i++] = (unsigned char)(h);
			    index++;
		    }
        }

		if (st[index] == ' ')
			index++;
	}
	*trame_len = i;
	return true;
};

#define 	PACKET_RADIO_ERP1 0x01
// Radio telegram

void TestData(ESP3_RORG_TYPE rorg, unsigned int sID,  unsigned char status , enocean::T_DATAFIELD * OffsetDes, ...)
{

	va_list value;
     uint8_t mBuffer[2014];
	unsigned char *buff = mBuffer;

	*buff++ = rorg;
	/* Initialize the va_list structure */
	va_start(value, OffsetDes);
	uint32_t total_bytes = SetRawValues(buff, OffsetDes, value);
	va_end(value);
	buff += total_bytes;
	//add adress
	*buff++ = (sID >> 24) & 0xff;		// Sender ID
	*buff++ = (sID >> 16) & 0xff;
	*buff++ = (sID >> 8) & 0xff;
	*buff++ = sID & 0xff;
	*buff++ = status ; //status

	CEnOceanESP3 *pEnocean = dynamic_cast <CEnOceanESP3*>(m_mainworker.GetHardware(   m_mainworker.FindDomoticzHardwareByType(HTYPE_EnOceanESP3)  ));

	uint16_t mdatalen = 1+total_bytes+4+1 ;
	pEnocean->ParseESP3Packet(PACKET_RADIO_ERP1, mBuffer, mdatalen, &mBuffer[mdatalen], 0);

}
void TestData(char * sdata, uint8_t pType,  const char * optData)
{
    char lBuffer[2014];
    uint8_t mBuffer[2014];

    //copy to ram 
    strncpy(lBuffer,sdata,sizeof(lBuffer)-1 );
    sdata=lBuffer;
    //optionnal is separated by : P1 XX XX XX : OO OO OO
    //get packet type : P1 --> PACKET_RADIO_ERP1
    
    if (sdata[0] == 'P')
    {
        pType = (sdata[1] - '0');
        sdata +=2 ;
    }
    char* pPoint = strchr(sdata,':');
    if(pPoint)
    {
        *pPoint = 0 ;
        optData = pPoint+1;
    }
    int  trame_len;
	ConvertHexaStringToArray(sdata, mBuffer, &trame_len);
    uint16_t mdatalen=trame_len;
	ConvertHexaStringToArray(optData, &mBuffer[mdatalen], &trame_len);
    uint8_t moptionallen=trame_len;

	CEnOceanESP3 *pEnocean = dynamic_cast <CEnOceanESP3*>(m_mainworker.GetHardware(   m_mainworker.FindDomoticzHardwareByType(HTYPE_EnOceanESP3)  ));

	pEnocean->ParseESP3Packet(pType, mBuffer, mdatalen, &mBuffer[mdatalen], moptionallen);
}
void testParsingData(int sec_counter)
{
    unsigned char vldData[4];
    vldData[0]=0x12;
    vldData[1]=0x34;
    vldData[2]=0x56; 
    vldData[3]=0x78;

//   int   cmd = GetRawValue(vldData, 24 ,  4 ) ; Log(LOG_NORM,"%x",cmd); if (cmd != 0x07) Log(LOG_NORM,"error %x\n",cmd);
//	     cmd = GetRawValue(vldData, 28, 4);     Log(LOG_NORM,"%x",cmd); if (cmd != 0x08) Log(LOG_NORM,"error %x\n",cmd);
//		 cmd = GetRawValue(vldData, 21, 3);;    Log(LOG_NORM,"%x",cmd); if (cmd != 0x06) Log(LOG_NORM,"error %x\n",cmd);
//		 cmd = GetRawValue(vldData, 4, 8);;     Log(LOG_NORM,"%x",cmd); if (cmd != 0x23) Log(LOG_NORM,"error %x\n",cmd);
//		 cmd = GetRawValue(vldData, 4, 16);;    Log(LOG_NORM,"%x",cmd); if (cmd != 0x2345) Log(LOG_NORM,"error %x\n",cmd);
//		 cmd = GetRawValue(vldData, 4, 15);;    Log(LOG_NORM,"%x",cmd); if (cmd != 0x11A2) Log(LOG_NORM,"error %x\n",cmd);
//
//memset(vldData,0,4) ; 
//SetRawValue(vldData , 1 , 0, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 2 , 0, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 3 , 0, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 4 , 0, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 15 , 0, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 0x55 , 4, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 0x55 , 24, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 0x55 , 28, 4 ); dumpPacket(vldData,4 );
//SetRawValue(vldData , 0x55 , 12, 8 ); dumpPacket(vldData,4 );
//memset(vldData,0,4) ; 
//SetRawValue(vldData , 0x55 , 13, 8 ); dumpPacket(vldData,4 );

//				if (sec_counter == 1)	TestData("P2 00 10 00 00 00 00 "); // base adresse 
    
    //			if (sec_counter == 5)	TestData("D5 0 01 02 03 04 00 ");
	//			if (sec_counter == 25)	TestData("D5 1 01 02 03 04 00 ");

//				if (sec_counter == 5)	TestData("D2 04 60 80 01 A6 54 28 00 "); //vld ch 0
//				if (sec_counter == 15)	TestData("D2 04 60 E4 01 A6 54 28 00 "); //vld ch 0

	//			if (sec_counter == 5)	TestData("D2 04 61 80 01 A6 54 28 00 "); // vld ch1
	//			if (sec_counter == 15)	TestData("D2 04 61 E4 01 A6 54 28 00 "); // vld ch1


//				if (sec_counter == 2)	TestData("D4 A0 02 46 00 12 01 D2 01 A6 54 27 00 "); // ute D2 01 12 RoolerShutter
//				if (sec_counter == 2)	TestData("D4 A0 02 46 00 0C 01 D2 01 A6 54 29 00 "); // ute D2 01 0C PilotWire

//				if (sec_counter == 6)	TestData("D4 A0 02 46 00 12 01 D2 01 A6 54 28 00 ", 1, "01 FF FF FF FF 2D 00");// ute opt data D2 01 12 : switch 2 channel

	//			if (sec_counter == 2)	TestData("A5 02 00 00 00 FF 99 DF 01 30 "); //4BS teach in variation 1 : noo eep

//	if (sec_counter == 2)	TestData("A5 b00001000 b00001000 46 b10000000 01 02 03 04 30 "); //4BS teach in variation 2 :  eep func A-02-01
//														        | eep
//													         | manufactuer ID nodon = 46

//	if (sec_counter == 3)	TestData("A5 00         00       46 b00001000 01 02 03 04 30 "); //4BS data  :  eep func A-02-01


	//function 6 bit  type:7 bits manufacyurer 11

//	if (sec_counter == 2)	TestData("A5 b00001100 b01010000 46 b10000000 00 00 00 01 30 "); //4BS teach in variation 2 :  eep func D2-03-0A
//												         | manufactuer ID nodon = 46
//														        | eep
	/*
	if (sec_counter == 1) {

		//send command stop si rappuie
		sendVld(0x1234, D20500_CMD2, 1, 2, END_ARG_DATA);
		sendVld(0x5678, D20500_CMD1, 100, 127, 0, 0, 1, 1, END_ARG_DATA);
		///erreur arg trop
		sendVld(0x1234, D20500_CMD2, 1, 2,  3 , END_ARG_DATA);
		sendVld(0x1234, D20500_CMD2, 1,  END_ARG_DATA);
	}
	*/

//ESP3_RORG rorg, unsigned sID,  unsigned char status , T_DATAFIELD * OffsetDes, ...
/*
	if (sec_counter == 2) {

		//4BS teachin D2-03-0A 

		TestData(RORG_4BS, 0x12345678, 0, TEACHIN_4BS, 0x03,0x0A, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);

		//data bat=50 button=1
		TestData(RORG_VLD, 0x12345678, 0, D2030A, 50, 1, END_ARG_DATA);
	}
*/
//test A5
/*
       if (sec_counter == 20000) {

		//4BS teachin A5-02-01 : temperature
        TestData(RORG_4BS, 0x123401 , 0, TEACHIN_4BS, 0x02,0x01, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);
		TestData(RORG_4BS, 0x123401, 0, A50201_CMD1, 1 , 20 , END_ARG_DATA);

		//4BS teachin A5-04-01 : temperature hum
        TestData(RORG_4BS, 0x123501 , 0, TEACHIN_4BS, 0x04,0x01, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);
		TestData(RORG_4BS, 0x123501, 0, A50401_CMD1, 1 , 20 , 50 , 1,  END_ARG_DATA);

        //{ 0xA5, 0x07, 0x01, "Occupancy Sensor                                                                " , "Occupancy with Supply voltage monitor                                           " },
        TestData(RORG_4BS, 0x123601 , 0, TEACHIN_4BS, 0x07,0x01, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);
		TestData(RORG_4BS, 0x123601, 0, A50701_CMD1 , 1 , 10 , 20 , 1,  END_ARG_DATA);

        //{ 0xA5, 0x07, 0x02, "Occupancy Sensor  02                                                              " , "Occupancy with Supply voltage monitor                                           " },
        TestData(RORG_4BS, 0x123701 , 0, TEACHIN_4BS, 0x07,0x02, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);
		TestData(RORG_4BS, 0x123701, 0, A50702_CMD1 , 1 , 10 ,  1,  END_ARG_DATA);

        //{ 0xA5, 0x07, 0x03, "Occupancy Sensor  03                                                              " , "Occupancy with Supply voltage monitor                                           " }
        TestData(RORG_4BS, 0x123801 , 0, TEACHIN_4BS, 0x07,0x03, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);
		TestData(RORG_4BS, 0x123801, 0, A50703_CMD1 , 1 , 10 , 20,  1,  END_ARG_DATA);

        //{ 0xA5, 0x09, 0x04, function type :0x04 : CO2 Sensor                                                        " , "Occupancy with Supply voltage monitor                                           " }
        TestData(RORG_4BS, 0x123901 , 0, TEACHIN_4BS, 0x09,0x04, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);
		TestData(RORG_4BS, 0x123901, 0, A50904_CMD1 , 1 , 10 , 20, 30,1, 1,  END_ARG_DATA);

        //0xA5, 0x12, 0x00, "Counter"
        TestData(RORG_4BS, 0x123A01 , 0, TEACHIN_4BS, 0x12,0x00, 0x46, WITH_EEP, TEACHIN , END_ARG_DATA);
		TestData(RORG_4BS, 0x123A01, 0, A51200_CMD1 , 1 , 20 , 1 ,1, 0,  END_ARG_DATA);

	}
*/

//	if (sec_counter == 2)	TestData("D2 32 01 00 00 00 01 30 "); //VLD D2  eep func D2-03-0A baterie 50% button 1

//			if (sec_counter == 3)	remoteLearning(0x01A65428 , true ,LEARN_IN ); //4BS data  :  eep func A-02-01

//			if (sec_counter == 5)	remoteLearning(0x01A65428, false, LEARN_IN); //4BS data  :  eep func A-02-01
//			if (sec_counter == 2)	ping(0x01A65428); 
//			if (sec_counter == 1)	unlock(0x01A65428,1);
//			if (sec_counter == 2)	action(0x01A65428);
//			if (sec_counter == 3)	action(0x01A65428);
//			if (sec_counter == 3)	getProductId();

//			if (sec_counter == 10)	getLinkTableMedadata(0x01A65428);
//			if (sec_counter == 12)	getallLinkTable(0x01A65428,0,3);
//			if (sec_counter == 4)	queryFunction(0x01A65428);

//	const char * VLD_switch_uti				= "D4 A0 02 46 00 12 01 D2 01 A6 54 28 00 ""; //01 FF FF FF FF 33"; //teach-in request received from 01A65428 (manufacturer: 046). number of channels: 2, device profile: D2-01-12
//	const char * VLD_switch_status            = "D2 04 60 80 01 A6 54 28 00 "; // 01 FF FF FF FF 31";
   

//	const char * VLD_BLINDS_D2_05_00_uti = "D4 A0 01 46 00 00 05 D2 05 85 87 4A 00 ";//  -01 FF FF FF FF 30 "; // 09.031 EnOcean : teach - in request received from 0585874A(manufacturer: 046).number of channels : 1, device profile : D2 - 05 - 00
//    if (sec_counter == 2 )	TestData(VLD_BLINDS_D2_05_00_uti );


//	const char * button_left_up_pressed  = "F6 30 00 34 F8 FE 30";
//	const char * button_left_up_released = "F6 00 00 34 F8 FE 20";
//
//	const char * button_left_do_pressed  = "F6 10 00 34 F8 FE 30";
//	const char * button_left_do_released = "F6 00 00 34 F8 FE 20";
//
//	const char * button_right_up_pressed  = "F6 70 00 34 F8 FE 30";
//	const char * button_right_up_released = "F6 00 00 34 F8 FE 20";
//
//    const char * button_right_do_pressed  = "F6 50 00 34 F8 FE 30";
//	const char * button_right_do_released = "F6 00 00 34 F8 FE 20";

//    if (sec_counter == 5)	{ TestData(button_left_up_pressed); TestData(button_left_up_released); }
//    if (sec_counter == 5)	{ TestData(button_left_do_pressed); TestData(button_left_do_released); }
//    if (sec_counter == 5)	{ TestData(button_right_up_pressed); TestData(button_right_up_released); }
//    if (sec_counter == 5)	{ TestData(button_right_do_pressed); TestData(button_right_do_released); }

//			if (sec_counter == 5)	TestData("F6 10 01 A6 54 28 30 "); //rps switch up
//			if (sec_counter == 6)	TestData("F6 00 01 A6 54 28 20 "); //rps switch

//			if (sec_counter == 5)	TestData("F6 30 01 A6 54 28 30 "); //rps switch down
//			if (sec_counter == 5)	TestData("F6 00 01 A6 54 28 20 "); //rps switch


//if (sec_counter == 1)
//	  SendSelectorSwitch(int NodeID, uint8_t ChildID, std::string &sValue, std::string &defaultname,  int customImage,  bool bDropdown,  std::string &LevelNames,  std::string &LevelActions,  bool bHideOff,  std::string &userName)
//	  SendSelectorSwitch( 0x1234   , 1              ,"0"                 ,"Select1"                ,  0              ,  false         ,  "Off|Eco|Confor|Frost" ,  "00|10|20|30|40|50"      ,  true         , ""  );

	// 
// button gauche Up
// recv 01 PACKET_RADIO (07/07) F6 30 00 34 F8 FE 30 - 01 FF FF FF FF 36 00 Dest: 0xffffffff RSSI: 46
// RPS data: Sender id: 0x0034f8fe Status: 30 Data: 30 T21:0
// RPS N-Message message: 0x30 Node 0x0034f8fe RockerID: 0 ButtonID: 1 Pressed: 1 UD: 1 Second Rocker ID: 0 SecondButtonID: 0 SUD: 0 Second Action: 0
// message: 0x30 Node 0x0034f8fe UnitID: 02 cmd: 01
//
// recv 01 PACKET_RADIO (07/07) F6 00 00 34 F8 FE 20 - 01 FF FF FF FF 36 00 Dest: 0xffffffff RSSI: 46
// RPS data: Sender id: 0x0034f8fe Status: 20 Data: 00 T21:0
// RPS T21-Message message: 0x00 Node 0x0034f8fe ButtonID: 0 Pressed: 0 UD: 1
// message: 0x00 Node 0x0034f8fe UnitID: 00 cmd: 03
//
//button gauche Down
// recv 01 PACKET_RADIO (07/07) F6 10 00 34 F8 FE 30 - 01 FF FF FF FF 39 00 Dest: 0xffffffff RSSI: 43
// RPS data: Sender id: 0x0034f8fe Status: 30 Data: 10 T21:0
// RPS N-Message message: 0x10 Node 0x0034f8fe RockerID: 0 ButtonID: 0 Pressed: 0 UD: 1 Second Rocker ID: 0 SecondButtonID: 0 SUD: 0 Second Action: 0
// message: 0x10 Node 0x0034f8fe UnitID: 01 cmd: 01
//
// recv 01 PACKET_RADIO (07/07) F6 00 00 34 F8 FE 20 - 01 FF FF FF FF 39 00 Dest: 0xffffffff RSSI: 43
// RPS data: Sender id: 0x0034f8fe Status: 20 Data: 00 T21:0
// RPS T21-Message message: 0x00 Node 0x0034f8fe ButtonID: 0 Pressed: 0 UD: 1
// message: 0x00 Node 0x0034f8fe UnitID: 00 cmd: 03
//
//
// button droite Up
//recv 01 PACKET_RADIO (07/07) F6 70 00 34 F8 FE 30 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52
// RPS data: Sender id: 0x0034f8fe Status: 30 Data: 70 T21:0
// RPS N-Message message: 0x70 Node 0x0034f8fe RockerID: 1 ButtonID: 3 Pressed: 1 UD: 1 Second Rocker ID: 0 SecondButtonID: 0 SUD: 0 Second Action: 0
// message: 0x70 Node 0x0034f8fe UnitID: 04 cmd: 01
//
// recv 01 PACKET_RADIO (07/07) F6 00 00 34 F8 FE 20 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52
// RPS data: Sender id: 0x0034f8fe Status: 20 Data: 00 T21:0
// RPS T21-Message message: 0x00 Node 0x0034f8fe ButtonID: 0 Pressed: 0 UD: 1
// message: 0x00 Node 0x0034f8fe UnitID: 00 cmd: 03
//
// button droite down
// recv 01 PACKET_RADIO (07/07) F6 50 00 34 F8 FE 30 - 01 FF FF FF FF 34 00 Dest: 0xffffffff RSSI: 48
// RPS data: Sender id: 0x0034f8fe Status: 30 Data: 50 T21:0
// RPS N-Message message: 0x50 Node 0x0034f8fe RockerID: 1 ButtonID: 2 Pressed: 0 UD: 1 Second Rocker ID: 0 SecondButtonID: 0 SUD: 0 Second Action: 0
// message: 0x50 Node 0x0034f8fe UnitID: 03 cmd: 01
//
// recv 01 PACKET_RADIO (07/07) F6 00 00 34 F8 FE 20 - 01 FF FF FF FF 36 00 Dest: 0xffffffff RSSI: 46
// RPS data: Sender id: 0x0034f8fe Status: 20 Data: 00 T21:0
// RPS T21-Message message: 0x00 Node 0x0034f8fe ButtonID: 0 Pressed: 0 UD: 1
// message: 0x00 Node 0x0034f8fe UnitID: 00 cmd: 03
//
// Need Teach-In for 0585874A : position retour
// recv 01 PACKET_RADIO (0A/07) D2 0A 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 2E 00 Dest: 0xffffffff RSSI: 54
// recv 01 PACKET_RADIO (0A/07) D2 14 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 2E 00 Dest: 0xffffffff RSSI: 54
// recv 01 PACKET_RADIO (0A/07) D2 1E 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 2E 00 Dest: 0xffffffff RSSI: 54
// recv 01 PACKET_RADIO (0A/07) D2 28 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52
// recv 01 PACKET_RADIO (0A/07) D2 32 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52
// recv 01 PACKET_RADIO (0A/07) D2 3C 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52
// recv 01 PACKET_RADIO (0A/07) D2 46 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 2E 00 Dest: 0xffffffff RSSI: 54
// recv 01 PACKET_RADIO (0A/07) D2 50 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52
// recv 01 PACKET_RADIO (0A/07) D2 5A 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52
// recv 01 PACKET_RADIO (0A/07) D2 64 00 00 04 05 85 87 4A 00 - 01 FF FF FF FF 30 00 Dest: 0xffffffff RSSI: 52

const char * VLD_BLINDS_D2_05_00_position [] ={

"D2 00 00 00 04 05 85 87 4A 00 ",
"D2 0A 00 00 04 05 85 87 4A 00 ",
"D2 14 00 00 04 05 85 87 4A 00 ",
"D2 1E 00 00 04 05 85 87 4A 00 ",
"D2 28 00 00 04 05 85 87 4A 00 ",
"D2 32 00 00 04 05 85 87 4A 00 ",
"D2 3C 00 00 04 05 85 87 4A 00 ",
"D2 46 00 00 04 05 85 87 4A 00 ",
"D2 50 00 00 04 05 85 87 4A 00 ",
"D2 5A 00 00 04 05 85 87 4A 00 ",
"D2 64 00 00 04 05 85 87 4A 00 ",
};
/*
if ( (sec_counter >= 2) && (sec_counter <= 12) )	 
    TestData(VLD_BLINDS_D2_05_00_position[sec_counter-2]); 
*/

//VLD: senderID: 01A65428 Func : 01 Type : 12
// 
// recv 01 PACKET_RADIO (09/07) D2 04 60 E4 01 A6 54 28 00 - 01 FF FF FF FF 2D 00 Dest: 0xffffffff RSSI: 55
// VLD : 0x04 Node 0x01a65428 UnitID: 01 cmd: 01
// recv 01 PACKET_RADIO (09/07) D2 04 61 E4 01 A6 54 28 00 - 01 FF FF FF FF 2D 00 Dest: 0xffffffff RSSI: 55
// VLD : 0x04 Node 0x01a65428 UnitID: 02 cmd: 01
// 
// recv 01 PACKET_RADIO (09/07) D2 04 60 80 01 A6 54 28 00 - 01 FF FF FF FF 2D 00 Dest: 0xffffffff RSSI: 55
// VLD : 0x04 Node 0x01a65428 UnitID: 01 cmd: 00
// recv 01 PACKET_RADIO (09/07) D2 04 61 80 01 A6 54 28 00 - 01 FF FF FF FF 2D 00 Dest: 0xffffffff RSSI: 55
// VLD : 0x04 Node 0x01a65428 UnitID: 02 cmd: 00


//	const char * VLD_switch_B1_ON   = "D2 04 60 E4 01 A6 54 28 00";
//	 const char * VLD_switch_B2_ON  = "D2 04 61 E4 01 A6 54 28 00";
//	 const char * VLD_switch_B1_OFF = "D2 04 60 80 01 A6 54 28 00";
//	 const char * VLD_switch_B2_OFF = "D2 04 61 80 01 A6 54 28 00";


// ping
//	55 00 0F 07 01 2B         C5 80 00 7F F0 06 00 00 00 00 00 00 00 00 8F        03 01 A6 54 28 FF 00 83
//response
//const char *pingAnswer = "P7 06 06 07 FF D2 01 12 2D : 05 01 33 BE 01 A6 54 28 2D 00 34";

//if (sec_counter == 2 )	TestData(pingAnswer );

// queryId 
//		    queryid send cmd EEP : 00000000 Mask : 0
//			Send :                                  : 55 00 0F 07 01 2B C5 80 01 FF F0 04 00 00 00 00 00 00 00 00 8F 03 FF FF FF FF FF 00 EE
//const char * queryIdAnswer = "P7 06 04 07 FF D2 05 00 : FF FF FF FF 05 85 87 4A 3D 00 ";
//if (sec_counter == 2 )	    TestData(queryIdAnswer );

//get product id  cmd 227 
//55 00 0F 07 01 2B         C5 80 00 7F F2 27 00 00 00 00 00 00 00 00 8F        03 FF FF FF FF FF 00             55
//reponse  manu 46 procuct ref 00010003
//const char * getproductidAnswer =  "P7 08 27 07 FF 00 46 00 01 00 03 : FF FF FF FF 01 A6 54 28 2C 00     B3 ";
//if (sec_counter == 2 )	    TestData(getproductidAnswer );

//get link table medatadate cmd 0210 : taille current/ max  table
//55 00 0F 07 01 2B         C5 40 00 7F F2 10 00 00 00 00 00 00 00 00 8F        03 01 A6 54 28 FF 00  AD
//response curren size 03 max x18=24
//const char * getlinktablemedatadateAnswer = "P7 08 10 07 FF 50 00 00 03 18 : FF FF FF FF 01 A6 54 28 2D 00 08";
//if (sec_counter == 2 )	    TestData(getlinktablemedatadateAnswer );


//get all link table
//55 00 0F 07 01 2B         C5 40 01 FF F2 11 00 00 17 00 00 00 00 00 8F        03 01 A6 54 28 FF 00 56
//response                                        ID DEVICE ID   PROFILE  CHN
//                          0  1    2  3     4    5  6  7  8  9  10 11 12 13
const char * getalllinktableAnswer[] = {
"P7 08 11   07 FF    00   00 FF 99 DF 01 D5 00 01 00   01 FF 99 DF 02 F6 02 01 00   02 FF 99 DF 02 F6 02 01 01    :   FF FF FF FF 01 A6 54 28 2E 00 FB",
"P7 08 11   07 FF    00   03 00 00 00 00 FF FF FF 00   04 00 00 00 00 FF FF FF 00   05 00 00 00 00 FF FF FF 00    :   FF FF FF FF 01 A6 54 28 2E 00 8F",
"P7 08 11   07 FF    00   06 00 00 00 00 FF FF FF 00   07 00 00 00 00 FF FF FF 00   08 00 00 00 00 FF FF FF 00    :   FF FF FF FF 01 A6 54 28 2E 00 DE",
"P7 08 11   07 FF    00   09 00 00 00 00 FF FF FF 00   0A 00 00 00 00 FF FF FF 00   0B 00 00 00 00 FF FF FF 00    :   FF FF FF FF 01 A6 54 28 2E 00 F4",
"P7 08 11   07 FF    00   0C 00 00 00 00 FF FF FF 00   0D 00 00 00 00 FF FF FF 00   0E 00 00 00 00 FF FF FF 00    :   FF FF FF FF 01 A6 54 28 2E 00 E1",
"P7 08 11   07 FF    00   0F 00 00 00 00 FF FF FF 00   10 00 00 00 00 FF FF FF 00   11 00 00 00 00 FF FF FF 00    :   FF FF FF FF 01 A6 54 28 2E 00 BC",
"P7 08 11   07 FF    00   15 00 00 00 00 FF FF FF 00   16 00 00 00 00 FF FF FF 00   17 00 00 00 00 FF FF FF 00    :   FF FF FF FF 01 A6 54 28 2E 00 66",
};
//if (sec_counter == 2 )	    for (int i=0;i< 7;i++) TestData(getalllinktableAnswer[i] );

//query function
//55 00 0F 07 01 2B	C5 80 00 7F F0 07 00 00 00 00 00 00 00 00 8F 				03 01 A6 54 28 FF 00     8D  opt 7
//const char * queryfunctionAnswer = "P7 06 07 07 FF 02 24 07 FF 02 27 07 FF 02 20 07 FF 02 10 07 FF 02 11 07 FF 02 12 07 FF 02 30 07 FF 02 31 07 FF 02 32 07 FF 02 33 07 FF 02 26 07 FF 00 00 00 00    :  FF FF FF FF 01 A6 54 28 2C 00     2E ";
//if (sec_counter == 2 )	    TestData( queryfunctionAnswer);

//const char * VLD_D2_CMD_0D   = "D2 0D 12 34 01 A6 54 28 00";
//if (sec_counter == 2 )	    TestData(VLD_D2_CMD_0D );

}

void dumpPacket(uint8_t *data, uint16_t datalen)
{
std::stringstream sstr;


	for (int i = 0; i < datalen; i++)
			sstr << ",0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (uint32_t) data[i];

//	return sstr.str();
    _log.Debug(DEBUG_HARDWARE, "Read: %s", sstr.str().c_str());
}

void updateCrc(uint8_t * buffer, int len )
{
		
		uint8_t crc = buffer[len-1];
        //ignore 
        if (len>14)
		//if (crc==0)
        {
			//compute crc
			uint16_t datalen = buffer[2];
			uint8_t optdatalen = buffer[3];
			uint8_t packettype = buffer[4];
			uint8_t *data  = &buffer[6];
			uint8_t *optdata  = &buffer[6+datalen];
			std::string sdata = CEnOceanESP3::FormatESP3Packet( packettype, data,  datalen, optdata,  optdatalen);
			buffer[len-1]= sdata[len-1];
		}
}


