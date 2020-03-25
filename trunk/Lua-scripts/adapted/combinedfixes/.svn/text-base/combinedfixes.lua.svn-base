-- combined exploit fix module
-- this module replaces wsfix.lua usinfocheck.lua guidcheck.lua and fakeplimit.lua
-- I am no longer distributing the above as individual modules
-- author: reyalp@gmail.com
-- confugration:
--  add combinedfixes.lua to lua_modules, or merge into your existing module
-- version: 3 
-- history: 
--  3 - crlf abuse http://aluigi.freeforums.org/quake3-engine-callvote-bug-t686.html
--  2 - string.find not string.match
--  1 - first standalone
--  see individual sections for older stuff


-- require("rllib/vstrict").init()
-- AutoDeclare()

-- client command checks, formerly wsfix
-- prevent ws overrun exploit, crlf abuse
-- history: 
--  2 - bugfix
-- TY McSteve for reporting this to us.

function et_ClientCommand(cno,cmd)
	cmd = string.lower(cmd)
	if cmd == "ws" then
		local n = tonumber(et.trap_Argv(1))
		if not n then
			et.G_LogPrint(string.format("wsfix: client %d bad ws not a number [%s]\n",cno,tostring(et.trap_Argv(1))))
			return 1
		end
		
		if n < 0 or n > 21 then
			et.G_LogPrint(string.format("wsfix: client %d bad ws %d\n",cno,n))
			return 1
		end
		return 0
	end
	if cmd == "callvote" or cmd == "ref" or cmd == "sa" or cmd == "semiadmin" then
		local args=et.ConcatArgs(1) 
--		et.G_LogPrint(string.format("combinedfixes: client %d %s [%s]\n",cno,cmd,args))
		if string.find(args,"[\r\n]") then
			et.G_LogPrint(string.format("combinedfixes: client %d bad %s [%s]\n",cno,cmd,args))
			return 1;
		end
		return 0
	end
	return 0
end


--  prevent various borkage by invalid userinfo
-- version: 4
-- history:
--  4 - check length and IP
--  3 - check for name exploit against guidcheck
--  2 - fix nil var ref if kicked in RunFrame
--      fix incorrect clientNum in log message for ClientConnect kick
--  1 - initial release

-- names that can be used to exploit some log parsers 
--  note: only console log parsers or print hooks should be affected, 
--  game log parsers don't see these at the start of a line
-- "^etpro IAC" check is required for guid checking
-- comment/uncomment others as desired, or add your own
-- NOTE: these are patterns for string.find
badnames = {
--	'^ShutdownGame',
--	'^ClientBegin',
--	'^ClientDisconnect',
--	'^ExitLevel',
--	'^Timelimit',
--	'^EndRound',
	'^etpro IAC',
--	'^etpro privmsg',
-- "say" is relatively likely to have false positives
-- but can potentially be used to exploit things that use etadmin_mod style !commands
--	'^say',
--	'^Callvote',
--	'^broadcast'
}

-- returns nil if ok, or reason
function check_userinfo( cno )
	local userinfo = et.trap_GetUserinfo(cno)
--	printf("check_userinfo: [%s]\n",userinfo)

	-- bad things can happen if it's full
	if string.len(userinfo) > 980 then
		return "oversized"
	end

	-- newlines can confuse various log parsers, and should never be there
	-- note this DOES NOT protect your log parsers, as the userinfo may
	-- already have been sent to the log
	if string.find(userinfo,"\n") then
		return "new line"
	end

	-- the game never seems to make userinfos without a leading backslash, 
	-- or with a trailing backslash, so reject those from the start
	if (string.sub(userinfo,1,1) ~= "\\" ) then
		return "missing leading slash"
	end
	-- shouldn't really be possible, since the engine stuffs ip\ip:port on the end
	if (string.sub(userinfo,-1,1) == "\\" ) then
		return "trailing slash"
	end

	-- now that we know it is properly formed, count the slashes
	local n = 0
	for _ in string.gfind(userinfo,"\\") do
		n = n + 1
	end

	if math.mod(n,2) == 1 then
		return "unbalanced"
	end

	local m
	local t = {}

	-- right number of slashes, check for dupe keys
	for m in string.gfind(userinfo,"\\([^\\]*)\\") do
		if string.len(m) == 0 then
			return "empty key"
		end
		m = string.lower(m)
		if t[m] then
			return "duplicate key"
		end
		t[m] = true 
	end

	-- they might hose the userinfo in some way that prevents the ip from being
	-- obtained. If so -> dump
	local ip = et.Info_ValueForKey( userinfo, "ip" )
	if ip == "" then
		return "missing ip"
	end
