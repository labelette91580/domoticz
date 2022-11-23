#pragma once

#include "DomoticzHardware.h"
#include "hardwaretypes.h"

namespace Json
{
	class Value;
} // namespace Json

class EnphaseAPI : public CDomoticzHardwareBase
{
public:
	EnphaseAPI(int ID, const std::string& IPAddress, unsigned short usIPPort, int PollInterval, const std::string& szUsername, const std::string& szPassword);
	~EnphaseAPI() override = default;
	bool WriteToHardware(const char* pdata, unsigned char length) override;

private:
	bool StartHardware() override;
	bool StopHardware() override;
	void Do_Work();

	bool GetSerialSoftwareVersion();
	bool GetAccessToken();
	bool getProductionDetails(Json::Value& result);

	void parseProduction(const Json::Value& root);
	void parseConsumption(const Json::Value& root);
	void parseNetConsumption(const Json::Value& root);
	void parseStorage(const Json::Value& root);

	int getSunRiseSunSetMinutes(bool bGetSunRise);

private:
	int m_poll_interval = 30;

	std::string m_szSerial;
	std::string m_szSoftwareVersion;
	std::string m_szToken;
	std::string m_szIPAddress;

	std::string m_szUsername;
	std::string m_szPassword;

	P1Power m_p1power;
	P1Power m_c1power;
	P1Power m_c2power;
	std::shared_ptr<std::thread> m_thread;
};
