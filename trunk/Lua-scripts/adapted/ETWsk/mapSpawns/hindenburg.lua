local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[2] = {radius = 500}
c.actions[1] = {spawn = 2, newstate = PROTECT_ALLIES, trigger = "destroyed the side entrance"}

return c