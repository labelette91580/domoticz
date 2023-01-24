    public :  
        uint32_t sendDataVld(unsigned int unitBaseAddr, unsigned int destID, enocean::T_DATAFIELD *OffsetDes, int *values, int NbValues);
	int getPositionFromCommandLevel(int cmnd, int pos);
	//new

    void parse_PACKET_MAN_SPECIFIC_COMMAND(unsigned char m_buffer[], int m_DataSize, int m_OptionalDataSize);

    bool updateSwitchType(int HardwareID, const char *deviceID, _eSwitchType SwitchType);

public : 
    void DeleteSensor(uint32_t sensorId);

	void setRepeaterLevelOn();

	void getRepeaterLevel();

	void parsePACKET_RESPONSE(uint8_t *data, uint16_t datalen);

    void DelNodeInfo(const uint32_t nodeID);

    void SetNodeTeachInStatus(const uint32_t nodeID, uint32_t TeachInStatus);

    uint32_t GetNodeTeachInStatus(const uint32_t nodeID);

    bool NodeIsAlreadyTeachedIn(const uint32_t nodeID);

    void GetDbValue(const char* tableName, const char* fieldName, const char* whereFieldName, const char* whereFielValue, uint32_t& Value);

    void SetDbValue(const char* tableName, const char* fieldName, const char* fieldValue, const char* whereFieldName, const char* whereFieldValue);

    void SetDbValue(const char* tableName, const char* fieldName, uint32_t fieldValue, const char* whereFieldName, const char* whereFielValue);

    std::string GetDbEnOceanValue(uint32_t DeviceId, const char* fieldName);

    void GetDbEnOceanValue(uint32_t DeviceId, const char* fieldName, uint32_t& Value);

    void SetDbEnOceanValue(uint32_t DeviceId, const char* fieldName, uint32_t Value);

    void SetDbTeachInStatus(uint32_t DeviceId, uint32_t TeachInStatus);

    void SetTeachInStatus(uint32_t DeviceId, uint32_t TeachInStatus);

    void TeachInNodeIfExist(const uint32_t nodeID, const uint16_t manID, const uint8_t RORG, const uint8_t func, const uint8_t type, const TeachinMode teachin_mode);


const char* getPACKET_TYPE_name(int ptype);

uint32_t enoceanGetSensorIdFromDevice(std::string& DeviceId, uint32_t unit, uint8_t devType, uint8_t subType);

void EnOceanDeleteSensorAssociatedWithDevice(const std::string& ID);