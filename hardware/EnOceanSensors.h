#pragma once
#include <map>

#include "EnOceanEEP.h"

//********************************************************************************************

typedef struct {
	int		Profile;
	unsigned int	SenderId;
	int		Channel;
	uint32_t Config ;
}T_LINK_TABLE;
enum TeachinMode : uint8_t
{
	GENERIC_NODE = 0,
	TEACHEDIN_NODE = 1,
	VIRTUAL_NODE = 2
};
namespace enocean {

#define SIZE_LINK_TABLE 24 

	constexpr int EMPTY_PROFILE = 0xFFFFFF;
	constexpr unsigned int EMPTY_ID = 0;
	constexpr unsigned int UnlockTimeOutInMs = 5 * 60 * 1000; //5 min

	class NodeInfo : public enocean::CEnOceanEEP
	{
	public:
		uint32_t idx;
		uint32_t nodeID;
		std::string name;
		uint16_t manufacturerID;
		uint8_t RORG;
		uint8_t func;
		uint8_t type;
		std::string description;
		TeachinMode teachin_mode;
		uint32_t Profile;
		uint32_t Reference;
		int		 CurrentSize;
		int		 NbValidId;
		int		 MaxSize;
		T_LINK_TABLE	LinkTable[SIZE_LINK_TABLE];
		time_t	TimeLastUnlockInMs;
	public:
		NodeInfo();
		void initEntry(int deb, int fin = SIZE_LINK_TABLE);
		int  getSensorRorg();
		int  getSensorFunc();
		int  getSensorType();
		static bool asLinkTable(unsigned int	pProfile);
		bool asLinkTable();
		int  getTableLinkMaxSize();
		//return true if lock timeout is achived
		bool UnLockTimeout();
		void SetUnLockTimeout();
		void ReSetUnLockTimeout();
		std::string Description();
		static std::string teachin_mode_string(TeachinMode pteachin_mode);
		std::string teachin_mode_string();
		void setLinkTableMedadata( int csize, int MaxSize);

	};
	typedef 	std::map<unsigned int, NodeInfo > T_SENSOR_MAP;

	class T_NODES : public T_SENSOR_MAP {
	public:

		void add(uint32_t idx, uint32_t nodeID, std::string Name, uint16_t manufacturerID, uint8_t  RORG, uint8_t  func, uint8_t  type, std::string description, TeachinMode teachin_mode);
		void setSensorManuf(uint32_t SensorId, uint16_t Manuf);
		void setSensorReference(uint32_t SensorId, uint32_t ref);
		void setTeachInStatus(uint32_t SensorId, uint32_t pTeachInStatus);
		uint32_t getTeachInStatus(uint32_t SensorId);
		bool IsAlreadyTeachedIn(const uint32_t nodeID);
		void setSensorProfile(uint32_t SensorId, uint32_t pProfile);
		void setSensorProfile(uint32_t SensorId, uint8_t  RORG, uint8_t  func, uint8_t  type);
		void setLinkTableMedadata(uint32_t SensorId, int csize, int MaxSize);
		void updateLinkConfig(uint32_t DeviceId, int entry, uint32_t pConfig);
		void addLinkTableEntry(uint32_t DeviceId, int entry, int profile, uint32_t sensorId, int channel);
		void deleteLinkTableEntry(uint32_t DeviceId, int entry);
		void printTableLink();
		int  getTableLinkMaxSize(unsigned int DeviceId);
		int  getTableLinkCurrentSize(unsigned int DeviceId);
		int  getTableLinkValidSensorIdSize(unsigned int DeviceId);
		NodeInfo* getNodeInfo(unsigned int  DeviceId);
		int  getEEP(unsigned int DeviceId);
		int  getSensorRorg(unsigned int DeviceId);
		int  getSensorFunc(unsigned int DeviceId);
		int  getSensorType(unsigned int DeviceId);
		int FindEmptyEntry(unsigned int  DeviceId);
		//return true if sensor as a link table 
		bool asLinkTable(unsigned int  DeviceId);
		T_LINK_TABLE* getLinkEntry(unsigned int  DeviceId, unsigned int  entry);
	};
};
