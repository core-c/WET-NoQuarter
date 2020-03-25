--  
-- NOQ install.lua
--

-- TODO
-- add table weaponstat afaik we get weapon stats at end ... (or get from cvar) 

--------------------------------------------------------------------------------
color = "^5"
version = "1"
commandprefix = "!"
debug = 1 -- debug 0/1
--------------------------------------------------------------------------------
-- db connection data
dbms	= "SQLite" -- possible values "mySQL", "postgreSQL" and "SQLite"
dbname  = "noquarter.sqlite" -- database name, also name for SQLite file
dbuser  = "SA" 
dbpassword = ""
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
	con = assert (env:connect( dbname )) -- this opens OR creates a sqlite3 db - if this Lua is loaded db is created -fix this?
else
  -- stop script
  	error("DBMS not supported.")
end

--cur = {}
res = {}
-- row = {}
--------------------------------------------------------------------------------

function et_InitGame( levelTime, randomSeed, restart )

	et.RegisterModname( "NOQ install " .. version .. " " .. et.FindSelf() )

	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"chat \"" .. color .. "NOQ install " .. version )
	end
end

function et_ConsoleCommand( command )
	
	if debug == 1 then
	  et.trap_SendServerCommand( -1 ,"chat \"" .. color .. "ConsoleCommand - command: " .. command )
	end
	
	if string.lower(et.trap_Argv(0)) == commandprefix.."sqlcreate" then 
		createTablesDBMS()
	elseif string.lower(et.trap_Argv(0)) == commandprefix.."sqlupdate" then 
		updateTablesDBMS()
	elseif string.lower(et.trap_Argv(0)) == commandprefix.."sqldrop" then 
		dropTablesDBMS()
	elseif string.lower(et.trap_Argv(0)) == commandprefix.."sqlclean" then
		-- drop all tables 
		cleanTablesDBMS()
	end
	-- add more cmds here ...
	
end

-- 
function cleanTablesDBMS()
	-- assert(con:execute"delete from player")
	-- assert(con:execute"delete from session")
end

-- For future versions if the db structure does exist
function updateTablesDBMS()
	-- alter tables ...
end

function dropTablesDBMS()
	res = assert(con:execute"DROP TABLE session")
	et.G_Print(res .. "\n")
	res = assert(con:execute"DROP TABLE player")
	et.G_Print(res .. "\n")
	res = assert(con:execute"DROP TABLE version")
	et.G_Print(res .. "\n")
end

