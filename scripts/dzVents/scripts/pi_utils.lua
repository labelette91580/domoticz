local M = {}

function CompteMax(pVal,pTime,Snap)
    if (pVal > Snap.val) then
        Snap.val=pVal
        Snap.time=pTime
    end
end
function M.computeTau(T1, T2, Text, dt)
    local num = T2 - Text
    local den = T1 - Text
    if den <= 0 or num <= 0 then return nil end
    local r = num / den
    if r <= 0 or r >= 1 then return nil end
    return -dt / math.log(r)
end

function M.computePIfromTau(tau_sec, alpha)
    alpha = alpha or 1.0
    local tau_h = tau_sec / 3600
    if tau_h <= 0 then return nil, nil end
    local Kp = 100 / alpha
    local Ki = Kp / tau_h
    return Kp, Ki
end

function M.classifyRadiator(delay_min, slope)
    if slope <= 0 then return "MEDIUM" end
    local idx = delay_min / slope
    if idx < 5 then return "FAST"
    elseif idx < 15 then return "MEDIUM"
    elseif idx < 40 then return "SLOW"
    else return "VERY_SLOW" end
end

function M.estimateStopDeltaT(slope, inertia)
    local t = { FAST=5, MEDIUM=10, SLOW=20, VERY_SLOW=45 }
    local tau = t[inertia] or 10
    return (slope / 60) * tau
end

function M.estimateStartDelay(Tint, Tcons, slope, inertia)
    if slope <= 0 then return nil end
    local dT = Tcons - Tint
    if dT <= 0 then return 0 end
    local t_main = dT / slope
    local t = { FAST=5, MEDIUM=10, SLOW=20, VERY_SLOW=40 }
    local t_rad = (t[inertia] or 10) / 60
    return (t_main + t_rad) * 3600
end

function M.computePI(error, integral, Kp, Ki, dt)
    integral = integral + error * (dt / 3600)
    local power = Kp * error + Ki * integral
    power = math.max(0, math.min(100, power))
    return power, integral
end

function M.runHeatingStep(state, Tint, Text, setpoint,power ,now)
    local slope = 0
    state.DTemp     = Tint-state.lastTemp    
    state.DTime     = now-state.lastTime    
    state.DSetPoint = setpoint-state.lastSetPoint
    state.DPower    = power-state.lastPower   

    if power == 0   then state.TimePower0   =state.TimePower0+1
    elseif power == 100 then state.TimePower100 =state.TimePower100+1
    end

    if state.lastTemp and state.lastTime and now > state.lastTime then
        slope = (Tint - state.lastTemp) / ((now - state.lastTime) / 3600)
    end

    if state.lastTemp and state.lastTime then
        local tau = M.computeTau(state.lastTemp, Tint, Text, now - state.lastTime)
        if tau then
            state.tau = tau
            local Kp, Ki = M.computePIfromTau(tau, 1.0)
            if Kp and Ki then
                state.Kp = Kp
                state.Ki = Ki
            end
        end
    end

    local dTstop = M.estimateStopDeltaT(slope, state.inertia)
    local stop = (Tint >= setpoint - dTstop)

    local error = setpoint - Tint
    local dt = state.lastTime and (now - state.lastTime) or 600
    local power; power, state.integral = M.computePI(error, state.integral, state.Kp, state.Ki, dt)

    state.lastTemp = Tint
    state.lastTime = now

    if power ~= 0   then state.TimePower0   = 0 end
    if power ~= 100 then state.TimePower100 = 0 end


    return {
        power = power,
        stop = stop,
        slope = slope,
        tau = state.tau
    }
end



return M

