#include "stdafx.h"
#include <string>
#include <json/json.h>
#include <boost/asio.hpp> 

#include "../main/Logger.h"
#include "../main/SQLHelper.h"
#include "../main/mainworker.h"
#include "../webserver/Base64.h"
#include "../hardware/ThermostatHardware.h"
#include "RFXNames.h"

#include "WebServerHelper.h"
extern http::server::CWebServerHelper m_webservers;

constexpr int MAX_LOG_SIZE = 2048;

enum DeviceTypeEnum
{
	DevDimmer = 0,
	DevSwitch,
	DevTemperature,
	DevCamera,
	DevCO2,
	DevShutter,
	DevDoor,
	DevFlood,
	DevMotion,
	DevSmoke,
	DevElectricity,
	DevGenericSensor,
	DevHygrometry,
	DevLuminosity,
	DevLock,              //affiche une serrure cadna ouvert/ferme //bouton ON/OFF
	DevMultiSwitch,
	DevNoise,
	DevPressure,
	DevRain,
	DevScene,
	DevUV,
	DevWind,
	DevCO2Alert,
	DevThermostat,
	DevRGBLight, // RGB(W) Light (dimmable)
	DevTempHygro		   // Temperature and Hygrometry combined sensor

};

//index of field in sql query
enum DeviceStatusField
{
	ID = 0,
	Name,
	nValue,
	Type,
	SubType,
	sValue,
	SwitchType,
	//	LastLevel
};

//map beetween DeviceId and RoomId
typedef std::map<std::string, std::string  > T_Map_Room_DeviceId;
static T_Map_Room_DeviceId       m_Map_Room_DeviceId;

//map beetween DeviceId and CDomoticzHardwareBase*
typedef std::map<std::string, CDomoticzHardwareBase* > T_Map_DeviceId_Hardware;
static T_Map_DeviceId_Hardware   m_Map_DeviceId_Hardware;

class  ImperiHome {
private:
	Json::Value root;
	Json::Value params;
	std::vector<std::string> sValueGlb;
	std::string nValueGlb;

	const char* GetTypeDevice(DeviceTypeEnum dev);
	void ManageAction(std::string& device, std::string& action, std::string& actionType, std::string actionValue);
	DeviceTypeEnum LightType(TSqlRowQuery* row, Json::Value& params);
	void updateRoot(std::string pidx, std::string pname, DeviceTypeEnum ApType);
	void updateRoot(std::string pidx, std::string pname, DeviceTypeEnum ApType, std::string proom);
	void DeviceContent(std::string& rep_content);
	void getRoomContent(std::string& rep_content);
	void SetKey(const char* KeyName, std::string KeyValue);
	void SetKey(const char* KeyName, std::string KeyValue, const char* Unit, bool graphable);
	void SetKeys(const char* KeyName, ...);
	void manageTypeGeneral(TSqlRowQuery* row, Json::Value& params);
	void ManageHisto(std::string& device, std::string& value, std::string& histo, std::string& from, std::string& to, std::string& rep_content);
	void getDeviceCamera();
	void getGraphic(std::string& idx, std::string TableName, std::string FieldName, std::string KeyName, time_t DateStart, time_t  DateEnd, std::string& rep_content, double CoefA, double CoefB);
	void         setRoomId(std::string& DeviceRowID, std::string RoomId);
	std::string  getRoomId(std::string& DeviceRowID);
	void clearRoomIds();
	void build_Map_Room_DeviceId();
	bool is_Map_Room_DeviceId_built();
	std::string  GetSelectorSwitchLevelString(const std::map<std::string, std::string>& options, int llevel);
public:
	bool Request(std::string& request_path, std::string& rep_content);
	void getScenes();
	void setGenericSensor(TSqlRowQuery* row);
	void getValueFromRow(TSqlRowQuery* row, std::vector<std::string>& sValueGlb, std::string& nValueGlb);
	CDomoticzHardwareBase* GetDeviceHardwareFromDeviceID(const std::string& deviceidx);
};

const char* DeviceTypeString[] = {
"DevDimmer",
"DevSwitch",
"DevTemperature",
"DevCamera",
"DevCO2",
"DevShutter",
"DevDoor",
"DevFlood",
"DevMotion",
"DevSmoke",
"DevElectricity",
"DevGenericSensor",
"DevHygrometry",
"DevLuminosity",
"DevLock",
"DevMultiSwitch",
"DevNoise",
"DevPressure",
"DevRain",
"DevScene",
"DevUV",
"DevWind",
"DevCO2Alert",
"DevThermostat",
"DevRGBLight",
"DevTempHygro"
};

const char* PKEYVALUE = "value";

//graphic table / field 
typedef struct {
	DeviceTypeEnum	IssType;			  //ISS device type DeviceTypeEnum
						  //if = 0 : all type
	std::string KeyName;		//key value name
	std::string Table;			//table name
	std::string Field;			//field name
	double		coefA;
	double		coefB;

}T_GRAPHIC;

//this table give the Table Name / field Name for the short log in order to get the graphic values
// from the ptype and ISS request key Name value

// used to display graphycal 
T_GRAPHIC GraphicTable[] = {
	//ISS device type       Json Key Name     Sql Table NAme       Sql field Name       coefA          CoefB
	//DeviceTypeEnum
	{ DevDimmer             ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevSwitch             ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevTemperature        ,PKEYVALUE       ,"TEMPERATURE"       ,"Temperature"       , 1.0         , 0.0      },
	{ DevCamera             ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevCO2                ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevShutter            ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevDoor               ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevFlood              ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevMotion             ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevSmoke              ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevElectricity        ,"Watts"         ,"Meter"             ,"Usage"             , 1.0         , 0.0      },
	{ DevGenericSensor      ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevHygrometry         ,PKEYVALUE       ,"TEMPERATURE"       ,"Humidity"          , 1.0         , 0.0      },
	{ DevLuminosity         ,PKEYVALUE       ,"Meter"             ,"Value"             , 1.0         , 0.0      },
	{ DevLock               ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevMultiSwitch        ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevNoise              ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevPressure           ,PKEYVALUE       ,"TEMPERATURE"       ,"Barometer"         , 1.0         , 0.0      },
	{ DevRain               ,"Value"         ,"Rain"              ,"Rate"              , 0.01        , 0.0      },
	{ DevRain               ,"Accumulation"  ,"Rain"              ,"Total"             , 1.0         , 0.0      },
	{ DevScene              ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevUV                 ,"Value"         ,"UV"                ,"Level"             , 1.0         , 0.0      },
	{ DevWind               ,"Speed"         ,"Wind"              ,"Speed"             , 1.0         , 0.0      },
	{ DevWind               ,"Direction"     ,"Wind"              ,"Direction"         , 1.0         , 0.0      },
	{ DevCO2Alert           ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevThermostat         ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevRGBLight           ,""              ,""                  ,""                  , 1.0         , 0.0      },
	{ DevTempHygro          ,"temp"          ,"TEMPERATURE"       ,"Temperature"       , 1.0         , 0.0      },
	{ DevTempHygro          ,"hygro"         ,"TEMPERATURE"       ,"Humidity"          , 1.0         , 0.0      },
};

//get DomoticzHardwareBase object from device Idx :deviceidx in device status table
CDomoticzHardwareBase* GetDeviceHardwareFromDeviceId(const std::string& deviceidx)
{
	//get device hardware ID from device ID
	auto result = m_sql.safe_query("SELECT HardwareID FROM DeviceStatus WHERE (ID == '%q')", deviceidx.c_str());
	if (result.size() < 1)
		return NULL;
	int HardwareID = std::stoi(result[0][0].c_str());

	//get CDomoticzHardwareBase 
	CDomoticzHardwareBase* pHardware = m_mainworker.GetHardware(HardwareID);
	return pHardware;
}

//get DomoticzHardwareBase object from device Idx :deviceidx in device status table
CDomoticzHardwareBase* ImperiHome::GetDeviceHardwareFromDeviceID(const std::string& deviceidx)
{
	CDomoticzHardwareBase* DomoticzHardware = m_Map_DeviceId_Hardware[deviceidx];
	if (DomoticzHardware == nullptr)
	{
		DomoticzHardware = GetDeviceHardwareFromDeviceId(deviceidx);
		m_Map_DeviceId_Hardware[deviceidx] = DomoticzHardware;
	}
	return DomoticzHardware;
}

