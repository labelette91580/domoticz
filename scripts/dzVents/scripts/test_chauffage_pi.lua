-- test_chauffage_pi.lua
-- Simulation chauffage PI sans Domoticz

local pi = require("pi_utils")

--------------------------------------------------
-- Etat simulé (équivalent domoticz.data)
--------------------------------------------------
local state = {
    lastTemp = nil,
    lastTime = nil,
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
    for line in f:lines() do
        local t, Tint, Text = line:match("([^;]+);([^;]+);([^;]+)")
        if t and Tint and Text then
            table.insert(data, {
                time = tonumber(t),
                Tint = tonumber(Tint),
                Text = tonumber(Text)
            })
        end
    end
    f:close()
    return data
end

--------------------------------------------------
-- Simulation
--------------------------------------------------
local data = readCSV("temperatures.csv")

print("time;Tint;Text;power;stop;tau;slope")

for _, row in ipairs(data) do
    local result = pi.runHeatingStep(state, row.Tint, row.Text, SETPOINT, row.time)

    print(string.format(
        "%d;%.2f;%.2f;%.1f;%s;%.0f;%.3f",
        row.time,
        row.Tint,
        row.Text,
        result.power,
        tostring(result.stop),
        result.tau or 0,
        result.slope
    ))
end