--	printf("checkuserinfo: ip [%s]\n", ip)

	-- make sure whatever is there is roughly valid while we are at it
	-- "localhost" may be present on a listen server. This module is not intended for listen servers.
	-- string.match 5.1.x
	-- string.find 5.0.x
	if string.find(ip,"^%d+%.%d+%.%d+%.%d+:%d+$") == nil then
		return "malformed ip"
	end

	-- check for this to prevent exploitation of guidcheck
	-- note the proper solution would be for chats to always have a prefix in the console. 
	-- Why the fuck does the server console need both
	-- say: [NW]reyalP: blah
	-- [NW]reyalP: blah
	
	local name = et.Info_ValueForKey( userinfo, "name" )
	if name == "" then
		return "missing name"
	end
--	printf("checkuserinfo %d name %s\n",cno,name)
	for _, badnamepat in ipairs(badnames) do
		local mstart,mend,cno = string.find(name,badnamepat)
		if mstart then
			return "name abuse"
		end
	end
	-- return nil
end

-- 3.2.6 and earlier doesn't actually call et_ClientUserinfoChanged 
-- every time the userinfo changes, 
-- so we use et_RunFrame to check every so often
-- comment this out or adjust to taste
infocheck_lasttime=0
infocheck_client=0
-- check a client every 5 sec
infocheck_freq=5000

function et_RunFrame( leveltime )
	if ( infocheck_lasttime + infocheck_freq > leveltime ) then
		return
	end

--	printf("infocheck %d %d\n", infocheck_client, leveltime)
	infocheck_lasttime = leveltime
	if ( et.gentity_get( infocheck_client, "inuse" ) ) then
		local reason = check_userinfo( infocheck_client )
		if ( reason ) then
			et.G_LogPrint(string.format("userinfocheck frame: client %d bad userinfo %s\n",infocheck_client,reason))
			et.trap_SetUserinfo( infocheck_client, "name\\badinfo" )
			et.trap_DropClient( infocheck_client, "bad userinfo", 0 )
		end
	end

	infocheck_client = infocheck_client + 1
	if ( infocheck_client >= tonumber(et.trap_Cvar_Get("sv_maxclients")) ) then
		infocheck_client = 0
	end
end

function et_ClientUserinfoChanged( cno )
--	printf("clientuserinfochanged %d\n",cno)
	local reason = check_userinfo( cno )
	if ( reason ) then
		et.G_LogPrint(string.format("userinfocheck infochanged: client %d bad userinfo %s\n",cno,reason))
		et.trap_SetUserinfo( cno, "name\\badinfo" )
		et.trap_DropClient( cno, "bad userinfo", 0 )
	end
end



-- prevent etpro guid borkage
-- version: 1
-- TY pants

-- default to kick with no temp ban for now
DEF_GUIDCHECK_BANTIME = 0

function bad_guid(cno,reason)
	local bantime = tonumber(et.trap_Cvar_Get( "guidcheck_bantime" ))
	if not bantime or bantime < 0 then
		bantime = DEF_GUIDCHECK_BANTIME
	end

	et.G_LogPrint(string.format("guidcheck: client %d bad GUID %s\n",cno,reason))
	-- we don't send them the reason. They can figure it out for themselves.
	et.trap_DropClient(cno,"You are banned from this server",bantime)
end