function createTablesDBMS()

		-- IMPORTANT NOTES for default field values:
		-- Mandatory fields to create a table are set as NOT NULL
		-- A non existing time is NULL
		-- If you add more fileds create usefull default values ...

		et.G_Print(color .. commandprefix.."sqlcreate for ".. dbms .." started\n") 
	
		-- SQLite
		if dbms == "SQLite" then
			
			-- Notes:
			-- We store timestamps as INTEGER - Unix Time, the number of seconds since 1970-01-01 00:00:00 UTC. 
			
			res = assert(con:execute"CREATE TABLE IF NOT EXISTS player ( \
				id 			INTEGER 		PRIMARY KEY,	\
				pkey 		TEXT 			UNIQUE NOT NULL,\
				conname 	TEXT 			NOT NULL,		\
				regname 	TEXT 			DEFAULT '',		\
				netname 	TEXT 			DEFAULT '',		\
				isBot 		INTEGER 		DEFAULT 0,		\
				clan 		TEXT 			DEFAULT '',		\
				level 		INTEGER 		DEFAULT 0,		\
				user 		TEXT 			DEFAULT '',		\
				password 	TEXT 			DEFAULT '',		\
				email 		TEXT 			DEFAULT '',		\
				xp0 		INTEGER 		DEFAULT 0,		\
				xp1 		INTEGER 		DEFAULT 0,		\
				xp2 		INTEGER 		DEFAULT 0,		\
				xp3 		INTEGER 		DEFAULT 0,		\
				xp4 		INTEGER 		DEFAULT 0,		\
				xp5 		INTEGER 		DEFAULT 0,		\
				xp6 		INTEGER 		DEFAULT 0,		\
				xptot		INTEGER 		DEFAULT 0,		\
				banreason 	TEXT 			DEFAULT '',		\
				bannedby 	TEXT 			DEFAULT '',		\
				banexpire 	INTEGER 		DEFAULT NULL,	\
				mutedreason TEXT			DEFAULT '',		\
				mutedby 	TEXT 			DEFAULT '',		\
				muteexpire 	DATE 			DEFAULT NULL,	\
				warnings 	INTEGER 		DEFAULT 0,		\
				suspect 	INTEGER 		DEFAULT 0,		\
				regdate 	DATE 			DEFAULT NULL,	\
				updatedate 	DATE 			NOT NULL,		\
				createdate 	DATE 			NOT NULL)" )
			et.G_Print(res .. "\n")
			

			res = assert(con:execute"CREATE TABLE IF NOT EXISTS session ( \
				id 			INTEGER 		PRIMARY KEY,	\
				pkey 		INTEGER 		NOT NULL,		\
				slot 		INTEGER 		NOT NULL,		\
				map 		TEXT 			NOT NULL,		\
				ip 			TEXT 			DEFAULT '',		\
				netname 	TEXT 			DEFAULT '',		\
				valid 		INTEGER 		DEFAULT NULL,	\
				start 		DATE 			NOT NULL,		\
				end 		DATE 			DEFAULT NULL,	\
				sptime 		INTEGER 		DEFAULT NULL,	\
				axtime 		INTEGER 		DEFAULT NULL,	\
				altime 		INTEGER 		DEFAULT NULL,	\
				lctime		INTEGER 		DEFAULT NULL,	\
				sstime		INTEGER 		DEFAULT NULL,	\
				xp0 		INTEGER 		DEFAULT 0,		\
				xp1 		INTEGER 		DEFAULT 0,		\
				xp2 		INTEGER 		DEFAULT 0,		\
				xp3 		INTEGER 		DEFAULT 0,		\
				xp4 		INTEGER 		DEFAULT 0,		\
				xp5 		INTEGER 		DEFAULT 0,		\
				xp6 		INTEGER 		DEFAULT 0,		\
				xptot 		INTEGER 		DEFAULT 0,		\
				acc 		REAL			DEFAULT 0.0,	\
				kills 		INTEGER 		DEFAULT 0,		\
				tkills 		INTEGER 		DEFAULT 0,		\
				death 		INTEGER 		DEFAULT 0,		\
				uci 		INTEGER 		DEFAULT 0)" )
			et.G_Print(res .. "\n")

			res = assert(con:execute"CREATE TABLE IF NOT EXISTS version ( 	\
					id INTEGER PRIMARY KEY,						 			\
					version INTEGER NOT NULL UNIQUE )" )
			et.G_Print(res .. "\n")
		
			-- SQLite needs exra cmds for setting up an index (aynbody knows syntax for create table stmd?)
			-- player
			res = assert(con:execute"CREATE INDEX p_regname ON player( regname )" )
			et.G_Print(res .. "\n")
			res = assert(con:execute"CREATE INDEX p_netname ON player( netname )" )
			et.G_Print(res .. "\n")
			-- session
			res = assert(con:execute"CREATE INDEX s_pkey ON session ( pkey)" )
			et.G_Print(res .. "\n")
			res = assert(con:execute"CREATE INDEX s_ip ON session ( ip)" )
			et.G_Print(res .. "\n")
		
