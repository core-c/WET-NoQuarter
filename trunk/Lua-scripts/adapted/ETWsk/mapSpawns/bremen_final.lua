local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[5] = {state = NO_PROTECT}
c.actions[1] = {spawn = 3, newstate = PROTECT_ALLIES, trigger = "main gate has been destroyed"}

return c