local c = {spawns = {}, actions = {}, defaults = true}


c.spawns[2] = {pos = {-1185,-3102,-615}}
c.spawns[3] = {pos = {2175,-1702,-591}, radius = 300}
c.spawns[4] = {state = NO_PROTECT}
c.spawns[5] = {state = NO_PROTECT}
c.spawns[6] = {state = NO_PROTECT}
c.spawns[7] = {name = "Town Entrance*", state = PROTECT_ALLIES, pos = {-2221,-3137,-615}, radius = 300}
c.actions[1] = {spawn = 6, newstate = PROTECT_ALLIES, trigger = "Village Gate has been breached"}

return c