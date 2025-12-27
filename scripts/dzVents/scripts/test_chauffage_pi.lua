-- test_chauffage_pi.lua
-- Simulation chauffage PI sans Domoticz

local pi = require("pi_utils")

--------------------------------------------------
-- Etat simulé (équivalent domoticz.data)
--------------------------------------------------
---
---
function Snap(pvalue,ptime)
    return {
        val = pvalue,
        time = ptime
    }
end

local state = {
    lastTemp     = nil,
    lastTime     = nil,
    lastSetPoint = nil,
    lastPower    = nil,

    DTemp = nil,
    DTime = nil,
    DSetPoint = nil,
    DPower = nil,

    TimePower100=0,
    TimePower0=0,
    TimeLastSetPoint=0,
    TimeLastDTemp=0,

    MaxTempPower0=Snap(0,0),

    integral = 0,
    tau = nil,
    inertia = "MEDIUM",
    Kp = 50,
    Ki = 5
}

local SETPOINT = 20.0

--------------------------------------------------
-- Lecture CSV
--------------------------------------------------
local function readCSV(filename)
    local data = {}
    local f = io.open(filename, "r")
    if not f then error("Impossible d'ouvrir le fichier CSV") end

    f:read("*l") -- skip header
    local ti=0
    for line in f:lines() do
        local t, Tint, Text,SetPoint,Power = line:match("([^;]+);([^;]+);([^;]+);([^;]+);([^;]+)")
        if t and Tint and Text then
            table.insert(data, {
                time = (ti),
                Tint = tonumber(Tint),
                Text = tonumber(Text),
                SetPoint = tonumber(SetPoint),
                Power = tonumber(Power),
                
            })
            ti=ti+60
        end
    end
    f:close()
    return data
end
function sec_to_hms(total_seconds)
    local h = math.floor(total_seconds / 3600)
    local m = math.floor((total_seconds % 3600) / 60)
    local s = total_seconds % 60
    return h, m, s
end
function sec_to_hms_str(total_seconds)
    local h = math.floor(total_seconds / 3600)
    local m = math.floor((total_seconds % 3600) / 60)
    local s = total_seconds % 60
    return string.format("%02d:%02d:%02d", h, m, s)
end
--------------------------------------------------
-- Simulation
--------------------------------------------------
local data = readCSV("temp.csv")

print("time;Tint;Text;power;stop;tau;slope")
local lastTau = nil
for _, row in ipairs(data) do

if (state.lastTemp == nil )then
    state.lastTemp      = row.Tint
    state.lastTime      = row.time
    state.lastSetPoint  =  row.SetPoint
    state.lastPower     = row.Power
end
    local result = pi.runHeatingStep(state, row.Tint, row.Text, row.SetPoint, row.Power,row.time)

    if lastTau ~= result.tau then 
        print(string.format(
            "%s;%.1f;%.1f;%.1f;%3.0f;%s;%.0f;%.3f",
            sec_to_hms_str(row.time),
            row.Tint,
            row.Text,
            row.SetPoint,
            row.Power,
            tostring(result.stop),
            result.tau or 0,
            result.slope
        ))
    end
    lastTau = result.tau  

end