/**
* Returns the level name   associated to a integer value 
*/
std::string  ImperiHome::GetSelectorSwitchLevelString(const std::map<std::string, std::string>& options, int llevel) {

std::map<std::string, std::string> statuses;
		GetSelectorSwitchStatuses(options, statuses);
		return statuses[std::to_string(llevel)];
}

void ImperiHome::getValueFromRow(TSqlRowQuery* row, std::vector<std::string>& sValueGlb, std::string& nValueGlb) {
	StringSplit((*row)[sValue], ";", sValueGlb);
	for (int i = sValueGlb.size(); i < 5; i++)
		sValueGlb.push_back("");
	nValueGlb = (*row)[nValue];
}

void ImperiHome::setGenericSensor(TSqlRowQuery* row) {
	std::string dID = (*row)[ID];
	std::string dName = (*row)[Name];
	//	if (nValueGlb != "0")
	//		SetKey( PKEYVALUE, nValueGlb);
	if (sValueGlb[0].length() != 0)
		SetKey(PKEYVALUE, sValueGlb[0]);
	else
		SetKey(PKEYVALUE, nValueGlb);
	updateRoot(dID, dName, DevGenericSensor);
}

const char* ImperiHome::GetTypeDevice(DeviceTypeEnum dev)
{
	if (dev < sizeof(DeviceTypeString) / sizeof(char*))
		return DeviceTypeString[dev];
	else
		return "UNKNOWN";
}

//device name is devXXX_type
std::string getDeviceId(std::string& device)
{
	//the dev Id is DEVnnn_zzz : nnn is the ID 
	return  device.substr(3, device.find("_") - 3);
}

std::string buildDeviceId(std::string& pidx, DeviceTypeEnum ApType)
{
	//the dev Id is DEVnnn_zzz : nnn is the ID  zzz: the DeviceTypeEnum ApType 
//	return  "dev" + pidx + "_" + DevTypeName;
	return  "dev" + pidx + "_" + std::to_string(ApType);
}

std::string getDeviceTypeName(std::string& device)
{
	//the dev Id is DEVnnn_zzz : nnn is the ID 
	//return zzz
	int posUnder = device.find("_");
	if (posUnder == std::string::npos)
		return "";
	else
		return  device.substr(posUnder + 1, 100);
}

void SqlGetTypeSubType(std::string& idx, int& dType, int& dSubType)
{
	dType = 0;
	dSubType = 0;
	auto result = m_sql.safe_query("SELECT Type,SubType  FROM DeviceStatus where (ID==%s)", idx.c_str());
	if (result.size() > 0)
	{
		TSqlRowQuery* row = &result[0];
		dType = std::stoi((*row)[0].c_str());
		dSubType = std::stoi((*row)[1].c_str());
	}
}
std::string  GetDeviceValue(const char* FieldName, const std::string& Idx)
{
	bool result = false;
	std::vector<std::vector<std::string> > sqlresult;

    sqlresult = m_sql.safe_query("SELECT %s FROM DeviceStatus WHERE ( ID= %s )" , FieldName, Idx.c_str() );

	if (!sqlresult.empty())
	{
		return sqlresult[0][0] ;
	}

	return "" ;
}

//from to ms since 1/1/1970
void ImperiHome::ManageHisto(std::string& device, std::string& value, std::string& histo, std::string& from, std::string& to, std::string& rep_content)
{
	time_t  DateStartSec;
	time_t  DateEndSec;

	//the dev Id is DEVnnn_zzz : nnn is the ID zzz : Iss Device type
	std::string ID = getDeviceId(device);

	std::string TypeName = getDeviceTypeName(device);
	DeviceTypeEnum IssType = (DeviceTypeEnum)std::stoi(TypeName.c_str());

	std::string name = GetDeviceValue("Name", ID);
	if (name.empty())
	{
		_log.Log(LOG_ERROR, "IMPE: Graphic Devices:%s not found ", device.c_str());
	}
	else
	{
		_log.Debug(DEBUG_NORM, "IMPE: Graphic Device:%s:%s Value:%s Histo:%s From:%s To:%s", device.c_str(), name.c_str(), value.c_str(), histo.c_str(), from.c_str(), to.c_str());

		//divide by 1000 = sec
		from = from.substr(0, from.length() - 3);
		to = to.substr(0, to.length() - 3);

		DateStartSec = atol(from.c_str());
		DateEndSec = atol(to.c_str());

		for (int i = 0; i < sizeof(GraphicTable) / sizeof(T_GRAPHIC); i++) {
			if ((GraphicTable[i].IssType == IssType) && (stricmp(GraphicTable[i].KeyName.c_str(), value.c_str())==0))
			{
				getGraphic(ID, GraphicTable[i].Table, GraphicTable[i].Field, value, DateStartSec, DateEndSec, rep_content,GraphicTable[i].coefA ,GraphicTable[i].coefB );
				return;
			}
		}
		_log.Log(LOG_ERROR, "IMPE: Graphic Devices:%s not found ", device.c_str());
	}

}

//convert thermostat string state to int state : OFF-->0  ECO-->1
unsigned int  ThermostatModeStringToInt(std::string& mode, std::string& AvailableMode)
{
	std::vector<std::string> ModeStr;
	StringSplit(AvailableMode, ",", ModeStr);

	for (size_t i = 0; i < ModeStr.size(); i++)
		if (strcmp(mode.c_str(), ModeStr[i].c_str()) == 0)
			return  i;
	return 0;
}
//convert interger thermostat state to string state : 0--> OFF 1-->ECO
std::string   ThermostatModeIntToString(unsigned int Mode, std::string& AvailableMode)
{
	std::vector<std::string> ModeStr;
	StringSplit(AvailableMode, ",", ModeStr);
	if (Mode < ModeStr.size())
		return ModeStr[Mode];
	else
		return "UNKNOWN";
}

void ImperiHome::ManageAction(std::string& device, std::string& action, std::string& actionType, std::string actionValue)
{
	//the dev Id is DEVnnn_zzz : nnn is the ID 
	std::string ID = getDeviceId(device);
	const std::string User = "IMPE";

	std::string name = GetDeviceValue("Name", ID);
	if (name.empty())
	{
		_log.Log(LOG_ERROR, "IMPE: Devices:%s not found ", device.c_str());
	}
	else {
		_log.Debug(DEBUG_NORM, "IMPE: Devices:%s:%s Action:%s request:%s Value:%s", device.c_str(), name.c_str(), action.c_str(), actionType.c_str(), actionValue.c_str());
		if (actionType == "setStatus")
		{
			if (actionValue == "1")
				m_mainworker.SwitchLight(ID, "On", "100", "0", "0", 0, User);
			else
				m_mainworker.SwitchLight(ID, "Off", "0", "0", "0", 0, User);
		}
		else if (actionType == "setLevel") {
			m_mainworker.SwitchLight(ID, "Set Level", actionValue, "0", "0", 0, User);
		}
		else if (actionType == "stopShutter") {
			m_mainworker.SwitchLight(ID, "Stop", actionValue, "0", "0", 0, User);

		}
		else if (actionType == "pulseShutter") {
		}
		else if (actionType == "launchScene") {
			m_mainworker.SwitchScene(ID, "On", User);
		}
		else if (actionType == "setChoice") {
			auto options = m_sql.GetDeviceOptions(ID);

			std::map<std::string, std::string> statuses;
			int level = GetSelectorSwitchLevel(options, actionValue);
			if (level < 0) {
				_log.Log(LOG_ERROR, "Setting a wrong level value %s to Selector device %lu", actionValue.c_str(), ID);
			}
			m_mainworker.SwitchLight(ID, "Set Level", std::to_string(level), "0", "0", 0, User);

		}
		else if (actionType == "setMode") {
			/*
							if (actionValue==ConfMode){
								 m_mainworker.SetSetPoint(ID, (float)m_VirtualThermostat->GetConfortTemp(ID.c_str()));
							}
							else
							if (actionValue==EcoMode){
								 m_mainworker.SetSetPoint(ID, (float)m_VirtualThermostat->GetEcoTemp(ID.c_str()));
							}
							if (actionValue==OffMode){
							}*/
			CThermostatHardware* pThermostatHardware = dynamic_cast<CThermostatHardware*>(GetDeviceHardwareFromDeviceID(ID));
			if (pThermostatHardware != 0)
				m_mainworker.SetThermostatState(ID, ThermostatModeStringToInt(actionValue, pThermostatHardware->GetAvailableMode()));
		}
		else if (actionType == "setSetPoint") {
			m_mainworker.SetSetPoint(ID, (float)std::stof(actionValue.c_str()));
		}
	}
}

