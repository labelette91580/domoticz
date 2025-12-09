--[[
	Assume you have two temperature sensors and a third dummy sensor that should be the
	difference of these two sensors (e.g. you want to see the difference between water temperature
	going into a radiator and the temperature of the water going out of it
]]--

local	FAN_NAME      = 'VMCCave'    -- exact device name of the switch turning on/off the ventilator
local	SENSOR_NAME   = 'Cave'     -- exact device name of the humidity sensor
local	EXTERIOR_NAME = 'Exterieur2'     -- exact device name of the extorior humidity sensor
local	SeuilHumidite = 'SeuilHumidite'

local   TEST_MODE = false                   -- when true TEST_MODE_HUMVAR is used instead of the real sensor
local   TEST_MODE_HUMVAR = 'testHumidity'   -- fake humidity value, give it a test value in domoticz/uservars
local   PRINT_MODE = true					        -- when true wil print output to log and send notifications
local   SAMPLE_INTERVAL = 10                -- 10 min
local   TIME_ON = 120                       --temps maximim fan   on 
local   TIME_OFF = 30                       --temps maximim fan  off

return {
	active = true,
	on = {
		['timer'] = {'every 10 minutes'}
	},
	data = {
            FanTimeOn  = { initial = 0 },
            lastState  = { initial = 0 },
			
    },	
	execute = function(domoticz)
		domoticz.data.FanTimeOn = domoticz.data.FanTimeOn + SAMPLE_INTERVAL
-- get the humidity thresHold:
		local humThresHold = domoticz.devices(SeuilHumidite).lastLevel
--if deactivated
		if (humThresHold == 0) then
			return 
		end
		
		local currentTemp  = domoticz.devices(SENSOR_NAME).temperature
		local extTemp      = domoticz.devices(EXTERIOR_NAME).temperature
		local currentHum   = domoticz.devices(SENSOR_NAME).humidity
		local extHum       = domoticz.devices(EXTERIOR_NAME).humidity
		local vmc          = domoticz.devices(FAN_NAME)

		local delta        = (currentHum - extHum)
		local deltaTemp    = ( extTemp-currentTemp )
		local  newCmd      = 0

		-- update the dummy sensor
--		domoticz.devices('Delta temp').updateTemperature(deltaTemp)
--		domoticz.log('CAVE: currentTemp: ' .. currentTemp  .. ' extTemp: ' .. extTemp  .. ' currentHum: ' .. currentHum  .. ' extHum: ' .. extHum  .. ' humThresHold: ' .. humThresHold .. ' lastState:' .. lastState , domoticz.LOG_INFO )
		
--si humidity greater than  humThresHold and exterior humidity is less and exterior temperature is less than interior temp + 5 
		if (currentHum >= humThresHold) and (delta > 1 ) and (deltaTemp < 5 ) then
			  if (domoticz.data.FanTimeOn >= TIME_OFF ) then
				newCmd  = 1 
			else
			  newCmd = domoticz.data.lastState
			end
		else
			newCmd  = 0
		end
		if ( delta > 10 ) then 
			TIME_ON = TIME_ON * 2 
		end

		if ( domoticz.data.lastState == 1 ) and (domoticz.data.FanTimeOn >= TIME_ON ) then
			newCmd  = 0 
		end
--si commande change
		if (newCmd ~= domoticz.data.lastState) then 	
			domoticz.data.FanTimeOn = 0
			if (newCmd == 0 ) then
				vmc.switchOff()
			else
				vmc.switchOn()
			end
		end
		if PRINT_MODE == true then
			print('CAVE:'..currentTemp..'C/'..currentHum..'%'..' Ext: '..extTemp..'C/'..extHum..'%'..' ThresHold:' .. humThresHold .. '% FanTimeOn:' .. domoticz.data.FanTimeOn .. ' Last Vmc State:' .. domoticz.data.lastState ..' New  Vmc State:' .. newCmd .. ' delta:' .. delta .. ' deltaTemp:' .. deltaTemp )
		end
		domoticz.data.lastState = newCmd ;
		
	end
}
