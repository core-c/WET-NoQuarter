-- Extended ban by luborg
-- Usage: 
-- Set bandat to your banfile-location
-- bandat-example:
-- 127.0.
-- 129.12.12.
-- exclude
-- <GUID without the brackets>
-- EOF
-- or just look in the example 
-- No warranty!



-- Your banfile:
bandat = "/home/et/.etwolf/nq/bans.cfg"

-- Arrays: 
 bans = {}
 exclude = {}

 function et_InitGame( levelTime, randomSeed, restart )

	et.RegisterModname( "Extended IpBan " .. et.FindSelf() )

	 et.G_LogPrint("^1Initializing Extended Ban \n")
				local	datei = io.open (bandat,"r") 
							local ban = true
							for line in datei:lines() do
		
		if line == nil then 
			break
		 end		
							if line == "exclude" then 
							ban = false 
							line = ""
							end
							if ban == true then
							bans[line] = true
							end
							if ban == false then
								if line ~= "" then
							exclude[line] = true
								end
							end
																																 
	end
	datei:close()
end

function et_ClientConnect( clientNum, firstTime, isBot )
	  local IP = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "ip" )
	et.G_LogPrint("IP:" .. IP .."\n")  
	str = string.gsub(IP, ":%d*","")
	   sep = "."
			fields = {str:match((str:gsub("[^"..sep.."]*"..sep, "([^"..sep.."]*)"..sep)))}

				
				if bans[str] == true then checkplayer(clientNum)
					return
				 end
				if  bans[ fields[1] ..".".. fields[2] ..".".. fields[3] .."." ]  == true then
					checkplayer(clientNum)
					return 
				end
				if  bans[ fields[1] ..".".. fields[2] .."." ]  == true then 
					checkplayer(clientNum)
					return
					end
									
				if  bans[ fields[1] .."." ] == true then
					checkplayer(clientNum)
					return
					end
					return 
end


function checkplayer(clientNum)
						local guid = string.upper( et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" ))
						
	                   		 if exclude[guid] == true then 
						et.G_LogPrint("Client "..clientNum.." had luck, was in excludelist\n")
						return 
					 else
						et.trap_DropClient( clientNum, "You were banned by Admin.", 10 )   
						et.G_LogPrint("Dropped Client " .. clientNum .. " cause he is in banlist\n")	
					end
end

