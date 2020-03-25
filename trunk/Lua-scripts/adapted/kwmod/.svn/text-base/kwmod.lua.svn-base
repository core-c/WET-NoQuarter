-- KWMod, to be the follow of the ETAdminmod.
-- Features:
-- Commands based on a Configfile with ETAdminMod-lie syntax, and on the Shrubbot levels provided by NQ
-- Also more sophisticated commands as Lua-Functions with all available infos as Parameter
-- 
-- PLEASE DONT USE THIS FOR YOUR SERVER, MOST OF ITS FUNCTIONALLITY WILL BE AVAILABLE IN NOQ-MOD
-- 
--
-- Luborg



-- Configfile, absolut Path (/home/user/.etwolf/nq/commands.cfg)
configda = et.trap_Cvar_Get("g_KWCMD")

-- Commandprefix
cmdprefix = "!"

-- Array for player greetings

greetings = {}

greetings[1]	= nil -- no greeting!
greetings[2]    = "Welcome ^7-^o=^lX^+(^4Advanced ^3Player^+)^lX^o=^7- <COLOR_PLAYER> "
-- Goes on for all your levels


-- Arrays for our Infos
commands = {}
lastkilledwith = {}
killedlastwith = {}

-- need to greet array for greetingfunction
ntg = {}

-- for lastkiller function && endgame event
lastkiller = nil
antiloop = 0

-- Stolen from kmod. Good mod with cool ideas, but not yet finished. and for Etpro...
team = { "AXIS" , "ALLIES" , "SPECTATOR" }
class = { [0]="SOLDIER" , "MEDIC" , "ENGINEER" , "FIELD OPS" , "COVERT OPS" }

-- This array is based on some NQ-Array, see /trunk/lua-scripts
meansofdeath = {
	[0]="MOD_UNKNOWN",
	"MACHINEGUN",
	"BROWNING",
	"MG42",
	"GRENADE",
	"KNIFE",
	"LUGER",
	"COLT",
	"MP40",
	"THOMPSON",
	"STEN",
	"GARAND",
	"SILENCER",
	"FG42",
	"FG42_SCOPE",
	"PANZERFAUST",
	"GRENADE", -- war grenade_launcher
	"FLAMETHROWER",
	"GRENADE_PINEAPPLE",
	"MAPMORTAR",
	"MAPMORTAR_SPLASH",
	"KICKED",
--	"GRABBER",
	"DYNAMITE",
	"AIRSTRIKE", 
	"SYRINGE",	
	"AMMO",	
	"ARTY",	
	"WATER",
	"SLIME",
	"LAVA",
	"CRUSH",
	"TELEFRAG",
	"FALLING",
	"SUICIDE",
	"TARGET_LASER",
	"TRIGGER_HURT",
	"EXPLOSIVE",
	"CARBINE",
	"KAR98",
	"GPG40",
	"M7",
	"LANDMINE",
	"SATCHEL",
	"SMOKEBOMB",
	"MOBILE_MG42",
	"SILENCED_COLT",
	"GARAND_SCOPE",
	"CRUSH_CONSTRUCTION",
	"CRUSH_CONSTRUCTIONDEATH",
	"CRUSH_CONSTRUCTIONDEATH_NOATTACKER",
	"K43",
	"K43_SCOPE",
	"MORTAR",
	"AKIMBO_COLT",
	"AKIMBO_LUGER",
	"AKIMBO_SILENCEDCOLT",
	"AKIMBO_SILENCEDLUGER",
	"SMOKEGRENADE",
	"SWAP_PLACES",
	"SWITCHTEAM",
	"GOOMBA",
	"POISON",
	"FEAR",
	"CENSORED",
	"SHOTGUN",	
	"BACKSTAB",
	"MOBILE_BROWNING",
	"BAR",
	"STG44",
	"BAZOOKA",
	"STEN_MKII",
	"MP34",
	"VENOM",
	"SHOVE",
	"THROWKNIFE",
	"NUM_MODS",
}

function et_InitGame (levelTime , randomSeed , restart )

et.RegisterModname ("KWMod " .. et.FindSelf() )

-- At start, parse Config in
--
kw_parseconf()

end

function et_ClientConnect( clientNum, firstTime, isBot ) 

if firstTime == 1 then
ntg[clientNum] = true
else
ntg[clientNum] = false
end

end -- end of ClientConnect


function et_ClientDisconnect( clientNum ) 
ntg[clientNum] = false
end


function et_ClientBegin (clientNum)


-- For our Greetings-function
lvl = et.G_shrubbot_level(clientNum)
if ntg[clientNum] == true then
	if greetings[lvl] ~= nil  then
	et.CS_PLAYERS = 689

	local cs = et.trap_GetConfigstring(et.CS_PLAYERS + clientNum)
	local name = et.Info_ValueForKey(cs, "n")
	local cmd = greetings[lvl]
	local cmd = string.gsub(cmd, "<COLOR_PLAYER>", name)
	local cmd = "cpm " .. cmd
	et.trap_SendConsoleCommand( et.EXEC_NOW, cmd )
	end

