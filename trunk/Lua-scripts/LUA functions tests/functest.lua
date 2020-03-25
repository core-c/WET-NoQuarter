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
	
	if arg0 == "testent" then
	
		local data = "Started the entitytest \n"
		et.trap_FS_Write(data, string.len(data), fd)
		filename= "/home/et/totest"

		io.input(filename)
		
		local lines = {}
		-- read the lines in table 'lines'
		for line in io.lines() do
		  table.insert(lines, line)
		end
	
		for i, l in ipairs(lines) do
			local outp = et.gentity_get( arg1 ,l)
			if outp ~= nil then
			
				data =  string.format("%30s | %15s | %15s \n", l,type(l), outp )
				et.trap_FS_Write(data, string.len(data), fd)
				
			else
			data =  string.format("%30s | %15s | %15s \n", l,type(l), "nil:/"  )
				et.trap_FS_Write(data, string.len(data), fd)
			end	
		end
		

	end
end


