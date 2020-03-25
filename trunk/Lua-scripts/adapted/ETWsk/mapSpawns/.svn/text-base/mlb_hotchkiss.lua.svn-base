local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[1] = {state = NO_PROTECT}
c.spawns[5] = {name = "Factory Entrance*", state = NO_PROTECT, pos = {-960, -1675, 88}, radius = 600}
c.spawns[6] = {name = "Main Gate*", state = NO_PROTECT, pos = {1760, -586, 280}}
c.actions[1] = {spawn = 5, newstate = PROTECT_AXIS, trigger = "breached the factory entrance gates"}
c.actions[2] = {spawn = 6, newstate = PROTECT_AXIS, trigger = "main wall has been breached"}

return c