ntg[clientNum] = false 
end

end -- end of clientBegin

function et_ClientCommand (clientNum, command)

local arg0 = string.lower(et.trap_Argv(0))
local arg1 = string.lower(et.trap_Argv(1))
local arg2 = string.lower(et.trap_Argv(2))

if arg0 == "say" and string.sub(arg1, 1,1) == cmdprefix then
gotCmd( clientNum, command , false)
end

if arg0 == "vsay" and string.sub(arg2 , 1, 1) == cmdprefix then
gotCmd (clientNum, command, true)
end

if arg0 == "readthefile" then 
kw_parseconf()
return 1
end


end

function et_Obituary( victim, killer, Deaththingie )
lastkilledwith[victim] = meansofdeath[Deaththingie]
killedlastwith[killer] = meansofdeath[Deaththingie]
--et.G_LogPrint ("MOD: "..victim .. " wurd kill " .. killer .."Index:".. Deaththingie .."  ;".. meansofdeath[Deaththingie].."\n")
lastkiller = killer
end

function et_RunFrame( levelTime ) 
if antiloop == 0 then -- this simulates a endmap event  (thanks to kmod!)
if tonumber(et.trap_Cvar_Get("gamestate")) == 3 then
execCmd(lastkiller, "chat \"^2And the last kill of the Round goes to: ^7<COLOR_PLAYER>\"" , lastkiller)
antiloop = 1
end
end
end

function gotCmd( clientNum, command, vsay)

local arg0 = string.lower(et.trap_Argv(0))
local arg1 = string.lower(et.trap_Argv(1))
local arg2 = string.lower(et.trap_Argv(2))

local cmd
local lvl = tonumber(et.G_shrubbot_level( clientNum ) )
local realcmd

if vsay == false then
cmd = string.sub(arg1 ,2)
argw = arg2
else 
cmd = string.sub(arg2 ,2)
argw = string.lower(et.trap_Argv(3))
end
if string.find(cmd, " ") ~= nil then
local t = justWords(cmd)
cmd = t[1]
argw = t[2]

end

for i=lvl, 0, -1 do
	if commands[i][cmd] ~= nil then
	execCmd(clientNum, commands[i][cmd], argw)
	break
	end
end

end

function justWords(str)
  local t = {}
  local function helper(word) table.insert(t, word) return "" end
  if not str:gsub("%w+", helper):find"%S" then return t end
end


