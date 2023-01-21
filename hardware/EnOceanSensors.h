#pragma once
#include <map>

#include "EnOceanEEP.h"

//********************************************************************************************

typedef struct {
	int		Profile;
	unsigned int	SenderId;
	int		Channel;
}T_LINK_TABLE;

	enum TeachinMode : uint8_t
	{
		GENERIC_NODE = 0,
		TEACHEDIN_NODE = 1,
		VIRTUAL_NODE = 2
	};

namespace enocean {

#define SIZE_LINK_TABLE 24 

constexpr int EMPTY_PROFILE = 0xFFFFFF ;
constexpr unsigned int EMPTY_ID      = 0;

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
	int		 NbValidId ;
	int		 MaxSize;
	T_LINK_TABLE	LinkTable[SIZE_LINK_TABLE];

public:
	NodeInfo();

	void initEntry(int deb,int fin=SIZE_LINK_TABLE);
	int  getSensorRorg();
	int  getSensorFunc();
	int  getSensorType();
    static bool asLinkTable(unsigned int	pProfile);
	bool asLinkTable();
	int  getTableLinkMaxSize();
};

typedef 	std::map<unsigned int, NodeInfo > T_SENSOR_MAP;


class T_NODES {
public:
	T_SENSOR_MAP m_sensors;

    NodeInfo& operator[](std::size_t idx)       { return m_sensors[idx]; }
    void clear();
	auto T_NODES::begin(){return m_sensors.begin();};
	auto T_NODES::end(){return m_sensors.end();};
    void add (
    uint32_t idx,
	uint32_t nodeID,
    std::string Name,
	uint16_t manufacturerID,
	uint8_t  RORG,
	uint8_t  func,
	uint8_t  type,
    std::string description,
    TeachinMode teachin_mode
    );

	void setSensorManuf(uint32_t SensorId, uint16_t Manuf );
	void setSensorReference(uint32_t SensorId,  uint32_t ref);
    void setTeachInStatus(uint32_t SensorId, uint32_t pTeachInStatus);
	void setSensorProfile(uint32_t SensorId, uint32_t pProfile);
	void setSensorProfile(uint32_t SensorId, uint8_t  RORG,	uint8_t  func,	uint8_t  type );
	void setLinkTableMedadata(uint32_t SensorId, int csize, int MaxSize);
	void addLinkTableEntry(uint32_t DeviceId, int entry, int profile, uint32_t sensorId, int channel);
	void deleteLinkTableEntry(uint32_t DeviceId, int entry);
	void printTableLink();
	int  getTableLinkMaxSize(unsigned int DeviceId);
	int  getTableLinkCurrentSize(unsigned int DeviceId);
	int  getTableLinkValidSensorIdSize(unsigned int DeviceId);
	NodeInfo*  find(unsigned int  DeviceId );
	int  getEEP(unsigned int DeviceId);
	int  getSensorRorg(unsigned int DeviceId);
	int  getSensorFunc(unsigned int DeviceId);
	int  getSensorType(unsigned int DeviceId);
	int FindEmptyEntry(unsigned int  DeviceId);
	//return true if sensor as a link table 
	bool asLinkTable(unsigned int  DeviceId);
    T_LINK_TABLE* getLinkEntry(unsigned int  DeviceId, unsigned int  entry);
    void  erase(const uint32_t nodeID);
};

};