void ImperiHome::SetKey(const char* KeyName, std::string KeyValue)
{
	int KeyNum = params.size();
	params[KeyNum]["key"] = KeyName;
	params[KeyNum]["value"] = KeyValue;
}

void ImperiHome::SetKey(const char* KeyName, std::string KeyValue, const char* Unit, bool graphable)
{
	int KeyNum = params.size();
	params[KeyNum]["key"] = KeyName;
	params[KeyNum]["value"] = KeyValue;
	if (KeyValue.length())
		if (Unit != 0)
			if (Unit[0] != 0)
				params[KeyNum]["unit"] = Unit;
	if (graphable)
		params[KeyNum]["graphable"] = graphable;
}

void ImperiHome::SetKeys(const char* KeyName, ...)
{
	va_list value;
	/* Initialize the va_list structure */
	va_start(value, KeyName);
	const char* KeyValue = va_arg(value, char*);
	const char* Unit = va_arg(value, char*);
	bool graphable = va_arg(value, bool);

	SetKey(KeyName, KeyValue, Unit, graphable);

	for (int KeyNum = 1; ; KeyNum++)
	{
		const char* KeyName = va_arg(value, char*);
		if (KeyName == 0)
			return;
		const char* KeyValue = va_arg(value, char*);
		const char* Unit = va_arg(value, char*);
		bool graphable = va_arg(value, bool);

		SetKey(KeyName, KeyValue, Unit, graphable);
	}
	va_end(value);
}

//manage pTypeGeneral
void ImperiHome::manageTypeGeneral(TSqlRowQuery* row, Json::Value& params)
{
	int dSubType = std::stoi((*row)[SubType].c_str());
	std::string dID = (*row)[ID];
	std::string dName = (*row)[Name];
	switch (dSubType)
	{
	case  sTypeVisibility:
		setGenericSensor(row);
		//							float vis = (float)std::stof(sValue.c_str());
		//							root["result"][ii]["SwitchTypeVal"] = metertype;
		break;
	case  sTypeSolarRadiation:
		setGenericSensor(row);
		//							float radiation = (float)std::stof(sValue.c_str());
		//							root["result"][ii]["SwitchTypeVal"] = metertype;
		break;
	case  sTypeSoilMoisture:
		setGenericSensor(row);
		//							sprintf(szTmp, "%d cb", nValue);
		//							root["result"][ii]["SwitchTypeVal"] = metertype;
		break;
	case  sTypeLeafWetness:
		setGenericSensor(row);
		//							sprintf(szTmp, "%d", nValue);
		//							root["result"][ii]["SwitchTypeVal"] = metertype;
		break;
	case  sTypeSystemTemp:
		SetKey(PKEYVALUE, sValueGlb[0], "째C", true);
		updateRoot(dID, dName, DevTemperature);
		//							double tvalue = ConvertTemperature(std::stof(sValue.c_str()), tempsign);
		//							root["result"][ii]["Type"] = "temperature";
		break;
	case  sTypePercentage:
		setGenericSensor(row);
		//							sprintf(szData, "%.2f%%", std::stof(sValue.c_str()));
		//							root["result"][ii]["TypeImg"] = "hardware";
		break;
	case  sTypeFan:
		setGenericSensor(row);
		//							sprintf(szData, "%d RPM", std::stoi(sValue.c_str()));
		//							root["result"][ii]["Type"] = "Fan";
		break;
	case  sTypeVoltage:
		setGenericSensor(row);
		//							sprintf(szData, "%.3f V", std::stof(sValue.c_str()));
		//							root["result"][ii]["Voltage"] = std::stof(sValue.c_str());
		break;
	case  sTypePressure:
		SetKey(PKEYVALUE, sValueGlb[0], "bar", true);
		updateRoot(dID, dName, DevPressure);

		//							sprintf(szData, "%.1f Bar", std::stof(sValue.c_str()));
		//							root["result"][ii]["Pressure"] = std::stof(sValue.c_str());
		break;
	case  sTypeSetPoint:
		//temperature
		SetKey("curmode", "Confor");
		SetKey("curtemp", (*row)[sValue], "째C", false);

		SetKey("cursetpoint", (*row)[sValue]);
		SetKey("step", "0.5");
		SetKey("availablemodes", "Confor");
		updateRoot(dID, dName, DevThermostat);
		break;
	case  sTypeTemperature:
		SetKey(PKEYVALUE, sValueGlb[0], "째C", true);
		updateRoot(dID, dName, DevTemperature);
		break;
	case  sTypeZWaveClock:
		setGenericSensor(row);
		//								day = std::stoi(tstrarray[0].c_str());
		//								hour = std::stoi(tstrarray[1].c_str());
		//								minute = std::stoi(tstrarray[2].c_str());
		//							root["result"][ii]["DayTime"] = sValue;
		break;
	case  sTypeTextStatus:
		setGenericSensor(row);
		//							root["result"][ii]["Data"] = sValue;//
		break;
	case  sTypeZWaveThermostatMode:
		setGenericSensor(row);
		//							sprintf(szData, "%s", ZWave_Thermostat_Modes[nValue]);
		//							root["result"][ii]["Data"] = szData;
		//							root["result"][ii]["Mode"] = nValue;
		break;
	case  sTypeZWaveThermostatFanMode:
		setGenericSensor(row);
		break;
	case  sTypeAlert:
		setGenericSensor(row);
		//							root["result"][ii]["Data"] = sValue;//
		break;
	case  sTypeCurrent:
		setGenericSensor(row);
		break;
	case  sTypeSoundLevel:
		setGenericSensor(row);
		break;
	case  sTypeBaro:
		SetKey(PKEYVALUE, sValueGlb[0], "bar", false);
		updateRoot(dID, dName, DevPressure);
		break;
	case  sTypeDistance:
		setGenericSensor(row);
		break;
	case  sTypeCounterIncremental:
		setGenericSensor(row);
		break;
	case  sTypeKwh:
		SetKey("Watts", sValueGlb[0], "W", true);
		SetKey("ConsoTotal", sValueGlb[1], "kWh", false);
		updateRoot(dID, dName, DevElectricity);
		break;
	case  sTypeWaterflow:
		setGenericSensor(row);
		break;
	case  sTypeCustom:
		setGenericSensor(row);
		break;
	default:
		setGenericSensor(row);
		break;
	}
}

