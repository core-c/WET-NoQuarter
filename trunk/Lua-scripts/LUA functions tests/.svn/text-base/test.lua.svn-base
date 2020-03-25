--[[
	Test script for LUA functions

// Callbacks
void G_LuaHook_InitGame(int levelTime, int randomSeed, int restart);
void G_LuaHook_ShutdownGame(int restart);
void G_LuaHook_RunFrame(int levelTime);
qboolean G_LuaHook_ClientConnect(int clientNum, qboolean firstTime, qboolean isBot, char *reason);
void G_LuaHook_ClientDisconnect(int clientNum);
void G_LuaHook_ClientBegin(int clientNum);
void G_LuaHook_ClientUserinfoChanged(int clientNum);
void G_LuaHook_ClientSpawn(int clientNum, qboolean revived, qboolean teamChange, qboolean restoreHealth);
qboolean G_LuaHook_ClientCommand(int clientNum, char *command);
qboolean G_LuaHook_ConsoleCommand(char *command);
qboolean G_LuaHook_UpgradeSkill(int cno, skillType_t skill);
qboolean G_LuaHook_SetPlayerSkill( int cno, skillType_t skill );
void G_LuaHook_Print( char *text );
qboolean G_LuaHook_Obituary( int victim, int killer, int meansOfDeath );

TODO:
Check the vars

--]]

---------------------------------------------------------------------------------------------
-- options of this lua
---------------------------------------------------------------------------------------------

color = "^5" 

-- Debug 0/1
debug = 1
debugRunFrame = 0 -- (popcorn!)
debugPrint = 0 
---------------------------------------------------------------------------------------------

-- test some of the supported lua functions
function test_lua_functions()
       et.trap_Cvar_Set( "bla1", "bla2" )
       et.G_Printf( "sv_hostname [%s]\n", et.trap_Cvar_Get( "sv_hostname" ) )
       et.G_Printf( "configstring 1 [%s] \n", et.trap_GetConfigstring( 1 ) )
       et.trap_SetConfigstring( 4, "yadda test" )
       et.G_Printf( "configstring 4 [%s]\n", et.trap_GetConfigstring( 4 ) )
       et.trap_SendConsoleCommand( et.EXEC_APPEND, "cvarlist *charge*\n" )
       et.trap_SendServerCommand( -1, "print \"Yadda yadda\"" )
       et.G_Printf( "gentity[1022].classname = [%s]", et.gentity_get( 1022, "classname" ) )
       
       --this code lets the server crash/gives segfault?
       --local ent = "test";
	   --local ent = et.G_EntitiesFree();
	   --et.trap_SendConsoleCommand(et.EXEC_APPEND, "chat " .. ent .. " <- Command outputt\n")
	   
end

-- ok
function et_InitGame( levelTime, randomSeed, restart )
	-- debug et_InitGame
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "InitGame - levelTime: " .. levelTime .. " randomSeed: " .. randomSeed .. " restart: " .. restart )
	end
	et.RegisterModname( "test.lua " .. et.FindSelf() )
end


-- ok
function et_ShutdownGame(restart )
	-- debug et_InitGame
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ShutdownGame - restart: " .. restart )
	end
end


-- ok
function et_RunFrame( levelTime )
	-- debug et_InitGame
	if debugRunFrame == 1 then
		if math.mod( levelTime, 1000 ) == 0 then
 			et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "RunFrame - levelTime: " .. levelTime )       
 		end
	end
end


-- ok
function et_ClientCommand( clientNum, command )
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ClientCommand - clientNum: " .. clientNum .. " command: " .. command )
	end
end


-- CHECK
function et_ClientUserinfoChanged( clientNum)
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ClientUserinfoChanged - clientNum: " .. clientNum )
	end
end


-- ok
function et_ClientConnect( clientNum, firstTime, isBot, reason)

	-- debug et_ClientConnect
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ClientConnect - firstTime: " .. firstTime .. " bot: " .. isBot )
	end

end


-- ok
function et_ClientBegin( clientNum )
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ClientBegin - clientNum: " .. clientNum )
	end
end


-- ok
function et_ClientSpawn(clientNum, revived, teamChange, restoreHealth)
	if debug == 1 then
	  et.trap_SendServerCommand( -1 , "cpm \"" .. color .. "ClientSpawn client: " .. clientNum .. " revived: " .. revived .. " teamChange: " .. teamChange .. " restoreHealth: " .. restoreHealth)
	end
end


-- ok
function et_ClientDisconnect( clientNum )
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ClientDisconnect - clientNum: " .. clientNum )
	end
end


-- test
function et_ConsoleCommand( command )
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "ConsoleCommand - command: " .. command )
	end
	et.G_Printf( "et_ConsoleCommand: [%s] [%s]\n", et.trap_Argc(), et.trap_Argv(0) )
   	if et.trap_Argv(0) == "listmods" then
           i = 1
           repeat
                   modname, signature = et.FindMod( i )
                   if modname and signature then
                           et.G_Printf( "vm slot [%d] name [%s] signature [%s]\n", i, modname, signature )
                           et.IPCSend( i, "hello" )
                   end
                   i = i + 1
           until modname == nil or signature == nil
           return 1
   	end
   	return 0
end


-- Different to ETPub!
-- ok
function et_Obituary( victim, killer, meansOfDeath )

	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "Obituary - victim: " .. victim .. " killer: " .. killer .. " meansOfDeath: " .. meansOfDeath )
	end
	
end


-- test
function et_UpgradeSkill(clientNum, skill )
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "UpgradeSkill - clientNum: " .. clientNum .. "skill: " .. skill )
	end
end

-- ok
function et_Print( text )
	if debugPrint == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "Print - text: " .. text)
	end
end

-- ok, but why called so often in game (for bots only?)
function et_SetPlayerSkill(clientNum, skill )
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"cpm \"" .. color .. "SetPlayerSkill - clientNum: " .. clientNum .. " skill: " .. skill )
	end
end

