---------------------------------
------- Dynamite counter --------
-------  By Necromancer  --------
-------    5/04/2009     --------
------- www.usef-et.org  --------
---------------------------------
-- This script can be freely used and modified as long as the original authors 
-- are mentioned (and their homepage: www.usef-et.org)

-- changes:
-- IRATA: adapted to NQ 
version = 2
---------------------------------
----- settings
---------------------------------

-- 0 means disable
-- 1 means only the team that planted the dyno
-- 2 means everyone
SHOW = 2

-- if set to 1, will process only one dynamite per place.
-- set to 0 to process all dyno's.
ONE = 1

-- <TIME> gets sabstatuted by the time left
-- <PLACE> sabstatuted by the thing going to blow
-- default - well, default message if nothing specified. (self-explenatory)
-- message[dynamite_time_left] = message_to_show
MESSAGE = {}
-- MESSAGE.DEFAULT = "^1<TIME> ^wsecs till explosion!!"
MESSAGE.DEFAULT = ""
MESSAGE[20] = "^a<TIME> ^wsecs till ^2<PLACE> ^wexplosion!!"
MESSAGE[10] = "^a<TIME> ^wsecs till ^2<PLACE> ^wexplosion!!"
MESSAGE[3] = "^13 .."
MESSAGE[2] = "^12 .."
MESSAGE[1] = "^11 .."
MESSAGE[0] = "" -- no message here, we get it from game

---------------------------------

gamename = et.trap_Cvar_Get("gamename")
if gamename == "etpro" then
	CHAT = "b 8"
	POPUP = "etpro" 
elseif gamename == "etpub" then
	CHAT = "chat" 
	POPUP = "etpub"
else -- "noquarter"
	CHAT = "cp" 
	POPUP = "nq"
end


timer = {}

OLD = os.time()

function et_InitGame( levelTime, randomSeed, restart )
	et.RegisterModname( "Dynamite counter " .. version .. " " .. et.FindSelf() )
end	


function et_RunFrame( levelTime )
	current = os.time()

	for dyno, temp in pairs(timer) do
		--[[
		if current - OLD >1 then 
			et.G_LogPrint("dynos: " .. table.getn(timer) .. "\n")
			et.G_LogPrint("dyno: " .. dyno .." time: " .. timer[dyno]["time"] - current .. "\n")
			OLD = current
		end
		--]]
		if timer[dyno]["time"] - current >= 0 then
			for key,temp in pairs(timer[dyno]) do
				if type(key) == "number" then
					if timer[dyno]["time"] - current == key then
						send_print(timer,dyno,key)
						timer[dyno][key] = nil	
						--et.G_LogPrint("dynamite key deleted: " .. dyno .." key: " .. key .. "\n")
					end
				end
			end

		else
			--et.G_LogPrint("dynamite out: " .. dyno .. "\n")
			place_destroyed(timer[dyno]["place"])
			--timer[dyno] = nil
		end
	end
end

function place_destroyed(place) -- removes any dynamties that were planted on this objective
	for dynamite, temp in pairs(timer) do
		if timer[dynamite]["place"] == place then
			timer[dynamite] = nil
		end
	end
end

function send_print(timer,dyno,ttime)
	if SHOW == 0 then return end
	if SHOW == 1 then
		for player=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			if et.gentity_get(player,"sess.sessionTeam") == timer[dyno]["team"] then
				print_message(player, ttime, timer[dyno]["place"])
			end
		end
	else
		print_message(-1, ttime, timer[dyno]["place"])
	end
end

function print_message(slot, ttime, place)
	message = MESSAGE.DEFAULT
	if type(MESSAGE[ttime]) == "string" and MESSAGE[ttime] ~= "" then
		message = MESSAGE[ttime]
	end
	message = string.gsub (message, "<TIME>", ttime)
	message = string.gsub (message, "<PLACE>", place)
	et.trap_SendServerCommand( slot , string.format('%s \"%s"\n',CHAT, message))


end

function et_Print( text )

	--etpro popup: axis planted "the Old City MG Nest"
	start,stop = string.find(text, POPUP .. " popup:",1,true) -- check that its not any player print, trying to manipulate the dyno counter

	if start and stop then
		
		start,stop,team,plant = string.find(text, POPUP .. " popup: (%S+) planted \"([^%\"]*)\"")

		if start and stop then -- dynamite planted
			



			if ONE ~= 0 then -- check if there's a dyo planted on that spot already
				for dynamite, temp in pairs(timer) do
					if timer[dynamite]["place"] == plant then
						return -- abort!! abort!! 
					end
				end
			end


			if team == "axis" then team = 1 
			else team = 2 end
			index = table.getn(timer)+1
			timer[index] = {}
			-- timer[index] = MESSAGE -- doesnt work :/
			-- i guess thats because lua does not copy tables field-to-field
			-- but just copys the pointer/reference to that table (when i later 'nil' it, it destroyes the original one)
			timer[index]["team"] = team
			timer[index]["place"] = plant
			timer[index]["time"] = os.time() +30
		
			for key,temp in pairs(MESSAGE) do
				if type(key) == "number" then
					timer[index][key] = true
				end
			end
	

			--et.G_LogPrint("dynamite set: " .. index .. "\n")
		end

		start,stop,team,plant = string.find(text, POPUP .. " popup: (%S+) defused \"([^%\"]*)\"")
		if start and stop then -- dynamite defused
			if team == "axis" then team = 1 
			else team = 2 end
			for index,temp in pairs(timer) do
				if timer[index]["place"] == plant then
					timer[index] = nil
					--et.G_LogPrint("dynamite removed: " .. index .. "\n")
					return
				end
			end
		end
	end
end