DeviceTypeEnum ImperiHome::LightType(TSqlRowQuery* row, Json::Value& params)
{
	DeviceTypeEnum ApType = DevSwitch;
	int nVal;
	int dtype = std::stoi((*row)[SwitchType].c_str());
	std::string DeviceID = ((*row)[ID].c_str());

	switch (dtype)
	{
	case STYPE_OnOff:
		ApType = DevSwitch;
		SetKey("Status", nValueGlb);
		break;
	case STYPE_Contact:
	case STYPE_DoorContact:
		//		ApType = DevDoor;                //display a padlock  : nValueGlb = 0 green padlock closed 1 red padlock open  | Armable=1 : Armed=0 : display unarmed  1: display Armed  | Armable=0 display OK Alert lighting
		//		SetKey( "Armable", "1");
		//		SetKey( "Armed"  , "0");       // Armed=0 : display unarmed  1: display Armed
		//		SetKey( "Tripped", nValueGlb); //display a padlock  : nValueGlb = 0 green padlock closed 1 red padlock open 

		//		SetKey( "Armable", "0");
		//		SetKey( "Tripped", nValueGlb); //display a padlock  : nValueGlb = 0 green padlock closed 1 red padlock open  | Armable=0 display OK Alert lighting

		//		ApType = DevLock;               //display a padlock  : nValueGlb = 0 green padlock closed 1 red padlock open  with switch  ON/OFF
		//		SetKey( "Status", nValueGlb); 

		//		ApType = DevGenericSensor;
		//		if (nValueGlb == "1")
		//			SetKey( "Value", "Open");
		//		else
		//			SetKey( "Value", "Closed");

		ApType = DevSwitch;
		SetKey("Status", nValueGlb);

		break;
	case STYPE_Blinds:
		ApType = DevShutter;
		SetKey("stopable", "0");
		SetKey("pulseable", "0");
		SetKey("Level", sValueGlb[0]);
		break;
	case STYPE_SMOKEDETECTOR:
		/*      Armable	Ability to arm the device : 1 = Yes / 0 = No
			Ackable	Ability to acknowledge alerts : 1 = Yes / 0 = No
			Armed	Current arming status : 1 = On / 0 = Off
			Tripped	Is the sensor tripped ? (0 = No - 1 = Tripped)			*/
		ApType = DevSmoke;
		SetKey("Armable", "0");
		SetKey("Ackable", "0");
		SetKey("Armed", "1");
		SetKey("Tripped", nValueGlb);
		break;
	case STYPE_Dimmer:
		ApType = DevDimmer;
		nVal = std::stoi(nValueGlb.c_str());
		if (nVal == 0)
			SetKey("Status", "0");
		else
			SetKey("Status", "1");

		SetKey("level", sValueGlb[0]);
		break;
	case STYPE_Motion:
		ApType = DevMotion;
		SetKey("Armable", "0");
		SetKey("Ackable", "0");
		SetKey("Armed", "1");
		SetKey("Tripped", nValueGlb);
		break;
	case STYPE_PushOn:
	case STYPE_PushOff:
		ApType = DevSwitch;
		SetKey("Status", nValueGlb);
		break;
	case STYPE_BlindsPercentage:
	case STYPE_BlindsPercentageWithStop:
	case STYPE_VenetianBlindsUS:
	case STYPE_VenetianBlindsEU:
		ApType = DevShutter;
		SetKey("stopable", "1");
		SetKey("pulseable", "0");
		SetKey("Level", sValueGlb[0]);
		break;
	case STYPE_DoorLock:
		//		ApType = DevMotion;
		ApType = DevLock;
		SetKey("Armable", "0");
		SetKey("Tripped", nValueGlb);
		break;
	case STYPE_DoorLockInverted:
		ApType = DevLock;               //display a padlock  : nValueGlb = 0 green padlock closed 1 red padlock open  with switch  ON/OFF
		if (nValueGlb == "1")nValueGlb = "0"; else nValueGlb = "1"; //inverted
		SetKey("Armable", "0");
		SetKey("Tripped", nValueGlb);
		break;
	case STYPE_Selector: {
		ApType = DevMultiSwitch;
		auto options = m_sql.GetDeviceOptions(DeviceID);
		std::string  sOptions = options["LevelNames"];
		std::replace(sOptions.begin(), sOptions.end(), '|', ',');
		int llevel = std::stoi(sValueGlb[0].c_str());
		std::string value = GetSelectorSwitchLevelString(options, llevel);
		SetKey("Value", value);
		SetKey("Choices", sOptions);
		}
		break;
	case STYPE_Dusk:
		ApType = DevSwitch;
		SetKey("Status", nValueGlb);
		break;
	case STYPE_X10Siren:
	case STYPE_Doorbell:
	case STYPE_Media:
	default:
		ApType = DevSwitch; SetKey("Status", nValueGlb); _log.Log(LOG_ERROR, "Swicth type not found %d for device:%s", SwitchType, (*row)[Name].c_str());
		break;
	}
	return ApType;
}

void ImperiHome::updateRoot(std::string pidx, std::string pname, DeviceTypeEnum ApType, std::string proom)
{
	int ii = root["devices"].size();

	root["devices"][ii]["id"] = buildDeviceId(pidx, ApType);
	root["devices"][ii]["name"] = pname;		//Name
	if (!proom.empty())
		root["devices"][ii]["room"] = proom;
	root["devices"][ii]["type"] = GetTypeDevice(ApType);		//type
	root["devices"][ii]["params"] = params;
	params.clear();
}
void ImperiHome::updateRoot(std::string pidx, std::string pname, DeviceTypeEnum ApType)
{
	std::string roomId = getRoomId(pidx);
	updateRoot(pidx, pname, ApType, roomId);
	//updateRoot(pidx, pname, ApType , "roomID0"); //all 
}

void ImperiHome::DeviceContent(std::string& rep_content)
{
	int dtype, dSubType = 0;
	root.clear();
	//build m_Map_Room_DeviceId if not done
	if (!is_Map_Room_DeviceId_built())
		build_Map_Room_DeviceId();

	auto result = m_sql.safe_query("SELECT ID,Name,nValue,Type,SubType,sValue,SwitchType  FROM DeviceStatus where (used==1) order by Type,SubType");

	for (unsigned int ii = 0; ii < result.size(); ii++)
	{
		TSqlRowQuery* row = &result[ii];
		params.clear();
		dtype = std::stoi((*row)[Type].c_str());
		dSubType = std::stoi((*row)[SubType].c_str());
		getValueFromRow(row, sValueGlb, nValueGlb);
		std::string dID = (*row)[ID];
		std::string dName = (*row)[Name];

		switch (dtype)
		{
		case pTypeLighting1:
		case pTypeLighting2:
		case pTypeLighting3:
		case pTypeLighting4:
		case pTypeLighting5:
		case pTypeLighting6:
			//        case pTypeLimitlessLights   :
		case pTypeSecurity1:
		case pTypeCurtain:
		case pTypeBlinds:
		case pTypeRFY:
		case pTypeChime:
		case pTypeThermostat2:
		case pTypeThermostat3:
		case pTypeRemote:
		case pTypeRego6XXValue:
			updateRoot(dID, dName, LightType(row, params));
			break;
		case pTypeTEMP:
			SetKey(PKEYVALUE, sValueGlb[0], "째C", true);
			updateRoot(dID, dName, DevTemperature);
			break;
		case pTypeHUM:
			SetKey(PKEYVALUE, nValueGlb, "%", false);
			updateRoot(dID, dName, DevHygrometry);
			break;
		case pTypeTEMP_HUM:
			//DevTempHygro
			SetKey("temp", sValueGlb[0], "째C", true);
			SetKey("hygro", sValueGlb[1], "%", true);
			updateRoot(dID, dName, DevTempHygro);
			break;
		case pTypeTEMP_HUM_BARO:
			//DevTempHygro
			SetKey("temp", sValueGlb[0], "째C", true);
			SetKey("hygro", sValueGlb[1], "%", true);
			updateRoot(dID, dName, DevTempHygro);
			//baro
			SetKey(PKEYVALUE, sValueGlb[3], "mbar", true);
			updateRoot(dID, dName, DevPressure);
			break;
		case pTypeTEMP_BARO:
			//temperature
			SetKey(PKEYVALUE, sValueGlb[0], "째C", true);
			updateRoot(dID, dName, DevTemperature);
			//baro
			SetKey(PKEYVALUE, sValueGlb[1], "mbar", true);
			updateRoot(dID, dName, DevPressure);
			break;
		case pTypeUV:
			//UVI
			SetKey(PKEYVALUE, sValueGlb[0], "", true);
			updateRoot(dID, dName, DevUV);
			//temperature
			SetKey(PKEYVALUE, sValueGlb[1], "째C", true);
			updateRoot(dID, dName, DevTemperature);
			break;
		case pTypeWIND:
			//wind direction /speed
			//sValueGlb[0] = direction float
			//sValueGlb[1] =  ?
			//sValueGlb[2] = speed
			//sValueGlb[3] = gust
			//sValueGlb[4] = temp
			//sValueGlb[5] = chil
			SetKey("Direction", sValueGlb[0], "�", false);
			{
				int intGust = std::stoi(sValueGlb[3].c_str());
				double speed = (intGust)*m_sql.m_windscale;
				SetKey("Speed", std_format("%4.1f", speed), "km/h", false);
			}
			updateRoot(dID, dName, DevWind);
			break;
		case pTypeRAIN:
			SetKey("Value", sValueGlb[0], "mm/h", true);
			SetKey("Accumulation", sValueGlb[1], "mm", true);
			updateRoot(dID, dName, DevRain);
			break;
		case pTypeCURRENTENERGY://CM180i
		case pTypePOWER:
			//data= I1;I2;I3
			SetKey("Watts", sValueGlb[0], "W", true);
			updateRoot(dID, dName, DevElectricity);
			/*            if (sValueGlb.size()>=2){
						  SetKey("Watts"     ,sValueGlb[1] ,"W" ,false );
						updateRoot( row , DevElectricity ,"i2");
						}
						if (sValueGlb.size()>=3){
						  SetKey("Watts"     ,sValueGlb[2] ,"W" ,false );
						updateRoot(  row , DevElectricity ,"i3");
						}
			*/

			break;
		case pTypeENERGY://   //CM119/160  //CM180
			SetKey("Watts", sValueGlb[0], "W", true);
			SetKey("ConsoTotal", sValueGlb[1], "kWh", false);
			updateRoot(dID, dName, DevElectricity);
			break;
		case pTypeAirQuality:
			SetKey("Value", nValueGlb, "ppm", false);
			updateRoot(dID, dName, DevCO2);
			break;
		case pTypeThermostat:
		{
			CDomoticzHardwareBase* pHardware = GetDeviceHardwareFromDeviceID((*row)[ID].c_str());
			CThermostatHardware* pThermostatHardware = dynamic_cast<CThermostatHardware*>(pHardware);
			if (pThermostatHardware != 0)
			{
				//temperature
				SetKey("curmode", pThermostatHardware->GetCurrentMode((*row)[ID]));
				SetKey("curtemp", pThermostatHardware->GetRoomTemperature((*row)[ID]), "째C", false);
				SetKey("cursetpoint", (*row)[sValue]);
				SetKey("step", "0.5");
				SetKey("availablemodes", pThermostatHardware->GetAvailableMode());
				updateRoot(dID, dName, DevThermostat);
			}
			else
			{
				//temperature
				SetKey("curmode", "Confor");
				SetKey("curtemp", (*row)[sValue], "째C", false);
				SetKey("cursetpoint", (*row)[sValue]);
				SetKey("step", "0.5");
				SetKey("availablemodes", "Confor");
				updateRoot(dID, dName, DevThermostat);
			}
		}
		break;
		case pTypeGeneral:
			manageTypeGeneral(row, params);
			break;
		case pTypeGeneralSwitch:
			//          SetKey("Status",nValueGlb) ;
			//		  updateRoot( dID,dName, DevSwitch);
			updateRoot(dID, dName, LightType(row, params));
			break;
		case pTypeLux:
			SetKey(PKEYVALUE, sValueGlb[0], "lux", true);
			updateRoot(dID, dName, DevLuminosity);
			break;
		case pTypeUsage:
			SetKey("Watts", sValueGlb[0], "W", true);
			SetKey("ConsoTotal", sValueGlb[1], "kWh", false);
			updateRoot(dID, dName, DevElectricity);
			break;
		case pTypeRFXSensor:
		case pTypeRFXMeter:
		case pTypeYouLess:
		case pTypeP1Power:
		case pTypeP1Gas:
		case pTypeCURRENT:
		case pTypeWEIGHT:
		default:
			setGenericSensor(row);
			_log.Log(LOG_ERROR, "device type not managed %d name:%s", dtype, (*row)[Name].c_str());
			break;
		}
	}
	getDeviceCamera();
	getScenes();
	rep_content = root.toStyledString();
}

