local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[5] = {state = NO_PROTECT}
c.spawns[3] = {state = NO_PROTECT}
c.spawns[4] = {state = NO_PROTECT}
c.actions[1] = {spawn = 3, newstate = PROTECT_ALLIES, trigger = "destroyed the Dock"}

return c