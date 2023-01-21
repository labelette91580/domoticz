#ifndef SENSORS_H
#define	SENSORS_H

#include "../main/Logger.h"
#include <map>

#include "EnOceanEEP.h"

//********************************************************************************************
#define SIZE_LINK_TABLE 24 

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
	NodeInfo()
	{
	idx=0;
	nodeID=0;
	manufacturerID=0;
	RORG=0;
	func=0;
	type=0;

	Profile=0;
	Reference=0;
    CurrentSize=0;
	NbValidId =0;
	MaxSize=0;

		initEntry(0,SIZE_LINK_TABLE);
	}

	#define EMPTY_PROFILE 0xFFFFFF 
	#define EMPTY_ID      0

	void initEntry(int deb,int fin=SIZE_LINK_TABLE)
	{
		NbValidId = 0;
        if(fin>SIZE_LINK_TABLE)
            fin=SIZE_LINK_TABLE;
		for (int i = deb; i < fin; i++)
		{
			LinkTable[i].Profile  = EMPTY_PROFILE ;
			LinkTable[i].SenderId = EMPTY_ID;
			LinkTable[i].Channel = 0;

		}
	}

	int  getSensorRorg()
	{
			return  getRorg(Profile);
	}
	int  getSensorFunc()
	{
		return  getFunc(Profile);
	}
	int  getSensorType()
	{
		return  getType(Profile);

	}

    static bool asLinkTable(unsigned int	pProfile)
	{
		return (getRorg(pProfile) == RORG_VLD);
	}
	bool asLinkTable()
	{
		return (asLinkTable(Profile) );
	}

	int  getTableLinkMaxSize()
	{
		return  MaxSize;
	}


};

typedef 	std::map<unsigned int, NodeInfo > T_SENSOR_MAP;


class T_NODES {
public:
	T_SENSOR_MAP m_sensors;

