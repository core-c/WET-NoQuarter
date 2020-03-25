function setSpawns()
	local c = {spawns = {}, actions = {}, defaults = true}
	c.spawns[1] = {radius = 600} -- decrease protection radius for heavy weapons inside bunker
	c.spawns[2] = {radius = 500} -- increase protection radius for beach spawn
	c.spawns[3] = {radius = 500} -- increase protection radius for beach spawn
	c.spawns[4] = {state = NO_PROTECT} -- CP spawn is not fix
	return c
end