void ImperiHome::getScenes()
{
	Json::Value root;
	m_webservers.RType_Scenes(root);
	if (root["status"] == "OK")
		for (unsigned int ii = 0; ii < root["result"].size(); ii++)
		{
			std::string lastU = root["result"][ii]["LastUpdate"].asString();
			params.clear();
			SetKey("LastRun", lastU);
			updateRoot(root["result"][ii]["idx"].asString(), root["result"][ii]["Name"].asString(), DevScene);
		}
	//					root["result"][ii]["idx"] = sd[0];
	//					root["result"][ii]["Name"] = sd[1];
	//					root["result"][ii]["HardwareID"] = HardwareID;
	//					root["result"][ii]["CodeDeviceName"] = CodeDeviceName;
	//					root["result"][ii]["Favorite"] = std::stoi(sd[3].c_str());
	//					root["result"][ii]["Protected"] = (iProtected != 0);
	//					root["result"][ii]["OnAction"] = onaction;
	//					root["result"][ii]["OffAction"] = offaction;
	//					root["result"][ii]["Type"] = "Scene";
	//					root["result"][ii]["Type"] = "Group";
	//					root["result"][ii]["LastUpdate"] = sLastUpdate;
	//					root["result"][ii]["Status"] = "Off";
	//					root["result"][ii]["Status"] = "On";
	//					root["result"][ii]["Status"] = "Mixed";
}

std::string GenerateCamImageURL(std::string address, std::string port, std::string username, std::string password, std::string  imageurl)
{
	std::string feedsrc = "http://";
	bool  bHaveUPinURL = (imageurl.find("#USERNAME") != std::string::npos) || (imageurl.find("#PASSWORD") != std::string::npos);

	if (!bHaveUPinURL) {
		if (username != "")
		{
			feedsrc += username + ":" + password + "@";
		}
	}
	feedsrc += address;
	if (port != "80") {
		feedsrc += ":" + port;
	}
	feedsrc += "/" + imageurl;
	if (bHaveUPinURL) {
		feedsrc = feedsrc.replace(feedsrc.find("#USERNAME"), 9, username);
		feedsrc = feedsrc.replace(feedsrc.find("#PASSWORD"), 9, password);
	}
	return feedsrc;
}

void ImperiHome::getDeviceCamera()
{
	//	var feedsrc=GenerateCamImageURL(csettings.address,csettings.port,csettings.username,csettings.password,csettings.imageurl);
	//  		szQuery << "SELECT ID, Name, Enabled, Address, Port, Username, Password, ImageURL FROM Cameras WHERE (Enabled=='1') ORDER BY ID ASC";
	//    TSqlQueryResult result=m_sql.safe_query("SELECT Name,nValue,ID,Type,SubType,sValue,SwitchType,LastLevel,RoomTemp,SwitchIdx,AddjValue,AddjValue2  FROM DeviceStatus where (used==1)"  ) ;
	auto result = m_sql.safe_query("SELECT ID, Name, Enabled, Address, Port, Username, Password, ImageURL FROM Cameras WHERE (Enabled=='1') ORDER BY ID ASC");

	for (unsigned int ii = 0; ii < result.size(); ii++)
	{
		TSqlRowQuery* sd = &result[ii];
		params.clear();
		std::string dID = (*sd)[ID];
		std::string dName = (*sd)[Name];

		/*				root["result"][ii]["idx"] = sd[0];
						root["result"][ii]["Name"] = sd[1];
						root["result"][ii]["Enabled"] = (sd[2] == "1") ? "true" : "false";
						root["result"][ii]["Address"] = sd[3];
						root["result"][ii]["Port"] = std::stoi(sd[4].c_str());
						root["result"][ii]["Username"] = base64_decode(sd[5]);
						root["result"][ii]["Password"] = base64_decode(sd[6]);
						root["result"][ii]["ImageURL"] = sd[7];
		*/
		std::string url = GenerateCamImageURL((*sd)[3], (*sd)[4], base64_decode((*sd)[5]), base64_decode((*sd)[6]), (*sd)[7]);
		//        SetKey("localjpegurl"     , "http://82.188.208.242/axis-cgi/mjpg/video.cgi?camera=&resolution=352x288"  );
		//        SetKey("remotemjpegurl"   , "http://82.188.208.242/axis-cgi/mjpg/video.cgi?camera=&resolution=352x288"   );
		SetKey("localjpegurl", url);
		SetKey("remotemjpegurl", url);
		updateRoot(dID, dName, DevCamera);
	}
}

void ImperiHome::setRoomId(std::string& DeviceRowID, std::string RoomId)
{
	m_Map_Room_DeviceId[DeviceRowID] = "roomID" + RoomId;
}

//get RoomId from DeviceRowID
std::string  ImperiHome::getRoomId(std::string& DeviceRowID)
{
	std::string RoomId = m_Map_Room_DeviceId[DeviceRowID];
	//	//if does not exist , take default RoomID1
	//	if (RoomId == "") {
	//		setRoomId(DeviceRowID, "0");
	//		RoomId = m_Map_Room_DeviceId[DeviceRowID];
	//	}
	return RoomId;
}

void ImperiHome::clearRoomIds()
{
	m_Map_Room_DeviceId.clear();
}

//build the m_Map_Room_DeviceId table in order to retrieve roomIDxx from DeviceRowId
//without database access
void ImperiHome::build_Map_Room_DeviceId()
{
	auto result = m_sql.safe_query("SELECT ID, Name FROM Plans ");
	clearRoomIds();
	//get device ID  with RoomId
	result = m_sql.safe_query("SELECT PlanID,DeviceRowID FROM DeviceToPlansMap WHERE (DevSceneType==0)");
	for (unsigned int ii = 0; ii < result.size(); ii++)
	{
		TSqlRowQuery* row = &result[ii];
		setRoomId((*row)[1], (*row)[0]);
	}
}

bool ImperiHome::is_Map_Room_DeviceId_built()
{
	return m_Map_Room_DeviceId.size();
}