function execCmd (clientNum , cmd, argw)
		local str = cmd
		local lastkilled = et.gentity_get(clientNum, "pers.lastkilled_client")
		local lastkiller = et.gentity_get(clientNum, "pers.lastkiller_client")
		if lastkilled == -1 then
		lastkilled = clientNum
		killedlastwith[clientNum] = "nothing"
		end
		if lastkiller == -1 then 
		lastkiller = clientNum
		lastkilledwith[clientNum] = "nothing"
		 end
		
		local otherplayer = argw
		--if vsay == false then
		--otherplayer = string.lower(et.trap_Argv(2))
		--else
		--otherplayer = string.lower(et.trap_Argv(3))
		--end		
		
		otherplayer = getPlayerId(otherplayer, clientNum)
		if otherplayer == nil then otherplayer = clientNum end  
			
		

		local t = tonumber(et.gentity_get(clientNum,"sess.sessionTeam"))
		local c = tonumber(et.gentity_get(clientNum,"sess.latchPlayerType"))
		local str = string.gsub(str, "<CLIENT_ID>", clientNum)
		local str = string.gsub(str, "<GUID>", et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" ))
		local str = string.gsub(str, "<COLOR_PLAYER>", et.gentity_get(clientNum,"pers.netname"))
		local str = string.gsub(str, "<ADMINLEVEL>", et.G_shrubbot_level (clientNum) )
		local str = string.gsub(str, "<PLAYER>", et.Q_CleanStr(et.gentity_get(clientNum,"pers.netname")))
		local str = string.gsub(str, "<PLAYER_CLASS>", class[c])
		local str = string.gsub(str, "<PLAYER_TEAM>", team[t])
		--local str = string.gsub(str, "<PARAMETER>", Cvar1..Cvar2)
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_ID>", lastkiller )
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_NAME>", et.Q_CleanStr( et.gentity_get(lastkiller, "pers.netname") ))
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_CNAME>", et.gentity_get(lastkiller, "pers.netname"))
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_WEAPON>", lastkilledwith[clientNum])
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_ID>", lastkilled )
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_NAME>", et.Q_CleanStr( et.gentity_get(lastkilled, "pers.netname")))
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_CNAME>", et.gentity_get(lastkilled, "pers.netname"))
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_WEAPON>", killedlastwith[clientNum])
		--local str = string.gsub(str, "<PNAME2ID>", pnameID)
		--local str = string.gsub(str, "<PBPNAME2ID>", PBpnameID)
		--local str = string.gsub(str, "<PB_ID>", PBID)
		--local str = string.gsub(str, "<RANDOM_ID>", randomC)
		--local str = string.gsub(str, "<RANDOM_CNAME>", randomCName)
		--local str = string.gsub(str, "<RANDOM_NAME>", randomName)
		--local str = string.gsub(str, "<RANDOM_CLASS>", randomClass)
		--local str = string.gsub(str, "<RANDOM_TEAM>", randomTeam)
		--local teamnumber = tonumber(et.gentity_get(PlayerID,"sess.sessionTeam"))
		--local classnumber = tonumber(et.gentity_get(PlayerID,"sess.latchPlayerType"))
		if otherplayer == clientNum then 
		 str = string.gsub(str, "<PART2PBID>", "-1" )
		end
			
		--else
		 str = string.gsub(str, "<PART2CNAME>", et.gentity_get(otherplayer, "pers.netname" ))
		 str = string.gsub(str, "<PART2ID>", otherplayer )
 		 str = string.gsub(str, "<PART2PBID>", otherplayer + 1 ) 
		 str = string.gsub(str, "<PART2GUID>", et.Info_ValueForKey( et.trap_GetUserinfo( otherplayer ), "cl_guid" ))
		 str = string.gsub(str, "<PART2LEVEL>", et.G_shrubbot_level (otherplayer) )
		 str = string.gsub(str, "<PART2NAME>", et.Q_CleanStr(et.gentity_get(otherplayer,"pers.netname")))
		--end
		
		if string.sub(str, 1,5) == "$LUA$" then
		local f = loadstring(string.sub(str,6))
		
	--	et.G_LogPrint("Executing lua ".. string.sub(str,6).." <<-- \n")
		f()
	--	et.G_LogPrint("Success  \n")
		return	
		end
		
		if string.sub(str, 1,5) == "$SHL$" then
	--	et.G_LogPrint("execing \n\n" .. string.sub(str, 6) .. "\n\n")
		execthis = io.popen(string.sub(str,6))
		readshit = execthis:read("*a")
		execthis:close()
		readshit = string.gsub(readshit, "\n","\"\nqsay \"")
		et.trap_SendConsoleCommand(et.EXEC_APPEND, "qsay \" ".. readshit .. " \"")
	--	et.G_LogPrint("GOT THIS:\n"..readshit.."\n\n")
		return	
		end
		

		et.trap_SendConsoleCommand( et.EXEC_APPEND, "".. str .. "\n " )

end

function getPlayerId(name, clientID)
    local i

    -- if it's nil, return nil and throw error
    if (name == "") then
        --printmsg("Not enough parameters.", clientID)
        return nil
    end

    -- if it's a number, interpret as slot number
    local clientnum = tonumber(name)
    if clientnum then
        if (clientnum <= tonumber(et.trap_Cvar_Get("sv_maxclients"))) and et.gentity_get(clientnum,"inuse") then
            return clientnum
        else
           -- printmsg("Slot "..clientnum.." not in use.", clientID)
            return nil
        end
    end

    -- exact match first
 --   for i=0,et.trap_Cvar_Get("sv_maxclients"),1 do
   --     playeri = et.gentity_get(i,"pers.netname")
     --   if playeri then
       --     if et.Q_CleanStr( playeri ) == et.Q_CleanStr( name ) then
         --       return i
         --   end
       -- end
   -- end
    -- partial match
   -- for i=0,et.trap_Cvar_Get("sv_maxclients"),1 do
       -- playeri = et.gentity_get(i,"pers.netname")
     --   for testingthis,playeri in pcall(et.gentity_get, i, "pers.netname") do
--	if testingthis == true then
 --           if (string.find(string.lower(et.Q_CleanStr( playeri )), string.lower(et.Q_CleanStr( name )), 1, true)) then
   --             return i
     --       end
       -- end
--	end
 --   end
	local test = et.ClientNumberFromString( name )
	if test == -1 then
    --printmsg("No name match found for '"..name.."'.", clientID)
    return nil
	else
	return test
	end
	
end


function kw_parseconf ()
	local	datei = io.open (configda,"r") 
		
	for i=0, 31, 1 do				
		commands[i] = {}
	end
	nmr = 1
	nmr2 = 1
	for line in datei:lines() do
		local filestr = line
		local testcase = string.find(filestr, "^%s*%#")
		if testcase == nil then
		for level,comm,commin in string.gfind(filestr, "[^%#](%d+)%s*%-%s*(%w+)%s*%=%s*(.*)[^%\n]*") do
	--		et.G_LogPrint ("Parsing CMD:"..comm .. "level: "..level.." Content: ".. commin .."\n")
			i = tonumber(level)
			commands[i][comm] = commin	
	
			nmr = nmr +1
		end
		end
	nmr2 = nmr2 +1
	end

	datei:close()
	et.G_LogPrint("parsed " ..nmr .." kwmodcommands from "..nmr2.." lines \n")
end


