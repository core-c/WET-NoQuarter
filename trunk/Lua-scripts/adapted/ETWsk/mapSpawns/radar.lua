local c = {spawns = {}, actions = {}, defaults = true}
c.spawns[1] = {state = NO_PROTECT} -- Side Gate CP spawn is not fix
c.spawns[3] = {state = NO_PROTECT}
c.actions[1] = {spawn = 3, newstate = PROTECT_ALLIES, trigger = "secured the Forward Bunker"}
return c