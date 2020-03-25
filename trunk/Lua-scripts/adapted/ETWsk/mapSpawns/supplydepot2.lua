local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[4] = {state = NO_PROTECT}
c.spawns[3] = {state = NO_PROTECT}
c.spawns[2] = {radius = 400}
c.actions[1] = {spawn = 3, newstate = PROTECT_ALLIES, trigger = "breached the Forward Bunker"}

return c