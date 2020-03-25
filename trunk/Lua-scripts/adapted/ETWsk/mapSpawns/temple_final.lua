local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[3] = {state = NO_PROTECT}
c.actions[1] = {spawn = 3, newstate = PROTECT_ALLIES, trigger = "reached the command post area"}
c.actions[2] = {spawn = 5, newstate = PROTECT_ALLIES, trigger = "reached the fort"}
c.actions[3] = {spawn = 4, newstate = PROTECT_ALLIES, trigger = "destroyed the Temple Entrance"}


return c