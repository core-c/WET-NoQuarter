--[[
	Announcer script for LUA

 - firstblood
 - lastblood
 - HP left message
 - Killing/death sprees
 - stats ?

--]]

---------------------------------------------------------------------------------------------
-- Modify! 
---------------------------------------------------------------------------------------------

greetingsmessage = "Welcome to the server!"
color = "^5" 

---------------------------------------------------------------------------------------------
-- Don't modify 
---------------------------------------------------------------------------------------------
scriptinfo = "^1" .. et.trap_Cvar_Get("fs_game") .. " ^8V" .. et.trap_Cvar_Get("mod_version")
---------------------------------------------------------------------------------------------

function et_ClientConnect( clientNum, firstTime, isBot, reason)

	-- this is called on the very first time a client connects
	if firstTime == 1 then
	  FirstConnect( clientNum )
	end
	-- every connect (also first time - map restarts)
	ClientConnect(clientNum )

end


-- Different to ETPub!
-- ok
function et_Obituary( victim, killer, meansOfDeath )
	
	-- http://wolfwiki.anime.net/index.php/User:Furion
	local victimteam = tonumber(et.gentity_get(victim, "sess.sessionTeam")) 
	local killerteam = tonumber(et.gentity_get(killer, "sess.sessionTeam")) 
	local posk = et.gentity_get(victim,"r.currentOrigin")
	local posv = et.gentity_get(killer,"r.currentOrigin")
    
	if victimteam ~= killerteam and killernum < 1022 then 
		local killername = string.gsub(et.gentity_get(killernum, "pers.netname"), "%^$", "^^ ") 
		local killerhp = et.gentity_get(killernum, "health") 
       		killdist = dist(posk,posv)

        -- add weapon
		msg = string.format("cp  \"" .. killername ..  "^1 killed you from ^o" .. killdist .. " ^1m\n^7He had ^o" .. killerhp.. "^7 HP\"\n")
		et.trap_SendServerCommand(victimnum, msg)
	end 
end


---------------------------------------------------------------------------------------------
-- Split et_ClientConnect for real
---------------------------------------------------------------------------------------------

function FirstConnect( clientNum )
	-- serverinfo & clan message
	et.trap_SendServerCommand( clientNum ,"cpm \"This server is running " .. scriptinfo)
	et.trap_SendServerCommand( clientNum ,"cpm \"" .. greetingsmessage)
end


function ClientConnect( clientNum )
	et.trap_SendServerCommand( clientNum ,"cpm \"This server is running " .. scriptinfo)
end

---------------------------------------------------------------------------------------------
-- Helper functions
---------------------------------------------------------------------------------------------

-- http://wolfwiki.anime.net/index.php/User:Furion
function dist(a ,b)
   local dx = math.abs(b[1] - a[1])
   local dy = math.abs(b[2] - a[2])
   local dz = math.abs(b[3] - a[3])
   local d = math.sqrt((dx ^ 2) + (dy ^ 2) + (dz ^ 2))
   return math.floor(d / 52.4934)
end 