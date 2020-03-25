local c = {spawns = {}, actions = {}, defaults = true}

c.actions[1] = {spawn = 1, newstate = PROTECT_ALLIES, trigger = "Allies capture forward flag"}
c.actions[2] = {spawn = 1, newstate = PROTECT_AXIS, trigger = "Axis own the forward flag"}

return c