function check_guid_line(text)
--find a GUID line
	local guid,netname
	local mstart,mend,cno = string.find(text,"^etpro IAC: (%d+) GUID %[")
	if not mstart then
		return
	end
	text=string.sub(text,mend+1)
	--GUID] [NETNAME]\n
	mstart,mend,guid = string.find(text,"^([^%]]*)%] %[")
	if not mstart then
		bad_guid(cno,"couldn't parse guid")
		return
	end
	--NETNAME]\n
	text=string.sub(text,mend+1)

	netname = et.gentity_get(cno,"pers.netname")

	mstart,mend = string.find(text,netname,1,true)
	if not mstart or mstart ~= 1 then
		bad_guid(cno,"couldn't parse name")
		return
	end

	text=string.sub(text,mend+1)
	if text ~= "]\n" then
		bad_guid(cno,"trailing garbage")
		return
	end

--	printf("guidcheck: etpro GUID %d %s %s\n",cno,guid,netname)
		
	-- {N} is too complicated!
	mstart,mend = string.find(guid,"^%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x$")
	if not mstart then
		bad_guid(cno,"malformed")
		return
	end
--	printf("guidcheck: OK\n")
end

function et_Print(text)
	check_guid_line(text)
end


-- limit fakeplayers DOS
-- http://aluigi.altervista.org/fakep.htm
-- used if cvar is not set
-- confugration:
-- set ip_max_clients cvar as desired. If not set, defaults to the value below.
--FAKEPLIMIT_VERSION = "1.0"
DEF_IP_MAX_CLIENTS = 3

et.G_Printf = function(...)
		et.G_Print(string.format(unpack(arg)))
end

function IPForClient(clientNum)
-- TODO listen servers may be 'localhost'
	local userinfo = et.trap_GetUserinfo( clientNum ) 
	if userinfo == "" then
		return ""
	end
	local ip = et.Info_ValueForKey( userinfo, "ip" )
-- find IP and strip port
	local ipstart, ipend, ipmatch = string.find(ip,"(%d+%.%d+%.%d+%.%d+)")
	-- don't error out if we don't match an ip
	if not ipstart then
		return ""
	end
--	et.G_Printf("IPForClient(%d) = [%s]\n",clientNum,ipmatch)
	return ipmatch
end

function et_ClientConnect( clientNum, firstTime, isBot )
-- userinfocheck stuff. Do this before IP limit
--	printf("connect %d\n",cno)
	local reason = check_userinfo( clientNum )
	if ( reason ) then
		et.G_LogPrint(string.format("userinfocheck connect: client %d bad userinfo %s\n",clientNum,reason))
		return "bad userinfo"
	end

-- note IP validity should be enforced by userinfocheck stuff
	local ip = IPForClient( clientNum )
	local count = 1 -- we count as the first one
	local max = tonumber(et.trap_Cvar_Get( "ip_max_clients" ))
	if not max or max <= 0 then
		max = DEF_IP_MAX_CLIENTS
	end
	-- et.G_Printf("firstTime %d\n",firstTime);
	-- it's probably safe to only do this on firsttime, but checking
	-- every time doesn't hurt much
	
	-- validate userinfo to filter out the people blindly using luigi's code
	local userinfo = et.trap_GetUserinfo( clientNum )
	-- et.G_Printf("userinfo: [%s]\n",userinfo)
	if et.Info_ValueForKey( userinfo, "rate" ) == "" then 
		et.G_Printf("fakeplimit.lua: invalid userinfo from %s\n",ip)
		return "invalid connection"
	end

	for i = 0, et.trap_Cvar_Get("sv_maxclients") - 1 do
		-- pers.connected is set correctly for fake players
		-- can't rely on userinfo being empty
		if i ~= clientNum and et.gentity_get(i,"pers.connected") > 0 and ip == IPForClient(i) then
			count = count + 1
			if count > max then
				et.G_Printf("fakeplimit.lua: too many connections from %s\n",ip)
				-- TODO should we drop / ban all connections from this IP ?
				return string.format("only %d connections per IP are allowed on this server",max)
			end
		end
	end
end
-- NoAutoDeclare()

