#include "stdafx.h"

#include "../main/Logger.h"

//#include "EnOceanEEP.h"
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

    void  T_NODES::erase(const uint32_t nodeID)
    {
	    auto node = m_sensors.find(nodeID);

	    if (node != m_sensors.end())
	    // Erase the element pointed by iterator it
            m_sensors.erase(node);
    }



