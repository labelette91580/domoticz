#ifndef CEnOceanRMCC_H
#define	CEnOceanRMCC_H

#include <json/json.h>
#include "ASyncSerial.h"
#include "DomoticzHardware.h"
#include "EnOceanEEP.h"
#include "EnOceanSensors.h"

#include <string>

#define RMCC_ACK_TIMEOUT			5
#define RMCC_GETPRODUCTID_TIMEOUT	10
#define RC_PACKET_RESPONSE 255
#define RMCC_NB_RETRY				2

#define ResetConfigurationParameters   (1<<7)
#define ResetSetIinboundLinkTable      (1<<6)
#define ResetOutboundLinkTable         (1<<5)
#define ResetToDefaultsCst             (0xE0 )


#define BROADCAST_ID 0xFFFFFFFF
typedef enum
{
	UNLOCK = 0x001, //Unlock
	LOCK = 0x002, //Lock
	SETCODE = 0x003, //Set code
	QUERYID = 0x004, //Query ID
	QUERYID_ANSWER = 0x604, //Query ID answer
	QUERYID_ANSWER_EXT = 0x704, //! Ext Query Id Answer
	ACTION = 0x005, //Action
	PING = 0x006, //Ping
	PING_ANSWER = 0x606, //Ping answer
	QUERY_FUNCTION = 0x007, //Query function
	QUERY_FUNCTION_ANSWER = 0x607, //Query function answer
	QUERY_STATUS = 0x008, //Query status
	QUERY_STATUS_ANSWER = 0x608, //Query status answer
	REMOTE_LEARNIN = 0x201, //Remote learn in
	REMOTE_FLASH_WRITE = 0x203, //Remote flash write
	REMOTE_FLASH_READ = 0x204, //Remote flash read
	REMOTE_FLASH_READ_ANSWER = 0x804, //Remote flash read answer
	SMARTACK_READ = 0x205, //SmartACK read
	SMARTACK_READ_MAILBOX_ANSWER = 0x805, //SmartACK read mailbox answer
	SMARTACK_READ_LEARNED_SENSOR_ANSWER = 0x806, //SmartACK read learned sensor answer
	SMARTACK_WRITE = 0x206, //SmartACK write
	RC_ACK = 0x240, //Remote Commissioning Acknowledge
	RC_GET_METADATA = 0x210, //Get Link Table Metadata Query
	RC_GET_METADATA_RESPONSE = 0x810, //Get Link Table Metadata Response
	RC_GET_TABLE = 0x211, //Get Link Table Query
	RC_GET_TABLE_RESPONSE = 0x811, //Get Link Table Response
	RC_SET_TABLE = 0x212, //Set Link Table Query
	RC_GET_GP_TABLE = 0x213, //Get Link Table GP Entry Query
	RC_GET_GP_TABLE_RESPONSE = 0x813, //Get Link Table GP Entry Response
	RC_SET_GP_TABLE = 0x214, //Set Link Table GP Entry Query
	RC_SET_LEARN_MODE = 0x220, //Remote Set Learn Mode
	RC_TRIG_OUTBOUND_TEACH_REQ = 0x221, //Trigger Outbound Remote Teach Request
	RC_GET_DEVICE_CONFIG = 0x230, //Get Device Configuration Query
	RC_GET_DEVICE_CONFIG_RESPONSE = 0x830, //Get Device Configuration Response
	RC_SET_DEVICE_CONFIG = 0x231, //Set Device Configuration Query
	RC_GET_LINK_BASED_CONFIG = 0x232, //Get Link Based Configuration Query
	RC_GET_LINK_BASED_CONFIG_RESPONSE = 0x832, //Get Link Based Configuration Response
	RC_SET_LINK_BASED_CONFIG = 0x233, //Set Link Based Configuration Query
	RC_APPLY_CHANGES = 0x226, //Apply Changes Command
	RC_RESET_TO_DEFAULTS = 0x224, //Reset to Defaults
	RC_RADIO_LINK_TEST_CONTROL = 0x225, //Radio Link Test Control
	RC_GET_PRODUCT_ID = 0x227, //Get Product ID Query
	RC_GET_PRODUCT_RESPONSE = 0x827, //Get Product ID Response
	RC_GET_REPEATER_FUNCTIONS = 0x250, //Get Repeater Functions Query
	RC_GET_REPEATER_FUNCTIONS_RESPONSE = 0x850, //Get Repeater Functions Response
	RC_SET_REPEATER_FUNCTIONS = 0x251, //Set Repeater Functions Query
	RC_SET_REPEATER_FILTER = 0x252  //Set Repeater Filter Query
}FCT_CODE;

typedef enum  {
	OFF,
	ON,
	FILTERED
}T_REP_ENABLE;

typedef enum  {
	LEVEL1=1,
	LEVEL2
}T_REP_LEVEL;


typedef enum T_COM_STATUS {

	COM_OK = 0,
	COM_TIMEOUT,


}T_COM_STATUS;

typedef enum T_LEARN_MODE {

	LEARN_IN = 0,
	LEARN_OUT   ,
	LEARN_EXIT,

}T_LEARN_MODE ;

typedef struct {
int function;
std::string message ;
unsigned int senderId;
}T_RMCC_RESULT ;

namespace enocean {
class CEnOceanRMCC : public AsyncSerial, public CDomoticzHardwareBase, public CEnOceanEEP
{

public:
	int m_Seq;

	T_COM_STATUS m_com_status;

	//Remote Mannagement Control Command reception Mutex
	std::mutex m_RMCC_Mutex;
	//Remote Mannagement Control Command reception Queue
	std::vector<T_RMCC_RESULT> m_RMCC_queue;

