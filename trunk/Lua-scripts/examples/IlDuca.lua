--[[
	NOQ.lua - game manager - basic ideas/features of script:
	- We have an independant cvar (var list) which stores ET & needed LUA vars
	- Player and game structure is prepared to put into db
	- slottable stores GUID
	

	TODO: 
	- Sanity checks if no cfg files are found!
	- html/xml/db history storage at end of map/level	
	- Team balance
	- Spoofing issues
	- Extend player structure
	- Update player table on several 'events' (write stats f.g.)
	- custom commands

	This is done:
	- table persistence
	
	Code examples to add lua vars:
	-- example to add vars	
	-- add_cvar("test5", "si" )

	-- just a test to add some more
	-- for i=1,10000 do add_cvar("lua_var" .. i, "si"..i ) end
	
--]]

--[[ MODIFY--]]
greetingsmessage = "Welcome to server."
luaServerCMD = "lua"
luaClientCMD = "!"

debug = 0

---------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------
--[[ DON'T MODIFY !--]]
module = "NOQ"
version = "V0.05"
sv_maxclients =  tonumber(et.trap_Cvar_Get("sv_maxclients"))
fs_homepath = et.trap_Cvar_Get("fs_homepath")
fs_basepath = et.trap_Cvar_Get("fs_basepath")
scriptinfo = "^1" .. et.trap_Cvar_Get("fs_game") .. " ^8V" .. et.trap_Cvar_Get("mod_version") .. " ^1 " .. module .. " ^8" .. version

-- All infos of current game (might be of interest to persist at end of map)
matchfilename = fs_homepath .. "/noquarter/lua_nq_match.xml" -- debug
matchtable = { 
	players={}, 
	mapname="", 
	speccount=0, axiscount=0, alliescount=0, 
	axiskills=0, alieskills=0,
	axisdeath=0, alliesdeath=0, 
	starttime=0 
}

-- manage on ClientConnect, ClientDisconnect, ClientSpawn etc
-- example (score is xp per skill - lightweapons ... check!)
-- start of data structure for db
playerfilename = fs_homepath .. "/noquarter/lua_nq_players.dat" -- debug
playertable = {}

-- current slot settings
slotfilename = fs_homepath .. "/noquarter/lua_nq_slots.dat" -- debug
slottable = {}

-- used weapons index
weaponfilename = fs_homepath .. "/noquarter/lua_nq_weapon.cfg"
weapontable = {}

-- used methods/means of death index
modfilename = fs_homepath .. "/noquarter/lua_nq_mod.cfg"
modtable = {}

-- load cvartable from file on init - or db? Files are more common
cvarfilename = fs_homepath .. "/noquarter/lua_nq_cvar.cfg"
cvartable = {}


team  = { [0]="SPECTATOR", "AXIS" , "ALLIES" }
class = { [0]="SOLDIER" , "MEDIC" , "ENGINEER" , "FIELD OPS" , "COVERT OPS" }

---------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------

function et_InitGame( levelTime, randomSeed, restart )
	-- load all the stuff
	initMatch(levelTime, randomSeed, restart)
	initLUAvars()
	initWeapons()
	initMods()
	initSlots()
end


function et_ShutdownGame( restart )
	-- TODO 
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ShutdownGame - restart: " .. restart )
	end
end


function et_ClientConnect( clientNum, firstTime, isBot, reason )
	-- this is called on the very first time a client connects
	if firstTime == 1 then
	  FirstConnect(clientNum, firstTime, isBot, reason)
	end
	-- every connect (also first time - map restarts)
	ClientConnect(clientNum )
end


function et_ClientBegin( clientNum )
	updatePlayer(clientNum)
end


function et_ClientDisconnect( clientNum )
	
	removePlayer(clientNum)
	-- TODO
	-- remove from playertable
	-- update game table
	-- add to history
end


function et_ConsoleCommand()

	local cmd = string.lower(et.trap_Argv(0))
	if cmd == "lua" then
		local cmds = et.trap_Argc()
		-- Check if we have 1 arg ( this means we just typed "lua" )
		if cmds == 1 then
        	et.G_Print("Usage: lua cvar - Get the value of the given cvar.\n" .. 
			"Usage: lua cvar value - Set the value of the given cvar.\n")
        	return 1
        -- Check if we have 2 args ( this means we wanna get the value of a lua cvar )
        elseif cmds == 2 then
        	local cmd1 = string.lower(et.trap_Argv(1)) 
        	local cvarvalue1 = get_cvar( cmd1 )
        	if cvarvalue1 == "null" then
            	et.G_Print("LUA cvar ".. cmd1 .." doesn't exist.\n")
            else
            	et.G_Print("LUA cvar ".. cmd1 .." value is ".. cvarvalue1 .. "\n")
            end
		-- Check if we have more then 2 args ( this means we wanna set the value of a lua cvar )
		else
			local cmd2 = string.lower(et.trap_Argv(1))
			local cvarvalue2 = string.lower(et.trap_Argv(2))
			if set_cvar( cmd2, cvarvalue2 ) == 1 then
				et.G_Print("LUA cvar ".. cmd2 .." now set to ".. cvarvalue2 .. "\n")
			else
 				et.G_Print("LUA cvar ".. cmd2 .." doesn't exist.\n")
 			end
		end	
		return 1
    end
    return 0
end 


-- Different to ETPub!
function et_Obituary( victim, killer, meansOfDeath )

	doKillStat(victim, killer, meansOfDeath)

end

---------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------
-- Helper functions
---------------------------------------------------------------------------------------------

-- cvartable functions

-- Gets cvarvale else null
function get_cvar ( cvarname )

	local value = cvartable[cvarname]
	if value then
	  return value
	else
	  return "null"
	end
end


-- Only sets cvar if known
function set_cvar ( cvarname, cvarvalue )

	-- maybe an idea to use get_cvar and check for null instead a for loop
	local value = cvartable[cvarname]
	if value=="null" then
	  return 0
	else
	  cvartable[cvarname] = cvarvalue
	  return 1
	end
end

-- Adds a cvar 
function add_cvar( cvarname, cvarvalue )
	-- TODO sanity checks, key already set ?
	-- cvartable[ "test3" ] = "this works!"
	cvartable[ cvarname ] = cvarvalue
end


-- Loads all cvars from file
function initLUAvars()
	et.G_Print("Loading lua vars " .. cvarfilename .. "\n" )
	cvartable = table.load( cvarfilename )
	if debug == 1 then
		getInfoFromTable(cvartable)
	end
end


-- Saves cvarstable to file (call after add_cvar)
function saveCvars()
	table.save(  cvartable, cvarfilename )
end


function initMatch( levelTime, randomSeed, restart )
	matchtable.mapname = string.lower(et.trap_Cvar_Get("mapname"))
	matchtable.starttime = levelTime
end


-- weapontable functions

function initWeapons()
	et.G_Print("Loading lua weapons " .. weaponfilename .. "\n" )
	weapontable = table.load( weaponfilename )
	if debug == 1 then	
		getInfoFromTable(weapontable)
	end
end


-- modtable functions

function initMods()
	et.G_Print("Loading lua mods " .. modfilename .. "\n" )
	modtable = table.load( modfilename )
	if debug == 1 then
		getInfoFromTable(modtable)
	end
end

-- slottable functions

function initSlots()
	
	et.G_Print("Initialize slots " .. sv_maxclients .. "\n" )
	for i=0, (sv_maxclients-1) do 
		slottable[i] = "free"
	end
	if debug == 1 then
		getInfoFromTable(slottable)
	end
end

-- general table helpers

function getInfoFromTable( table )
	-- table.sort(cvartable)
	for k,v in pairs(table) do et.G_Print(k .. "=" .. v .. "\n") end
	-- setn not set so empty
	-- et.G_Print("size:" .. table.getn(cvartable) .. "\n")
end


-- general table helpers
function initPlayer( clientNum, firstTime, isBot, reason ) 
 	 -- local playerslots = tonumber(et.trap_Cvar_Get("sv_maxclients"))  
   	-- check if in table
	-- only on firstTime
	local GUID = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" )
	local IP = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "ip" )
	local NETNAME = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "name" )
	local SNAPS = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "snaps" )
	local RATE = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "rate" )
	
	if GUID then
		if debug == 1 then
			et.G_Print("Adding LUA player "  .. GUID .. "\n")
		end

		-- TODO check player
		-- add player, get from file or db by
		playertable[GUID] = {
			clientNum=clientNum, 	-- slot
			clientName=NETNAME,  	-- player name
			team="0",		-- current/last team
			isBot=isBot,		-- bot ?
			reconnects="0", 	-- (during match)
			rate=RATE,		-- rate (needed ?)
			snaps=SNAPS,		-- snaps (needed ?)
			death="0",		-- death count
			kills="0",		-- kill count

			-- store by guid
			ip=IP,			-- client IP /last ip
			password="test",	-- server password			
			score={0,0,0,0,0,0,0},	-- levelsore
			warnings="0",		-- admin warnings received
			adminlevel="0",		-- current player level
			trusted="0",		-- player is trusted
			entryCreated="",	-- first connect date
			lastConnected="",	-- last seen
			entryUpdated=""}	-- guid of player updated this	

		if debug == 1 then		
			table.save(  playertable, playerfilename )
		end
  	else
		et.G_Print("Init player failed " .. clientNum .. "\n" )
	end
