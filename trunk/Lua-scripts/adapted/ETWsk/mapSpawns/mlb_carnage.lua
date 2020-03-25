local c = {spawns = {}, actions = {}, defaults = true}


c.spawns[2] = {pos = {1537,7111,-744}}
c.spawns[7] = {name = "beach bunker*", state = NO_PROTECT, pos = {5435,3398,-583}}
c.spawns[6] = {state = NO_PROTECT, pos = {10330,4382,-207}}
c.spawns[8] = {name = "allies spawn*", state = PROTECT_ALLIES, pos = {7104,6979,-599}}
c.actions[1] = {spawn = 6, newstate = PROTECT_ALLIES, trigger = "blown the main beach"}
c.actions[2] = {spawn = 7, newstate = PROTECT_ALLIES, trigger = "blown the lower beach"}

return c