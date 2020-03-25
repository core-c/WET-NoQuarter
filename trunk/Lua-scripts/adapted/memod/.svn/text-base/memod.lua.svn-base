-- /me Mod for Noquarter
-- Made by Luborg
--
-- Implements the /me command know from IRC and IM
--
-- No warranty!

function et_InitGame( levelTime, randomSeed, restart )
	et.RegisterModname( "Memod by luborg" )
		 et.G_LogPrint("^1Memod is running!\n")
end

function et.G_Printf(...)
       et.G_Print(string.format(unpack(arg)))
end


function et_ClientCommand( clientNum, command )
	local name = et.gentity_get(clientNum,"pers.netname")
	local arg0 = string.lower(et.trap_Argv(0))
	local arg1 = string.lower(et.trap_Argv(1))
	local muted = et.gentity_get(clientNum, "sess.muted")
	local bol = 0
	if muted ~= 1 then
		if arg0 == "me" then
				if arg1 ~= "" then
					et.trap_SendServerCommand( -1, "chat \"* ".. name  .." ".. et.ConcatArgs( 1) .."")  
					return 1
				end
			et.trap_SendServerCommand( clientNum, "chat \"Syntax : /me text to do some action ") 
			return 1
		elseif arg0 == "say" then
			if arg1 == "me"  then
				if string.lower(et.trap_Argv(2)) ~= "" then
					et.trap_SendServerCommand( -1, "chat \"* ".. name  .." ".. et.ConcatArgs( 2) .."")  
					return 1
				end		
			end
			if string.sub(arg1, 1,2) == "me" and string.sub(arg1,3,3) == " " then
				if string.sub(arg1, 3) ~= "" then
					et.trap_SendServerCommand( -1, "chat \"* ".. name  .." ".. string.sub(arg1, 3) .."") 
					return 1
				end
			end
			if string.sub(arg1, 1,3) == "/me" then
				if string.sub(arg1, 4) ~= "" then
					et.trap_SendServerCommand( -1, "chat \"* ".. name  .." ".. string.sub(arg1, 4) .."") 
					return 1
				end
			end
			
		end
	end
	
	return bol
end

