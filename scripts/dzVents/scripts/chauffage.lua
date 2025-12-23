local Log = require("scripts.log_utils")

return {
    on = {
        timer = { 'every minute' },
        httpResponses = { 'cb_domoticz' }
    },

    data = {
        integral = { initial = 0 },
        T_filtre = { initial = 0 },
        power    = { initial = 0 },
		
        buffer = { initial = {} },
        T_filt = { initial = nil }
		
    },

    execute = function(domoticz,item)

        ----------------------------------------------------------------
        -- FONCTION : enregistrement générique
        ----------------------------------------------------------------
        local function getDevice(domoticz, item)
			if item.isTimer then
				local idx = 11 -- IDX du ThSalle

            	--                'http://127.0.0.1:8080/json.htm?type=devices&rid=' .. idx
				local url = 'http://192.168.1.6:8080/json.htm?type=command&param=getdevices&filter=utility&used=true&rid=' .. idx 
				domoticz.openURL({url = url,method = 'GET',callback = 'cb_domoticz'  })
				return 0
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
                --				domoticz.log('JSON brut: ' .. item.data, domoticz.LOG_INFO)
				local dev = json.result[1]
				return dev
			end
		end
		
		local devThSalle = getDevice(domoticz, item)
        if devThSalle == 0 then return end
		local thSallePower = devThSalle.Power
		if thSallePower == 0 then return else  domoticz.data.power = thSallePower end

        Log.dumpTableDz(domoticz, domoticz.devices('ThSalle'))

        -- PARAMÈTRES
        local Ts = 60
        local cycle = 600
        local Kp = 55
        local Ki = 0.05
        
		local text = domoticz.devices('Exterieur').temperature

		if text < 5 then
			Kp = 60; Ki = 0.06
		elseif text < 12 then
			Kp = 55; Ki = 0.05
		else
			Kp = 40; Ki = 0.03
		end
		
		local temp = domoticz.devices('Salle2').temperature
        local setpoint = domoticz.devices('ThSalle').setPoint

        -- ERREUR
        local error = setpoint - temp

        -- INTÉGRALE
        domoticz.data.integral =
            domoticz.data.integral + error * Ts

        -- PI
        local power = Kp * error + Ki * domoticz.data.integral

        -- SATURATION
        if power > 100 then power = 100 end
        if power < 0 then power = 0 end

        -- ANTI-WINDUP
        if power == 0 or power == 100 then
            domoticz.data.integral = 0
--                domoticz.data.integral - error * Ts
        end

        -- PWM temporel
        local now = os.time()
        local on_time = power / 100 * cycle

        -- if (now % cycle) < on_time then
        --     domoticz.devices('SW1').switchOn()
        -- else
        --     domoticz.devices('SW1').switchOff()
        -- end
		
		local Temp = string.format  ("%.1f ", temp )
		local Text = string.format  ("%.1f ", text )
		local Integ  = string.format("%.1f ", domoticz.data.integral )
		local Power = string.format ("%.0f ", power )

-----------------
        local N = 5
        local alpha = 0.2

        local buf = domoticz.data.buffer
        table.insert(buf, temp)

        if #buf > N then
            table.remove(buf, 1)
        end

        -- copie + tri
        local tmp = {}
        for i,v in ipairs(buf) do tmp[i] = v end
        table.sort(tmp)

        local T_med = tmp[math.ceil(#tmp/2)]

        if domoticz.data.T_filt == nil then
            domoticz.data.T_filt = T_med
        else
            domoticz.data.T_filt = alpha * T_med + (1 - alpha) * domoticz.data.T_filt
        end
		local TempFil = string.format("%.1f ", domoticz.data.T_filt )
-----------------
		if (domoticz.data.T_filtre == 0 ) then
			domoticz.data.T_filtre=temp
		end
		if (temp - domoticz.data.T_filtre) > 0.4 then
		else
			domoticz.data.T_filtre = temp
		end
		local T_filtre = string.format("%.1f ", domoticz.data.T_filtre )
	
    	domoticz.log( 'Kp:' .. Kp .. ' Ki:' .. Ki  .. ' Temp:' .. Temp .. TempFil .. T_filtre .. ' SetPoint:' .. setpoint .. ' Text:' .. Text .. ' Power:' .. Power .. '%' .. ' Int:' .. Integ )
--        local filename = '/home/pascal/temperature_log.csv'
        local filename = 'C:/domoticz/TempSimu/temp.csv'
--        logMessage(filename, msg)		
--		Log.logMessage2(filename, '%s;%s;%s;%.1f', Temp, TempFil,T_filtre , domoticz.data.power )
		Log.log( '%s;%s;%s;%.1f', Temp, TempFil,T_filtre , domoticz.data.power )

    end
}
