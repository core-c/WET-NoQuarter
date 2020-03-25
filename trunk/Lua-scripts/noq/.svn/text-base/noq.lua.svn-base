-- .
-- The NOQ - No Quarter Lua game manager
--
-- noq.lua - A Shrubbot replacement and also kind of new game manager and tracking system based on dbms
-- mysql or sqlite3. Both are supported and in case of sqlite there is no extra sqlite installation needed.

-- (p) 2009-2010 NQ team 2009-2010 - No warrenty :)

-- 
-- NOTE: use with NQ 1.2.8 and later only
--

-- Setup:
-- - Make sure needed Lua SQL libs are on server and run properly. You only need the lib for the selected dbms.
-- - Copy the content of this path to fs_homepath/nq 
-- - NOQ is set up to sqlite by default make sure your server instance has write permissions in fs_homepath. 
--   SQLite will create a file "noquarter.sqlite" at this location.
-- - Make the config to your own and edit the script if you you want to use the mysql.
-- - Restart the server and check if all lua_modules install.lua, console.lua and noq.lua are registered.
-- - Call /rcon !sqlcreate - Done. Your system is set up.
--
-- noq_i.lua - Install script remove after install
--
-- noq_c.lua - Additional tool to enter sql cmds on the ET console
--
-- noq_config.cfg - Stores all data to run & control the NOQ. Make this file your own!
--
-- noq_commands.cfg - Commands definition file - Make this file your own! 
--
-- noq_mods.cfg - Methods of death enum file - never touch!
-- noq_weapons.cfg - Weapon enum config file - never touch!
--
-- nqconst.lua - No Quarter constants
--


-- TODOs: 
-- sort out debug messages and print only in debug mode
-- firsttime global - we don't get the used playername on clientconnect ... 
-- implement tracker
-- add custom commands - add existing shrubbot cmds like !ban, !mute, !setlevel
-- add client[conname] to dbase and queries
-- check for inconsistences in data by reconnects on mapstart or change
-- add a option to restore xp out of database - done ?
-- don't write to database during game
--    BIGTASK
--    add kind of cache tables for players and sessions
--    on disconnect we put the data not into db but in this tables
--    if a client reconnects we don't have to get the data from db again (happens quite often)
--    All sessions and players will be written into database at end of game
--    This also avoids massive session entries! 
-- 	  Note: maybe we can use Lua copy table functions for this job
-- pump database values of level and mute, ban etc into the mod - this lets us use existing shrubbot mod code and the ability to disable the shrubbot file I/O crap			
-- move command definition to db ?
-- get connect count (session entries)
-- add announcer.lua features to the NOQ
-- cleanup / optimize ...
-- TEST ALL !!!		

scriptpath = et.trap_Cvar_Get("fs_homepath") .. "/nq/" -- full qualified path for the NOQ scripts
-- scriptpath = "/usr/local/games/enemy-territory/tmp/nq/" -- full qualified path for the NOQ scripts
			
-------------------------------------------------------------------------------

-- SCRIPT VARS - don't touch !

-------------------------------------------------------------------------------

-- LUA module version
version = "1" -- see version table
databasecheck = 0

-------------------------------------------------------------------------------
-- table functions - don't move down!
-------------------------------------------------------------------------------

-- The table load 
function table.load( sfile )
   -- catch marker for stringtable
   if string.sub( sfile,-3,-1 ) == "--|" then
	  tables,err = loadstring( sfile )
   else
	  tables,err = loadfile( sfile )
   end
   if err then return _,err
   end
   tables = tables()
   for idx = 1,#tables do
	  local tolinkv,tolinki = {},{}
	  for i,v in pairs( tables[idx] ) do
		 if type( v ) == "table" and tables[v[1]] then
			table.insert( tolinkv,{ i,tables[v[1]] } )
		 end
		 if type( i ) == "table" and tables[i[1]] then
			table.insert( tolinki,{ i,tables[i[1]] } )
		 end
	  end
	  -- link values, first due to possible changes of indices
	  for _,v in ipairs( tolinkv ) do
		 tables[idx][v[1]] = v[2]
	  end
	  -- link indices
	  for _,v in ipairs( tolinki ) do
		 tables[idx][v[2]],tables[idx][v[1]] =  tables[idx][v[1]],nil
	  end
   end
   return tables[1]
end


-- table helper
function getInfoFromTable( table )
	-- table.sort(cvartable)
	for k,v in pairs(table) do et.G_Print(k .. "=" .. v .. "\n") end
	-- setn not set so empty
	-- et.G_Print("size:" .. table.getn(cvartable) .. "\n")
end
-- table functions end


et.G_LogPrint ("Loading NOQ config from ".. scriptpath.."\n")
noqvartable		= assert(table.load( scriptpath .. "noq_config.cfg"))
meansofdeath 	= assert(table.load( scriptpath .. "noq_mods.cfg")) -- all MODS 
weapons 		= assert(table.load( scriptpath .. "noq_weapons.cfg")) -- all weapons 

-- Gets varvalue else null
function getConfig ( varname )
	if noqvartable[varname] then
		return noqvartable[varname]
	else
		et.G_Print("warning, invalid config value for " .. varname .. "\n")
		return "null"
	end
end

-- Sets varvalue 
function setConfig ( varname, newvalue )
	if noqvartable[varname] then
		noqvartable[varname] = newvalue
		return noqvartable[varname]
	else
		et.G_Print("warning, invalid config value for " .. varname .. " Can't set " .. newvalue .. "\n")
		return "null"
	end
end


-- don't get often used vars from noqvartable ...

-- Database managment system to use
dbms	= getConfig("dbms") -- possible values mySQL, SQLite
dbname  = getConfig("dbname")

-- under construction:
mail = getConfig("mail") -- enables mail option, make sure all required libs are available
mailserv = getConfig("mailserv")
mailport = getConfig("mailport")
mailfrom = getConfig("mailfrom")

recordbots = tonumber(getConfig("recordbots")) -- don't write session for bots

color = getConfig("color")
commandprefix = getConfig("commandprefix")
debug = tonumber(getConfig("debug")) -- debug 0/1
usecommands = tonumber(getConfig("usecommands"))

if debug == 1 then
	et.G_Print("************************\n")
	getInfoFromTable(noqvartable)
	et.G_Print("************************\n")
end

xprestore = tonumber(getConfig("xprestore"))
pussyfact = true
-------------------------------------------------------------------------------
-- DOCU of client structur in this script

-- The table "client" is created each time someone connect and will store the current client informations
-- The current fields are:

-- CLIENT STRUCTURE  - we have fields in DB and extra needed for script
-- client["team"] = nil

-- table player
-- client["id"] = nil
-- client["pkey"] = 0
-- client["conname"] = row.conname
-- client["regname"] = row.regname
-- client["netname"] = row.netname
-- client["isBot"] = 0	
-- client["clan"] = 0
-- client["level"] = 0	
-- client["user"] = 0
-- client["password"] = 0
-- client["email"] = 0
-- client["banreason"] = 0 
-- client["bannedby"] = 0 
-- client["banexpire"] = 0 
-- client["mutedreason"] = 0
-- client["mutedby"] = 0
-- client["muteexpire"] = 0
-- client["warnings"] = 0 	
-- client["suspect"] = 0
-- client["regdate"] = 0
-- client["updatedate"] = 0	
-- client["createdate"] = 0	

