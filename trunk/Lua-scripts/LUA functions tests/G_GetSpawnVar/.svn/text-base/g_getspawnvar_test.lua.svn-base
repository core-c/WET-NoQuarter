onlyonce = 0
cyclecounter = 0

function et_RunFrame( levelTime )
       
	if onlyonce == 100 then
		for i = 64, 1021 do

			if cyclecounter > i then
				return
			end

			-- Test for G_GetSpawnVal
			
			et.G_Print( "Entity "..i..":\n")
			
			local classname = et.G_GetSpawnVar( i, "classname" )
			if classname == nil then
				classname = "nil"
			end
			et.G_Print( "classname = "..classname.."\n")

			local origin = et.G_GetSpawnVar( i, "origin" )
			if origin == nil then
				origin = "nil"
			end
			if origin[1] == nil then
				et.G_Print( "origin = string "..origin.."\n")
			else
				et.G_Print( "origin = x "..origin[1].." y "..origin[2].." z "..origin[3].."\n")
			end
			
			local model = et.G_GetSpawnVar( i, "model" )
			if model == nil then
				model = "nil"
			end
			et.G_Print( "model = "..model.."\n")

			local model2 = et.G_GetSpawnVar( i, "model2" )
			if model2 == nil then
				model2 = "nil"
			end
			et.G_Print( "model2 = "..model2.."\n")

			local spawnflags = et.G_GetSpawnVar( i, "spawnflags" )
			if spawnflags == nil then
				spawnflags = "nil"
			end
			et.G_Print( "spawnflags = "..spawnflags.."\n")

			local eflags = et.G_GetSpawnVar( i, "eflags" )
			if eflags == nil then
				eflags = "nil"
			end
			et.G_Print( "eflags = "..eflags.."\n")

			local svflags = et.G_GetSpawnVar( i, "svflags" )
			if svflags == nil then
				svflags = "nil"
			end
			et.G_Print( "svflags = "..svflags.."\n")

			local maxs = et.G_GetSpawnVar( i, "maxs" )
			if maxs == nil then
				maxs = "nil"
			end
			if maxs[1] == nil then
				et.G_Print( "maxs = string "..maxs.."\n")
			else
				et.G_Print( "maxs = x "..maxs[1].." y "..maxs[2].." z "..maxs[3].."\n")
			end

			local mins = et.G_GetSpawnVar( i, "mins" )
			if mins == nil then
				mins = "nil"
			end
			if mins[1] == nil then
				et.G_Print( "mins = string "..mins.."\n")
			else
				et.G_Print( "mins = x "..mins[1].." y "..mins[2].." z "..mins[3].."\n")
			end
			
			local speed = et.G_GetSpawnVar( i, "speed" )
			if speed == nil then
				speed = "nil"
			end
			et.G_Print( "speed = "..speed.."\n")

			local closespeed = et.G_GetSpawnVar( i, "closespeed" )
			if closespeed == nil then
				closespeed = "nil"
			end
			et.G_Print( "closespeed = "..closespeed.."\n")

			local target = et.G_GetSpawnVar( i, "target" )
			if target == nil then
				target = "nil"
			end
			et.G_Print( "target = "..target.."\n")

			local targetname = et.G_GetSpawnVar( i, "targetname" )
			if targetname == nil then
				targetname = "nil"
			end
			et.G_Print( "targetname = "..targetname.."\n")

			local message = et.G_GetSpawnVar( i, "message" )
			if message == nil then
				message = "nil"
			end
			et.G_Print( "message = "..message.."\n")

			local popup = et.G_GetSpawnVar( i, "popup" )
			if popup == nil then
				popup = "nil"
			end
			et.G_Print( "popup = "..popup.."\n")

			local book = et.G_GetSpawnVar( i, "book" )
			if book == nil then
				book = "nil"
			end
			et.G_Print( "book = "..book.."\n")

			local team = et.G_GetSpawnVar( i, "team" )
			if team == nil then
				team = "nil"
			end
			et.G_Print( "team = "..team.."\n")

			local wait = et.G_GetSpawnVar( i, "wait" )
			if wait == nil then
				wait = "nil"
			end
			et.G_Print( "wait = "..wait.."\n")

			local random = et.G_GetSpawnVar( i, "random" )
			if random == nil then
				random = "nil"
			end
			et.G_Print( "random = "..random.."\n")

			local count = et.G_GetSpawnVar( i, "count" )
			if count == nil then
				count = "nil"
			end
			et.G_Print( "count = "..count.."\n")

			local health = et.G_GetSpawnVar( i, "health" )
			if health == nil then
				health = "nil"
			end
			et.G_Print( "health = "..health.."\n")

			local light = et.G_GetSpawnVar( i, "light" )
			if light == nil then
				light = "nil"
			end
			et.G_Print( "light = "..light.."\n")

			local dmg = et.G_GetSpawnVar( i, "dmg" )
			if dmg == nil then
				dmg = "nil"
			end
			et.G_Print( "dmg = "..dmg.."\n")

			local angles = et.G_GetSpawnVar( i, "angles" )
			if angles == nil then
				angles = "nil"
			end
			if angles[1] == nil then
				et.G_Print( "angles = string "..angles.."\n")
			else
				et.G_Print( "angles = x "..angles[1].." y "..angles[2].." z "..angles[3].."\n")
			end

			-- In etpro, this is a int or float
			local angle = et.G_GetSpawnVar( i, "angle" )
			if angle == nil then
				angle = "nil"
			end
			if angle[1] == nil then
				et.G_Print( "angle = string "..angle.."\n")
			else
				et.G_Print( "angle = x "..angle[1].." y "..angle[2].." z "..angle[3].."\n")
			end

			local duration = et.G_GetSpawnVar( i, "duration" )
			if duration == nil then
				duration = "nil"
			end
			et.G_Print( "duration = "..duration.."\n")

			local rotate = et.G_GetSpawnVar( i, "rotate" )
			if rotate == nil then
				rotate = "nil"
			end
			if rotate[1] == nil then
				et.G_Print( "rotate = string "..rotate.."\n")
			else
				et.G_Print( "rotate = x "..rotate[1].." y "..rotate[2].." z "..rotate[3].."\n")
			end

			local degrees = et.G_GetSpawnVar( i, "degrees" )
			if degrees == nil then
				degrees = "nil"
			end
			et.G_Print( "degrees = "..degrees.."\n")

			local time = et.G_GetSpawnVar( i, "time" )
			if time == nil then
				time = "nil"
			end
			et.G_Print( "time = "..time.."\n")

			local skin = et.G_GetSpawnVar( i, "skin" )
			if skin == nil then
				skin = "nil"
			end
			et.G_Print( "skin = "..skin.."\n")

			local _color = et.G_GetSpawnVar( i, "_color" )
			if _color == nil then
				_color = "nil"
			end
			if _color[1] == nil then
				et.G_Print( "_color = string ".._color.."\n")
			else
				et.G_Print( "_color = x ".._color[1].." y ".._color[2].." z ".._color[3].."\n")
			end

			local color = et.G_GetSpawnVar( i, "color" )
			if color == nil then
				color = "nil"
			end
			if color[1] == nil then
				et.G_Print( "color = string "..color.."\n")
			else
				et.G_Print( "color = x "..color[1].." y "..color[2].." z "..color[3].."\n")
			end

			local stylestring = et.G_GetSpawnVar( i, "stylestring" )
			if stylestring == nil then
				stylestring = "nil"
			end
			et.G_Print( "stylestring = "..stylestring.."\n")
			
			local shader = et.G_GetSpawnVar( i, "shader" )
			if shader == nil then
				shader = "nil"
			end
			et.G_Print( "shader = "..shader.."\n")
			
			local key = et.G_GetSpawnVar( i, "key" )
			if key == nil then
				key = "nil"
			end
			et.G_Print( "key = "..key.."\n")

			local harc = et.G_GetSpawnVar( i, "harc" )
			if harc == nil then
				harc = "nil"
			end
			et.G_Print( "harc = "..harc.."\n")

			local varc = et.G_GetSpawnVar( i, "varc" )
			if varc == nil then
				varc = "nil"
			end
			et.G_Print( "varc = "..varc.."\n")

			local delay = et.G_GetSpawnVar( i, "delay" )
			if delay == nil then
				delay = "nil"
			end
			et.G_Print( "delay = "..delay.."\n")

			local radius = et.G_GetSpawnVar( i, "radius" )
			if radius == nil then
				radius = "nil"
			end
			et.G_Print( "radius = "..radius.."\n")

			local missionlevel = et.G_GetSpawnVar( i, "missionlevel" )
			if missionlevel == nil then
				missionlevel = "nil"
			end
			et.G_Print( "missionlevel = "..missionlevel.."\n")

			local start_size = et.G_GetSpawnVar( i, "start_size" )
			if start_size == nil then
				start_size = "nil"
			end
			et.G_Print( "start_size = "..start_size.."\n")

			local end_size = et.G_GetSpawnVar( i, "end_size" )
			if end_size == nil then
				end_size = "nil"
			end
			et.G_Print( "end_size = "..end_size.."\n")

			local shard = et.G_GetSpawnVar( i, "shard" )
			if shard == nil then
				shard = "nil"
			end
			et.G_Print( "shard = "..shard.."\n")

			local spawnitem = et.G_GetSpawnVar( i, "spawnitem" )
			if spawnitem == nil then
				spawnitem = "nil"
			end
			et.G_Print( "spawnitem = "..spawnitem.."\n")

			local track = et.G_GetSpawnVar( i, "track" )
			if track == nil then
				track = "nil"
			end
			et.G_Print( "track = "..track.."\n")

			local scriptName = et.G_GetSpawnVar( i, "scriptName" )
			if scriptName == nil then
				scriptName = "nil"
			end
			et.G_Print( "scriptName = "..scriptName.."\n")

			local shortname = et.G_GetSpawnVar( i, "shortname" )
			if shortname == nil then
				shortname = "nil"
			end
			et.G_Print( "shortname = "..shortname.."\n")

			local constages = et.G_GetSpawnVar( i, "constages" )
			if constages == nil then
				constages = "nil"
			end
			et.G_Print( "constages = "..constages.."\n")

			local desstages = et.G_GetSpawnVar( i, "desstages" )
			if desstages == nil then
				desstages = "nil"
			end
			et.G_Print( "desstages = "..desstages.."\n")

			local partofstage = et.G_GetSpawnVar( i, "partofstage" )
			if partofstage == nil then
				partofstage = "nil"
			end
			et.G_Print( "partofstage = "..partofstage.."\n")

			local override = et.G_GetSpawnVar( i, "override" )
			if override == nil then
				override = "nil"
			end
			et.G_Print( "override = "..override.."\n")

			local damageparent = et.G_GetSpawnVar( i, "damageparent" )
			if damageparent == nil then
				damageparent = "nil"
			end
			et.G_Print( "damageparent = "..damageparent.."\n")

			-- Not implemented in etpro
			local numPlayers = et.G_GetSpawnVar( i, "numPlayers" )
			if numPlayers == nil then
				numPlayers = "nil"
			end
			et.G_Print( "numPlayers = "..numPlayers.."\n")

			local contents = et.G_GetSpawnVar( i, "contents" )
			if contents == nil then
				contents = "nil"
			end
			et.G_Print( "contents = "..contents.."\n")

			local clipmask = et.G_GetSpawnVar( i, "clipmask" )
			if clipmask == nil then
				clipmask = "nil"
			end
			et.G_Print( "clipmask = "..clipmask.."\n")

			local count2 = et.G_GetSpawnVar( i, "count2" )
			if count2 == nil then
				count2 = "nil"
			end
			et.G_Print( "count2 = "..count2.."\n")

			-- Not implemented in etpro
			local baseAngle = et.G_GetSpawnVar( i, "baseAngle" )
			if baseAngle == nil then
				baseAngle = "nil"
			end
			if baseAngle[1] == nil then
				et.G_Print( "baseAngle = string "..baseAngle.."\n")
			else
				et.G_Print( "baseAngle = x "..baseAngle[1].." y "..baseAngle[2].." z "..baseAngle[3].."\n")
			end

			-- Not implemented in etpro
			local baseOrigin = et.G_GetSpawnVar( i, "baseOrigin" )
			if baseOrigin == nil then
				baseOrigin = "nil"
			end
			if baseOrigin[1] == nil then
				et.G_Print( "baseOrigin = string "..baseOrigin.."\n")
			else
				et.G_Print( "baseOrigin = x "..baseOrigin[1].." y "..baseOrigin[2].." z "..baseOrigin[3].."\n")
			end

			cyclecounter = i		

		end
	end
	onlyonce = onlyonce + 1
end