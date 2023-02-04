#include "stdafx.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <ctime>
#include <stdarg.h>

#include <boost/exception/diagnostic_information.hpp>

#include "../main/Logger.h"
#include "../main/Helper.h"
#include "../main/RFXtrx.h"
#include "../main/SQLHelper.h"
#include "../main/WebServerHelper.h"
#include "../main/WebServer.h"
#include "../main/HTMLSanitizer.h"
#include "../main/mainworker.h"
#include "../main/localtime_r.h"

#include "hardwaretypes.h"
#include "EnOceanESP3.h"

#include "EnOceanRMCC.h"
//#include "eep-d2.h"
// #include "EnOceanXmlReader.h"

using namespace enocean;

//#define USE_PROFIL
#ifdef USE_PROFIL
#include "EnOceanProfil.h"
#endif
#include "EnOceanEepXml.h"

constexpr int CO_WR_REPEATER = 9 ;				// Set Repeater Level
constexpr int CO_RD_REPEATER = 10;			// Read Repeater Level
constexpr int PACKET_COMMON_COMMAND = 0x05;	// Common command


///---------------------nodes ------------------------------------------------------
void  CEnOceanESP3::SetNodeTeachInStatus(const uint32_t nodeID, uint32_t TeachInStatus)
{
	auto node = GetNodeInfo(nodeID);
	if (node != nullptr)
		node->teachin_mode = (TeachinMode)TeachInStatus;
}
uint32_t  CEnOceanESP3::GetNodeTeachInStatus(const uint32_t nodeID)
{
	auto node = GetNodeInfo(nodeID);
	if (node != nullptr)
		return node->teachin_mode;
	else
		return 0;
}
bool  CEnOceanESP3::NodeIsAlreadyTeachedIn(const uint32_t nodeID)
{
	return GetNodeTeachInStatus(nodeID) != 0;
}
///--- db 
void CEnOceanESP3::GetDbValue(const char* tableName, const char* fieldName, const char* whereFieldName, const char* whereFieldValue, uint32_t& Value)
{
	std::string result;
	result = GetDbValue(tableName, fieldName, whereFieldName, whereFieldValue);
	if (result.empty())
		Value = 0;
	else
		Value = static_cast<uint32_t>(std::stoul(result));
}
void CEnOceanESP3::SetDbValue(const char* tableName, const char* fieldName, const char* fieldValue, const char* whereFieldName, const char* whereFieldValue)
{
	m_sql.safe_query("UPDATE %s SET %s='%s'   WHERE (%s = '%s' )", tableName, fieldName, fieldValue, whereFieldName, whereFieldValue);
}
void CEnOceanESP3::SetDbValue(const char* tableName, const char* fieldName, uint32_t fieldValue, const char* whereFieldName, const char* whereFieldValue)
{
	m_sql.safe_query("UPDATE %s SET %s=%d   WHERE (%s = '%s' )", tableName, fieldName, fieldValue, whereFieldName, whereFieldValue);
}
std::string GetEnOceanNodeIdToDecString(unsigned int DeviceID)
{
	char szDeviceID[20];
	sprintf(szDeviceID, "%d", (unsigned int)DeviceID);
	return szDeviceID;
}
std::string CEnOceanESP3::GetDbEnOceanValue(uint32_t DeviceId, const char* fieldName)
{
	std::string result = GetDbValue("EnOceanNodes", fieldName, "NodeID", GetEnOceanNodeIdToDecString(DeviceId).c_str());
	return result;
}
void CEnOceanESP3::GetDbEnOceanValue(uint32_t DeviceId, const char* fieldName, uint32_t& Value)
{
	GetDbValue("EnOceanNodes", fieldName, "NodeID", GetEnOceanNodeIdToDecString(DeviceId).c_str(), Value);
}
void CEnOceanESP3::SetDbEnOceanValue(uint32_t DeviceId, const char* fieldName, uint32_t Value)
{
	SetDbValue("EnOceanNodes", fieldName, Value, "NodeID", GetEnOceanNodeIdToDecString(DeviceId).c_str());
}
void CEnOceanESP3::SetDbTeachInStatus(uint32_t DeviceId, uint32_t TeachInStatus)
{
	SetDbEnOceanValue(DeviceId, "nValue", TeachInStatus);
}
void CEnOceanESP3::SetTeachInStatus(uint32_t DeviceId, uint32_t TeachInStatus)
{
	SetDbTeachInStatus(DeviceId, TeachInStatus);
	SetNodeTeachInStatus(DeviceId, TeachInStatus);
}
void CEnOceanESP3::setRepeaterLevelOn()
{
	//Request CO_WR_REPEATER : set repeater ON level 1
	uint8_t buf[4];
	buf[0] = CO_WR_REPEATER; buf[1] = ON; buf[2] = LEVEL1;
	SendESP3PacketQueued(PACKET_COMMON_COMMAND, buf, 3, NULL, 0);
}
void CEnOceanESP3::getRepeaterLevel()
{
	//Request CO_RD_REPEATER
	uint8_t buf[4];
	buf[0] = CO_RD_REPEATER;
	SendESP3PacketQueued(PACKET_COMMON_COMMAND, buf, 1, NULL, 0);
	Log(LOG_NORM, "Set repeater mode ON ");
}
void CEnOceanESP3::parsePACKET_RESPONSE(uint8_t* data, uint16_t datalen)
{
	//CO_RD_REPEATER response
	if (datalen == 3)
	{
		const char* Repeater_status[] = { "OFF","ON", "FILTER" };
		int REP_ENABLE = data[1] % 3;
		int REP_LEVEL = data[2];
		Log(LOG_NORM, " REPEATER function Enable=%d:%s Level=%d", REP_ENABLE, Repeater_status[REP_ENABLE], REP_LEVEL);
	}
	setRemote_man_answer(RC_PACKET_RESPONSE, (char*)GetReturnCodeLabel(data[0]), 0);
}
const char* PACKET_TYPE_name[] = {
	"PACKET_RESERVED"          ,
	"PACKET_RADIO_ERP1"             ,
	"PACKET_RESPONSE"          ,
	"PACKET_RADIO_SUB_TEL"     ,
	"PACKET_EVENT"             ,
	"PACKET_COMMON_COMMAND"    ,
	"PACKET_SMART_ACK_COMMAND" ,
	"PACKET_REMOTE_MAN_COMMAND",
	"PACKET_PRODUCTION_COMMAND",
	"PACKET_RADIO_MESSAGE"     ,
	"PACKET_RADIO_ADVANCED"
};
const char* getPACKET_TYPE_name(int ptype)
{
	if (ptype < 11)
		return PACKET_TYPE_name[ptype];
	else
		return "UNKNOWN PACKET";
}
int StrToInt(std::string value)
{
	char* p;
	p = (char*)strchr(value.c_str(), 'x');
	if (p)
	{
		*p = '0';
		return std::stoi(value, 0, 16);
	}
	p = (char*)strchr(value.c_str(), 'b');
	if (p)
	{
		*p = '0';
		return std::stoi(value, 0, 2);
	}
	return std::stoi(value, 0, 10);
}
//Webserver helpers
//namespace http {
//	namespace server {
		std::string getDeviceId(const http::server::request& req, unsigned int argNb)
		{
			std::string cmd = http::server::request::findValue(&req, std::to_string(argNb).c_str());
			std::vector<std::string> splitresults;
			StringSplit(cmd, ";", splitresults);
			if (splitresults.size() >= 1)
				return splitresults[0];
			else
				return "";
		}
		std::string getDeviceUnit(const http::server::request& req, unsigned int argNb)
		{
			std::string cmd = http::server::request::findValue(&req, std::to_string(argNb).c_str());
			std::vector<std::string> splitresults;
			StringSplit(cmd, ";", splitresults);
			if (splitresults.size() >= 2)
				return splitresults[1];
			else
				return "1";
		}
		std::string getLinkEntry(const http::server::request& req, unsigned int argNb)
		{
			std::string cmd = http::server::request::findValue(&req, "entry");
			std::vector<std::string> splitresults;
			StringSplit(cmd, ";", splitresults);
			if (splitresults.size() >= (argNb + 1))
				return splitresults[argNb];
			else
				return "";
		}
		void checkComStatus(CEnOceanESP3* pEnocean, Json::Value& root)
		{
			if (pEnocean->isCommStatusOk())
				root["status"] = "OK";
			else {
				root["message"] = "Communication Timeout";
				//			Log(LOG_ERROR, "EnOcean: Server Error: %s  cmd:%s Hwid:%s arg:%s  Entry=%s", root["message"].asString().c_str(), cmd.c_str(), hwid.c_str(), arg.c_str(), request::findValue(&req, "entry").c_str());
				pEnocean->Log(LOG_ERROR, "Server Error: %s  cmd:%s ", root["message"].asString().c_str(), root["cmd"].asString().c_str());
				//			pEnocean->setCommStatus(COM_OK);
			}
		}