end


function removePlayer( client )

	slottable[client] = "free"
	
	local GUID = et.Info_ValueForKey( et.trap_GetUserinfo( client ), "cl_guid" )
	if GUID then
		-- add data to history 
		-- local player = 
		table.save( playertable[GUID] , fs_homepath .. "/noquarter/sessions/" .. GUID..".dat" )
		et.G_Print("Session saved " .. GUID .. "\n" )
		-- remove from playertable (but not for real, so we have real player list ate end of level)
		-- playertable contains all players connected in game
-- et.G_Print("################################## " .. GUID .. "\n" )
		playertable[GUID].clientNum = -1	
  	else
		et.G_Print("Removing player failed " .. client .. "\n" )
	end
end


function updatePlayer( clientNum )

	local GUID = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" )
	local TEAM = et.gentity_get(clientNum,"sess.sessionTeam")
	if debug == 1 then
		et.G_Print("ClientBegin".. GUID .. " team:" .. TEAM .. "\n")
	end
	if GUID then
	-- TODO
		-- playertable[GUID].team = ""..TEAM -- team is null?
  	else
		et.G_Print("Updating player failed " .. clientNum .. "\n" )
	end

	
	-- TODO: set team 
	--if debug == 1 then
	--  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ClientBegin - clientNum: " .. clientNum )
	--end