-- table session
-- client["session"] -- last used or in use session see table session.id // was client["id"] before!			
-- client["ip"] = 0	
-- client["valid "] -- not used in script only written into db if player enters for real 
-- client["start"] = 0		
-- client["end"] = 0  -- not used in script only written into db
-- client["axtime"] = 0
-- client["altime"] = 0
-- client["sptime"] = 0
-- client["lctime"] = 0
-- client["sstime"] = 0
-- client["xp0"] = 0
-- client["xp1"] = 0
-- client["xp2"] = 0
-- client["xp3"] = 0
-- client["xp4"] = 0
-- client["xp5"] = 0
-- client["xp6"] = 0
-- client["xptot"] = 0
-- client["acc"] = 0
-- client["kills"] = 0
-- client["tkills"] = 0
-- client["death"] = 0
-- client["uci"] = 0


--[[ Added Fields during ingame session in slot[clientNum]
slot[clientNum]["victim"] = last victim of clientNum(ID)
slot[clientNum]["killwep"] = meansofdeathbumber
slot[clientNum]["killer"] = last person who killed clientNum(ID)
slot[clientNum]["deadwep"] =  meansfdeathnumber


]]

-- The table "slot" will store a table "client" for each slot on the server
-- slot[_clientNum][level] i.e. to access level 
slot = {}

-- We do this for accessing the table with [][] syntax, dirty but it works
for i=0, et.trap_Cvar_Get( "sv_maxclients" ), 1 do				
	slot[i] = {}	
end
-- command table
commands = {}
-- Shrub uses only 31 Levels. at least wiki says TODO: VERIFY
for i=0, 31, 1 do				
	commands[i] = {}
end

--[[ 
--For testing, the !owned known from ETadmin
commands[0]['owned'] = "print ^1Ha^3ha^5ha^3, i owned ^7<PLAYER_LAST_VICTIM_CNAME>^3 with my ^7<PLAYER_LAST_VICTIM_WEAPON>^7!!!"
commands[0]['pants'] = "print ^1No^3no^5noooo^7, i was killed by ^3<PLAYER_LAST_KILLER_CNAME>^7 with a ^3<PLAYER_LAST_KILLER_WEAPON>^7!!!"
commands[0]['parsecmds'] = "$LUA$ parseconf()"
commands[0]['pussyfactor'] = "$LUA$ pussyout(<PART2IDS>)"
commands[0]['spectime'] = "$LUA$ time = slot[_clientNum]['sptime']; et.trap_SendServerCommand(et.EXEC_APPEND , 'print \"..time.. \" seconds in spec')"
commands[0]['axtime'] = "$LUA$ time = slot[_clientNum]['axtime']; et.trap_SendServerCommand(et.EXEC_APPEND , 'print \"..time.. \" seconds in axis')"
commands[0]['altime'] = "$LUA$ time = slot[_clientNum]['altime']; et.trap_SendServerCommand(et.EXEC_APPEND , 'print \"..time.. \" seconds in allies')"
commands[0]['noqban'] = "$LUA$ ban(<PART2ID>)" --TODO The BANFUNCTION...
-- ^      ^      ^ 
-- Array  |      |
--      level    |
-- 			   Part after Prefix
-- Its possible to implement 2 commands with same commandname but different functions for different levels
-- TODO: Method for aquiring the commands -- Either use KWMODS/KMods syntax or/and get them from a speical dbasetable.
]]

-- current map
map = ""
--Gamestate 1 ,2 , 3 = End of Map 
gstate = nil

-- Maxclients TODO: Check if we can init here ... (move up in case of)
maxclients = 0

-------------------------------------------------------------------------------

-- Handle different dbms
if dbms == "mySQL" then
  require "luasql.mysql"
  env = assert( luasql.mysql() )
  con = assert( env:connect(getConfig("dbname"), getConfig("dbuser"), getConfig("dbpassword"), getConfig("dbhostname"), getConfig("dbport")) )
elseif dbms == "SQLite" then
	require "luasql.sqlite3" 
	env = assert( luasql.sqlite3() )
	-- this opens OR creates a sqlite db - if this file is loaded db is created -fix this?
	con = assert( env:connect( getConfig("dbname") ) )
else
  -- stop script
  error("DBMS not supported.")
end

-- Declare some vars we will use in the script
-- We could allocate this each time, but since are used lot of times is better to make them global
-- TODO: cur and res are exactly the same things, so we could save memory using only one of them
cur = {}  -- Will handle the SQL commands returning informations ( es: SELECT )
res = {}  -- Will handle SQL commands without outputs ( es: INSERT )
row = {}  -- To manipulate the outputs of SQL command
row1 = {} -- To manipulate the outputs of SQL command in case we need more than one request

-- mail setup
if mail == "1" then -- does anybody know core.so/core.lua? This lib is needed & I can't find it
	smtp = require("socket.smtp")
	mime = require("mime")
	ltn12 = require("ltn12")
end

team = { "AXIS" , "ALLIES" , "SPECTATOR" }
class = { [0]="SOLDIER" , "MEDIC" , "ENGINEER" , "FIELD OPS" , "COVERT OPS" }

-------------------------------------------------------------------------------
-- ET functions
-------------------------------------------------------------------------------

function et_InitGame( _levelTime, _randomSeed, _restart )
	et.RegisterModname( "NOQ version " .. version .. " " .. et.FindSelf() )
    initNOQ()
	getDBVersion()
	if usecommands ~= 0 then
		parseconf () --uncomment for automatic commandos -- IRATA: Lets do some ;)
	end
end

function et_ClientConnect( _clientNum, _firstTime, _isBot )
	initClient( _clientNum, _firstTime, _isBot )
	
	local ban = checkBan ( _clientNum )
	
	if ban ~= nil then
		return ban
	end
	-- valid client
	
	checkMute( _clientNum )
	-- server greetings ?
	return nil
end

-- This function is called:
--	- after the connection is over, so when you first join the game world
--
-- Before r3493 also:
--	- when you change team
--	- when you are spectator and switch from "free look mode" to "follow player mode"
-- IRATA: check et_ClientSpawn()
-- TODO/NOTE: Afaik we only need to check if ClientBegin is called once to keep 1.2.7 compatinility
function et_ClientBegin( _clientNum )

	-- TODO Move this functionality in an own function
	
	-- Get the player name if its not set
	if slot[_clientNum]["netname"] == false then
		slot[_clientNum]["netname"] = et.gentity_get( _clientNum ,"pers.netname")
		-- Well, if he has no name, he first connected - so we set his team.
		slot[_clientNum]["team"] = tonumber(et.gentity_get(_clientNum,"sess.sessionTeam"))
		slot[_clientNum]["lastTeamChange"] = (et.trap_Milliseconds() / 1000) -- Hossa! We needa seconds
	end
	
	if databasecheck == 1 then
		-- If we have Dbaccess, then we will create new Playerentry if necessary
		if slot[_clientNum]["new"] == true then
			createNewPlayer ( _clientNum )
		end
	        --- seems to be a leftover from old teamcode, verify!! TODO
		if slot[_clientNum]["team"] == false then
			local currentteam = tonumber(et.gentity_get(_clientNum,"sess.sessionTeam"))
			if currentteam ~= 3 then
				slot[_clientNum]["team"] = currentteam
			end
		else
			slot[_clientNum]["team"] = tonumber(et.gentity_get(_clientNum,"sess.sessionTeam"))
		end
		
		-- if we have xprestore, we need to restore now!
		if slot[_clientNum]["setxp"] == true then
			-- But only, if xprestore is on!
			if xprestore == 1 then
				updatePlayerXP( _clientNum )
			end
			slot[_clientNum]["setxp"] = nil
		end
	end -- end databasecheck
	
	updateTeam(_clientNum)
