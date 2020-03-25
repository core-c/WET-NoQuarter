-- LUA module version
 Mysqlxpsave = "0.1"
 databaseversion = "1"
 -- databasecheck = 0

 -- NOTE
 -- When you call con:execute to query a SQL statment, if it feedbacks informations ( i.e. a SELECT )
 -- alweys remember to call cur:close() when you have done using those informations
 -- i.e.
 --	 cur = con:execute( BLA BLA BLA )
 --      row = cur:fetch ({}, "a")
 --	 while row do
 --		COMMANDS
 --		row = cur:fetch (row, "a")
 --	 end
 --      cur:close()
 -- If the SQL statment doesn't feedback informations ( i.e. an INSERT ), then you don't need to colse the res
 -- i.e.
 --	 res = con:execute( BLA BLA BLA )

 -- Database managment system to use
 dbname = " "
 dbuser = " "
 dbpassword = " "

 -- We need a Ddase with those tables:
 -- battle
 -- engi
 -- medic
 -- signals
 -- light
 -- heavy
 -- covert
 -- date (datetime)
 -- ip
 -- xp
 -- name
 -- nameoh

 -- The next vars are used only with MySQL and PostgreSQL extensions
 dbhostname = "localhost"
 dbport = "3306"

 require "luasql.mysql"
 env = assert (luasql.mysql())
 con = assert (env:connect (dbname, dbuser, dbpassword, dbhostname, dbport))
 cur = {}
 res = {}
 row = {}
 player = {} -- Hold actual players data.

 -- The table "slot" will store a table "client" for each slot on the server
 slot = {}

 -- The table "client" is created each time someone connect and will store the current client informations
 -- The current fields are:
 -- client["id"] = stores the player id from the database


 -- We are connected to the database	

 function et.G_Printf(...)
 et.G_Print(string.format(unpack(arg)))
 end



function et_InitGame( levelTime, randomSeed, restart )

et.RegisterModname( "Mysqlxpsave version " .. Mysqlxpsave .. " " .. et.FindSelf() )


et.G_LogPrint("^1Initializing "..dbname.." as XPsource\n")

	end

function et_ShutdownGame( restart ) 

con:close()
env:close()

end



function et_ClientConnect( clientNum, firstTime, isBot )
																										
Get player GUId
local guid = string.upper( et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" ))
																																

if firstTime == 1 then
--It's the first time the players connects ( = it's not a reconnection )
Search the GUId in the database ( GUId is UNIQUE, so we just have 1 result, stop searching when we have it )
cur = assert (con:execute("SELECT * from data WHERE guid='"..guid.."' LIMIT 1"))
row = cur:fetch ({}, "a")
cur:close()
if row then
This player is already present in the database, we copy the xp-datasets
player['guid'] = guid
player['battle'] = row.battle
player['engi'] = row.engi
player['medic'] = row.medic
player['signals'] = row.signals
player['light'] = row.light
player['heavy'] = row.heavy
player['covert'] = row.covert
player['id'] = row.id
player['freshcon'] = true

slot[clientNum] = player

et.G_LogPrint("Clientnum:".. clientNum.." scraped from Mysql\n")
--Well, could do that here to, but IlDuca mentioned that could lead to strange behaviour, so we only save it until client begins
--et.G_XP_Set ( clientNum , player['battle'], 0, 0 )
--et.G_XP_Set ( clientNum , player['engi'], 1, 0 ) 
--et.G_XP_Set ( clientNum , player['medic'], 2, 0 )
--et.G_XP_Set ( clientNum , player['signals'], 3, 0 )
--et.G_XP_Set ( clientNum , player['light'], 4, 0 )
-- et.G_XP_Set ( clientNum , player['heavy'], 5, 0 )
-- et.G_XP_Set ( clientNum , player['covert'], 6, 0 )
																																																																																																																																														 return nil
																																																																																																																																																	else
--This player is a new one: get some infos and create a new database entry
et.G_Printf("^1Welcome a new player on our servers:"..name.."/n")
local name =  et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "name" )
local IP = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "ip" )
res = assert (con:execute("INSERT INTO data (guid,		ip, name,	date,	battle,		engi,	medic,	signals,	light,	heavy,	covert,		nameoh, xp) VALUES ('"..guid.."', '"..IP.."','"..name.."', NOW(), '0', '0', '0','0', '0','0','0', "..et.Q_CleanStr(name)..", '0')"))
slot[clientNum]['freshcon'] = false 
return nil
end
end


return nil
end

function et_ClientBegin( clientNum )

if slot[clientNum]['freshcon'] == true then -- Hey, its his first Join/Begin

et.G_XP_Set ( clientNum , slot[clientNum]['battle'], 0, 0 )
et.G_XP_Set ( clientNum , slot[clientNum]['engi'], 1, 0 ) 
et.G_XP_Set ( clientNum , slot[clientNum]['medic'], 2, 0 )
et.G_XP_Set ( clientNum , slot[clientNum]['signals'], 3, 0 )
et.G_XP_Set ( clientNum , slot[clientNum]['light'], 4, 0 )
et.G_XP_Set ( clientNum , slot[clientNum]['heavy'], 5, 0 )
et.G_XP_Set ( clientNum , slot[clientNum]['covert'], 6, 0 )
slot[clientNum]['freshcon'] = false -- remember that we already set XP
else 
return nil
end
return nil	

end

function et_ClientDisconnect( clientNum )

	--on disconnect, gather Info and save it.
	local guid = string.upper( et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "cl_guid" ))
	local name =  et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "name" )

	local IP = et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "ip" )
	
	player['battle']	=	et.gentity_get(clientNum,"sess.skillpoints",0)
	player['engi']		=	et.gentity_get(clientNum,"sess.skillpoints",1)
	player['medic']		=	et.gentity_get(clientNum,"sess.skillpoints",2)
	player['signals']	=	et.gentity_get(clientNum,"sess.skillpoints",3)
	player['light']		=	et.gentity_get(clientNum,"sess.skillpoints",4)
	player['heavy']		=	et.gentity_get(clientNum,"sess.skillpoints",5)
	player['covert']	=	et.gentity_get(clientNum,"sess.skillpoints",6)
	
	--player['id'] = slot[clientNum]['id']
	
	--Add XP up. Shit is this long.
	player['xp'] = player['battle'] + player['engi'] + player['medic'] + player['signals']  + player['light'] + player['heavy'] + player['covert'] 

	et.G_Printf("^1Saving :"..name.." to Dbase \n")
	
	--we move Old entry into history
	res = assert (con:execute("INSERT INTO data2 SELECT * FROM data WHERE GUID='"..guid.."' LIMIT 1"))
	
	--res = assert (con:execute("DELETE FROM data WHERE GUID='"..guid.."' LIMIT 1"))
	--then we create new one
	res = assert (con:execute("UPDATE data SET ip= '"..IP.."' , name= '"..name.."',	date=NOW(),	battle='"..player['battle'].."',	engi='"..player['engi'].."',	medic='"..player['medic'].."',	signals='"..player['signals'].."',	light='"..player['light'].."',	heavy='"..player['heavy'].."',	covert='"..player['covert'].."',	nameoh='"..et.Q_CleanStr(name).."', xp ='" .. player['xp'] .."' WHERE GUID='"..guid.."'"))

end