end


function doKillStat( victim, killer, meansOfDeath )

	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "Obituary - victim: " .. victim .. " killer: " .. killer .. " meansOfDeath: " .. modtable[meansOfDeath] )
	end

	local vteamid = et.gentity_get(victim, "sess.sessionTeam")
  	local kteamid = et.gentity_get(killer, "sess.sessionTeam")
end


---------------------------------------------------------------------------------------------
-- Split et_ClientConnect for real
---------------------------------------------------------------------------------------------

function FirstConnect(clientNum, firstTime, isBot, reason)
	initPlayer(clientNum, firstTime, isBot, reason)
	-- serverinfo & clan message

	--et.trap_SendServerCommand( clientNum ,"cpm \"This server is running " .. scriptinfo)
	--et.trap_SendServerCommand( clientNum ,"cpm \"" .. greetingsmessage)
end


function ClientConnect( clientNum )

	et.trap_SendServerCommand( clientNum ,"cpm \"This server is running " .. scriptinfo .. "\n")
	et.trap_SendServerCommand( clientNum ,"cpm \"" .. greetingsmessage .. "\n")

	local GUID = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" )
	if GUID then
	-- TODO
		slottable[clientNum] = GUID
  	else
		et.G_Print("Player failed " .. clientNum .. "\n" )
	end
	getInfoFromTable(slottable)
end

---------------------------------------------------------------------------------------------
-- Table persistence
---------------------------------------------------------------------------------------------