void ImperiHome::getRoomContent(std::string& rep_content)
{
	char line[1024];
	//retrieve maps for rooms
	auto result = m_sql.safe_query("SELECT ID, Name FROM Plans ");

	rep_content = "";
	rep_content += "{                                    ";
	rep_content += "  \"rooms\": [                       ";

	//default room if not defined
	rep_content += "    {                                ";
	rep_content += "      \"id\": \"roomID0\",           ";
	rep_content += "      \"name\": \"all\"      ";
	rep_content += "    }                                ";

	for (unsigned int ii = 0; ii < result.size(); ii++)
	{
		TSqlRowQuery* row = &result[ii];
		rep_content += ",";
		rep_content += "    {                                ";
		sprintf(line, "      \"id\": \"roomID%s\",", (*row)[0].c_str());  rep_content += line;
		sprintf(line, "      \"name\": \"%s\"     ", (*row)[1].c_str());  rep_content += line;
		rep_content += "    }                                ";
	}
	rep_content += "  ]                                  ";
	rep_content += "}                                    ";

	//build the m_Map_Room_DeviceId table in order to retrieve roomIDxx from DeviceRowId
	//without database access
	build_Map_Room_DeviceId();
}

/* for test
void DeviceContent1(std::string& rep_content)
{
	rep_content = "";
	rep_content += "{                                   ";
	rep_content += "  \"devices\": [                    ";
	rep_content += "    {                               ";
	rep_content += "      \"id\": \"dev01\",            ";
	rep_content += "      \"name\": \"My Lamp\",        ";
	rep_content += "      \"type\": \"DevDimmer\",      ";
	rep_content += "      \"room\": \"roomID1\",        ";
	rep_content += "      \"params\": [                 ";
	rep_content += "        {                           ";
	rep_content += "          \"key\": \"Level\",       ";
	rep_content += "          \"value\": \"75\"         ";
	rep_content += "        },                          ";
	rep_content += "        {                           ";
	rep_content += "          \"key\": \"Status\",      ";
	rep_content += "          \"value\": \"1\"          ";
	rep_content += "        }                           ";
	rep_content += "      ]                             ";
	rep_content += "    },                              ";
	rep_content += "    {                               ";
	rep_content += "      \"id\": \"dev02\",            ";
	rep_content += "      \"name\": \"My Lamp2\",        ";
	rep_content += "      \"type\": \"DevDimmer\",      ";
	rep_content += "      \"room\": \"roomID3\",        ";
	rep_content += "      \"params\": [                 ";
	rep_content += "        {                           ";
	rep_content += "          \"key\": \"Level\",       ";
	rep_content += "          \"value\": \"75\"         ";
	rep_content += "        },                          ";
	rep_content += "        {                           ";
	rep_content += "          \"key\": \"Status\",      ";
	rep_content += "          \"value\": \"1\"          ";
	rep_content += "        }                           ";
	rep_content += "      ]                             ";
	rep_content += "    },                              ";
	rep_content += "    {                                                                                         ";
	rep_content += "      \"id\": \"dev03\",                                                                      ";
	rep_content += "      \"name\": \"Temperature Sensor\",                                                       ";
	rep_content += "      \"type\": \"DevTemperature\",                                                           ";
	rep_content += "      \"room\": \"roomID1\",                                                                  ";
	rep_content += "      \"params\": [                                                                           ";
	rep_content += "        {                                                                                     ";
	rep_content += "          \"key\": \"Value\",                                                                 ";
	rep_content += "          \"value\": \"21.5\",                                                                ";
	rep_content += "          \"unit\": \"째C\",                                                                   ";
	rep_content += "          \"graphable\": true                                                                 ";
	rep_content += "        },                                                                                     ";
	rep_content += "      ]                                                                                       ";
	rep_content += "    },                                                                                        ";

	rep_content += "    {                                       ";
	rep_content += "       \"id\": \"dev28\",                   ";
	rep_content += "       \"name\": \"Home Thermostat\",       ";
	rep_content += "       \"type\": \"DevThermostat\",         ";
	rep_content += "       \"room\": \"roomID1\",               ";
	rep_content += "       \"params\": [                        ";
	rep_content += "         {                                  ";
	rep_content += "           \"key\": \"curmode\",            ";
	rep_content += "           \"value\": \"Conf\"           ";
	rep_content += "         },                                 ";
	rep_content += "         {                                  ";
	rep_content += "           \"key\": \"curtemp\",            ";
	rep_content += "           \"value\": \"19.2\",             ";
	rep_content += "           \"unit\": \"째C\"            ";
	rep_content += "         },                                 ";
	rep_content += "         {                                  ";
	rep_content += "           \"key\": \"cursetpoint\",        ";
	rep_content += "           \"value\": \"20\"              ";
	rep_content += "         },                                 ";
	rep_content += "         {                                  ";
	rep_content += "           \"key\": \"step\",               ";
	rep_content += "           \"value\": \"0.5\"               ";
	rep_content += "         },                                 ";
	rep_content += "         {                                  ";
	rep_content += "           \"key\": \"availablemodes\",     ";
	rep_content += "           \"value\": \"Conf, Eco, Off\" ";
	rep_content += "         }                                  ";
	rep_content += "      ]                                     ";
	rep_content += "    },                                       ";

	rep_content += "    {                                                                                         ";
	rep_content += "      \"id\": \"dev13\",                                                                      ";
	rep_content += "      \"name\": \"Bogus sensor\",                                                             ";
	rep_content += "      \"type\": \"DevGenericSensor\",                                                         ";
	rep_content += "      \"room\": \"roomID1\",                                                                  ";
	rep_content += "      \"params\": [                                                                           ";
	rep_content += "        {                                                                                     ";
	rep_content += "          \"key\": \"Value\",                                                                 ";
	rep_content += "          \"value\": \"Blahblah\"                                                             ";
	rep_content += "        },                                                                                    ";
	rep_content += "        {                                                                                     ";
	rep_content += "          \"key\": \"Watt\",                                                                 ";
	rep_content += "          \"value\": \"1000\"                                                             ";
	rep_content += "        }                                                                                     ";
	rep_content += "      ]                                                                                       ";
	rep_content += "    },                                                                                        ";


	rep_content += "    {                               ";
	rep_content += "      \"id\": \"dev27\",            ";
	rep_content += "      \"name\": \"Light and Conso\",";
	rep_content += "      \"type\": \"DevDimmer\",      ";
	rep_content += "      \"room\": \"roomID2\",        ";
	rep_content += "      \"params\": [                 ";
	rep_content += "        {                           ";
	rep_content += "          \"key\": \"Status\",      ";
	rep_content += "          \"value\": \"1\"          ";
	rep_content += "        },                          ";
	rep_content += "        {                           ";
	rep_content += "          \"key\": \"Level\",       ";
	rep_content += "          \"value\": \"80\"         ";
	rep_content += "        },                          ";
	rep_content += "        {                           ";
	rep_content += "          \"key\": \"Energy\",      ";
	rep_content += "          \"value\": \"349\",       ";
	rep_content += "          \"unit\": \"W\"           ";
	rep_content += "        }                           ";
	rep_content += "      ]                             ";
	rep_content += "    }                               ";
	rep_content += "  ]                                 ";
	rep_content += "}                                   ";
}

void DeviceContent2(std::string& rep_content)
{
	rep_content = "";
	rep_content += "{                                                                                             \n";
	rep_content += "  \"devices\": [                                                                              \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev01\",                                                                      \n";
	rep_content += "      \"name\": \"My Lamp\",                                                                  \n";
	rep_content += "      \"type\": \"DevDimmer\",                                                                \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Level\",                                                                 \n";
	rep_content += "          \"value\": \"75\"                                                                   \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Status\",                                                                \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                        \n ";
	rep_content += "      \"id\": \"dev02\",                                                                      \n";
	rep_content += "      \"name\": \"Kitchen Light\",                                                            \n";
	rep_content += "      \"type\": \"DevSwitch\",                                                                \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Status\",                                                                \n";
	rep_content += "          \"value\": \"0\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev03\",                                                                      \n";
	rep_content += "      \"name\": \"Temperature Sensor\",                                                       \n";
	rep_content += "      \"type\": \"DevTemperature\",                                                           \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"21.5\",                                                                \n";
	rep_content += "          \"unit\": \"째C\",                                                                  \n ";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev04\",                                                                      \n";
	rep_content += "      \"name\": \"My Camera\",                                                                \n";
	rep_content += "      \"type\": \"DevCamera\",                                                                \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"localjpegurl\",                                                          \n";
	rep_content += "          \"value\": \"http://83.61.22.4:8080/axis-cgi/jpg/image.cgi\"                        \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"remotemjpegurl\",                                                        \n";
	rep_content += "          \"value\": \"http://83.61.22.4:8080/axis-cgi/mjpg/video.cgi?resolution=320x240\"    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev05\",                                                                      \n";
	rep_content += "      \"name\": \"Diox sensor\",                                                              \n";
	rep_content += "      \"type\": \"DevCO2\",                                                                   \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"528\",                                                                 \n";
	rep_content += "          \"unit\": \"ppm\",                                                                  \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev07\",                                                                      \n";
	rep_content += "      \"name\": \"Window shutter 1\",                                                         \n";
	rep_content += "      \"type\": \"DevShutter\",                                                               \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Level\",                                                                 \n";
	rep_content += "          \"value\": \"75\"                                                                   \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev08\",                                                                      \n";
	rep_content += "      \"name\": \"Door sensor\",                                                              \n";
	rep_content += "      \"type\": \"DevDoor\",                                                                  \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"armable\",                                                               \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Armed\",                                                                 \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Tripped\",                                                               \n";
	rep_content += "          \"value\": \"0\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev09\",                                                                      \n";
	rep_content += "      \"name\": \"My Flood sensor\",                                                          \n";
	rep_content += "      \"type\": \"DevFlood\",                                                                 \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"ackable\",                                                               \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Armed\",                                                                 \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Tripped\",                                                               \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev10\",                                                                      \n";
	rep_content += "      \"name\": \"Motion sensor\",                                                            \n";
	rep_content += "      \"type\": \"DevMotion\",                                                                \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Armed\",                                                                 \n";
	rep_content += "          \"value\": \"0\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Tripped\",                                                               \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev11\",                                                                      \n";
	rep_content += "      \"name\": \"Smoke sensor\",                                                             \n";
	rep_content += "      \"type\": \"DevSmoke\",                                                                 \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Armed\",                                                                 \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Tripped\",                                                               \n";
	rep_content += "          \"value\": \"0\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev12\",                                                                      \n";
	rep_content += "      \"name\": \"Electricity conso\",                                                        \n";
	rep_content += "      \"type\": \"DevElectricity\",                                                           \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Watts\",                                                                 \n";
	rep_content += "          \"value\": \"620\",                                                                 \n";
	rep_content += "          \"unit\": \"W\",                                                                    \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"ConsoTotal\",                                                            \n";
	rep_content += "          \"value\": \"983\",                                                                 \n";
	rep_content += "          \"unit\": \"kWh\",                                                                  \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev13\",                                                                      \n";
	rep_content += "      \"name\": \"Bogus sensor\",                                                             \n";
	rep_content += "      \"type\": \"DevGenericSensor\",                                                         \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"Blahblah\"                                                             \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev14\",                                                                      \n";
	rep_content += "      \"name\": \"Hum sensor\",                                                               \n";
	rep_content += "      \"type\": \"DevHygrometry\",                                                            \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"67\",                                                                  \n";
	rep_content += "          \"unit\": \"%\",                                                                    \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev15\",                                                                      \n";
	rep_content += "      \"name\": \"Lum sensor\",                                                               \n";
	rep_content += "      \"type\": \"DevLuminosity\",                                                            \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"432\",                                                                 \n";
	rep_content += "          \"unit\": \"lux\",                                                                  \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev16\",                                                                      \n";
	rep_content += "      \"name\": \"Entrance door Lock\",                                                       \n";
	rep_content += "      \"type\": \"DevLock\",                                                                  \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Status\",                                                                \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev17\",                                                                      \n";
	rep_content += "      \"name\": \"Heater\",                                                                   \n";
	rep_content += "      \"type\": \"DevMultiSwitch\",                                                           \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"Eco\"                                                                  \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Choices\",                                                               \n";
	rep_content += "          \"value\": \"Eco,Comfort,Freeze,Stop\"                                              \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev18\",                                                                      \n";
	rep_content += "      \"name\": \"Noise sensor\",                                                             \n";
	rep_content += "      \"type\": \"DevNoise\",                                                                 \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"33\",                                                                  \n";
	rep_content += "          \"unit\": \"db\",                                                                   \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev19\",                                                                      \n";
	rep_content += "      \"name\": \"Outdoor pressure\",                                                         \n";
	rep_content += "      \"type\": \"DevPressure\",                                                              \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"1432\",                                                                \n";
	rep_content += "          \"unit\": \"mbar\",                                                                 \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev20\",                                                                      \n";
	rep_content += "      \"name\": \"Rain sensor\",                                                              \n";
	rep_content += "      \"type\": \"DevRain\",                                                                  \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"5\",                                                                   \n";
	rep_content += "          \"unit\": \"mm/h\",                                                                 \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Accumulation\",                                                          \n";
	rep_content += "          \"value\": \"182\",                                                                 \n";
	rep_content += "          \"unit\": \"mm\",                                                                   \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev21\",                                                                      \n";
	rep_content += "      \"name\": \"Night mode\",                                                               \n";
	rep_content += "      \"type\": \"DevScene\",                                                                 \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"LastRun\",                                                               \n";
	rep_content += "          \"value\": \"2014-03-12 23:15:65\"                                                  \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev22\",                                                                      \n";
	rep_content += "      \"name\": \"Away mode\",                                                                \n";
	rep_content += "      \"type\": \"DevScene\",                                                                 \n";
	rep_content += "      \"room\": \"roomID1\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"LastRun\",                                                               \n";
	rep_content += "          \"value\": \"2014-03-16 23:15:65\"                                                  \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev23\",                                                                      \n";
	rep_content += "      \"name\": \"Sun UV sensor\",                                                            \n";
	rep_content += "      \"type\": \"DevUV\",                                                                    \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Value\",                                                                 \n";
	rep_content += "          \"value\": \"5\",                                                                   \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev24\",                                                                      \n";
	rep_content += "      \"name\": \"Wind sensor\",                                                              \n";
	rep_content += "      \"type\": \"DevWind\",                                                                  \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Speed\",                                                                 \n";
	rep_content += "          \"value\": \"12\",                                                                  \n";
	rep_content += "          \"unit\": \"km/h\",                                                                 \n";
	rep_content += "          \"graphable\": true                                                                 \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Direction\",                                                             \n";
	rep_content += "          \"value\": \"182\",                                                                 \n";
	rep_content += "          \"unit\": \"�\"                                                                     \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev25\",                                                                      \n";
	rep_content += "      \"name\": \"CO2 Alert sensor\",                                                         \n";
	rep_content += "      \"type\": \"DevCO2Alert\",                                                              \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"armable\",                                                               \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"ackable\",                                                               \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Armed\",                                                                 \n";
	rep_content += "          \"value\": \"0\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Tripped\",                                                               \n";
	rep_content += "          \"value\": \"0\"                                                                    \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev26\",                                                                      \n";
	rep_content += "      \"name\": \"Outlet and Conso\",                                                         \n";
	rep_content += "      \"type\": \"DevSwitch\",                                                                \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Status\",                                                                \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Energy\",                                                                \n";
	rep_content += "          \"value\": \"349\",                                                                 \n";
	rep_content += "          \"unit\": \"W\"                                                                     \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";
	rep_content += "    {                                                                                         \n";
	rep_content += "      \"id\": \"dev27\",                                                                      \n";
	rep_content += "      \"name\": \"Light and Conso\",                                                          \n";
	rep_content += "      \"type\": \"DevDimmer\",                                                                \n";
	rep_content += "      \"room\": \"roomID2\",                                                                  \n";
	rep_content += "      \"params\": [                                                                           \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Status\",                                                                \n";
	rep_content += "          \"value\": \"1\"                                                                    \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Level\",                                                                 \n";
	rep_content += "          \"value\": \"80\"                                                                   \n";
	rep_content += "        },                                                                                    \n";
	rep_content += "        {                                                                                     \n";
	rep_content += "          \"key\": \"Energy\",                                                                \n";
	rep_content += "          \"value\": \"349\",                                                                 \n";
	rep_content += "          \"unit\": \"W\"                                                                     \n";
	rep_content += "        }                                                                                     \n";
	rep_content += "      ]                                                                                       \n";
	rep_content += "    },                                                                                        \n";

	rep_content += "    {                                       \n";
	rep_content += "       \"id\": \"dev28\",                   \n";
	rep_content += "       \"name\": \"Home Thermostat\",       \n";
	rep_content += "       \"type\": \"DevThermostat\",         \n";
	rep_content += "       \"room\": \"roomID1\",               \n";
	rep_content += "       \"params\": [                        \n";
	rep_content += "         {                                  \n";
	rep_content += "           \"key\": \"curmode\",            \n";
	rep_content += "           \"value\": \"Comfort\"           \n";
	rep_content += "         },                                 \n";
	rep_content += "         {                                  \n";
	rep_content += "           \"key\": \"curtemp\",            \n";
	rep_content += "           \"value\": \"19.2\",             \n";
	rep_content += "           \"unit\": \"째C\"                \n";
	rep_content += "         },                                 \n";
	rep_content += "         {                                  \n";
	rep_content += "           \"key\": \"cursetpoint\",        \n";
	rep_content += "           \"value\": \"20.3\"              \n";
	rep_content += "         },                                 \n";
	rep_content += "         {                                  \n";
	rep_content += "           \"key\": \"step\",               \n";
	rep_content += "           \"value\": \"0.5\"               \n";
	rep_content += "         },                                 \n";
	rep_content += "         {                                  \n";
	rep_content += "           \"key\": \"availablemodes\",     \n";
	rep_content += "           \"value\": \"Comfort, Eco, Off\" \n";
	rep_content += "         }                                  \n";
	rep_content += "      ]                                     \n";
	rep_content += "    }                                       \n";

	rep_content += "  ]                                 ";
	rep_content += "}                                           ";
}

*/
/*
void Histo(std::string& rep_content, std::string& from)
{
	int delta = 20;
	long fr = atol(from.c_str());
	fr -= 10 * 60 * delta; //10 min
	rep_content = "";
	rep_content += "{                              ";
	rep_content += "  \"values\": [                ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 18.2         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 21.3         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 17.6         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 23.1         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 24.9         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 18.5         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 20.0         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + "000,"; fr += 60 * delta;
	rep_content += "      \"value\" : 20.1         ";
	rep_content += "    },                         ";
	rep_content += "    {                          ";
	rep_content += "      \"date\" : " + boost::to_string(fr) + ","; fr += 60 * delta;
	rep_content += "      \"value\" : 19.6         ";
	rep_content += "    }                          ";
	rep_content += "  ]                            ";
	rep_content += "}																";
}
*/
std::string getIpAdress()
{
	boost::asio::io_service io_service;

	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
	boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
	boost::asio::ip::tcp::endpoint endpoint = *it;
	//return endpoint.address().to_string();
	return boost::asio::ip::host_name();
}

