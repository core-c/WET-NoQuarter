local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[3] = {state = NO_PROTECT}
c.actions[1] = {spawn = 3, newstate = PROTECT_ALLIES, trigger = "use the obelisk"}

return c