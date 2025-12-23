local M = {}
--        local filename = '/home/pascal/temperature_log.csv'
local filename = 'C:/domoticz/scripts/dzVents/scripts/temp.csv'

function M.logMessage2(filename, fmt, ...)
	local msg = string.format(fmt, ...)  -- formate comme en C
	local time = os.time()
	local date = os.date('%Y-%m-%d %H:%M:%S', time)
	
	local file = io.open(filename, 'a')
	if file then
		file:write(string.format("%s;%s\n", date, msg))
		file:close()
	else
		print("Impossible d’ouvrir le fichier")
	end
end		

function M.logMessage(filename, Message)

	local time = os.time()
	local date = os.date('%Y-%m-%d %H:%M:%S', time)

	local file = io.open(filename, 'a')
	if not file then
		domoticz.log('Erreur ouverture fichier' .. filename, domoticz.LOG_ERROR)
		return
	end

	-- Format CSV : timestamp;date;Message
--            file:write(string.format('%d;%s;%s\n', time, date, Message))
	file:write(string.format('%s;%s\n', date, Message))
	file:close()
end
function M.log( fmt, ...)
	local msg = string.format(fmt, ...)  -- formate comme en C
	local time = os.time()
	local date = os.date('%Y-%m-%d %H:%M:%S', time)
	
	local file = io.open(filename, 'a')
	if file then
		file:write(string.format("%s;%s\n", date, msg))
		file:close()
	else
		print("Impossible d’ouvrir le fichier")
	end

end

function M.dumpTableDz(domoticz, t, indent, visited)
    indent  = indent or 0
    visited = visited or {}

    if visited[t] then
        domoticz.log(string.rep("  ", indent) .. "*CYCLE*")
        return
    end
    visited[t] = true

    local prefix = string.rep("  ", indent)

    for k, v in pairs(t) do
        local vtype = type(v)

        -- ⛔ ignore les fonctions
		if vtype ~= "function" and vtype ~= "userdata" and vtype ~= "thread" then
            if vtype == "table" then
                domoticz.log(prefix .. tostring(k) .. " = {")
                M.dumpTableDz(domoticz, v, indent + 1, visited)
                domoticz.log(prefix .. "}")
            else
                domoticz.log(
                    prefix .. tostring(k) .. " = " .. tostring(v)
                )
            end
        end
    end
end


return M