//implemtation of ImperiHome Request 
//return false if not a ImperiHome Request
bool  ImperiHome::Request(std::string& request_path, std::string& rep_content)
{
	if (request_path == "/system")
	{
		_log.Debug(DEBUG_NORM, "IMPE: System request");
		rep_content = " { \"id\": \"Domoticz" + getIpAdress() + "\",  \"apiversion\": 1 }";
	}
	else  if (request_path.find("/devices") == 0)
	{
		std::vector<std::string> results;
		StringSplit(request_path.substr(1), "/", results);
		if (results.size() == 1)
		{
			_log.Debug(DEBUG_NORM, "IMPE: Devices request");
			DeviceContent(rep_content);
		}
		else 	if (results.size() == 5)
		{
			ManageAction(results[1], results[2], results[3], results[4]);
		}
		else 	if (results.size() == 4)
		{
			ManageAction(results[1], results[2], results[3], "");
		}
		else 	if (results.size() == 6)
		{
			ManageHisto(results[1], results[2], results[3], results[4], results[5], rep_content);
		}
	}
	else  if (request_path == "/rooms") {
		_log.Debug(DEBUG_NORM, "IMPE: Rooms request");
		getRoomContent(rep_content);
	}
	else
		return false;
	return true;
}

bool  ImperiHomeRequest(std::string request_path, std::string& rep_content)
{
	ImperiHome m_ImperiHome;
	bool ret = m_ImperiHome.Request(request_path, rep_content);
	if (ret)
		if (!_log.CheckIfMessageIsFiltered("IMPA"))
		//    if(_log.IsDebugLevelEnabled("IMPA"))
		if (rep_content.length())
		{
			if (rep_content.length() < 2048)
				_log.Debug(DEBUG_NORM, "IMPA: IIS Answer %s", rep_content.c_str());
			else
			{
				_log.Debug(DEBUG_NORM, "IMPA: IIS Answer :");
				size_t  len = 0;
				while (len <= rep_content.length())
				{
					std::string line = rep_content.substr(len, MAX_LOG_SIZE);
					_log.Debug(DEBUG_NORM, line);
					len += MAX_LOG_SIZE;
				}
			}
		}
	return ret;
}
//convert date string 10/12/2014 10:45:58 en  struct tm
void DateAsciiTotmTime(std::string& sTime, struct tm& tmTime)
{
	tmTime.tm_isdst = 0; //dayly saving time
	tmTime.tm_year = std::stoi(sTime.substr(0, 4).c_str()) - 1900;
	tmTime.tm_mon = std::stoi(sTime.substr(5, 2).c_str()) - 1;
	tmTime.tm_mday = std::stoi(sTime.substr(8, 2).c_str());
	tmTime.tm_hour = std::stoi(sTime.substr(11, 2).c_str());
	tmTime.tm_min = std::stoi(sTime.substr(14, 2).c_str());
	tmTime.tm_sec = std::stoi(sTime.substr(17, 2).c_str());
}
//convert struct tm time to char
void AsciiTime(struct tm& ltime, char* pTime)
{
	sprintf(pTime, "%04d-%02d-%02d %02d:%02d:%02d", ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday, ltime.tm_hour, ltime.tm_min, ltime.tm_sec);
}

