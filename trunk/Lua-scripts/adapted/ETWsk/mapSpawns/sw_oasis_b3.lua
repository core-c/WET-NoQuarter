local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[4] = {radius = 600}
c.spawns[5] = {name = "Allied Camp*", state = PROTECT_ALLIES, pos = {832, 2664, -509}}
c.actions[1] = {spawn = 2, newstate = PROTECT_ALLIES, trigger = "breached the Old City wall"}
return c