    NodeInfo& operator[](std::size_t idx)       { return m_sensors[idx]; }
    void clear()
    {
        m_sensors.clear();
    }

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
    
    )

    {
        NodeInfo node;
		node.idx            = idx;
		node.nodeID         = nodeID;
        node.name           = Name,
		node.manufacturerID = manufacturerID;
		node.RORG           = RORG;
		node.func           = func;
		node.type           = type;
		node.description    = description;
		node.teachin_mode   = teachin_mode;
        node.Profile        = RorgFuncTypeToProfile(RORG,func,type) ;
        m_sensors[node.nodeID] = node ;

/*
		Debug(DEBUG_NORM, "Add node : Idx %u Node %08X Name '%s'",
			node.idx, node.nodeID, node.name.c_str());
		Debug(DEBUG_NORM, "LoadNodesFromDatabase: %sEEP %02X-%02X-%02X (%s)",
			(node.teachin_mode == GENERIC_NODE) ? "Generic " : ((node.teachin_mode == VIRTUAL_NODE) ? "Virtual " : ""),
			node.RORG, node.func, node.type, GetEEPLabel(node.RORG, node.func, node.type));
		Debug(DEBUG_NORM, "LoadNodesFromDatabase: Manufacturer %03X (%s) Description '%s'",
			node.manufacturerID, GetManufacturerName(node.manufacturerID), node.description.c_str());
*/

    }

	void setSensorManuf(uint32_t SensorId, uint16_t Manuf )
	{
		m_sensors[SensorId].manufacturerID = Manuf;
	}
	void setSensorReference(uint32_t SensorId,  uint32_t ref)
	{
		m_sensors[SensorId].Reference    = ref;
	}

    void setTeachInStatus(uint32_t SensorId, uint32_t pTeachInStatus)
    {
        m_sensors[SensorId].teachin_mode = (TeachinMode)pTeachInStatus ;
    }
	void setSensorProfile(uint32_t SensorId, uint32_t pProfile)
	{
		m_sensors[SensorId].Profile = pProfile;
        m_sensors[SensorId].RORG    = (uint8_t)getRorg(pProfile);
        m_sensors[SensorId].func    = (uint8_t)getFunc(pProfile);
        m_sensors[SensorId].type    = (uint8_t)getType(pProfile);
	}
	void setSensorProfile(uint32_t SensorId, uint8_t  RORG,	uint8_t  func,	uint8_t  type )
	{
        m_sensors[SensorId].RORG    = RORG;
        m_sensors[SensorId].func    = func;
        m_sensors[SensorId].type    = type;
		m_sensors[SensorId].Profile = RorgFuncTypeToProfile(RORG,	  func,	  type ) ;
	}



	void setLinkTableMedadata(uint32_t SensorId, int csize, int MaxSize)
	{
		m_sensors[SensorId].CurrentSize = csize;
		m_sensors[SensorId].MaxSize = MaxSize;
		m_sensors[SensorId].initEntry(csize);
	}
	void addLinkTableEntry(uint32_t DeviceId, int entry, int profile, uint32_t sensorId, int channel)
	{
		if (entry < SIZE_LINK_TABLE) {
			m_sensors[DeviceId].LinkTable[entry].Profile = profile;
			m_sensors[DeviceId].LinkTable[entry].SenderId = sensorId;
			m_sensors[DeviceId].LinkTable[entry].Channel = channel;
			if ((profile != EMPTY_PROFILE) && (sensorId != EMPTY_ID))
				m_sensors[DeviceId].NbValidId++;
		}

	}
	void deleteLinkTableEntry(uint32_t DeviceId, int entry)
	{
		if (entry < SIZE_LINK_TABLE) {
			m_sensors[DeviceId].initEntry(entry,entry+1);
		}

	}

	void printTableLink()
	{
		for (auto itt = m_sensors.begin(); itt != m_sensors.end(); itt++)
		{

			_log.Log(LOG_NORM, "EnOcean: Print Link Table DeviceId:%08X  Profile:%0X Manufacturer:%d CurrentSize:%d MaxSize:%d", itt->first, itt->second.Profile, itt->second.manufacturerID, itt->second.CurrentSize, itt->second.MaxSize);
			for (int i = 0; i < itt->second.CurrentSize; i++)
				_log.Debug(DEBUG_NORM, "                      Entry:%d Id:%08X Profile:%X Channel:%d", i, itt->second.LinkTable[i].SenderId, itt->second.LinkTable[i].Profile, itt->second.LinkTable[i].Channel);

		}

	}
	int  getTableLinkMaxSize(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->getTableLinkMaxSize();
		else
			return  0;
	}
	int  getTableLinkCurrentSize(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->CurrentSize;
		else
			return  0;

	}
	int  getTableLinkValidSensorIdSize(unsigned int DeviceId)
	{
		return  m_sensors[DeviceId].NbValidId;
	}

	NodeInfo*  find(unsigned int  DeviceId )
	{
		auto itt = m_sensors.find(DeviceId);

		if (itt != m_sensors.end())
				return &itt->second;
		return 0;
	}
	int  getEEP(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->Profile;
		else
			return  0;

	}

	int  getSensorRorg(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return   ms->getSensorRorg() ;
		else
			return  0;

	}
	int  getSensorFunc(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->getSensorFunc();
		else
			return  0;

	}
	int  getSensorType(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  (ms->getSensorType());
		else
			return  0;

	}

	int FindEmptyEntry(unsigned int  DeviceId)
	{
		NodeInfo* sensor = find(DeviceId);
		if (sensor!=0)
		for (int i = 0; i < SIZE_LINK_TABLE; i++)
		{
			if (sensor->LinkTable[i].Profile == EMPTY_PROFILE)
				return i;
		}
		return -1;
	}
	//return true if sensor as a link table 
	bool asLinkTable(unsigned int  DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return ms->asLinkTable();
		else
			return  false;

	}

    T_LINK_TABLE* getLinkEntry(unsigned int  DeviceId, unsigned int  entry)
    {
        T_LINK_TABLE* lEntry = 0;
    if(entry<SIZE_LINK_TABLE){
        NodeInfo* sensor  =  find(   DeviceId );
        if(sensor)
            lEntry=&sensor->LinkTable[entry];
    }
    return lEntry;
    }

    void  erase(const uint32_t nodeID)
    {
	    auto node = m_sensors.find(nodeID);

	    if (node != m_sensors.end())
	    // Erase the element pointed by iterator it
            m_sensors.erase(node);
    }

};

};
#endif

