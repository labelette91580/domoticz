return {
	active = true, -- optional
    on = {
        timer = {'every 1 minutes'},
        httpResponses = { 'cb_domoticz' }
    },

    execute = function(domoticz, item)

        local function getPower(domoticz, item)
			if item.isTimer then
				local idx = 11 -- IDX du ThSalle

	--                'http://127.0.0.1:8080/json.htm?type=devices&rid=' .. idx
				local url = 'http://192.168.1.6:8080/json.htm?type=command&param=getdevices&filter=utility&used=true&rid=' .. idx 
				domoticz.openURL({url = url,method = 'GET',callback = 'cb_domoticz'  })
				return
			end
			if item.isHTTPResponse and item.callback == 'cb_domoticz' then
				if not item.ok then
					domoticz.log('Erreur HTTP Domoticz ' .. item.statusCode,domoticz.LOG_ERROR                )
					return
				end
				local json = domoticz.utils.fromJSON(item.data)
				if not json or not json.result or not json.result[1] then
					domoticz.log('JSON Domoticz invalide', domoticz.LOG_ERROR)
					return
				end
	--			domoticz.log('JSON brut: ' .. item.data, domoticz.LOG_INFO)
				local dev = json.result[1]
				local temp = tonumber(dev.SwitchIdx1)
				domoticz.log('Power = ' .. dev.Power)
			end
		end
		getPower(domoticz, item)
    end
}