void AsciiTime(time_t DateStart, char* DateStr)
{
	struct tm ltime;
	localtime_r(&DateStart, &ltime);
	AsciiTime(ltime, DateStr);
}

time_t DateAsciiToTime_t(std::string& DateStr)
{
	struct tm tmTime;
	DateAsciiTotmTime(DateStr, tmTime);
	return mktime(&tmTime);
}

void ImperiHome::getGraphic(std::string& idx, std::string TableName, std::string FieldName, std::string KeyName, time_t DateStart, time_t  DateEnd, std::string& rep_content, double CoefA, double CoefB)
{
	char DateStartStr[40];
	char DateEndStr[40];
	int dType = 0;
	int dSubType = 0;

	AsciiTime(DateStart, DateStartStr);
	AsciiTime(DateEnd, DateEndStr);

	rep_content = "";
	rep_content += "{";
	rep_content += "  \"values\": [";

	SqlGetTypeSubType(idx, dType, dSubType);

	auto result = m_sql.safe_query("SELECT %s , Date FROM %s WHERE (DeviceRowID==%s AND Date>='%s' AND Date<='%s' ) ORDER BY Date ASC", FieldName.c_str(), TableName.c_str(), idx.c_str(), DateStartStr, DateEndStr);
	for (unsigned int i = 0; i < result.size(); i++)
	{
		TSqlRowQuery* sd = &result[i];
		time_t timeSec = DateAsciiToTime_t((*sd)[1]);
		double tvalue = std::stof((*sd)[0].c_str());
		if ((dType == pTypeGeneral) && (dSubType == sTypeKwh))
			tvalue /= 10.0f;

		if (KeyName == "temp")
			tvalue = ConvertTemperature(tvalue, m_sql.m_tempsign[0]);

		tvalue = tvalue * CoefA + CoefB ;
		char tv[14];
		sprintf(tv, "%3.1f", tvalue);
		if (i > 0)
			rep_content += ",";
		rep_content += "{";
		rep_content += "\"date\" : " + boost::to_string((int)timeSec) + "000,";
		rep_content += "\"value\": " + std::string(tv);
		rep_content += "}";
	}
	rep_content += "  ]                            ";
	rep_content += "}																";
	_log.Debug(DEBUG_NORM, "IMPE: Graphic Id:%s from:%lu=%s to:%lu=%s Points:%d", idx.c_str(), (long)DateStart, DateStartStr, (long)DateEnd, DateEndStr, result.size());
}

void http::server::CWebServer::ImperihomeServices(WebEmSession& session, const request& req, reply& rep)
{
	ImperiHomeRequest((std::string)req.uri, rep.content);
	rep.status = reply::ok;
	//extension = "html";
}