-- http://lua-users.org/wiki/SaveTableToFile
--[[
   Save Table to File/Stringtable
   Load Table from File/Stringtable
   v 0.94
   
   Lua 5.1 compatible
   
   Userdata and indices of these are not saved
   Functions are saved via string.dump, so make sure it has no upvalues
   References are saved
   ----------------------------------------------------
   table.save( table [, filename] )
   
   Saves a table so it can be called via the table.load function again
   table must a object of type 'table'
   filename is optional, and may be a string representing a filename or true/1
   
   table.save( table )
      on success: returns a string representing the table (stringtable)
      (uses a string as buffer, ideal for smaller tables)
   table.save( table, true or 1 )
      on success: returns a string representing the table (stringtable)
      (uses io.tmpfile() as buffer, ideal for bigger tables)
   table.save( table, "filename" )
      on success: returns 1
      (saves the table to file "filename")
   on failure: returns as second argument an error msg
   ----------------------------------------------------
   table.load( filename or stringtable )
   
   Loads a table that has been saved via the table.save function
   
   on success: returns a previously saved table
   on failure: returns as second argument an error msg
   ----------------------------------------------------
   
   chillcode, http://lua-users.org/wiki/SaveTableToFile
   Licensed under the same terms as Lua itself.
]]--

    -- declare local variables
    --// exportstring( string )
    --// returns a "Lua" portable version of the string
    local function exportstring( s )
       s = string.format( "%q",s )
       -- to replace
       s = string.gsub( s,"\\\n","\\n" )
       s = string.gsub( s,"\r","\\r" )
       s = string.gsub( s,string.char(26),"\"..string.char(26)..\"" )
       return s
    end


    --// The Save Function
    function table.save(  tbl,filename )
       local charS,charE = "   ","\n"
       local file,err
       -- create a pseudo file that writes to a string and return the string
       if not filename then
          file =  { write = function( self,newstr ) self.str = self.str..newstr end, str = "" }
          charS,charE = "",""
       -- write table to tmpfile
       elseif filename == true or filename == 1 then
          charS,charE,file = "","",io.tmpfile()
       -- write table to file
       -- use io.open here rather than io.output, since in windows when clicking on a file opened with io.output will create an error
       else
          file,err = io.open( filename, "w" )
          if err then return _,err end
       end
       -- initiate variables for save procedure
       local tables,lookup = { tbl },{ [tbl] = 1 }
       file:write( "return {"..charE )
       for idx,t in ipairs( tables ) do
          if filename and filename ~= true and filename ~= 1 then
             file:write( "-- Table: {"..idx.."}"..charE )
          end
          file:write( "{"..charE )
          local thandled = {}
          for i,v in ipairs( t ) do
             thandled[i] = true
             -- escape functions and userdata
             if type( v ) ~= "userdata" then
                -- only handle value
                if type( v ) == "table" then
                   if not lookup[v] then
                      table.insert( tables, v )
                      lookup[v] = #tables
                   end
                   file:write( charS.."{"..lookup[v].."},"..charE )
                elseif type( v ) == "function" then
                   file:write( charS.."loadstring("..exportstring(string.dump( v )).."),"..charE )
                else
                   local value =  ( type( v ) == "string" and exportstring( v ) ) or tostring( v )
                   file:write(  charS..value..","..charE )
                end
             end
          end
          for i,v in pairs( t ) do
             -- escape functions and userdata
             if (not thandled[i]) and type( v ) ~= "userdata" then
                -- handle index
                if type( i ) == "table" then
                   if not lookup[i] then
                      table.insert( tables,i )
                      lookup[i] = #tables
                   end
                   file:write( charS.."[{"..lookup[i].."}]=" )
                else
                   local index = ( type( i ) == "string" and "["..exportstring( i ).."]" ) or string.format( "[%d]",i )
                   file:write( charS..index.."=" )
                end
                -- handle value
                if type( v ) == "table" then
                   if not lookup[v] then
                      table.insert( tables,v )
                      lookup[v] = #tables
                   end
                   file:write( "{"..lookup[v].."},"..charE )
                elseif type( v ) == "function" then
                   file:write( "loadstring("..exportstring(string.dump( v )).."),"..charE )
                else
                   local value =  ( type( v ) == "string" and exportstring( v ) ) or tostring( v )
                   file:write( value..","..charE )
                end
             end
          end
          file:write( "},"..charE )
       end
       file:write( "}" )
       -- Return Values
       -- return stringtable from string
       if not filename then
          -- set marker for stringtable
          return file.str.."--|"
       -- return stringttable from file
       elseif filename == true or filename == 1 then
          file:seek ( "set" )
          -- no need to close file, it gets closed and removed automatically
          -- set marker for stringtable
          return file:read( "*a" ).."--|"
       -- close file and return 1
       else
          file:close()
          return 1
       end
    end


    --// The Load Function
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


    function printtbl( t,tab,lookup )
    	local lookup = lookup or { [t] = 1 }
    	local tab = tab or ""
    	for i,v in pairs( t ) do
    		et.G_Print( tab..tostring(i), v )
    		if type(i) == "table" and not lookup[i] then
    			lookup[i] = 1
    			et.G_Print( tab.."Table: i" )
    			printtbl( i,tab.."\t",lookup )
    		end
    		if type(v) == "table" and not lookup[v] then
    			lookup[v] = 1
    			et.G_Print( tab.."Table: v" )
    			printtbl( v,tab.."\t",lookup )
    		end
    	end
    end

