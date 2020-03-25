local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[4] = {state = NO_PROTECT}
c.actions[1] = {spawn = 1, newstate = PROTECT_ALLIES, trigger = "breached the Serpentine Barricade"}


return c