[[			
			-- TODO create trigger for updatedate
			res = assert(con:execute"CREATE TRIGGER insert_player_updatedate AFTER  INSERT ON player \
     			BEGIN																		\
      				UPDATE player SET updatedate = DATETIME('NOW')  WHERE id = new.id;		\
     			END;")
			et.G_Print(res .. "\n")
]]		
			-- insert data
			res = assert(con:execute"INSERT INTO version VALUES ( 1, ".. version ..")")	
			et.G_Print(res .. "\n")
			
		-- mySQL	
		elseif dbms == "mySQL" then
		
			res = assert(con:execute"CREATE TABLE player ( \
				id 			INT 			PRIMARY KEY AUTO_INCREMENT,\
				pkey 		VARCHAR(32) 	UNIQUE  NOT NULL,		\
				conname 	VARCHAR(36) 	NOT NULL,				\
				regname 	VARCHAR(36) 	DEFAULT '',				\
				netname 	VARCHAR(36) 	DEFAULT '',				\
				isBot 		BOOLEAN 		DEFAULT 0,				\
				clan 		VARCHAR(20) 	DEFAULT '',				\
				level 		INT 			DEFAULT 0,				\
				user 		VARCHAR(20) 	DEFAULT '',				\
				password 	VARCHAR(32) 	DEFAULT '',				\
				email 		VARCHAR(50) 	DEFAULT '',				\
				xp0 		INT 			DEFAULT 0,				\
				xp1 		INT 			DEFAULT 0,				\
				xp2 		INT 			DEFAULT 0,				\
				xp3 		INT 			DEFAULT 0,				\
				xp4 		INT 			DEFAULT 0,				\
				xp5 		INT 			DEFAULT 0,				\
				xp6 		INT 			DEFAULT 0,				\
				xptot 		INT 			DEFAULT 0,				\
				banreason 	VARCHAR(1024) 	DEFAULT '',				\
				bannedby 	VARCHAR(36) 	DEFAULT '',				\
				banexpire 	DATETIME 		DEFAULT '1000-01-01 00:00:00',			\
				mutedreason VARCHAR(1024)	DEFAULT '',				\
				mutedby 	VARCHAR(36) 	DEFAULT '',				\
				muteexpire 	DATETIME 		DEFAULT '1000-01-01 00:00:00',			\
				warnings 	SMALLINT 		DEFAULT 0,				\
				suspect 	TINYINT 		DEFAULT 0,				\
				regdate 	DATETIME 		DEFAULT NULL,			\
				updatedate 	TIMESTAMP 		DEFAULT '0000-00-00 00:00:00' on update CURRENT_TIMESTAMP,\
				createdate 	DATETIME 		NOT NULL,				\
				INDEX(`regname`),									\
				INDEX(`netname`)									\
				) ENGINE=InnoDB" )		
			et.G_Print(res .. "\n")

			res = assert(con:execute"CREATE TABLE session (			\
				id 			INT 			PRIMARY KEY NOT NULL AUTO_INCREMENT,\
				pkey 		VARCHAR(32) 	NOT NULL,				\
				slot 		SMALLINT 		NOT NULL,				\
				map 		VARCHAR(36) 	NOT NULL,				\
				ip 			VARCHAR(25) 	DEFAULT '',				\
				netname 	VARCHAR(36) 	DEFAULT '',				\
				valid 		BOOLEAN 		DEFAULT NULL,			\
				start 		DATETIME 		NOT NULL,				\
				end 		DATETIME 		DEFAULT NULL,			\
				sptime 		TIME 			DEFAULT NULL,			\
				axtime 		TIME 			DEFAULT NULL,			\
				altime 		TIME 			DEFAULT NULL,			\
				lctime 		TIME 			DEFAULT NULL,			\
				sstime 		TIME 			DEFAULT NULL,			\
				xp0 		INT 			DEFAULT 0,				\
				xp1 		INT 			DEFAULT 0,				\
				xp2 		INT 			DEFAULT 0,				\
				xp3 		INT 			DEFAULT 0,				\
				xp4 		INT 			DEFAULT 0,				\
				xp5 		INT 			DEFAULT 0,				\
				xp6 		INT 			DEFAULT 0,				\
				xptot		INT 			DEFAULT 0,				\
				acc 		DOUBLE (10,2) 	DEFAULT 0.0,			\
				kills 		SMALLINT 		DEFAULT 0,				\
				tkills 		SMALLINT 		DEFAULT 0,				\
				death 		SMALLINT 		DEFAULT 0,				\
				uci 		TINYINT 		DEFAULT 0,				\
				INDEX(`pkey`),										\
				INDEX(`ip`)											\
				) ENGINE=InnoDB" )
			et.G_Print(res .. "\n")

			res = assert(con:execute"CREATE TABLE version (			\
					id INT PRIMARY KEY NOT NULL,					\
					version INT NOT NULL UNIQUE						\
				) ENGINE=InnoDB" )
			et.G_Print(res .. "\n")
				
			-- TODO create trigger for updatedate	
				
			-- insert data
			res = assert(con:execute"INSERT INTO version VALUES ( 1, ".. version ..")")		
			et.G_Print(res .. "\n")
		end
		
		et.G_Print(color .. commandprefix.."sqlcreate database created version: "..version .."\n")  
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
