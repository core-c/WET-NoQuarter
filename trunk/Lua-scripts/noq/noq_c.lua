--  
-- ET Lua SQL console - console.lua (p) 2010 IRATA [*]
--

--
-- Execute SQL via the ET server console like
-- [commandprefix]sql "select * from players" 
-- 

--
-- Enables faster debugging for SQL based Lua scripts, adds some new options and is nice to have ...
--

-- Notes:
-- There are limits by the buffer of ET. Avoid very long statements and don't expect you always get the full result printed.
-- Keep it short

-- TODO
-- test all 

--------------------------------------------------------------------------------
color = "^5"
version = 1
commandprefix = "!"
debug = 0 -- debug 0/1
tablespacer = " " -- use something like " " or "|"
--------------------------------------------------------------------------------
-- db connection data
dbms	= "SQLite" -- possible values "mySQL", "postgreSQL" and "SQLite"
dbname  = "noquarter.sqlite" -- also filename for SQLite file
dbuser  = "myuser" 
dbpassword = "mypassword"
--------------------------------------------------------------------------------
env = nil
con = nil

-- Connect & handle different dbms
if dbms == "mySQL" then
  	require "luasql.mysql"
  	env = assert (luasql.mysql())
  	con = assert (env:connect( dbname, dbuser, dbpassword, dbhostname, dbport ))
elseif dbms == "SQLite" then
	require "luasql.sqlite3" 
	env = assert (luasql.sqlite3())
	con = assert (env:connect( dbname )) -- this opens OR creates a sqlite db - if this Lua is loaded db is created -fix this?
else
	-- stop script
  	error("DBMS not supported.")
end

cur = {}
res = {}
row = {}
--------------------------------------------------------------------------------

function et_InitGame( levelTime, randomSeed, restart )

	et.RegisterModname( "ET SQL console " .. version .. " " .. et.FindSelf() )

	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"chat \"" .. color .. "ET Lua SQL console " .. version )
	end
end

function et_ConsoleCommand( command )
	
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"chat \"" .. color .. "ConsoleCommand - command: " .. command )
	end

	-- TODO should be used by admins only 
	if string.lower(et.trap_Argv(0)) == commandprefix.."sql" then
	
		-- TODO sanity checks - help output
		-- 2 ?
		if (et.trap_Argc() < 1) then 
			et.G_Print(color..commandprefix.."sql is used to access the db with common sql commands.\n" .. "usage: ...\n")
			return 1
		end 
		
		-- we have some cases now - get the sql command ... insert, update
		local cmd = string.lower( string.sub(et.trap_Argv(1), 0 , 6) )

		if debug == 1 then
			et.G_Print(color .. commandprefix.."sql: " .. et.trap_Argv(1) .. "\n")
		end		

		-- ok, does work
		if cmd == "select" then

			cur = assert (con:execute(et.trap_Argv(1)))
			row = cur:fetch ({}, "a")	-- the rows will be indexed by field names

			local collect = ""
			for i,v in pairs(cur:getcolnames()) do collect = collect .. v .. tablespacer end
			et.G_Print(collect .. "\n") -- fix this order is not in sync with following output

			-- add a limit to 20 rows ?
			while row do
				collect = ""
				for i,v in pairs(row) do collect = collect .. v .. tablespacer end
				-- send more rows each print ? (depends on table size)
				et.G_Print(collect .. "\n")
				row = cur:fetch (row, "a")	-- reusing the table of results
			end
			cur:close()

		elseif cmd == "insert" or "create" or "delete" then
			-- exec cmd
			res = assert (con:execute(et.trap_Argv(1)))
			et.G_Print(res .. "\n")

		elseif cmd == "vacuum" then 
			-- only sqlite	defrag the database
			if dbms == "SQLite" then
				res = assert (con:execute(et.trap_Argv(1)))
				et.G_Print(res .. "\n")
			else
				et.G_Print(color..commandprefix.."sql: Command unknown for this dbms\n")
			end
		else
			-- cmd is 5 char based ?
			cmd = string.lower( string.sub(et.trap_Argv(1), 0 , 5) )
			
			-- alter 
			if cmd == "alter" then
				res = assert (con:execute(et.trap_Argv(1)))
				et.G_Print(res .. "\n")
			else 
				-- cmd is 4 char based
				cmd = string.lower( string.sub(et.trap_Argv(1), 0 , 4) )
				
				-- drop
				if cmd == "drop" then
					-- create a row of data
					res = assert (con:execute(et.trap_Argv(1)))
					cur:close()
				
				-- untested (only mysql atm)	
				elseif cmd == "show" then
					cur = assert (con:execute(et.trap_Argv(1)))
					row = cur:fetch ({}, "a")	-- the rows will be indexed by field names
					local collect = ""
					for i,v in pairs(cur:getcolnames()) do collect = collect .. v .. tablespacer end
					et.G_Print(collect .. "\n")

					-- add a limit to 20 rows ?
					while row do
						collect = ""
						for i,v in pairs(row) do collect = collect .. v .. tablespacer end
						-- send more rows each print ? (depends on table size
						et.G_Print(collect .. "\n")
						row = cur:fetch (row, "a")	-- reusing the table of results
					end
					cur:close()
						
				else
					et.G_Print(color..commandprefix.."sql: Command unknown\n")
				end
			end
		end
	end
	-- add more cmds here ...

end

function shuttdownDBMS()
    
	if dbms == "mySQL" or dbms == "SQLite" then
		con:close()
		env:close()
	else 
		-- should never happen
		error("DBMS not supported.")
	end
end

function et_ShutdownGame( restart )
	shuttdownDBMS()
end
