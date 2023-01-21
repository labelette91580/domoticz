#include "stdafx.h"

#include "../main/Logger.h"
#include <map>

#include "EnOceanEEP.h"
#include "EnOceanSensors.h"

using namespace enocean;

NodeInfo::NodeInfo()
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
	void NodeInfo::initEntry(int deb,int fin )
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

	int  NodeInfo::getSensorRorg()
	{
			return  getRorg(Profile);
	}
	int  NodeInfo::getSensorFunc()
	{
		return  getFunc(Profile);
	}
	int  NodeInfo::getSensorType()
	{
		return  getType(Profile);
	}

    bool NodeInfo::asLinkTable(unsigned int	pProfile)
	{
		return (getRorg(pProfile) == RORG_VLD);
	}
	bool NodeInfo::asLinkTable()
	{
		return (asLinkTable(Profile) );
	}

	int  NodeInfo::getTableLinkMaxSize()
	{
		return  MaxSize;
	}


//    NodeInfo& T_NODES::operator[](std::size_t idx)       { return m_sensors[idx]; }
    void T_NODES::clear()
    {
        m_sensors.clear();
    }

    void T_NODES::add (
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

	void T_NODES::setSensorManuf(uint32_t SensorId, uint16_t Manuf )
	{
		m_sensors[SensorId].manufacturerID = Manuf;
	}
	void T_NODES::setSensorReference(uint32_t SensorId,  uint32_t ref)
	{
		m_sensors[SensorId].Reference    = ref;
	}

    void T_NODES::setTeachInStatus(uint32_t SensorId, uint32_t pTeachInStatus)
    {
        m_sensors[SensorId].teachin_mode = (TeachinMode)pTeachInStatus ;
    }
	void T_NODES::setSensorProfile(uint32_t SensorId, uint32_t pProfile)
	{
		m_sensors[SensorId].Profile = pProfile;
        m_sensors[SensorId].RORG    = (uint8_t)getRorg(pProfile);
        m_sensors[SensorId].func    = (uint8_t)getFunc(pProfile);
        m_sensors[SensorId].type    = (uint8_t)getType(pProfile);
	}
	void T_NODES::setSensorProfile(uint32_t SensorId, uint8_t  RORG,	uint8_t  func,	uint8_t  type )
	{
        m_sensors[SensorId].RORG    = RORG;
        m_sensors[SensorId].func    = func;
        m_sensors[SensorId].type    = type;
		m_sensors[SensorId].Profile = RorgFuncTypeToProfile(RORG,	  func,	  type ) ;
	}



	void T_NODES::setLinkTableMedadata(uint32_t SensorId, int csize, int MaxSize)
	{
		m_sensors[SensorId].CurrentSize = csize;
		m_sensors[SensorId].MaxSize = MaxSize;
		m_sensors[SensorId].initEntry(csize);
	}
	void T_NODES::addLinkTableEntry(uint32_t DeviceId, int entry, int profile, uint32_t sensorId, int channel)
	{
		if (entry < SIZE_LINK_TABLE) {
			m_sensors[DeviceId].LinkTable[entry].Profile = profile;
			m_sensors[DeviceId].LinkTable[entry].SenderId = sensorId;
			m_sensors[DeviceId].LinkTable[entry].Channel = channel;
			if ((profile != EMPTY_PROFILE) && (sensorId != EMPTY_ID))
				m_sensors[DeviceId].NbValidId++;
		}

	}
	void T_NODES::deleteLinkTableEntry(uint32_t DeviceId, int entry)
	{
		if (entry < SIZE_LINK_TABLE) {
			m_sensors[DeviceId].initEntry(entry,entry+1);
		}

	}

	void T_NODES::printTableLink()
	{
		for (auto itt = m_sensors.begin(); itt != m_sensors.end(); itt++)
		{

			_log.Log(LOG_NORM, "EnOcean: Print Link Table DeviceId:%08X  Profile:%0X Manufacturer:%d CurrentSize:%d MaxSize:%d", itt->first, itt->second.Profile, itt->second.manufacturerID, itt->second.CurrentSize, itt->second.MaxSize);
			for (int i = 0; i < itt->second.CurrentSize; i++)
				_log.Debug(DEBUG_NORM, "                      Entry:%d Id:%08X Profile:%X Channel:%d", i, itt->second.LinkTable[i].SenderId, itt->second.LinkTable[i].Profile, itt->second.LinkTable[i].Channel);

		}

	}
	int  T_NODES::getTableLinkMaxSize(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->getTableLinkMaxSize();
		else
			return  0;
	}
	int  T_NODES::getTableLinkCurrentSize(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->CurrentSize;
		else
			return  0;

	}
	int T_NODES::getTableLinkValidSensorIdSize(unsigned int DeviceId)
	{
		return  m_sensors[DeviceId].NbValidId;
	}

	NodeInfo*  T_NODES::find(unsigned int  DeviceId )
	{
		auto itt = m_sensors.find(DeviceId);

		if (itt != m_sensors.end())
				return &itt->second;
		return 0;
	}
	int  T_NODES::getEEP(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->Profile;
		else
			return  0;

	}

	int  T_NODES::getSensorRorg(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return   ms->getSensorRorg() ;
		else
			return  0;

	}
	int  T_NODES::getSensorFunc(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  ms->getSensorFunc();
		else
			return  0;

	}
	int  T_NODES::getSensorType(unsigned int DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return  (ms->getSensorType());
		else
			return  0;

	}

	int T_NODES::FindEmptyEntry(unsigned int  DeviceId)
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
	bool T_NODES::asLinkTable(unsigned int  DeviceId)
	{
		NodeInfo* ms = find(DeviceId);
		if (ms)
			return ms->asLinkTable();
		else
			return  false;

	}

    T_LINK_TABLE* T_NODES::getLinkEntry(unsigned int  DeviceId, unsigned int  entry)
    {
        T_LINK_TABLE* lEntry = 0;
    if(entry<SIZE_LINK_TABLE){
        NodeInfo* sensor  =  find(   DeviceId );
        if(sensor)
            lEntry=&sensor->LinkTable[entry];
    }
    return lEntry;
    }

    void  T_NODES::erase(const uint32_t nodeID)
    {
	    auto node = m_sensors.find(nodeID);

	    if (node != m_sensors.end())
	    // Erase the element pointed by iterator it
            m_sensors.erase(node);
    }



