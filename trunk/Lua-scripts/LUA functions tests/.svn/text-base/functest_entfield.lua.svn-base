filename= "testnq.txt"
filecheck=1

fd, len = et.trap_FS_FOpenFile(filename, et.FS_APPEND)

if len == 1 then
	et.G_Print(string.format("Could not open %s\n", fielname))
	filecheck=0
end

function et_InitGame( levelTime, randomSeed, restart )

	et.RegisterModname( "functest")
	
	if filecheck==1 then
	    local data = string.format("et_InitGame (%d|%d|%d)\n", levelTime, randomSeed, restart)

  		et.trap_FS_Write(data, string.len(data), fd)

	end


end

function et_ClientConnect( clientNum, firstTime, isBot )
	
	if filecheck==1 then
		local data = string.format("et_ClientConnect (%d|%d|%d)\n", clientNum, firstTime, isBot)

  		et.trap_FS_Write(data, string.len(data), fd)

	end
    return nil
end


function et_ClientBegin( clientNum )

    if filecheck==1 then
	    local data = string.format("et_ClientBegin (%d)\n", clientNum)

  		et.trap_FS_Write(data, string.len(data), fd)

	end

end


function et_ClientDisconnect( clientNum )

    if filecheck==1 then
	    local data = string.format("et_ClientDisconnect (%d)\n", clientNum)

  		et.trap_FS_Write(data, string.len(data), fd)

	end

end

function et_ShutdownGame( restart )

    if filecheck==1 then
	    local data = string.format("et_ShutdownGame (%d)\n", restart)

  		et.trap_FS_Write(data, string.len(data), fd)

	end

	et.trap_FS_FCloseFile(fd)


end

function et_ConsoleCommand( command )

    if filecheck==1 then
	    local data = string.format("et_ConsoleCommand (%s) %s %s \n", command, et.trap_Argc(), et.trap_Argv(0)  )

  		et.trap_FS_Write(data, string.len(data), fd)
	end
end

function et_ClientCommand( clientNum, command )

    if filecheck==1 then
	    local data = string.format("et_ClientCommand (%d) (%s) (%s) (%s) (%s) \n", clientNum, command, et.trap_Argc(), et.trap_Argv(0) , et.trap_Argv(1) )

  		et.trap_FS_Write(data, string.len(data), fd)
	end
	
	local arg0 = string.lower(et.trap_Argv(0))
	local arg1 = string.lower(et.trap_Argv(1))
	
	if arg0 == "testentfield" then
		-- Description:
		-- Some fields are of type FIELD_ENTITY, which is a pointer to a gentity_t.
		-- The et.gentity_get() function does not return a pointer, but an entNum instead.. or nil otherwise.
		-- This Lua-script demonstrates how to deal with such fields.
		-- It reads the "tankLink" field of player[clientNum], which is me executing this script..,
		-- it then writes the entNum for the field "tankLink" to disk.
		-- I tested the script while mounting/unmounting the fueldump tank.
		-- Depending on wether i'm on the tank or not, a valid/invalid entNum is returned for: entNum = et.gentity_get(clientNum,"tankLink").
		-- If i do have a valid entNum, it is the entity-number of the tank i have mounted..
		-- ..i read the scriptname of the tank in the next step of the script. This to prove the found entNum really points to the tank.
		
	
		local data = "Started the entitytest \n"
		et.trap_FS_Write(data, string.len(data), fd)

		-- if a player is mounted on a tank, member ent->tankLink holds the address of the tank entity.. (else tankLink == NULL)
		-- However, a pointer-value is not returned. Instead, the possible entity-number is returned (or nil if pointer is NULL).
		entNum = et.gentity_get( clientNum, "tankLink")
		if entNum == nil then
			data =  string.format("%30s | %15s | entNum = %15s\n", "tankLink", type(entNum), "nil" )
			et.trap_FS_Write(data, string.len(data), fd)
		else
			data =  string.format("%30s | %15s | entNum = %15s\n", "tankLink", type(entNum), entNum )
			et.trap_FS_Write(data, string.len(data), fd)
			s = et.gentity_get( entNum, "scriptname")
			if s ~= nil then
				data =  string.format( "tank scriptname = %s\n", s )
				et.trap_FS_Write(data, string.len(data), fd)
			end
		end

	end
end