end


-- Possible values are :
--	- slot[_clientNum].team == nil -> the player connected and disconnected without join the gameworld = not-valid session
--	- slot[_clientNum].gstate = 0 and gstate = 0 -> we have to update playing time and store all the player infos = valid session
--	- slot[_clientNum].gstate = 1 or 2 and gstate = 1 or 2 -> player connected during warmup and disconnected during warmup = store only start and end time + valid session
--	- slot[_clientNum].gstate = 3 and gstate = 3 -> player connected during intermission and disconnected during intermission = store only start and end time + valid session
--	- slot[_clientNum].gstate = 0 and gstate = 3 -> we have to store all the player infos = valid session

function et_ClientDisconnect( _clientNum )
	if databasecheck == 1 then
		local endtime = timehandle ('N');
		-- TODO : check if this works. Is the output from 'D' option in the needed format for the database?
		local timediff = timehandle('D','N', slot[_clientNum]["start"])
		
		WriteClientDisconnect( _clientNum , endtime, timediff )
	end
	slot[_clientNum] = {}
end

function et_ClientCommand (_clientNum, _command)
	local arg0 = string.lower(et.trap_Argv(0))
	--local arg1 = string.lower(et.trap_Argv(1))
	--local arg2 = string.lower(et.trap_Argv(2))

	-- switch to disable the !commands 
	if usecommands ~= 0 then
		if arg0 == "say" and string.sub( string.lower(et.trap_Argv(1)), 1,1) == commandprefix then
			gotCmd( _clientNum, _command , false)
		end
		
		if arg0 == "vsay" and string.sub( string.lower(et.trap_Argv(2)) , 1, 1) == commandprefix then
			gotCmd ( _clientNum, _command, true)
		end
	end

	-- register command
	if arg0 == "register" then
		local arg1 = string.lower(et.trap_Argv(1)) -- username
		local arg2 = string.lower(et.trap_Argv(2)) -- password
		local name = string.gsub(arg1,"\'", "\\\'")
		if arg1 ~= "" and arg2 ~= "" then
			slot[_clientNum]["user"] = name 
			res = assert (con:execute("UPDATE player SET user='" .. name .."', password='MD5("..arg2..")' WHERE pkey='"..slot[_clientNum]["pkey"].."'"))
			et.trap_SendServerCommand( _clientNum, "print \"^3Successfully registered. To reset password just re-register. \n\"" ) 
			return 1
		else
			et.trap_SendServerCommand( _clientNum, "print \"^3Syntax for the register Command: /register username password  \n\"" ) 
			et.trap_SendServerCommand( _clientNum, "print \"^3Username is your desired username (for web & offlinemessages)  \n\"" )
			et.trap_SendServerCommand( _clientNum, "print \"^3Password will be your password for your webaccess  \n\"" ) 
			return 1
		end

	end
end

function et_ShutdownGame( _restart )
	if databasecheck == 1 then
		-- We write only the informations from a session that gone till intermission end
		if tonumber(et.trap_Cvar_Get( "gamestate" )) == -1 then
			-- This is when the map ends: we have to close all opened sessions
			-- Cycle between all possible clients
			
			local endgametime = timehandle('N');
			
			for i=0, maxclients, 1 do
				if et.gentity_get(i,"classname") == "player" then
	
     				-- TODO : check if this works. Is the output from 'D' option in the needed format for the database?
					local timediff = timehandle('D',endgametime,slot[i]["start"])

					WriteClientDisconnect( i , endgametime, timediff )

					slot[i] = nil
				end
			end

			con:close()
			env:close()
		end
	end
end

function et_RunFrame( _levelTime )
	-- TODO: is this what we want? I suppose yes...	
    -- This check works only once, when the intermission start: here we have to close sptime, axtime and altime
	-- For all players in the LUA table "slot"
	if ( gstate == 0 ) and ( tonumber(et.trap_Cvar_Get( "gamestate" )) == 3 ) then
		local now = timehandle()			

		for i=0, maxclients, 1 do
			if et.gentity_get(i,"classname") == "player" then
				-- @Ilduca note: client["team"] is set to false somewhere in this code
				if slot[i]["team"] ~= -1 then
					closeTeam ( i )
				end
			end	
		end

		gstate = tonumber(et.trap_Cvar_Get( "gamestate" ))
	end
end

function et_Obituary( _victim, _killer, _mod )
	-- _killer might be null/nil if done by <world> <borg: no, if world its 1023>
	if _killer ~= nil then return end

		if pussyfact == true then
		-- determine teamkill or not
		if tonumber(et.gentity_get(_killer,"sess.sessionTeam")) == tonumber(et.gentity_get(_victim,"sess.sessionTeam")) then
			-- here it is teamkill
			-- NOTE: teamkill is not counted as a kill, wich means all added here is even stronger in its weight
			if _mod == 15 or _mod == 69 then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 170
			else
			slot[_killer]["pf"] = slot[_killer]["pf"] + 110
			end
			
		else
			-- no teamkill
		
			--Knivekill
			if _mod == 5 or _mod == 65 then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 70
			
			-- PF
			elseif _mod == 15 or _mod == 69 then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 140
			
			-- Flamer
			elseif _mod == 17  then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 115
			
			--poison
			elseif _mod == 61 then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 65
			
			-- goomba
			elseif _mod == 60  then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 60
			
			-- kick
			elseif _mod == 21 then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 40
			
			-- sniper
			elseif _mod == 51 or _mod == 14 or _mod == 46 then
			slot[_killer]["pf"] = slot[_killer]["pf"] + 90
			else
			-- if we count 100 up, nothing changes. at least it should 
			slot[_killer]["pf"] = slot[_killer]["pf"] + 100
			end
		end -- teamkill end
	end -- pussy end
	

	slot[_killer]["victim"] = _victim
	slot[_killer]["killwep"] = meansofdeath[_mod]

	slot[_victim]["killer"] = _killer
	slot[_victim]["deadwep"] = meansofdeath[_mod]
	if debug == 1 then
		et.G_LogPrint ("MOD: ".._victim .. " wurd kill " .._killer .."Index:".. _Deaththingie .."  ;".. meansofdeath[_mod].."\n")
	end
	lastkiller = _killer
end

function et_ConsoleCommand( _command )
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ConsoleCommand - command: " .. _command )
	end
	
	-- noq cmds ...
	if string.lower(et.trap_Argv(0)) == commandprefix.."noq" then  
		if (et.trap_Argc() < 2) then 
			et.G_Print("#sql is used to access the db with common sql commands.\n") 
			et.G_Print("usage: ...")
			return 1 
		end 
	-- noq warn ...	
	elseif string.lower(et.trap_Argv(0)) == commandprefix.."warn" then
		-- try first param to cast as int
		-- if int check if slot .. ban
		-- if not try to get player via part of name ...
		
	end
	-- add more cmds here ...
