local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[5] = {state = NO_PROTECT}
c.actions[1] = {spawn = 4, newstate = NO_PROTECT, trigger = "access to the side path"}
c.actions[2] = {spawn = 5, newstate = PROTECT_ALLIES, trigger = "access to the side path"}

return c