#define WEB_CMD_ARG   http::server::WebEmSession &session, const http::server::request &req, Json::Value &root, int nbSelectedDevice , int iHardwareID ,CEnOceanESP3 *pEnocean
		typedef std::function<void(WEB_CMD_ARG)> EnOcean_web_function;
		typedef struct {
			char* name;
			EnOcean_web_function fct;
		} enocean_web_cmd_t;
		//--------------------- web command function
		static void GetNodeList(WEB_CMD_ARG)
		{
			pEnocean->GetNodeList(std::to_string(iHardwareID), root);
		}
		static void SendCode(WEB_CMD_ARG)
		{
			std::string deviceId;
			unsigned int code = pEnocean->GetLockCode();
			if (nbSelectedDevice == 0)
			{
				pEnocean->setcode(BROADCAST_ID, code);
				pEnocean->waitRemote_man_answer(RC_PACKET_RESPONSE, RMCC_ACK_TIMEOUT);
			}
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);  if (deviceId.empty())	return;
				pEnocean->setcode(DeviceIdStringToUInt(deviceId), code);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void Lock(WEB_CMD_ARG)
		{
			std::string deviceId;
			unsigned int code = pEnocean->GetLockCode();
			if (nbSelectedDevice == 0)
			{
				pEnocean->lock(BROADCAST_ID, code);
				pEnocean->waitRemote_man_answer(RC_PACKET_RESPONSE, RMCC_ACK_TIMEOUT);
			}
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);  if (deviceId.empty())	return;
				pEnocean->lock(DeviceIdStringToUInt(deviceId), code);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void UnLock(WEB_CMD_ARG)
		{
			std::string deviceId;
			if (nbSelectedDevice == 0) {
				pEnocean->unlockDevice(BROADCAST_ID, false);
			}
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);  if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId), false);
			}
			checkComStatus(pEnocean, root);
		}
		static void SetCode(WEB_CMD_ARG)
		{
			//
			std::string code = http::server::request::findValue(&req, "code");
			if (code.empty())
				return;
			pEnocean->SetLockCode(code);
			root["status"] = "OK";
		}
		static void GetLinkTableList(WEB_CMD_ARG)
		{
			std::string deviceId;
			deviceId = getDeviceId(req, 0);  if (deviceId.empty())	return;
			pEnocean->GetLinkTableList(root, deviceId, pEnocean->m_id_chip);
			checkComStatus(pEnocean, root);
		}
		static void LearnIn(WEB_CMD_ARG)
		{
			std::string deviceId;
			std::string unit;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				unit = getDeviceUnit(req, i);
				pEnocean->TeachIn(deviceId, unit, LEARN_IN);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void LearnOut(WEB_CMD_ARG)
		{
			std::string deviceId;
			std::string unit;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				unit = getDeviceUnit(req, i);
				pEnocean->TeachIn(deviceId, unit, LEARN_OUT);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void GetProductId(WEB_CMD_ARG)
		{
			//pEnocean->unlock(BROADCAST_ID, pEnocean->GetLockCode());
			std::string deviceId;
			//assume all new device is power on
			if (nbSelectedDevice == 0){
				pEnocean->getProductId(BROADCAST_ID);
				nbSelectedDevice = 0x7ff;
			}
			else
				for (int i = 0; i < nbSelectedDevice; i++) {
					deviceId = getDeviceId(req, i);  if (deviceId.empty())	return;
					pEnocean->getProductId(DeviceIdStringToUInt(deviceId));
				}
			//wait response 
			int nbDeviceDiscovered = 0;
			T_RMCC_RESULT answ;
			std::map<uint32_t, uint32_t> devicesId;
			do
			{
				int timeout = RMCC_GETPRODUCTID_TIMEOUT;
				//if(nbDeviceDiscovered==0) timeout*=2;
				answ = pEnocean->waitRemote_man_answer(RC_GET_PRODUCT_RESPONSE, timeout);
				if (answ.function != 0) {
					nbDeviceDiscovered++;
					pEnocean->ping(answ.senderId);
					devicesId[answ.senderId] = answ.senderId;
					//					    answ =  pEnocean->waitRemote_man_answer(PING_ANSWER, timeout );
				}
			} while ( (answ.function != 0) && (nbDeviceDiscovered< nbSelectedDevice) );
			std::string discoveredDevice = std_format("%d devices discovered ", nbDeviceDiscovered);
			for (const auto& deviceId : devicesId)
			{
				auto node = pEnocean->GetNodeInfo(deviceId.second);
				discoveredDevice += "<BR>" + node->Description();
			}
			pEnocean->Log(LOG_NORM, discoveredDevice.c_str());
			root["status"] = "OK";
			root["message"] = discoveredDevice;
		}
		static void QueryId(WEB_CMD_ARG)
		{
			pEnocean->unlockDevice(0xFFFFFFFF);
			pEnocean->queryid(0, 0);
			pEnocean->waitRemote_man_answer(QUERYID_ANSWER_EXT, RMCC_ACK_TIMEOUT);
			checkComStatus(pEnocean, root);
		}
		static void GetLinkTable(WEB_CMD_ARG)
		{
			std::string deviceId;
			deviceId = getDeviceId(req, 0);    if (deviceId.empty())	return;
			//pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
			pEnocean->getLinkTable(DeviceIdStringToUInt(deviceId));
			checkComStatus(pEnocean, root);
		}
		static void QueryStatus(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				if (!pEnocean->isCommStatusOk())
					break;
				pEnocean->queryStatus(DeviceIdStringToUInt(deviceId));
				root["message"] = pEnocean->waitRemote_man_answer(QUERY_STATUS_ANSWER, RMCC_ACK_TIMEOUT).message;
			}
			checkComStatus(pEnocean, root);
		}
		static void ResetToDefault(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				pEnocean->resetToDefaults(DeviceIdStringToUInt(deviceId), ResetToDefaults);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void QueryFunction(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				if (pEnocean->isCommStatusOk())
				{
					pEnocean->queryFunction(DeviceIdStringToUInt(deviceId));
					root["message"] = pEnocean->waitRemote_man_answer(QUERY_FUNCTION_ANSWER, RMCC_ACK_TIMEOUT).message;
				}
			}
			checkComStatus(pEnocean, root);
		}
		static void DeleteEntrys(WEB_CMD_ARG)
		{
			{
				std::string deviceId;
				deviceId = getLinkEntry(req, 0);    if (deviceId.empty())	return;
				unsigned int DeviceId = DeviceIdStringToUInt(deviceId);
				int entryNb = 1;
				std::string entry;
				entry = getLinkEntry(req, entryNb);
				T_LINK_TABLE* lEntry = pEnocean->m_nodes.getLinkEntry(DeviceId, std::stoi(entry, 0, 0));
				//if entry is equal to chip id , delete the teach in status = teachout
				if (lEntry->SenderId == pEnocean->m_id_chip)
					pEnocean->SetTeachInStatus(DeviceId, 0);
				pEnocean->unlockDevice((DeviceId));
				while ((pEnocean->isCommStatusOk()) && (!entry.empty()))
				{
					pEnocean->setLinkEntryTable(DeviceId, std::stoi(entry, 0, 0), 0, 0, 0);
					pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
					if (pEnocean->isCommStatusOk())
						pEnocean->m_nodes.deleteLinkTableEntry(DeviceId, std::stoi(entry, 0, 0));
					entryNb++;
					entry = getLinkEntry(req, entryNb);
				}
				//pEnocean->getLinkTable(DeviceId);
				checkComStatus(pEnocean, root);
			}
		}
		static void Ping(WEB_CMD_ARG)
		{
			std::string deviceId;
			if (nbSelectedDevice == 0)
				pEnocean->ping(BROADCAST_ID);
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);  if (deviceId.empty())	return;
				pEnocean->ping(DeviceIdStringToUInt(deviceId));
				root["message"] = pEnocean->waitRemote_man_answer(PING_ANSWER, RMCC_ACK_TIMEOUT).message;
			}
			checkComStatus(pEnocean, root);
		}
		static void Action(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);  if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				pEnocean->action(DeviceIdStringToUInt(deviceId));
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void Link(WEB_CMD_ARG)
		{
			if (nbSelectedDevice != 2) { root["message"] = "Only 2 devices selected"; return; }
			int deviceId1 = DeviceIdStringToUInt(getDeviceId(req, 0));
			int deviceId2 = DeviceIdStringToUInt(getDeviceId(req, 1));
			if (pEnocean->m_nodes.asLinkTable(deviceId1))
				pEnocean->getLinkTable(deviceId1);
			if (!pEnocean->isCommStatusOk()) { checkComStatus(pEnocean, root);  return; }
			if (pEnocean->m_nodes.asLinkTable(deviceId2))
				pEnocean->getLinkTable(deviceId2);
			if (!pEnocean->isCommStatusOk()) { checkComStatus(pEnocean, root);  return; }
			int LinkSize1 = pEnocean->m_nodes.getTableLinkMaxSize(deviceId1);
			int LinkSize2 = pEnocean->m_nodes.getTableLinkMaxSize(deviceId2);
			int deviceIdSender, deviceIdReceiver, receiverChannel, senderChannel;
			if ((LinkSize1 == 0) && (LinkSize2 != 0))
			{
				deviceIdSender = deviceId1;
				deviceIdReceiver = deviceId2;
				senderChannel = std::stoi(getDeviceUnit(req, 0));
				receiverChannel = std::stoi(getDeviceUnit(req, 1));
			}
			else
				if ((LinkSize1 != 0) && (LinkSize2 == 0))
				{
					deviceIdSender = deviceId2;
					deviceIdReceiver = deviceId1;
					senderChannel = std::stoi(getDeviceUnit(req, 1));
					receiverChannel = std::stoi(getDeviceUnit(req, 0));
				}
				else
				{
					root["message"] = "Could not link devices"; return;
				}
			//get empty entry in receiver
			int emptyEntry = pEnocean->m_nodes.FindEmptyEntry(deviceIdReceiver);
			if (emptyEntry < 0)
			{
				root["message"] = "Link table full or device not found "; return;
			}
			else
			{
				int senderEEP = pEnocean->m_nodes.getEEP(deviceIdSender);
				pEnocean->setLinkEntryTable(deviceIdReceiver, emptyEntry, deviceIdSender, senderEEP, receiverChannel - 1);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
				if (pEnocean->isCommStatusOk()) {
					pEnocean->m_nodes.addLinkTableEntry(deviceIdReceiver, emptyEntry, senderEEP, deviceIdSender, receiverChannel - 1);
					pEnocean->Log(LOG_NORM, "Link receiver %08X channel %d to sender %08X(%06X) ", deviceIdReceiver, receiverChannel, deviceIdSender, senderEEP);
				}
				else
					pEnocean->Log(LOG_ERROR, "Linking receiver %08X channel %d to sender %08X(%06X) ", deviceIdReceiver, receiverChannel, deviceIdSender, senderEEP);
			}
			checkComStatus(pEnocean, root);
		}
		static void getCases(WEB_CMD_ARG)
		{
			//return the list of eep cases for the profil 
			std::string sprofil = http::server::request::findValue(&req, "profil"); if (sprofil.empty())return;
#ifdef USE_PROFIL
			Profils.LoadXml();
			T_PROFIL_EEP* prof = Profils.getProfil(DeviceIdStringToUInt(sprofil));
			for (unsigned int caseNb = 0; caseNb < prof->cases.size(); caseNb++)
			{
				root["result"][caseNb]["Num"] = caseNb + 1;
				root["result"][caseNb]["Title"] = prof->cases[caseNb].Title;
				root["result"][caseNb]["Description"] = prof->cases[caseNb].Desc;
			}
#endif				
			root["status"] = "OK";
		}
		static void getCases2(WEB_CMD_ARG)
		{
			//return the list of eep cases for the profil 
			//std::string sprofil = request::findValue(&req, "profil");if (sprofil.empty())return;
			//T_PROFIL_LIST * prof = getProfil (DeviceIdStringToUInt(sprofil));
			//if(prof)
			//for (unsigned int caseNb = 0; caseNb < prof->nbCases; caseNb++)
			//{
			//	root["result"][caseNb]["Num"]         = caseNb +1;
			//	root["result"][caseNb]["Title"]       = prof->cases[caseNb]->Title;
			//	root["result"][caseNb]["Description"] = prof->cases[caseNb]->Desc;
			//}
			root["status"] = "OK";
		}
		static void getCaseShortCut(WEB_CMD_ARG)
		{
			//return the list of shorcuts for the  case for the profil 
			std::string sprofil = http::server::request::findValue(&req, "profil"); if (sprofil.empty())	return;
			std::string scaseNb = http::server::request::findValue(&req, "casenb"); if (scaseNb.empty())	return;
#ifdef USE_PROFIL
			Profils.LoadXml();
			T_EEP_CASE* Case = Profils.getCase(DeviceIdStringToUInt(sprofil), std::stoi(scaseNb, nullptr, 0));
			for (unsigned int i = 0; i < Case->size(); i++)
			{
				root["result"][i]["Short"] = Case->at(i)->ShortCut;
				root["result"][i]["Desc"] = Case->at(i)->description;
				root["result"][i]["Enum"] = Case->at(i)->enumerate;
			}
#endif			
			root["status"] = "OK";
		}
		static void getCaseShortCut2(WEB_CMD_ARG)
		{
			//return the list of shorcuts for the  case for the profil 
			std::string sprofil = http::server::request::findValue(&req, "profil"); if (sprofil.empty())	return;
			std::string scaseNb = http::server::request::findValue(&req, "casenb"); if (scaseNb.empty())	return;
			//T_EEP_CASE_ * Case = getProfilCase (DeviceIdStringToUInt(sprofil),  std::stoi(scaseNb, nullptr, 0)   );
			//if(Case)
			//	for (unsigned int i = 0; i < 50 ; i++)
			//	{
			//		if (Case->Dataf[i].Size ==0)
			//			break;
			//		root["result"][i]["Short"] = Case->Dataf[i].ShortCut ;
			//		root["result"][i]["Desc"]  = Case->Dataf[i].description;
			//		root["result"][i]["Enum"] = "";//Case->at(i).Enum ;
			//	}
			root["status"] = "OK";
		}
		static void sendvld(WEB_CMD_ARG)
		{
			//return the list of shorcuts for the  case for the profil 
			std::string sprofil         = http::server::request::findValue(&req, "profil"); if (sprofil.empty())	return;
			std::string scaseNb         = http::server::request::findValue(&req, "casenb"); if (scaseNb.empty())	return;
			std::string sdevidx         = http::server::request::findValue(&req, "devidx"); if (sdevidx.empty())	return;
			std::string sdevicebaseAddr = http::server::request::findValue(&req, "baseAddr"); if (sdevidx.empty())	return;
			int NbValues = req.parameters.size() - 7;
			int values[256];
			for (int i = 0; i < NbValues; i++) {
				std::string value = getDeviceId(req, i).c_str();
				int val = StrToInt(getDeviceId(req, i));
				values[i] = val;
			}
#ifdef USE_PROFIL
			T_EEP_CASE* Case = Profils.getCase(DeviceIdStringToUInt(sprofil), std::stoi(scaseNb, nullptr, 0));
			//T_EEP_CASE_* Case = getProfilCase(DeviceIdStringToUInt(sprofil), std::stoi(scaseNb, nullptr, 0));
			if (Case)
			{
				unsigned int DeviceId = DeviceIdStringToUInt(sdevidx);
				//					unsigned int DeviceId = DeviceIdStringToUInt(sdevicebaseAddr);
				//                    int siz = Case->dataFileds.size();
				T_DATAFIELD* dataf = Case->dataFileds.data();
				pEnocean->sendDataVld(pEnocean->m_id_chip, DeviceId, dataf, values, NbValues);
				//					pEnocean->senDatadVld(pEnocean->m_id_chip, DeviceId , Case->Dataf, values,  NbValues);
				root["status"] = "OK";
			}
#endif
		}
		static void SetRepeaterNodonLevelOff(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->SetNodonRepeaterLevel(pEnocean->m_id_chip, DeviceIdStringToUInt(deviceId), 0);
				pEnocean->waitRemote_man_answer(RC_PACKET_RESPONSE, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void SetRepeaterNodonLevel1(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->SetNodonRepeaterLevel(pEnocean->m_id_chip, DeviceIdStringToUInt(deviceId), 1);
				pEnocean->waitRemote_man_answer(RC_PACKET_RESPONSE, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void SetRepeaterNodonLevel2(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->SetNodonRepeaterLevel(pEnocean->m_id_chip, DeviceIdStringToUInt(deviceId), 2);
				pEnocean->waitRemote_man_answer(RC_PACKET_RESPONSE, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void GetRepeaterQuery(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				pEnocean->GetRepeaterQuery(DeviceIdStringToUInt(deviceId));
				root["message"] = pEnocean->waitRemote_man_answer(RC_GET_REPEATER_FUNCTIONS_RESPONSE, RMCC_ACK_TIMEOUT).message;
			}
			checkComStatus(pEnocean, root);
		}
		static void SetRepeaterQueryLevelOff(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				pEnocean->SetRepeaterQuery(DeviceIdStringToUInt(deviceId), 0, 1, 0);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void SetRepeaterQueryLevel1(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				pEnocean->SetRepeaterQuery(DeviceIdStringToUInt(deviceId), 1, 1, 0);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void SetRepeaterQueryLevel2(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				pEnocean->unlockDevice(DeviceIdStringToUInt(deviceId));
				pEnocean->SetRepeaterQuery(DeviceIdStringToUInt(deviceId), 1, 2, 0);
				pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
			}
			checkComStatus(pEnocean, root);
		}
		static void Delete(WEB_CMD_ARG)
		{
			std::string deviceId;
			std::string unit;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				uint32_t NodeID = DeviceIdStringToUInt(deviceId);
				unit = getDeviceUnit(req, i);
				std::vector<std::vector<std::string> > result;
				//trqnsfor 
				std::string enOceanId = deviceId;
				deviceId = pEnocean->GetDeviceID(DeviceIdStringToUInt(deviceId));
				result = m_sql.safe_query("SELECT ID FROM DeviceStatus  WHERE (DeviceID=='%s') ", deviceId.c_str());
				int NbDeviceId = result.size();
				//for (int j = 0; j < NbDeviceId; j++)
				//    m_sql.DeleteDevices(result[i][0]);
				pEnocean->DeleteSensor(NodeID);
				pEnocean->Log(LOG_NORM, "CSQLHelper::DeleteDevices: EnOceanNodes  ID: %s", enOceanId.c_str());
			}
			checkComStatus(pEnocean, root);
		}
		static void GetCode(WEB_CMD_ARG)
		{
			//
			unsigned int code = pEnocean->GetLockCode();
			if ((code != 0) && (code != 0xFFFFFFFF))
				root["status"] = "OK";
			else
				root["status"] = "KO";
		}
		static void GetLinkConfig(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				uint32_t ideviceId = DeviceIdStringToUInt(deviceId);
				//					int entryNb = 1;
				std::string sentry = getLinkEntry(req, 1);
				if (sentry.empty())	return;
				//pEnocean->getDeviceConfiguration(ideviceId, 0 ,1 , 1);
				//root["message"] = pEnocean->waitRemote_man_answer(RC_GET_DEVICE_CONFIG_RESPONSE, RMCC_ACK_TIMEOUT);	
				int entry = std::stoi(sentry, 0, 0);
				//for (int entry = 0; entry < 1 ; entry++ )
				int begin = 0;
				int end = 1;
				for (begin = 0; begin <= end; begin++)
				{
					int length = 1;
					pEnocean->getDeviceLinkBaseConfiguration((ideviceId), entry, begin, end, length);
					root["message"] = pEnocean->waitRemote_man_answer(RC_GET_LINK_BASED_CONFIG_RESPONSE, RMCC_ACK_TIMEOUT).message;
				}
			}
			checkComStatus(pEnocean, root);
		}
		static void SetLinkConfig(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);    if (deviceId.empty())	return;
				uint32_t ideviceId = DeviceIdStringToUInt(deviceId);
				std::string sentry = getLinkEntry(req, 1);
				if (sentry.empty())	return;
				int entry = std::stoi(sentry, 0, 0);
				{
					int begin = 0;
					int end = 0;
					uint8_t data[] = { 0xC0,0x56, 0x01 };
					pEnocean->setDeviceLinkBaseConfiguration(ideviceId, entry, begin, end, 3, data);
					pEnocean->waitRemote_man_answer(RC_ACK, RMCC_ACK_TIMEOUT);
				}
			}
			checkComStatus(pEnocean, root);
		}
		static void CreateSensor(WEB_CMD_ARG)
		{
			//
			std::string id = http::server::request::findValue(&req, "id");
			if (id.empty())
				return;
			std::string eep = http::server::request::findValue(&req, "eep");
			if (eep.empty())
				return;
			uint32_t senderID = DeviceIdStringToUInt(id);
			uint32_t eepProfil = DeviceIdStringToUInt(eep);
			//create sensor
			if (pEnocean->GetEEP(getRorg(eepProfil), getFunc(eepProfil), getType(eepProfil)) != nullptr)
				pEnocean->TeachInNode(senderID, UNKNOWN_MANUFACTURER, getRorg(eepProfil), getFunc(eepProfil), getType(eepProfil), GENERIC_NODE);
			else {
				root["message"] = "Profil " + eep + " does not exist";
				return;
			}
			root["status"] = "OK";
		}
		static void ClearTeachInStatus(WEB_CMD_ARG)
		{
			std::string deviceId;
			for (int i = 0; i < nbSelectedDevice; i++) {
				deviceId = getDeviceId(req, i);  if (deviceId.empty())	return;
				pEnocean->SetTeachInStatus(DeviceIdStringToUInt(deviceId), 0);
			}
			root["status"] = "OK";
		}
		//------------------------
		std::map < std::string, EnOcean_web_function > EnOcean_webcommands = {
{			"GetNodeList"                     ,			GetNodeList                },
{			"SendCode"                        ,			SendCode                   },
{			"Lock"                            ,			Lock                       },
{			"UnLock"                          ,			UnLock                     },
{			"SetCode"                         ,			SetCode                    },
{			"GetLinkTableList"                ,			GetLinkTableList           },
{			"LearnIn"                         ,			LearnIn                    },
{			"LearnOut"                        ,			LearnOut                   },
{			"GetProductId"                    ,			GetProductId               },
{			"QueryId"                         ,			QueryId                    },
{			"GetLinkTable"                    ,			GetLinkTable               },
{			"QueryStatus"                     ,			QueryStatus                },
{			"ResetToDefaults"                 ,			ResetToDefault             },
{			"QueryFunction"                   ,			QueryFunction              },
{			"DeleteEntrys"                    ,			DeleteEntrys               },
{			"Ping"                            ,			Ping                       },
{			"Action"                          ,			Action                     },
{			"Link"                            ,			Link                       },
{			"getCases"                        ,			getCases                   },
{			"getCases2"                       ,			getCases2                  },
{			"getCaseShortCut"                 ,			getCaseShortCut            },
{			"getCaseShortCut2"                ,			getCaseShortCut2           },
{			"sendvld"                         ,			sendvld                    },
{			"SetRepeaterNodonLevelOff"        ,			SetRepeaterNodonLevelOff   },
{			"SetRepeaterNodonLevel1"          ,			SetRepeaterNodonLevel1     },
{			"SetRepeaterNodonLevel2"          ,			SetRepeaterNodonLevel2     },
{           "GetRepeaterQuery"                ,         GetRepeaterQuery          },
{			"SetRepeaterQueryLevelOff"        ,			SetRepeaterQueryLevelOff   },
{			"SetRepeaterQueryLevel1"          ,			SetRepeaterQueryLevel1     },
{			"SetRepeaterQueryLevel2"          ,			SetRepeaterQueryLevel2     },
{			"Delete"                          ,			Delete                     },
{			"GetCode"                         ,			GetCode                    },
{			"GetLinkConfig"                   ,			GetLinkConfig              },
{			"SetLinkConfig"                   ,			SetLinkConfig              },
{			"CreateSensor"                    ,			CreateSensor               },
{			"ClearTeachInStatus"              ,			ClearTeachInStatus         }
		};
//	}
//}
void RType_OpenEnOcean(http::server::WebEmSession& session, const http::server::request& req, Json::Value& root)
{
	root["status"] = "ERR";
	root["title"] = "teachin";
	root["message"] = "Undefined";
	std::string hwid = http::server::request::findValue(&req, "hwid");
	if (hwid.empty())
		return;
	std::string cmd = http::server::request::findValue(&req, "cmd");
	if (cmd.empty())
		return;
	root["cmd"] = cmd;
	int iHardwareID = atoi(hwid.c_str());
	CEnOceanESP3* pEnocean = dynamic_cast <CEnOceanESP3*>(m_mainworker.GetHardware(iHardwareID));
	if (pEnocean == NULL)
		return;
	if (pEnocean->HwdType != HTYPE_EnOceanESP3)
		return;
	int nbSelectedDevice = req.parameters.size() - 4;
	//log arguments
	{
		std::string arg;
		for (int i = 0; i < nbSelectedDevice; i++) {
			arg += http::server::request::findValue(&req, std::to_string(i).c_str()) + "-";
		}
		pEnocean->Log(LOG_NORM, "WEBS: Server received cmd:%s Hwid:%s arg:%s Entry=%s ", cmd.c_str(), hwid.c_str(), arg.c_str(), http::server::request::findValue(&req, "entry").c_str());
	}
	//handle command if registered
	auto pfunction = EnOcean_webcommands.find(cmd);
	if (pfunction != EnOcean_webcommands.end())
	{
		pEnocean->setCommStatus(COM_OK);
		pfunction->second(session, req, root, nbSelectedDevice, iHardwareID, pEnocean);
		//		        pEnocean->setCommStatus(COM_OK);
	}
	return;
}

uint32_t enoceanGetSensorIdFromDevice(std::string& strDeviceId, uint32_t unit, uint8_t devType, uint8_t subType)
{
	uint32_t sensorId = 0;
	uint32_t DeviceId = 0;
	std::vector<std::vector<std::string> > result;
	std::vector<std::vector<std::string> > deviceStatusResult;
	int deviceStatusResultSize = 1;
#define  DeviceID " printf(\"%%.08X\", NodeId  ) AS DeviceID " 
	switch (devType)
	{
		//%.ID2.ID1.ID0
		//sprintf(szTmp, "%d", (pResponse->TEMP.id1 * 256) + pResponse->TEMP.id2);
		//		Unit = (pResponse->TEMP.rssi << 4) | pResponse->TEMP.battery_level;
	case pTypeTEMP:
		DeviceId = std::stoul(strDeviceId, 0, 10);
		sensorId = (DeviceId << 8) + unit;
		result = m_sql.safe_query("SELECT " DeviceID " FROM EnOceanNodes  WHERE (DeviceID like '%%%06X') ", sensorId);
		break;
		//%.ID2.ID1.%
		//	sprintf(szTmp, "%d", (pResponse->TEMP_HUM.id1 * 256) + pResponse->TEMP_HUM.id2);
		//sTypeTH5 : unit = 0
	case pTypeTEMP_HUM:
		DeviceId = std::stoul(strDeviceId, 0, 10);
		sensorId = (DeviceId << 8) + unit;
		result = m_sql.safe_query("SELECT " DeviceID " FROM EnOceanNodes  WHERE (DeviceID like '%%%04X__') ", DeviceId);
		deviceStatusResultSize = m_sql.safe_query("SELECT DeviceID FROM DeviceStatus  WHERE (DeviceID == '%d') ", DeviceId).size();
		break;
		//%.ID2.ID1.%
		//	sprintf(szTmp, "%d", (pResponse->TEMP_HUM.id1 * 256) + pResponse->TEMP_HUM.id2);
		// unit = 1
	case pTypeHUM:
		DeviceId = std::stoul(strDeviceId, 0, 10);
		sensorId = (DeviceId << 8) + unit;
		result = m_sql.safe_query("SELECT " DeviceID " FROM EnOceanNodes  WHERE (DeviceID like '%%%04X__') ", DeviceId);
		deviceStatusResultSize = m_sql.safe_query("SELECT DeviceID FROM DeviceStatus  WHERE (DeviceID == '%d') ", DeviceId).size();
		break;
		//ID1.ID0
		//	sprintf(szTmp, "%d", pResponse->RFXSENSOR.id); = ID1
		// Unit = (pResponse->RFXSENSOR.rssi << 4) | pResponse->RFXSENSOR.filler;  ID0
	case pTypeRFXSensor:
		DeviceId = std::stoul(strDeviceId, 0, 10);
		sensorId = (DeviceId << 8) + unit;
		result = m_sql.safe_query("SELECT " DeviceID " FROM EnOceanNodes  WHERE (DeviceID like '%%%04X') ", sensorId);
		deviceStatusResultSize = m_sql.safe_query("SELECT DeviceID FROM DeviceStatus  WHERE (DeviceID == '%d') and (Unit == '%d') ", DeviceId, unit).size();
		break;
		//ID3.ID2.ID1.ID0
		//		sprintf(szTmp, "%08X", pSwitch->id);
		//		sprintf(szTmp, "%X%02X%02X%02X", pResponse->LIGHTING2.id1, pResponse->LIGHTING2.id2, pResponse->LIGHTING2.id3, pResponse->LIGHTING2.id4);
	case pTypeGeneralSwitch:
	case pTypeLux:
	case pTypeLighting2:
		DeviceId = std::stoul(strDeviceId, 0, 16);
		sensorId = DeviceId;
		result = m_sql.safe_query("SELECT DeviceID FROM DeviceStatus  WHERE (DeviceID like '%%%07X' ) ", sensorId);
		deviceStatusResultSize = result.size();
		break;
		//ID3.ID2.ID1.%
		// sprintf(szTmp, "%02X%02X%02X", pResponse->FAN.id1, pResponse->FAN.id2, pResponse->FAN.id3);
		//unit = 0 
	case pTypeFan:
		DeviceId = std::stoul(strDeviceId, 0, 16);
		//sensorId = (DeviceId <<8)  ;
		result = m_sql.safe_query("SELECT " DeviceID " FROM EnOceanNodes  WHERE (DeviceID like '%06X__') ", DeviceId);
		break;
		//%.ID2.ID1.%
		//		sprintf(szTmp, "%d", (pResponse->RFXMETER.id1 * 256) + pResponse->RFXMETER.id2);
		//unit=0
	case pTypeRFXMeter:
		DeviceId = std::stoul(strDeviceId, 0, 10);
		//sensorId = (DeviceId<<8)  ;
		result = m_sql.safe_query("SELECT " DeviceID " FROM EnOceanNodes  WHERE (DeviceID like '%%%04X__') ", DeviceId);
		break;
		//pTypeAirQuality device = ID2 unit=id1
	case pTypeAirQuality:
		DeviceId = std::stoul(strDeviceId, 0, 10);
		DeviceId = (DeviceId << 8) + unit;
		result = m_sql.safe_query("SELECT " DeviceID " FROM EnOceanNodes  WHERE (DeviceID like '%%%04X__') ", DeviceId);
		break;
	default:
		sensorId = DeviceId;
		break;
	}
	int NbDeviceId = result.size();
	if ((NbDeviceId == 1) && (deviceStatusResultSize == 1))
		sensorId = std::stoul(result[0][0], nullptr, 16);
	else
		sensorId = 0;
	return sensorId;
}
//delete the sensor in EnOceanNodes table assossiated to the device with ID in deviceStatus table
//take care that we could have many device.
void EnOceanDeleteSensorAssociatedWithDevice(const std::string& ID)
{
	//get the number of enocean devices with same DeviceId in order to delete from EnOceanNodes
	std::string SensorDeviceID = "";
	std::vector<std::vector<std::string> > result;
	//get DeviceID = DeviceID in EnOceanNodes
	result = m_sql.safe_query("SELECT DeviceID , HardwareID ,Unit , Type FROM DeviceStatus   WHERE (ID==%s) ", (ID).c_str());
	if (result.size() > 0) {
		SensorDeviceID = result[0][0];
		std::string HwID = result[0][1];
		uint32_t unit = std::stoul(result[0][2]);
		uint32_t devType = std::stoul(result[0][3]);
		//check if it is ESP3  hardware
		CEnOceanESP3* pEnoceanHardware = reinterpret_cast<CEnOceanESP3*>(m_mainworker.GetHardwareByIDType(HwID, HTYPE_EnOceanESP3));
		if (pEnoceanHardware != nullptr)
		{
			uint32_t iSensorDeviceID = enoceanGetSensorIdFromDevice(SensorDeviceID, unit, devType, 0);
			//select device with same  EnOceanNodes NodeID
			//delete only on the last device
			if (iSensorDeviceID != 0) {
				pEnoceanHardware->DeleteSensor(iSensorDeviceID);
			}
		}
	}
}
void CEnOceanESP3::parse_PACKET_MAN_SPECIFIC_COMMAND(unsigned char data[], int m_DataSize, int m_OptionalDataSize)
{
	// repeater level 0 55 00 0B 06 01 95   D1 00 46 08 00 00   05 01 33 BE  00     03 04 11 EA 38 FF     09
	// reponse          55 00 0A 07 01 EB   D1 46 00 0A 00      04 11 EA 38  00     01 FF FF FF FF 2D 00  58
		//if nodon
	if (data[1] == NODON)
	{
		if (data[3] == 0x0A)
		{
			const char* Repeater_status[] = { "OFF","1", "2" };
			int REP_LEVEL = data[4];
			if (REP_LEVEL <= 2)
			{
				uint32_t senderID = DeviceArrayToInt(&data[5]);
				Debug(DEBUG_NORM, "MSC : NODON id:%08X REPEATER Level=%d: %s ", senderID, REP_LEVEL, Repeater_status[REP_LEVEL]);
				return;
			}
		}
	}
	Log(LOG_ERROR, "MSC :ERP1: Manufacturer specific command not supported");
}
void CEnOceanESP3::DeleteSensor(uint32_t sensorId)
{
	m_sql.safe_exec_no_return("DELETE FROM EnOceanNodes WHERE (NodeID == %d )", sensorId);
	Debug(DEBUG_NORM, "Enocean: Delete Sensors  ID: %s", GetEnOceanIDToString(sensorId).c_str());
	LoadNodesFromDatabase();
}
uint32_t CEnOceanESP3::sendDataVld(unsigned int srcID, unsigned int destID, enocean::T_DATAFIELD* OffsetDes, int* values, int NbValues)
{
	uint8_t data[256 + 2];
	memset(data, 0, sizeof(data));
	uint32_t DataSize = setRawDataValues(data, OffsetDes, values, NbValues);
	if (DataSize)
		sendVld(srcID, destID, data, DataSize);
	else
		Log(LOG_ERROR, "Error argument number in sendVld : cmd :%s :%s ", OffsetDes->ShortCut.c_str(), OffsetDes->description.c_str());
	return DataSize;
}
void CEnOceanESP3::TeachInNodeIfExist(const uint32_t nodeID, const uint16_t manID, const uint8_t RORG, const uint8_t func, const uint8_t type, const TeachinMode teachin_mode)
{
	if (GetNodeInfo(nodeID) == nullptr)
	{
		TeachInNode(nodeID, manID, RORG, func, type, teachin_mode);
	}
	else
	{
		//already created 
		//yes if update the profile : used on ping received
		if (RORG != 0)
		{
			SetDbEnOceanValue(nodeID, "Rorg", RORG);
			SetDbEnOceanValue(nodeID, "Func", func);
			SetDbEnOceanValue(nodeID, "Type", type);
			m_nodes.setSensorProfile(nodeID, RORG, func, type);
			Log(LOG_NORM, "Update Node EEP : HwdID %u Node %08X Manufacturer %03X (%s) %sEEP %02X-%02X-%02X (%s)",
				m_HwdID, nodeID, manID, GetManufacturerName(manID),
				(teachin_mode == GENERIC_NODE) ? "Generic " : ((teachin_mode == VIRTUAL_NODE) ? "Virtual " : ""),
				RORG, func, type, GetEEPLabel(RORG, func, type));
		}
		if (manID != 0)
		{
			SetDbEnOceanValue(nodeID, "ManufacturerId", manID);
			m_nodes.setSensorManuf(nodeID, manID);
			Log(LOG_NORM, "Update Node Manufacturer: HwdID %u Node %08X Manufacturer %03X (%s) %sEEP %02X-%02X-%02X (%s)",
				m_HwdID, nodeID, manID, GetManufacturerName(manID),
				(teachin_mode == GENERIC_NODE) ? "Generic " : ((teachin_mode == VIRTUAL_NODE) ? "Virtual " : ""),
				RORG, func, type, GetEEPLabel(RORG, func, type));
		}
		if (teachin_mode > GENERIC_NODE)
			SetTeachInStatus((nodeID), teachin_mode);
	}
}
extern http::server::CWebServerHelper m_webservers;
void CEnOceanESP3::registerWebServerEntry(void)
{
	m_webservers.RegisterRType("enocean", RType_OpenEnOcean );
}