end

function et_ClientSpawn( _clientNum, _revived )
	-- TODO: check if this works
	-- _revived == 1 means he was revived
	if _revived ~= 1 then
		updateTeam(_clientNum)	
	end
end

-------------------------------------------------------------------------------
-- help functions
-------------------------------------------------------------------------------

-- initClient (Gets DbInfos and checks for Ban and Mute)
-- 	the very first action  
function initClient ( _clientNum, _FirstTime, _isBot)
	-- note: this script should work w/o db connection

	--Get player GUID
	slot[_clientNum]["pkey"] 	= string.upper( et.Info_ValueForKey( et.trap_GetUserinfo( _clientNum ), "cl_guid" ))
	slot[_clientNum]["ip"] 		= et.Info_ValueForKey( et.trap_GetUserinfo( _clientNum ), "ip" )
	slot[_clientNum]["isBot"] 	= _isBot
	slot[_clientNum]["conname"] = et.Info_ValueForKey( et.trap_GetUserinfo( _clientNum ), "name" )
	slot[_clientNum]["netname"] = false
	slot[_clientNum]["victim"] 	= -1
	slot[_clientNum]["killwep"] = -1
	slot[_clientNum]["killer"] 	= -1
	slot[_clientNum]["deadwep"] =  -1
	
	-- session
	-- Get the start connection time
	slot[_clientNum]["start"] 	= timehandle('N')
	slot[_clientNum]["team"] 	= false -- set the team on client begin (don't use nil here, as it deletes the index!)
	slot[_clientNum]["axtime"] 	= 0
	slot[_clientNum]["altime"] 	= 0
	slot[_clientNum]["sptime"] 	= 0
	slot[_clientNum]["lctime"] 	= 0
	slot[_clientNum]["acc"] 	= 0
	slot[_clientNum]["kills"] 	= 0
	slot[_clientNum]["tkills"] 	= 0
	slot[_clientNum]["death"] 	= 0
	slot[_clientNum]["uci"] 	= 0
	slot[_clientNum]["pf"]		= 0
					
	if debug == 1 then
		et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT CLIENT \n \"" )
	end
	
	if databasecheck == 1 then
		if debug == 1 then
			et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT DATABASECHECK EXEC \n \"" )
		end
		
		updatePlayerInfo(_clientNum)
		

			slot[_clientNum]["setxp"] = true
			slot[_clientNum]["xpset"] = false
		
		return nil
	--[[ Dont bother for first time or not, we need to update anyway....
		
		-- It's the first time the players connects ( = it's not a reconnection )
		if _FirstTime == 1 then
			if debug == 1 then
				et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT FIRSTIME GOOD \n \"" )
			end
	
			updatePlayerInfo(_clientNum)
			
			return nil
			
		end  -- end of firsttime

		-- Its a reconnect! But maybe we lost the Data due to a Change between Warmup and normal Game.
		-- lets check that.
		if slot[_clientNum]["xptot"] == "ignore" then
	
			if debug == 1 then
				et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT NEW DUE TO MISSING DATA \n \"" )
			end
			
			updatePlayerInfo(_clientNum)
		
		end
				
		if debug == 1 then
			et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT CLIENT RECONNECT! \n \"" )
		end
		]]
	--else -- end databasecheck
	
		--[[We just do nothing. What should we do anyway?
		-- set values we get from db otherwise
		slot[_clientNum]["xp0"] = 0
		slot[_clientNum]["xp1"] = 0
		slot[_clientNum]["xp2"] = 0
		slot[_clientNum]["xp3"] = 0
		slot[_clientNum]["xp4"] = 0
		slot[_clientNum]["xp5"] = 0
		slot[_clientNum]["xp6"] = 0
		slot[_clientNum]["xptot"] = 0
		-- fixme add all
		]]
		
	end
	
	if debug == 1 then		
		et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT CLIENT NO DATABASE INTERACTION \n \"" )
	end
	
    return nil

end


-- Updates the Playerinformation out of the Database (IF POSSIBLE!)
function updatePlayerInfo ( _clientNum )

	--Search the GUID in the database ( GUID is UNIQUE, so we just have 1 result, stop searching when we have it )
	cur = assert (con:execute("SELECT * FROM player WHERE pkey='".. slot[_clientNum]["pkey"] .."' LIMIT 1"))
	row = cur:fetch ({}, "a")
	cur:close()
	
	-- This player is already present in the database
	if row then
		if debug == 1 then
			et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT CLIENT ROW EXISTS \n \"" )
		end
		-- Start to collect related information for this player id
		-- player
		slot[_clientNum]["id"] = row.id
		slot[_clientNum]["regname"] = row.regname
		slot[_clientNum]["conname"] = row.conname
		--slot[_clientNum]["netname"] = row.netname --we dont set netname to a invalid old databaseentry
		slot[_clientNum]["clan"] = row.clan	
		slot[_clientNum]["user"] = row.user -- only for admin info
		slot[_clientNum]["banreason"] = row.banreason
		slot[_clientNum]["bannedby"] = row.bannedby
		slot[_clientNum]["banexpire"] = row.banexpire
		slot[_clientNum]["mutedreason"] = row.mutedreason
		slot[_clientNum]["mutedby"] = row.mutedby
		slot[_clientNum]["muteexpire"] = row.muteexpire
		slot[_clientNum]["warnings"] = row.warnings
		slot[_clientNum]["suspect"] = row.suspect
		slot[_clientNum]["regdate"] = row.regdate
		slot[_clientNum]["createdate"] = row.createdate -- first seen
		slot[_clientNum]["level"] = et.G_shrubbot_level( _clientNum ) --TODO: REAL LEVEL/Who is more important, shrub or database?

				
		--Perhaps put into updatePlayerXP
		slot[_clientNum]["xp0"] = row.xp0
		slot[_clientNum]["xp1"] = row.xp1
		slot[_clientNum]["xp2"] = row.xp2
		slot[_clientNum]["xp3"] = row.xp3
		slot[_clientNum]["xp4"] = row.xp4
		slot[_clientNum]["xp5"] = row.xp5
		slot[_clientNum]["xp6"] = row.xp6
		slot[_clientNum]["xptot"] = row.xptot
			
		if debug == 1 then
			et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT CLIENT FROM ROW GOOD\n \"" )
		end
	else	
		if debug == 1 then
			et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"LUA: INIT CLIENT NO ROW -> NEW \n \"" )
		end
		-- We just don't do anything, we let him go to Clientbegin and say hes new
		slot[_clientNum]["new"] = true
	end

end


