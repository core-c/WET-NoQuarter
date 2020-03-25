local c = {spawns = {}, actions = {}, defaults = true}

c.spawns[4] = {radius = 300}
c.actions[1] = {spawn = 2, newstate = PROTECT_ALLIES, trigger = "Allied autospawn to Ammo Depot"}

return c