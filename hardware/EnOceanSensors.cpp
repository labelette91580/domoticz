#include "stdafx.h"

#include "../main/Logger.h"
#include <map>

#include "EnOceanEEP.h"
#include "EnOceanSensors.h"
#include "../main/Helper.h"

using namespace enocean;

NodeInfo::NodeInfo()
{
	idx = 0;
	nodeID = 0;
	manufacturerID = 0;
	RORG = 0;
	func = 0;
	type = 0;
	Profile = 0;
	Reference = 0;
	CurrentSize = 0;
	NbValidId = 0;
	MaxSize = 0;
	TimeLastUnlockInMs = 0;
	initEntry(0, SIZE_LINK_TABLE);
}
void NodeInfo::initEntry(int deb, int fin)
{
	NbValidId = 0;
	if (fin > SIZE_LINK_TABLE)
		fin = SIZE_LINK_TABLE;
	for (int i = deb; i < fin; i++)
	{
		LinkTable[i].Profile = EMPTY_PROFILE;
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
	return (asLinkTable(Profile));
}
int  NodeInfo::getTableLinkMaxSize()
{
	return  MaxSize;
}
//return true if lock timeout is achived
bool NodeInfo::UnLockTimeout()
{
	time_t timeInMs = GetClockTicks();
	bool timeout = (timeInMs - TimeLastUnlockInMs) > UnlockTimeOutInMs;
	//unlock cmd shall be send
	if (timeout)
		TimeLastUnlockInMs = timeInMs;
	return (timeout);
}
void NodeInfo::SetUnLockTimeout()
{
	TimeLastUnlockInMs = GetClockTicks();
}
std::string NodeInfo::Description()
{
	return std_format("SenderId: %08X Profile:%06X : %s : %s", nodeID, Profile, GetEEPLabel(RORG, func, type), GetEEPDescription(RORG, func, type));
}
std::string NodeInfo::teachin_mode_string(TeachinMode pteachin_mode)
{
	if (pteachin_mode == GENERIC_NODE)
		return  "Generic ";
	if (pteachin_mode == VIRTUAL_NODE)
		return  "Virtual ";
	if (pteachin_mode == TEACHEDIN_NODE)
		return "Teachin";
	return "";
}
std::string NodeInfo::teachin_mode_string()
{
	return teachin_mode_string(teachin_mode);
}
void T_NODES::add(uint32_t idx, uint32_t nodeID, std::string Name, uint16_t manufacturerID, uint8_t  RORG, uint8_t  func, uint8_t  type, std::string description, TeachinMode teachin_mode)
{
	NodeInfo node;
	node.idx = idx;
	node.nodeID = nodeID;
	node.name = Name,
	node.manufacturerID = manufacturerID;
	node.RORG = RORG;
	node.func = func;
	node.type = type;
	node.description = description;
	node.teachin_mode = teachin_mode;
	node.Profile = RorgFuncTypeToProfile(RORG, func, type);
	(*this)[node.nodeID] = node;
}
void T_NODES::setSensorManuf(uint32_t SensorId, uint16_t Manuf)
{
	(*this)[SensorId].manufacturerID = Manuf;
}
void T_NODES::setSensorReference(uint32_t SensorId, uint32_t ref)
{
	(*this)[SensorId].Reference = ref;
}
void T_NODES::setTeachInStatus(uint32_t SensorId, uint32_t pTeachInStatus)
{
	(*this)[SensorId].teachin_mode = (TeachinMode)pTeachInStatus;
}
void T_NODES::setSensorProfile(uint32_t SensorId, uint32_t pProfile)
{
	(*this)[SensorId].Profile = pProfile;
	(*this)[SensorId].RORG = (uint8_t)getRorg(pProfile);
	(*this)[SensorId].func = (uint8_t)getFunc(pProfile);
	(*this)[SensorId].type = (uint8_t)getType(pProfile);
}
void T_NODES::setSensorProfile(uint32_t SensorId, uint8_t  RORG, uint8_t  func, uint8_t  type)
{
	(*this)[SensorId].RORG = RORG;
	(*this)[SensorId].func = func;
	(*this)[SensorId].type = type;
	(*this)[SensorId].Profile = RorgFuncTypeToProfile(RORG, func, type);
}
void T_NODES::setLinkTableMedadata(uint32_t SensorId, int csize, int MaxSize)
{
	(*this)[SensorId].CurrentSize = csize;
	(*this)[SensorId].MaxSize = MaxSize;
	(*this)[SensorId].initEntry(csize);
}
void T_NODES::addLinkTableEntry(uint32_t DeviceId, int entry, int profile, uint32_t sensorId, int channel)
{
	if (entry < SIZE_LINK_TABLE) {
		(*this)[DeviceId].LinkTable[entry].Profile = profile;
		(*this)[DeviceId].LinkTable[entry].SenderId = sensorId;
		(*this)[DeviceId].LinkTable[entry].Channel = channel;
		if ((profile != EMPTY_PROFILE) && (sensorId != EMPTY_ID))
			(*this)[DeviceId].NbValidId++;
	}
}
void T_NODES::deleteLinkTableEntry(uint32_t DeviceId, int entry)
{
	if (entry < SIZE_LINK_TABLE) {
		(*this)[DeviceId].initEntry(entry, entry + 1);
	}
}
void T_NODES::printTableLink()
{
	for (auto itt = begin(); itt != end(); itt++)
	{
		_log.Log(LOG_NORM, "EnOcean: Print Link Table DeviceId:%08X  Profile:%0X Manufacturer:%d CurrentSize:%d MaxSize:%d", itt->first, itt->second.Profile, itt->second.manufacturerID, itt->second.CurrentSize, itt->second.MaxSize);
		for (int i = 0; i < itt->second.CurrentSize; i++)
			_log.Debug(DEBUG_NORM, "                      Entry:%d Id:%08X Profile:%X Channel:%d", i, itt->second.LinkTable[i].SenderId, itt->second.LinkTable[i].Profile, itt->second.LinkTable[i].Channel);
	}
}
int  T_NODES::getTableLinkMaxSize(unsigned int DeviceId)
{
	NodeInfo* ms = search(DeviceId);
	if (ms)
		return  ms->getTableLinkMaxSize();
	else
		return  0;
}
int  T_NODES::getTableLinkCurrentSize(unsigned int DeviceId)
{
	NodeInfo* ms = search(DeviceId);
	if (ms)
		return  ms->CurrentSize;
	else
		return  0;
}
int T_NODES::getTableLinkValidSensorIdSize(unsigned int DeviceId)
{
	return  (*this)[DeviceId].NbValidId;
}
NodeInfo* T_NODES::search(unsigned int  DeviceId)
{
	auto itt = (*this).find(DeviceId);
	if (itt != end())
		return &itt->second;
	return 0;
}
int  T_NODES::getEEP(unsigned int DeviceId)
{
	NodeInfo* ms = search(DeviceId);
	if (ms)
		return  ms->Profile;
	else
		return  0;
}
int  T_NODES::getSensorRorg(unsigned int DeviceId)
{
	NodeInfo* ms = search(DeviceId);
	if (ms)
		return   ms->getSensorRorg();
	else
		return  0;
}
int  T_NODES::getSensorFunc(unsigned int DeviceId)
{
	NodeInfo* ms = search(DeviceId);
	if (ms)
		return  ms->getSensorFunc();
	else
		return  0;
}
int  T_NODES::getSensorType(unsigned int DeviceId)
{
	NodeInfo* ms = search(DeviceId);
	if (ms)
		return  (ms->getSensorType());
	else
		return  0;
}
int T_NODES::FindEmptyEntry(unsigned int  DeviceId)
{
	NodeInfo* sensor = search(DeviceId);
	if (sensor != 0)
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
	NodeInfo* ms = search(DeviceId);
	if (ms)
		return ms->asLinkTable();
	else
		return  false;
}
T_LINK_TABLE* T_NODES::getLinkEntry(unsigned int  DeviceId, unsigned int  entry)
{
	T_LINK_TABLE* lEntry = 0;
	if (entry < SIZE_LINK_TABLE) {
		NodeInfo* sensor = search(DeviceId);
		if (sensor)
			lEntry = &sensor->LinkTable[entry];
	}
	return lEntry;
}