	T_NODES m_nodes;

    void extractEEP(unsigned char eep[], int* Rorg, int* Func, int* Type);

    CEnOceanRMCC();

	void setRorg(unsigned char * buff, int idx=0);

	void parse_PACKET_REMOTE_MAN_COMMAND(unsigned char m_buffer[], int m_DataSize, int m_OptionalDataSize);

	void RemoteSetLearnMode(unsigned int destID, int channel, T_LEARN_MODE Device_LRN_Mode);

	void remoteSetLearnMode(unsigned int destID, int channel, T_LEARN_MODE Device_LRN_Mode);

	void Unlock(unsigned int destID, unsigned int code);

	void unlock(unsigned int destID, unsigned int code);

	void Lock(unsigned int destID, unsigned int code);

	void lock(unsigned int destID, unsigned int code);

	void Setcode(unsigned int destID, unsigned int code);

	void setcode(unsigned int destID, unsigned int code);

	void queryid(unsigned int EEP, unsigned int mask);

	T_RMCC_RESULT Ping(unsigned int destID);

	void ping(unsigned int destID);

	void Action(unsigned int destID);

	void action(unsigned int destID);

	void getProductId(unsigned int destination = 0xFFFFFFFF );

	void getLinkTableMedadata(uint32_t destID);

	void GetLinkTableMedadata(uint32_t destID);

	T_RMCC_RESULT QueryFunction(uint32_t destID);

	void queryFunction(uint32_t destID);

	void queryStatus(uint32_t destID);

	T_RMCC_RESULT GetDeviceConfiguration(uint32_t destID, int begin, int end, int length);

    void getDeviceConfiguration(uint32_t SensorId, int begin, int end, int length);

    void getDeviceLinkBaseConfiguration(uint32_t SensorId, int Linkindex, int begin, int end, int length);

	void GetDeviceLinkBaseConfiguration(uint32_t SensorId, int Linkindex, int begin, int end, int length);

    void sendSysExMessage(uint32_t destSensorId, uint8_t data[], uint8_t len);

	void SetDeviceLinkBaseConfiguration(uint32_t destID, int Linkindex, int indexParam, int NbParam, int length, uint8_t data[]);

    void setDeviceLinkBaseConfiguration(uint32_t SensorId, int Linkindex, int indexParam, int NbParam, int length, uint8_t data[]);

    void setDeviceLinkBaseConfiguration2(uint32_t SensorId, int Linkindex, int indexParam, int NbParam, int length, uint8_t data[]);

    void getallLinkTable(uint32_t SensorId, int begin, int end);

	void GetallLinkTable(uint32_t destID, int begin, int end);

    void getGPTable(uint32_t SensorId, int index);

	void getLinksConfig(uint32_t DeviceId);

	void getLinkTable(uint32_t DeviceId);

	void SetLinkEntryTable(uint32_t destID, int begin, uint32_t ID, int EEP, int channel);

	void setLinkEntryTable(uint32_t SensorId, int begin, uint32_t ID, int EEP, int channel);

	void ResetToDefaults(uint32_t destID, int resetAction);

	void resetToDefaults(uint32_t destID, int resetAction);

	T_RMCC_RESULT GetRepeaterQuery(unsigned int destID);

	void getRepeaterQuery(unsigned int destination);

	void SetRepeaterQuery(unsigned int destID, int Repeaterfunction, int Repeaterlevel, int RepeaterFilter);

	void setRepeaterQuery(unsigned int destination, int Repeaterfunction, int Repeaterlevel, int RepeaterFilter);

    void setNodonRepeaterLevel(unsigned int source, unsigned int destination, int Repeaterlevel);

	void TeachIn(std::string & sidx ,  T_LEARN_MODE Device_LRN_Mode);

	void TeachIn(std::string & deviceId, std::string & unit, T_LEARN_MODE Device_LRN_Mode);

	void GetNodeList(std::string hwid, Json::Value & root);

	void GetLinkTableList(Json::Value & root, std::string &DeviceId, unsigned int id_chip);

	unsigned int GetLockCode();

	void SetLockCode(std::string scode);

    void setRemote_man_answer(int premote_man_answer, char* message, unsigned int senderId);

	T_RMCC_RESULT getRemote_man_answer();

    int getRemote_man_answer_queue_size();

	void clearRemote_man_answer();

	T_RMCC_RESULT waitRemote_man_answer(int premote_man_answer, int timeoutInSec = 1 );

	void setCommStatus(T_COM_STATUS status);

	T_COM_STATUS getCommStatus();

	bool isCommStatusOk();

    bool unlockDevice(unsigned int deviceId , bool testUnLockTimeoutBeforeSend = true );
	
	virtual void SendESP3PacketQueued(unsigned char frametype, unsigned char *databuf, unsigned short datalen, unsigned char *optdata, unsigned char optdatalen) { return ; };
	virtual void TeachInNodeIfExist(const uint32_t nodeID, const uint16_t manID, const uint8_t RORG, const uint8_t func, const uint8_t type, const TeachinMode teachin_mode){};
	virtual std::string GetDbEnOceanValue(uint32_t DeviceId, const char* fieldName){return "";};

    virtual void GetDbEnOceanValue(uint32_t DeviceId, const char* fieldName, uint32_t& Value){};

    virtual void SetDbEnOceanValue(uint32_t DeviceId, const char* fieldName, uint32_t Value){};

};
}
const char *RMCC_Cmd_Desc(const uint32_t tType);

#endif