-- Update a players xp from the values in his !TABLE!
-- just a g_xp_setfunction for all values
function  updatePlayerXP( _clientNum )

	et.G_XP_Set ( _clientNum , slot[_clientNum]["xp0"], 0, 0 ) -- battle
	et.G_XP_Set ( _clientNum , slot[_clientNum]["xp1"], 1, 0 ) -- engi
	et.G_XP_Set ( _clientNum , slot[_clientNum]["xp2"], 2, 0 ) -- medic
	et.G_XP_Set ( _clientNum , slot[_clientNum]["xp3"], 3, 0 ) -- signals
	et.G_XP_Set ( _clientNum , slot[_clientNum]["xp4"], 4, 0 ) -- light
	et.G_XP_Set ( _clientNum , slot[_clientNum]["xp5"], 5, 0 ) -- heavy
	et.G_XP_Set ( _clientNum , slot[_clientNum]["xp6"], 6, 0 ) -- covert
	slot[_clientNum]["xpset"] = true

end


-- Check if player is banned 
function checkBan ( _clientNum )
	-- TODO Implement consequences!
	if slot[_clientNum]["bannedby"] ~= "" then
		if  slot[_clientNum]["banreason"] ~= "" then
			if  slot[_clientNum]["banexpire"] ~= "1000-01-01 00:00:00" then
				-- TODO check for expired ban
				return "You are banned by "..slot[_clientNum]["bannedby"].." until "..slot[_clientNum]["banexpire"]..". Reason: "..slot[_clientNum]["banreason"]
			else
				return "You are permanently banned by "..slot[_clientNum]["bannedby"]..". Reason: "..slot[_clientNum]["banreason"]
			end
		else
			if  slot[_clientNum]["banexpire"] then
				return "You are banned by "..slot[_clientNum]["bannedby"].." until "..slot[_clientNum]["banexpire"]
			else
				return "You are permanently banned by "..slot[_clientNum]["bannedby"]
			end
		end
	end
	return nil

end


-- Check if player is muted
function checkMute ( _clientNum )
	-- TODO give hint and mute player ...
	if slot[_clientNum]["mutedby"] then
		if slot[_clientNum]["mutedreason"] then
			if slot[_clientNum]["muteexpire"] then
				--return "You are muted by "..slot[_clientNum][mutedby].." until "..row.muteexpire..". Reason: "..slot[_clientNum][mutedreason]
			else
				--return "You are permanently muted by "..slot[_clientNum][mutedby]..". Reason: "..slot[_clientNum][mutedreason]
			end
		else
			if slot[_clientNum]["muteexpire"] then
				-- return "You are muted by "..slot[_clientNum][mutedby].." until "..slot[_clientNum][muteexpire}
			else
				-- return "You are permanently muted by "..slot[_clientNum][mutedby]
			end
		end
	end
	
	return nil
	
end


--Create a new Player: write to Database, set Xp 0
--maybe could also be used to reset Player, as pkey is unique
function createNewPlayer ( _clientNum )
	local name = string.gsub(slot[_clientNum]["netname"],"\'", "\\\'")
	local conname = string.gsub(slot[_clientNum]["conname"],"\'", "\\\'")

	-- This player is a new one: create a new database entry with our Infos
	res = assert (con:execute("INSERT INTO player (pkey, isBot, netname, updatedate, createdate, conname) VALUES ('"
		..slot[_clientNum]["pkey"].."', "
		..slot[_clientNum]["isBot"]..", '"
		..name.."', '"
		..slot[_clientNum]["start"] .."', '"
		..slot[_clientNum]["start"] .."', '"
		..conname.."')"))
		
	-- Jep, these values are correct, as he is new!
	slot[_clientNum]["xp0"] = 0
	slot[_clientNum]["xp1"] = 0
	slot[_clientNum]["xp2"] = 0
	slot[_clientNum]["xp3"] = 0
	slot[_clientNum]["xp4"] = 0
	slot[_clientNum]["xp5"] = 0
	slot[_clientNum]["xp6"] = 0
	slot[_clientNum]["xptot"] = 0
	slot[_clientNum]["suspect"] = 0
	slot[_clientNum]["new"] = nil
	
	-- And now we will get all our default values 
	updatePlayerInfo (_clientNum)

end


-- Function to handle times
-- TODO : check if the time returned with option 'D' is in the right format we need
-- NOTE ABOUT TIME IN LUA: the function os.difftime works only with arguments passed in seconds, so
--						   before pass anything to that functions we have to convert the date in seconds
--						   with the function os.time, then convert back the result with os.date
function timehandle ( op, time1, time2)
	-- The os.* functions needs a shell to be linked and accessible by the process running LUA
	-- TODO : this check should be moved at script start because os.* functions are really
	-- 		  "popular" so we may use them in other functions too
	if os.execute() == 0 then
		error("This process needs an active shell to be executed.")
	end

	local timed = nil

	if op == 'N' then
		-- N -> return current date ( NOW )
		local timed = os.date("%Y-%m-%d %X")
		if timed then
			return timed
		end
		return nil
	elseif op == 'D' then
	    -- D -> compute time difference time1-time2
	    if time1==nil or time2==nil then
	        error("You must to input 2 arguments to use the 'D' option.")
	    end

	    -- Check if time1 is 'N' ( NOW )
	    if time1 == 'N' then
	    	-- Check if time2 is in the right format
	    	if string.len(time2) == 19 then
	    		timed = os.difftime(os.time(),os.time{year=tonumber(string.sub(time2,1,4)), month=tonumber(string.sub(time2,6,7)), day=tonumber(string.sub(time2,9,10)), hour=tonumber(string.sub(time2,12,13)), min=tonumber(string.sub(time2,15,16)), sec=tonumber(string.sub(time2,18,19))})
			end
	    end
	    -- Check if time1 and time2 are in the right format
	    if string.len(time1) == 19 and string.len(time2) == 19 then
      		timed = os.difftime(os.time{year=tonumber(string.sub(time1,1,4)), month=tonumber(string.sub(time1,6,7)), day=tonumber(string.sub(time1,9,10)), hour=tonumber(string.sub(time1,12,13)), min=tonumber(string.sub(time1,15,16)), sec=tonumber(string.sub(time1,18,19))},os.time{year=tonumber(string.sub(time2,1,4)), month=tonumber(string.sub(time2,6,7)), day=tonumber(string.sub(time2,9,10)), hour=tonumber(string.sub(time2,12,13)), min=tonumber(string.sub(time2,15,16)), sec=tonumber(string.sub(time2,18,19))})
	    end
	end

 	if timed then
		if timed < 60 then
		    if timed < 10 then
		        return string.format("00:00:0%d",timed)
		    else
	    		return string.format("00:00:%d",timed)
	    	end
	    end

	    local seconds = timed % 60
	    local minutes = (( timed - seconds ) / 60 )

	    if minutes < 60 then
	        if minutes < 10 and seconds < 10 then
	    		return string.format("00:0%d:0%d",minutes,seconds)
	    	elseif minutes < 10 then
	    	    return string.format("00:0%d:%d",minutes,seconds)
			elseif seconds < 10 then
			    return string.format("00:%d:0%d",minutes,seconds)
			else
			    return string.format("00:%d:%d",minutes,seconds)
			end
	    end

	    minutes = minutes % 60
	    local houres = ((( timed - seconds ) / 60 ) - minutes ) / 60

		if minutes < 10 and seconds < 10 then
			return string.format("%d:0%d:0%d",houres,minutes,seconds)
		elseif minutes < 10 then
	    	return string.format("%d:0%d:%d",houres,minutes,seconds)
		elseif seconds < 10 then
			return string.format("%d:%d:0%d",houres,minutes,seconds)
		else
			return string.format("%d:%d:%d",houres,minutes,seconds)
		end
	end

	return nil
