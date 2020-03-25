local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[2] = {state = NO_PROTECT}
c.spawns[5] = {state = NO_PROTECT}
c.spawns[8] = {state = NO_PROTECT}
c.spawns[10] = {state = NO_PROTECT}
c.spawns[11] = {state = NO_PROTECT}
c.spawns[12] = {state = NO_PROTECT}
c.spawns[13] = {name = "Inner Temmple*", state = NO_PROTECT, pos = {-1350, -5013, -1064}}
c.actions[1] = {spawn = 11, newstate = PROTECT_AXIS, trigger = "second barrier has been blown"}
c.actions[2] = {spawn = 8, newstate = NO_PROTECT, trigger = "second barrier has been blown"}
c.actions[3] = {spawn = 13, newstate = PROTECT_AXIS, trigger = "breached the Temple entrance"}

return c