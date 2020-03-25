function et_ConsoleCommand()

	if et.trap_Argv(0) == "testmute" then

	et.G_LogPrint( "Starting mute Function tests.\n")
	local playermutetest
	-- We just get the first player. one needs to be online. else error
	for i=0, et.trap_Cvar_Get( "sv_maxclients" ), 1 do				
		
		if et.gentity_get(i,"classname") == "player" then
			playermutetest = i
			break
		end
	end

	local name = et.gentity_get(playermutetest,"pers.netname")
	et.G_LogPrint( "Working with Player: " .. name .. " \n")

	if et.gentity_get(playermutetest,"sess.muted") == 1 then
		-- first player is muted
		local timemuted = et.gentity_get(playermutetest,"sess.auto_mute_time")
		local bymuted = et.gentity_get(playermutetest,"sess.muted_by")

		et.G_LogPrint( "Player already muted. \n ")
		et.G_LogPrint( "Muted for: " .. timemuted .. " \n ") -- returns an int, wich gets biger overtime, seems to be leveltime.
		et.G_LogPrint( "Muted by : " .. bymuted .. " \n ") -- returns a small number, wich is not the ?slotnum?

		et.G_LogPrint( "Unmuting him trough lua:\n ")
		et.UnmutePlayer( playermutetest )
		et.G_LogPrint( "Success?:\n ")

	else
		-- We can mute him
		if muteway ~= 1 then
			et.G_LogPrint( "Now muting trough functioncalls. 30secs \n ")
			et.MutePlayer( playermutetest, 30 )
			et.G_LogPrint( "Success?:\n ")

			muteway = 1
		else
			-- muted him already last time.. with Luacalls, now we do it the hard way! just to check

			et.G_LogPrint( "Now muting manually. 30secs \n ")
			et.gentity_set(playermutetest,"sess.muted", 1)
			et.gentity_set(playermutetest,"sess.auto_mute_time", 30)
			et.gentity_set(playermutetest,"sess.muted_by", "LUA CONSOLE")
			et.G_LogPrint( "Success?:\n ")


		end

	end


end -- end
	return 0
end