end


-- Dumps Client into Dbase at Disconnect or end of round
-- This function really dumps everything!
function WriteClientDisconnect( _clientNum, _now, _timediff )
	if slot[_clientNum]["team"] == false then
		slot[_clientNum]["uci"] = et.gentity_get( _clientNum ,"sess.uci")
	
		-- In this case the player never entered the game world, he disconnected during connection time
		query = "INSERT INTO session (pkey, slot, map, ip, valid, start, end, sstime, uci) VALUES ('"
			..slot[_clientNum]["pkey"].."', '"
			.._clientNum.."', '"
			..map.."', '"
			..slot[_clientNum]["ip"].."', '" 
			.."1".."', '"
			..slot[_clientNum]["start"].."', '"
			..timehandle('N').."' , '"
            -- TODO : check if this works. Is the output from 'D' option in the needed format for the database?
			..timehandle('D','N',slot[_clientNum]["start"]).."' , '"
			.. slot[_clientNum]["uci"].."')"

		--[[query = "INSERT INTO session (pkey, slot, map, ip, valid, uci) VALUES ('"
			..slot[_clientNum]["pkey"].."', '"
			.._clientNum.."', '"
			..map.."', '"
			..slot[_clientNum]["ip"].."', '" 
			.."1".."', '"
			.. slot[_clientNum]["uci"].."')"
		]]--
			
		if debug == 1 then
			et.trap_SendConsoleCommand(et.EXEC_NOW , "cpm \"\n ".. query .."<<\"" )
		end
		res = assert (con:execute( query ))
		et.G_LogPrint( "Noq: saved player ".._clientNum.." to Database\n" ) 
	else
		-- The player disconnected during a valid game session. We have to close his playing time
		-- If "team" == -1 means we already closed the team time, so we don't have to do it again
		-- This is needed to stop team time at map end, when debriefing starts
		if slot[_clientNum]["team"] ~= -1 then
			closeTeam ( _clientNum )
		end
						
		-- Write to session if player was in game
		saveSession ( _clientNum )
		savePlayer ( _clientNum )
		 et.G_LogPrint( "Noq: saved player and session ".._clientNum.." to Database\n" )

	end		
end

-- Dumps into player table
--  all data, call if you changed something important to secure it in database
function savePlayer ( _clientNum )

	slot[_clientNum]["ip"] = et.Info_ValueForKey( et.trap_GetUserinfo( _clientNum ), "ip" )
	if slot[_clientNum]["ip"] == "localhost" then
		-- He is a bot, mark it's ip as "localhost"
		slot[_clientNum]["ip"] = "127.0.0.1"
	else
		s,e,slot[_clientNum]["ip"] = string.find(slot[_clientNum]["ip"],"(%d+%.%d+%.%d+%.%d+)")
	end 
	
	  if slot[_clientNum]["xpset"] == false and xprestore == 1 then

        et.G_LogPrint("NOQ: ERROR while setting xp in database: XP not properly restored!\n")
        return
        end

	
		local battle	=	et.gentity_get(_clientNum,"sess.skillpoints",0) 
		local engi	=	et.gentity_get(_clientNum,"sess.skillpoints",1)
		local medic	=	et.gentity_get(_clientNum,"sess.skillpoints",2)
		local signals	=	et.gentity_get(_clientNum,"sess.skillpoints",3)
		local light	=	et.gentity_get(_clientNum,"sess.skillpoints",4)
		local heavy	=	et.gentity_get(_clientNum,"sess.skillpoints",5)
		local covert	=	et.gentity_get(_clientNum,"sess.skillpoints",6)
	
		-- We also write to player, for our actual data
		-- TODO
		-- slot[_clientNum]["user"] 
		-- slot[_clientNum]["password"] 
		-- slot[_clientNum]["email"] 
		-- slot[_clientNum]["netname"] ????

		 local name = string.gsub(slot[_clientNum]["netname"],"\'", "\\\'")

                -- FIXME getting attempt to concatenate field 'muteexpire' (a nil value) (sqlite
                res = assert (con:execute("UPDATE player SET clan='".. slot[_clientNum]["clan"] .."',           \
                                                     netname='".. name  .."',\
													 xp0='".. battle .."', 	\
													 xp1='".. engi .."', 	\
													 xp2='".. medic .."', 	\
													 xp3='".. signals .."',	\
													 xp4='".. light .."', 	\
													 xp5='".. heavy .."',	\
													 xp6='".. covert .."',	\
													 xptot='".. battle + engi + medic + signals + light + heavy + covert .. "',\
													 level='".. slot[_clientNum]["level"] .."',				\
													 banreason='".. slot[_clientNum]["banreason"]  .."',	\
													 bannedby='".. slot[_clientNum]["bannedby"]  .."',		\
													 mutedreason='".. slot[_clientNum]["mutedreason"] .."',	\
													 mutedby='".. slot[_clientNum]["mutedby"] .."',			\
													 warnings='".. slot[_clientNum]["warnings"] .."',		\
													 suspect='".. slot[_clientNum]["suspect"] .."'			\
													 WHERE pkey='".. slot[_clientNum]["pkey"] .."'"))

--													 muteexpire='".. slot[_clientNum]["muteexpire"] .."',	\
--													 banexpire='".. slot[_clientNum]["banexpire"] .."',		\
end

-- Dumps the sessiondata
-- should only be used on sessionend to not falsify sessions
function saveSession ( _clientNum )

	if recordbots == 0 and slot[_clientNum]["isBot"] == 1 then
		 et.G_LogPrint( "Noq: not saved bot session ".._clientNum.." to Database" )

		return
	end

	-- TODO: fixme sqlite only ?
	slot[_clientNum]["sptime"] = "0"

	slot[_clientNum]["uci"] = et.gentity_get( _clientNum ,"sess.uci")
	slot[_clientNum]["ip"] = et.Info_ValueForKey( et.trap_GetUserinfo( _clientNum ), "ip" )
	
	if slot[_clientNum]["ip"] == "localhost" then
		-- He is a bot, mark it's ip as "localhost"
		slot[_clientNum]["ip"] = "127.0.0.1"
	else
		s,e,slot[_clientNum]["ip"] = string.find(slot[_clientNum]["ip"],"(%d+%.%d+%.%d+%.%d+)")
	end 

		-- If player was ingame, we really should save his XP to!
		local battle	=	et.gentity_get(_clientNum,"sess.skillpoints",0) 
		local engi		=	et.gentity_get(_clientNum,"sess.skillpoints",1)
		local medic		=	et.gentity_get(_clientNum,"sess.skillpoints",2)
		local signals	=	et.gentity_get(_clientNum,"sess.skillpoints",3)
		local light		=	et.gentity_get(_clientNum,"sess.skillpoints",4)
		local heavy		=	et.gentity_get(_clientNum,"sess.skillpoints",5)
		local covert	=	et.gentity_get(_clientNum,"sess.skillpoints",6)
			
		local name = string.gsub(slot[_clientNum]["netname"],"\'", "\\\'")
		local deaths = et.gentity_get(_clientNum,"sess.deaths")
		local kills = et.gentity_get(_clientNum,"sess.kills")
		local tkills = et.gentity_get(_clientNum,"sess.team_kills")
		-- Write to session if player was in game
		

		local sessquery = "INSERT INTO session (pkey, slot, map, ip, netname, valid, start, end, sstime, axtime, altime, sptime, xp0, xp1, xp2, xp3, xp4, xp5, xp6, xptot, acc, kills, tkills, death) VALUES ('"
			..slot[_clientNum]["pkey"].."', '"
			.._clientNum .."', '"
			..map.."', '"
			..slot[_clientNum]["ip"].."', '"
			.. name .."', "
			.."1" ..", '"
			..slot[_clientNum]["start"].."', '"
			..timehandle('N').. "',  '"
            -- TODO : check if this works. Is the output from 'D' option in the needed format for the database?
			..timehandle('D','N',slot[_clientNum]["start"]).."' , '"
			..slot[_clientNum]["axtime"].."', '"
			..slot[_clientNum]["altime"].."', '"
			..slot[_clientNum]["sptime"].."', '"
			..battle.."', '"
			..engi.."', '"
			..medic.."', '"
			..signals.."', '"
			..light.."', '"
			..heavy.."', '"
			..covert.."','"
			..(battle + engi + medic + signals + light + heavy + covert ).."','"
			.."0".."', '"
			..kills.."', '"
			..tkills.."', '"
			..deaths.. "' )"
		res = assert (con:execute(sessquery))
		--et.G_LogPrint( "\n\n".. sessquery .. "\n\n" ) 
end

-- From KWmod: Custom Commands Helper functions
function gotCmd( _clientNum, _command, _vsay)

	local arg0 = string.lower(et.trap_Argv(0))
	local arg1 = string.lower(et.trap_Argv(1))
	local arg2 = string.lower(et.trap_Argv(2))

	local cmd
	local lvl = tonumber(et.G_shrubbot_level( _clientNum ) )
	local realcmd

	if vsay == false then
		cmd = string.sub(arg1 ,2)
		argw = arg2
	else 
		cmd = string.sub(arg2 ,2)
		argw = string.lower(et.trap_Argv(3))
	end

	-- thats a hack to clearly get the second parameter.
	-- NQ-Gui chat uses cvars to pass the say-content
	if string.find(cmd, " ") ~= nil then
	local t = justWords(cmd)
		cmd = t[1]
		argw = t[2]
	end

	-- We search trough the commands-array for a suitable command
	for i=lvl, 0, -1 do
		if commands[i][cmd] ~= nil then
			execCmd(_clientNum, commands[i][cmd], argw)
		break
		end
	end

end

--Splits a string into a table on occurence of Whitespaces
function justWords( _str )
	local t = {}
	local function helper(word) table.insert(t, word) return "" end
	if not _str:gsub("^%s+", helper):find"%S" then return t end
end

-- The real work to exec a cmd is done here
function execCmd (clientNum , _cmd, _argw)
		local str = _cmd
		--[[
		local lastkilled = et.gentity_get(clientNum, "pers.lastkilled_client")
		local lastkiller = et.gentity_get(clientNum, "pers.lastkiller_client")
		]]
		local lastkilled = slot[clientNum]["victim"]
		local lastkiller = slot[clientNum]["killer"] 
		
		if lastkilled == 1023 then
			lastkilled = clientNum
			slot[clientNum]["killwep"] = "World"
		end
		if lastkiller == 1023 then 
			lastkiller = clientNum
			slot[clientNum]["deadwep"] = "World"
		end
		
		local otherplayer = _argw
		--if vsay == false then
		--otherplayer = string.lower(et.trap_Argv(2))
		--else
		--otherplayer = string.lower(et.trap_Argv(3))
		--end		
		
		otherplayer = getPlayerId(otherplayer)
		--remember! if nobody else is meant, is falls back to you! TODO: Implement other way	
		if otherplayer == nil then otherplayer = clientNum end  
		

		local t = tonumber(et.gentity_get(clientNum,"sess.sessionTeam"))
		local c = tonumber(et.gentity_get(clientNum,"sess.latchPlayerType"))
		local str = string.gsub(str, "<CLIENT_ID>", clientNum)
		local str = string.gsub(str, "<GUID>", et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" ))
		local str = string.gsub(str, "<COLOR_PLAYER>", et.gentity_get(clientNum,"pers.netname"))
		local str = string.gsub(str, "<ADMINLEVEL>", slot[clientnum]["level"] )
		local str = string.gsub(str, "<PLAYER>", et.Q_CleanStr(et.gentity_get(clientNum,"pers.netname")))
		local str = string.gsub(str, "<PLAYER_CLASS>", class[c])
		local str = string.gsub(str, "<PLAYER_TEAM>", team[t])
		local str = string.gsub(str, "<PARAMETER>", et.ConcatArgs( 2 ) )
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_ID>", lastkiller )
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_NAME>", et.Q_CleanStr( et.gentity_get(lastkiller, "pers.netname") ))
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_CNAME>", et.gentity_get(lastkiller, "pers.netname"))
		local str = string.gsub(str, "<PLAYER_LAST_KILLER_WEAPON>", slot[clientNum]["deadwep"])
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_ID>", lastkilled )
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_NAME>", et.Q_CleanStr( et.gentity_get(lastkilled, "pers.netname")))
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_CNAME>", et.gentity_get(lastkilled, "pers.netname"))
		local str = string.gsub(str, "<PLAYER_LAST_VICTIM_WEAPON>", slot[clientNum]["killwep"])
		--TODO
		-- local str = string.gsub(str, "<PLAYER_LAST_KILL_DISTANCE>", calculate! )
		
		--TODO Implement them
		--  Other possible Variables: <CVAR_XXX> <????>
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
		
		
		if otherplayer == clientNum then -- "light security" to not ban or kick yourself (use only ids to ban or kick, then its safe)
			str = string.gsub(str, "<PART2PBID>", "65" )
			str = string.gsub(str, "<PART2ID>", "65" ) 
		end
			
		--else
		local t = tonumber(et.gentity_get(otherplayer,"sess.sessionTeam"))
		local c = tonumber(et.gentity_get(otherplayer,"sess.latchPlayerType"))
		str = string.gsub(str, "<PART2_CLASS>", class[c])
		str = string.gsub(str, "<PART2_TEAM>", team[t])
		str = string.gsub(str, "<PART2CNAME>", et.gentity_get(otherplayer, "pers.netname" ))
		str = string.gsub(str, "<PART2CNAME>", et.gentity_get(otherplayer, "pers.netname" ))
		str = string.gsub(str, "<PART2ID>", otherplayer )
		str = string.gsub(str, "<PART2PBID>", otherplayer + 1 ) 
		str = string.gsub(str, "<PART2GUID>", et.Info_ValueForKey( et.trap_GetUserinfo( otherplayer ), "cl_guid" ))
		str = string.gsub(str, "<PART2LEVEL>", et.G_shrubbot_level (otherplayer) )
		str = string.gsub(str, "<PART2NAME>", et.Q_CleanStr(et.gentity_get(otherplayer,"pers.netname")))
		--end
		
		-- This allows execution of lua-code in a normal Command. 
		if string.sub(str, 1,5) == "$LUA$" then
			local f = loadstring(string.sub(str,6))
			f()
			return	
		end
		
		-- This allows Shell commands. WARNING: As long as lua waits for the command to complete, NQ+ET arent responding to anything, they are HALTED!
		-- Response of the Script is piped into NQ-Console(via print, so no commands)
		if string.sub(str, 1,5) == "$SHL$" then
			execthis = io.popen(string.sub(str,6))
			readshit = execthis:read("*a")
			execthis:close()
			readshit = string.gsub(readshit, "\n","\"\nqsay \"")
			et.trap_SendConsoleCommand(et.EXEC_APPEND, "qsay \" ".. readshit .. " \"")
			return	
		end
		-- well, at the end we send the command to the console
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "".. str .. "\n " )

end


function getPlayerId( _name )
    -- if it's nil, return nil
    if (_name == "") then
        return nil
    end

    -- if it's a number, interpret as slot number
    local clientnum = tonumber(_name)
    if clientnum then
        if (clientnum <= tonumber(et.trap_Cvar_Get("sv_maxclients"))) and et.gentity_get(clientnum,"inuse") then
            return clientnum
        else
            return nil
        end
    end

	local test = et.ClientNumberFromString( _name ) -- Cool NQ function!
	if test == -1 then
    	return nil
	else
		return test
	end
	
end


-------------------------------------------------------------------------------
-- Parse commandos from commandofile function
-------------------------------------------------------------------------------
function parseconf ()
	local	datei = io.open ( (scriptpath .. "noq_commands.cfg" ) ,"r") 
	
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
	et.G_LogPrint("Parsed " ..nmr .." commands from "..nmr2.." lines. \n")
end


-------------------------------------------------------------------------------
-- Init NOQ function
-------------------------------------------------------------------------------
function initNOQ ()

	-- get all we need at gamestart from game
	gstate = tonumber(et.trap_Cvar_Get( "gamestate" ))
	map = tostring(et.trap_Cvar_Get("mapname"))
	maxclients = tonumber(et.trap_Cvar_Get("sv_maxclients"))-1 -- add 1 again if used in view    
	timelimit = tonumber(et.trap_Cvar_Get("timelimit")) -- update this on frame (if changed during game?)
	
end

-- Checks for correct DBVersion
-- Disables DBaccess on wrong version!
function getDBVersion ()
	-- Check the database version
	cur = assert (con:execute("SELECT version FROM version ORDER BY id DESC LIMIT 1"))
	row = cur:fetch ({}, "a")
	cur:close()
	
	if row.version == version then
		databasecheck = 1
		et.G_LogPrint("^1Database "..dbname.." is up to date. Script version is ".. version .."\n")
	else
		et.G_LogPrint("^1Database "..dbname.." is not up to date: SQL support disabled! Requested version is ".. version .."\n")
		-- We don't need to keep the connection with the database open
		con:close()
		env:close()
	end
	
end

-------------------------------------------------------------------------------
-- updateTeam of player _clientNum, set times accordingly when the player
-- changes team
-------------------------------------------------------------------------------
function updateTeam(_clientNum )
	local teamTemp = tonumber(et.gentity_get(_clientNum,"sess.sessionTeam"))
	
	if teamTemp ~= tonumber(slot[_clientNum]["team"]) then -- now we have teamchange!!!
		
		if debug == 1 then
			et.SendConsoleCommand(et.EXEC_APPEND, "chat \" TEAMCHANGE: " ..team[tonumber(slot[_clientNum]["team"])] .." to " .. team[teamTemp] .. "  \"  ")
		end
		
		closeTeam ( _clientNum )
		-- Now, we change the teamchangetime & team
		slot[_clientNum]["lastTeamChange"] = (et.trap_Milliseconds() / 1000 )
		slot[_clientNum]["team"] = teamTemp
	end
	
end

-------------------------------------------------------------------------------
-- closeTeam of player _clientNum, close a time session for a player
-------------------------------------------------------------------------------

function closeTeam ( _clientNum )
	if slot[_clientNum]["team"] == 1 then -- axis
		slot[_clientNum]["axtime"] = slot[_clientNum]["axtime"] +( (et.trap_Milliseconds() / 1000) - slot[_clientNum]["lastTeamChange"]  )
	elseif slot[_clientNum]["team"] == 2 then -- allies
		slot[_clientNum]["altime"] = slot[_clientNum]["altime"] +( (et.trap_Milliseconds() / 1000) - slot[_clientNum]["lastTeamChange"]  )
	elseif slot[_clientNum]["team"] == 3 then -- Spec
		slot[_clientNum]["sptime"] = slot[_clientNum]["sptime"] +( (et.trap_Milliseconds() / 1000) - slot[_clientNum]["lastTeamChange"]  )
	end
	
	-- Set the player team to -1 so we know he cannot to change team anymore
	slot[_clientNum]["team"] = -1
end	

-------------------------------------------------------------------------------
--Displays the Pussyfactor for Player _ClientNum
-------------------------------------------------------------------------------
function pussyout ( _clientNum )
	local pf = slot[tonumber(_clientNum)]["pf"]
	local kills = tonumber(et.gentity_get(_clientNum,"sess.kills"))
	local realpf = 1

	if pf == 0 or kills == 0 then
		et.trap_SendServerCommand(-1, "chat \"^1Do some kills first...\"")
		return
	else
		realpf = string.format("%.1f", ( pf / (100 * kills) ) )
	end

	et.trap_SendServerCommand(-1,"chat \""..slot[tonumber(_clientNum)]["netname"].."^3's pussyfactor is at: ".. realpf ..".Higher is worse. \"" ) 
	
	if debug == 1 then
		et.G_LogPrint("NOQ: PUSSY:"..slot[tonumber(_clientNum)]["netname"].." at ".. realpf .."\n")
	end
end


-------------------------------------------------------------------------------
-- mail functions
-------------------------------------------------------------------------------

function sendMail(_to, _subject, _text)
	rcpt = { "<XXX>", "<XXX>" }

	local mesgt =  smtp.message {
	   headers = {
		  to = _to,
		  subject = _subject
	   },
	   body = {
		  preable = "This email contains attachments.",
		  [1] = {
		 body = mime.eol(0, [[
					   "With any luck, we are able to see a Lua-sent
					   email here."
				 ]])
		  },
		  [2] = {
		 headers = {
			["content-type"] = 'text/plain; name="smtp-test.lua"',
			["content-disposition"] = 'attachment; filename="smtp-test.lua"',
			["content-description"] = 'the send script\'s source',
			["content-transfer-encoding"] = "BASE64"
		 },
		 body = ltn12.source.chain(
					   ltn12.source.file(io.open("smtp-test.lua", "r")), 
					   ltn12.filter.chain(
								  mime.encode("base64"),
								  mime.wrap()
							   )
					)
		  },
		  epilogue = "A few words at the end..."
	   }
	}

	r, e = smtp.send {
	   from = mailfrom,
	   rcpt = rcpt, 
	   source = mesgt,
	   server = mailserv,
	   port = mailport
	}

	if (e) then
	   et.G_LogPrint("NOQ: Could not send email: "..e.. "\n")
	end
	
end


