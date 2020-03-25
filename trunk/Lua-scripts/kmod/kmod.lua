KMODversion = "Beta 1.5"
KMODversion2 = "1.5"
k_commandprefix = "!" -- change this to your desired prefix
k_shrubbot_file_name = "shrubbot_lua.cfg"

--[[
This lua was created to replace most features of etadmin mod
and is much faster in response time and includes some features
that etadmin mod does not have and vice versa.

FEATURES:
	*** Killingsprees plus all the other killingspree type stuff (Sounds Included)
	*** Doublekills, Multikills, Ultrakills, Monsterkills, and Ludicrouskills (Sounds Included)
	*** Flakmonkey's - When you get 3 kills with either panzer or a riflenade.
		Flakmonkey is reset if you get any other type of kill/teamkill/or if you die (Sound included)
	*** Firstblood (Sound Included)
	*** Lastblood
	*** Spreerecord (not including records for individual maps)
	*** Enhanced Private Messaging - The sender can use partial name of recipiant
		or can now use the recipiants slot number.  When using ETPro 3.2.6 or higher,
		a new sound will be played letting you know that you have a private message.
		Players can now private message all 2+ level admins currently on the server using
		/ma or /pma then your message.
	*** Vote disable was taken directly from ETAdmin mod and is slightly enhanced such that
		it will detect changes to the timelimit.  (see config for details)
	*** Antiunmute - When a player is muted he may not be unmuted via vote
	*** Advanced Adrenaline - Players using adrenaline now have a timer in their cp area
		displaying the amount of adrenaline time they have left.
		A sound will also be played in their general location letting everyone else
		know that they are using adrenaline (disableable) (sound included)
	*** Killer's HP - Killer's HP is displayed to their victims.  When you kill somone
		and are killed in return within a certain amount of time no message will be displayed.
		When a killer is using adrenaline the victim will see a message displaying so.
	*** Advanced players - Time nudge and max packets are removed from players list
		and admins may see which admins (level 2+) are on the server using /admins
	*** Chat log - All chats are logged along with player renames/connects/disconnects/and map restarts
	*** Crazygravity - The exact same crazy gravity you've come to know and love
	*** Team kill restriction - Taken from Etadmin mod and uses punkbuster to kick (see config)
	*** /kill limit - After the max amount of slash kills is reached they are no longer
		able to /kill.
	*** Endround shuffle - At the end of each round teams are shuffled automatically
		I recomend using this on servers with alot of people.
	*** Noise reduction - ETPRO 3.2.6 OR HIGHER IS REQUIRED!!!  Plays all killingsprees
		multikills/deathsprees/and firstblood to the killer or victim depending on which one
	*** Color codes can be changed for lastblood and killer HP display in config
	*** Spawn kill protection - A newly spawned player will keep his spawn shield
		Until he either moves or fires his weapon.  (see config)
	*** Paths to sounds can be changed to fit server admins needs.

CREDITS:
	Special to
		Noobology
		Armymen
		Rikku
		Monkey Spawn
		Brunout
		Dominator
		James
		Pantsless Victims
	For helping with testing

SOURCES:
	Some code and ideas dirived from G073nks which can be found here
		http://wolfwiki.anime.net/index.php/User:Gotenks
	Code to get slot number from name was from the slap command I found on the ETPro forum here 
		http://bani.anime.net/banimod/forums/viewtopic.php?t=6579&highlight=slap
	Infty's NoKill lua code was used and edited for the slashkill limit which can be found here
		http://wolfwiki.anime.net/index.php/User:Infty
	Ideas dirived from ETAdmin_mod found here
		http://et.d1p.de/etadmin_mod/wiki/index.php/Main_Page

	If you see your code in here let me know and I'll add you to the credits for future releases

	The rest of the code is mine :D
--]]

killingspree = {}
flakmonkey = {}
deathspree = {}
multikill = {}
playerwhokilled = {}
killedwithweapk = {}
killedwithweapv = {}
playerlastkilled = {}
muted = {}
muteDuration = {}
nummutes = {}
chkIP = {}
antiloopadr1 = {}
antiloopadr2 = {}
adrenaline = {}
adrnum = {}
adrnum2 = {}
adrtime = {}
adrtime2 = {}
adrendummy = {}
clientrespawn = {}
invincStart = {}
invincDummy = {}
switchteam = {}
gibbed = {}
randomClient = {}

timecounter = 1

kill1 = {}
kill2 = {}
kill3 = {}
kill4 = {}
kill5 = {}
kill6 = {}
kill7 = {}
kill8 = {}
killr = {}
selfkills = {}
teamkillr = {}
khp = {}
PlayerName = {}
Bname = {}

EV_GLOBAL_CLIENT_SOUND = 54
et.CS_PLAYERS = 689
EV_GENERAL_SOUND = 49

team = { "AXIS" , "ALLIES" , "SPECTATOR" }
class = { [0]="SOLDIER" , "MEDIC" , "ENGINEER" , "FIELD OPS" , "COVERT OPS" }

--AdminLV0 = {}
--AdminLV1 = {}
--AdminLV2 = {}
--AdminLV3 = {}
AdminLV = {}
for z=0, 9999, 1 do
	AdminLV[z] = {}
end
chkGUID = {}
AdminName = {}
originalclass = {}
originalweap = {}

et.MAX_WEAPONS = 50
GAMEPAUSED = 0

pweapons = {
	nil,	--// 1
	false,	--WP_LUGER,				// 2
	false,	--WP_MP40,				// 3
	false,	--WP_GRENADE_LAUNCHER,	// 4
	true,	--WP_PANZERFAUST,		// 5
	false,	--WP_FLAMETHROWER,		// 6
	false,	--WP_COLT,				// 7	// equivalent american weapon to german luger
	false,	--WP_THOMPSON,			// 8	// equivalent american weapon to german mp40
	false,	--WP_GRENADE_PINEAPPLE,	// 9
	false,	--WP_STEN,				// 10	// silenced sten sub-machinegun
	false,	--WP_MEDIC_SYRINGE,		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	false,	--WP_AMMO,				// 12	// JPW NERVE likewise
	false,	--WP_ARTY,				// 13
	false,	--WP_SILENCER,			// 14	// used to be sp5
	false,	--WP_DYNAMITE,			// 15
	nil,	--// 16
	nil,	--// 17
	nil,		--// 18
	false,	--WP_MEDKIT,			// 19
	false,	--WP_BINOCULARS,		// 20
	nil,	--// 21
	nil,	--// 22
	false,	--WP_KAR98,				// 23	// WolfXP weapons
	false,	--WP_CARBINE,			// 24
	false,	--WP_GARAND,			// 25
	false,	--WP_LANDMINE,			// 26
	false,	--WP_SATCHEL,			// 27
	false,	--WP_SATCHEL_DET,		// 28
	nil,	--// 29
	false,	--WP_SMOKE_BOMB,		// 30
	false,	--WP_MOBILE_MG42,		// 31
	false,	--WP_K43,				// 32
	false,	--WP_FG42,				// 33
	nil,	--// 34
	false,	--WP_MORTAR,			// 35
	nil,	--// 36
	false,	--WP_AKIMBO_COLT,		// 37
	false,	--WP_AKIMBO_LUGER,		// 38
	nil,	--// 39
	nil,	--// 40
	false,	--WP_SILENCED_COLT,		// 41
	false,	--WP_GARAND_SCOPE,		// 42
	false,	--WP_K43_SCOPE,			// 43
	false,	--WP_FG42SCOPE,			// 44
	false,	--WP_MORTAR_SET,		// 45
	false,	--WP_MEDIC_ADRENALINE,	// 46
	false,	--WP_AKIMBO_SILENCEDCOLT,// 47
	false	--WP_AKIMBO_SILENCEDLUGER,// 48
}

fweapons = {
	nil,	--// 1
	true,	--WP_LUGER,				// 2
	true,	--WP_MP40,				// 3
	true,	--WP_GRENADE_LAUNCHER,	// 4
	true,	--WP_PANZERFAUST,		// 5
	true,	--WP_FLAMETHROWER,		// 6
	true,	--WP_COLT,				// 7	// equivalent american weapon to german luger
	true,	--WP_THOMPSON,			// 8	// equivalent american weapon to german mp40
	true,	--WP_GRENADE_PINEAPPLE,	// 9
	true,	--WP_STEN,				// 10	// silenced sten sub-machinegun
	true,	--WP_MEDIC_SYRINGE,		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	true,	--WP_AMMO,				// 12	// JPW NERVE likewise
	true,	--WP_ARTY,				// 13
	true,	--WP_SILENCER,			// 14	// used to be sp5
	true,	--WP_DYNAMITE,			// 15
	nil,	--// 16
	nil,	--// 17
	nil,		--// 18
	true,	--WP_MEDKIT,			// 19
	true,	--WP_BINOCULARS,		// 20
	nil,	--// 21
	nil,	--// 22
	true,	--WP_KAR98,				// 23	// WolfXP weapons
	true,	--WP_CARBINE,			// 24
	true,	--WP_GARAND,			// 25
	true,	--WP_LANDMINE,			// 26
	true,	--WP_SATCHEL,			// 27
	true,	--WP_SATCHEL_DET,		// 28
	nil,	--// 29
	true,	--WP_SMOKE_BOMB,		// 30
	true,	--WP_MOBILE_MG42,		// 31
	true,	--WP_K43,				// 32
	true,	--WP_FG42,				// 33
	nil,	--// 34
	true,	--WP_MORTAR,			// 35
	nil,	--// 36
	true,	--WP_AKIMBO_COLT,		// 37
	true,	--WP_AKIMBO_LUGER,		// 38
	nil,	--// 39
	nil,	--// 40
	true,	--WP_SILENCED_COLT,		// 41
	true,	--WP_GARAND_SCOPE,		// 42
	true,	--WP_K43_SCOPE,			// 43
	true,	--WP_FG42SCOPE,			// 44
	true,	--WP_MORTAR_SET,		// 45
	false,	--WP_MEDIC_ADRENALINE,	// 46
	true,	--WP_AKIMBO_SILENCEDCOLT,// 47
	true	--WP_AKIMBO_SILENCEDLUGER,// 48
}

aweapons = {
	nil,	--// 1
	true,	--WP_LUGER,				// 2
	true,	--WP_MP40,				// 3
	true,	--WP_GRENADE_LAUNCHER,	// 4
	true,	--WP_PANZERFAUST,		// 5
	true,	--WP_FLAMETHROWER,		// 6
	true,	--WP_COLT,				// 7	// equivalent american weapon to german luger
	true,	--WP_THOMPSON,			// 8	// equivalent american weapon to german mp40
	true,	--WP_GRENADE_PINEAPPLE,	// 9
	true,	--WP_STEN,				// 10	// silenced sten sub-machinegun
	true,	--WP_MEDIC_SYRINGE,		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	true,	--WP_AMMO,				// 12	// JPW NERVE likewise
	true,	--WP_ARTY,				// 13
	true,	--WP_SILENCER,			// 14	// used to be sp5
	true,	--WP_DYNAMITE,			// 15
	nil,	--// 16
	nil,	--// 17
	nil,		--// 18
	true,	--WP_MEDKIT,			// 19
	true,	--WP_BINOCULARS,		// 20
	nil,	--// 21
	nil,	--// 22
	true,	--WP_KAR98,				// 23	// WolfXP weapons
	true,	--WP_CARBINE,			// 24
	true,	--WP_GARAND,			// 25
	true,	--WP_LANDMINE,			// 26
	true,	--WP_SATCHEL,			// 27
	true,	--WP_SATCHEL_DET,		// 28
	nil,	--// 29
	true,	--WP_SMOKE_BOMB,		// 30
	true,	--WP_MOBILE_MG42,		// 31
	true,	--WP_K43,				// 32
	true,	--WP_FG42,				// 33
	nil,	--// 34
	true,	--WP_MORTAR,			// 35
	nil,	--// 36
	true,	--WP_AKIMBO_COLT,		// 37
	true,	--WP_AKIMBO_LUGER,		// 38
	nil,	--// 39
	nil,	--// 40
	true,	--WP_SILENCED_COLT,		// 41
	true,	--WP_GARAND_SCOPE,		// 42
	true,	--WP_K43_SCOPE,			// 43
	true,	--WP_FG42SCOPE,			// 44
	true,	--WP_MORTAR_SET,		// 45
	false,	--WP_MEDIC_ADRENALINE,	// 46
	true,	--WP_AKIMBO_SILENCEDCOLT,// 47
	true	--WP_AKIMBO_SILENCEDLUGER,// 48
}

gweapons = {
	nil,	--// 1
	false,	--WP_LUGER,				// 2
	false,	--WP_MP40,				// 3
	true,	--WP_GRENADE_LAUNCHER,	// 4
	false,	--WP_PANZERFAUST,		// 5
	false,	--WP_FLAMETHROWER,		// 6
	false,	--WP_COLT,				// 7	// equivalent american weapon to german luger
	false,	--WP_THOMPSON,			// 8	// equivalent american weapon to german mp40
	true,	--WP_GRENADE_PINEAPPLE,	// 9
	false,	--WP_STEN,				// 10	// silenced sten sub-machinegun
	false,	--WP_MEDIC_SYRINGE,		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	false,	--WP_AMMO,				// 12	// JPW NERVE likewise
	false,	--WP_ARTY,				// 13
	false,	--WP_SILENCER,			// 14	// used to be sp5
	false,	--WP_DYNAMITE,			// 15
	nil,	--// 16
	nil,	--// 17
	nil,		--// 18
	false,	--WP_MEDKIT,			// 19
	false,	--WP_BINOCULARS,		// 20
	nil,	--// 21
	nil,	--// 22
	false,	--WP_KAR98,				// 23	// WolfXP weapons
	false,	--WP_CARBINE,			// 24
	false,	--WP_GARAND,			// 25
	false,	--WP_LANDMINE,			// 26
	false,	--WP_SATCHEL,			// 27
	false,	--WP_SATCHEL_DET,		// 28
	nil,	--// 29
	false,	--WP_SMOKE_BOMB,		// 30
	false,	--WP_MOBILE_MG42,		// 31
	false,	--WP_K43,				// 32
	false,	--WP_FG42,				// 33
	nil,	--// 34
	false,	--WP_MORTAR,			// 35
	nil,	--// 36
	false,	--WP_AKIMBO_COLT,		// 37
	false,	--WP_AKIMBO_LUGER,		// 38
	nil,	--// 39
	nil,	--// 40
	false,	--WP_SILENCED_COLT,		// 41
	false,	--WP_GARAND_SCOPE,		// 42
	false,	--WP_K43_SCOPE,			// 43
	false,	--WP_FG42SCOPE,			// 44
	false,	--WP_MORTAR_SET,		// 45
	false,	--WP_MEDIC_ADRENALINE,	// 46
	false,	--WP_AKIMBO_SILENCEDCOLT,// 47
	false	--WP_AKIMBO_SILENCEDLUGER,// 48
}

sweapons = {
	nil,	--// 1
	false,	--WP_LUGER,				// 2
	false,	--WP_MP40,				// 3
	false,	--WP_GRENADE_LAUNCHER,	// 4
	false,	--WP_PANZERFAUST,		// 5
	false,	--WP_FLAMETHROWER,		// 6
	false,	--WP_COLT,				// 7	// equivalent american weapon to german luger
	false,	--WP_THOMPSON,			// 8	// equivalent american weapon to german mp40
	false,	--WP_GRENADE_PINEAPPLE,	// 9
	false,	--WP_STEN,				// 10	// silenced sten sub-machinegun
	false,	--WP_MEDIC_SYRINGE,		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	false,	--WP_AMMO,				// 12	// JPW NERVE likewise
	false,	--WP_ARTY,				// 13
	false,	--WP_SILENCER,			// 14	// used to be sp5
	false,	--WP_DYNAMITE,			// 15
	nil,	--// 16
	nil,	--// 17
	nil,		--// 18
	false,	--WP_MEDKIT,			// 19
	false,	--WP_BINOCULARS,		// 20
	nil,	--// 21
	nil,	--// 22
	false,	--WP_KAR98,				// 23	// WolfXP weapons
	false,	--WP_CARBINE,			// 24
	true,	--WP_GARAND,			// 25
	false,	--WP_LANDMINE,			// 26
	false,	--WP_SATCHEL,			// 27
	nil,	--WP_SATCHEL_DET,		// 28
	nil,	--// 29
	false,	--WP_SMOKE_BOMB,		// 30
	false,	--WP_MOBILE_MG42,		// 31
	true,	--WP_K43,				// 32
	true,	--WP_FG42,				// 33
	nil,	--// 34
	false,	--WP_MORTAR,			// 35
	nil,	--// 36
	false,	--WP_AKIMBO_COLT,		// 37
	false,	--WP_AKIMBO_LUGER,		// 38
	nil,	--// 39
	nil,	--// 40
	false,	--WP_SILENCED_COLT,		// 41
	true,	--WP_GARAND_SCOPE,		// 42
	true,	--WP_K43_SCOPE,			// 43
	true,	--WP_FG42SCOPE,			// 44
	false,	--WP_MORTAR_SET,		// 45
	false,	--WP_MEDIC_ADRENALINE,	// 46
	false,	--WP_AKIMBO_SILENCEDCOLT,// 47
	false	--WP_AKIMBO_SILENCEDLUGER,// 48
}

--[[
lvl0c = 5
lvl0 = {
	k_commandprefix.."admintest",
	k_commandprefix.."time",
	k_commandprefix.."date",
	k_commandprefix.."spree_record",
	k_commandprefix.."tk_index",
}

lvl1c = 1
lvl1 = {
	k_commandprefix.."spec999",
}

lvl2c = 1
lvl2 = {
	k_commandprefix.."finger",
}

lvl3c = 13
lvl3 = {
	k_commandprefix.."ban",
	k_commandprefix.."readconfig",
	k_commandprefix.."spree_restart",
	k_commandprefix.."getip",
	k_commandprefix.."crazygravity",
	k_commandprefix.."getguid",
	k_commandprefix.."setlevel",
	k_commandprefix.."gib",
	k_commandprefix.."slap",
	k_commandprefix.."panzerwar",
	k_commandprefix.."frenzy",
	k_commandprefix.."grenadewar",
	k_commandprefix.."sniperwar",
}
--]]
lvls = {}
lvlsc = {}

numAxisPlayers = 0
numAlliedPlayers = 0
active_players = 0
total_players = 0

firstblood = 0
lastblood = ""
oldspree = ""
oldspree2 = ""
intmrecord = ""
oldmapspree = ""
oldmapspree2 = ""
intmMaprecord = ""

panzer_antiloop = 0
panzer_antiloop1 = 0
panzer_antiloop2 = 0
panzers_enabled = 0

panzers = ""
medics = ""
cvops = ""
fops = ""
engie = ""
flamers = ""
mortars = ""
mg42s = ""
soldcharge = ""
speed = ""
redlimbo = ""
bluelimbo = ""

floodprotect = 0
commandSaid = false
kick = false
fullcom = ""
finger = false
removereferee = false
makereferee = false
removeshoutcaster = false
makeshoutcaster = false
putspec = false
putallies = false
putaxis = false
unmute = false
mute = false
warn = false
ban = false
crazygravity = false
crazytime = 0

--[[
--Defaults

killingspreesound="sound/misc/killingspree.wav"
rampagesound="sound/misc/rampage.wav"
dominatingsound="sound/misc/dominating.wav"
unstopablesound="sound/misc/unstoppable.wav"
godlikesound="sound/misc/godlike.wav"
wickedsicksound="sound/misc/wickedsick.wav"
flakmonkeysound="sound/misc/flakmonkey.wav"
firstbloodsound="sound/misc/firstblood.wav"

deathspreesound1="sound/misc/humiliation.wav"
deathspreesound2="sound/misc/you_suck.wav"
deathspreesound3="sound/misc/ae821.wav"

doublekillsound="sound/misc/doublekill.wav"
multikillsound="sound/misc/multikill.wav"
ultrakillsound="sound/misc/ultrakill.wav"
monsterkillsound="sound/misc/monsterkill.wav"
ludicrouskillsound="sound/misc/ludicrouskill.wav"

k_spreesounds = 1
k_sprees = 1 -- includes sounds
k_multikillsounds = 1
k_multikills = 1 --includes sounds
k_flakmonkeysound = 1
k_flakmonkey = 1 --includes sounds
k_firstbloodsound = 1
k_firstblood = 1 --includes sound
k_lastblood = 1
k_killerhpdisplay = 1

k_spreerecord = 1
k_advplayers = 1
k_crazygravityinterval = 30
k_slashkilllimit = 1
k_slashkills = 3
k_teamkillrestriction = 1
k_tklimit_high = 3
k_tklimit_low = -3
k_color = "^2"
k_advancedpms = 1
k_playdead = 0
k_logchat = 1
k_disablevotes = 1
k_dvmode = 2
k_dvpercent = 90
k_adrensound = 1
k_advancedadrenaline = 1
k_noisereduction = 0
k_endroundshuffle = 0
k_deathsprees = 1
k_deathspreesounds = 1
k_antiunmute = 1
k_advancedspawn = 0
--]]

pmsound = "sound/misc/NewBeep.wav"

antiloop = 0
antiloop2 = 0
antiloop3 = 0
antiloop4 = 0
antiloopes = 0
antilooppw = 0
confirm = 0
spreerecordkills = 0
mapspreerecordkills = 0
crazydv = 1
CGactive = 0
panzdv = 0
sldv = 0
frenzdv = 0
grendv = 0
snipdv = 0
antiloopm = 0
pausedv = 0
pausedv2 = 0
pausetime = 0
timedv = 0
timedvs = 0
refreshrate = 0
timedelay_antiloop = 0
egamemodes = 0
run_once = 0

for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
	antiloopadr1[i] = 0
	antiloopadr2[i] = 0
	adrenaline[i] = 0
	adrnum[i] = 0
	adrnum2[i] = 0
	adrtime[i] = 0
	adrtime2[i] = 0
end

k_Admin = {}

qwerty = 0

function et_RunFrame( levelTime )
	mtime=tonumber(levelTime) -- still cannot remember why i made this but its used in alot of stuff so i'll leave it

	if run_once == 0 then
		k_panzersperteam2 = tonumber(et.trap_Cvar_Get("team_maxpanzers"))
		run_once = 1
	end
	if timedvs == 0 then
		local ktime = (((mtime - initTime)/1000))
		timecounter = ktime
		timedvs = 1
	end

	timelimit=tonumber(et.trap_Cvar_Get("timelimit"))

	Gamestate=tonumber(et.trap_Cvar_Get("gamestate"))

	if GAMEPAUSED == 1 then
		if pausedv == 0 then
			pausetime = mtime
			pausedv = 1
		end

		if ((mtime - pausetime)/1000) >= 180 then    -- Server is paused for 3 minutes (180 seconds)
			GAMEPAUSED = 0
		end
	elseif GAMEPAUSED == 0 and pausedv == 1 then
		if pausedv2 == 0 then
			pausetime = mtime
			pausedv2 = 1
		end
		if ((mtime - pausetime)/1000) >= 10 then     -- when unpaused before 3 minutes is up it counts down from 10 seconds
			pausedv = 0
			pausedv2 = 0
			timedv1 = nil
			timedv2 = nil
		end
	else
		if timedv == 0 then
			timedv1 = mtime
			timedv = 1
			if type(timedv2) ~= "nil" then
				timecounter=timecounter+((timedv1 - timedv2)/1000)
				s,e,thous = string.find(timecounter, "%d*%.%d%d(%d*)")
--				s,e = string.find(thous, "9")
				if thous == 9999999 then
					timecounter=timecounter+0.000000001
				end
			end
		else
			timedv2 = mtime
			timedv = 0
			timecounter=timecounter+((timedv2 - timedv1)/1000)
			s,e,thous = string.find(timecounter, "%d*%.%d%d(%d*)")
--			s,e = string.find(thous, "9")
			if thous == 9999999 then
				timecounter=timecounter+0.000000001
			end
		end

--		timecounter=timecounter+0.05
	end

	killingspreesound = tostring(et.trap_Cvar_Get("killingspreesound"))
	k_color = tostring(et.trap_Cvar_Get("k_color"))
	rampagesound = tostring(et.trap_Cvar_Get("rampagesound"))
	dominatingsound = tostring(et.trap_Cvar_Get("dominatingsound"))
	unstopablesound = tostring(et.trap_Cvar_Get("unstopablesound"))
	godlikesound = tostring(et.trap_Cvar_Get("godlikesound"))
	wickedsicksound = tostring(et.trap_Cvar_Get("wickedsicksound"))
	flakmonkeysound = tostring(et.trap_Cvar_Get("flakmonkeysound"))
	firstbloodsound = tostring(et.trap_Cvar_Get("firstbloodsound"))
	deathspreesound1 = tostring(et.trap_Cvar_Get("deathspreesound1"))
	deathspreesound2 = tostring(et.trap_Cvar_Get("deathspreesound2"))
	deathspreesound3 = tostring(et.trap_Cvar_Get("deathspreesound3"))
	doublekillsound = tostring(et.trap_Cvar_Get("doublekillsound"))
	multikillsound = tostring(et.trap_Cvar_Get("multikillsound"))
	megakillsound = tostring(et.trap_Cvar_Get("megakillsound"))
	ultrakillsound = tostring(et.trap_Cvar_Get("ultrakillsound"))
	monsterkillsound = tostring(et.trap_Cvar_Get("monsterkillsound"))
	ludicrouskillsound = tostring(et.trap_Cvar_Get("ludicrouskillsound"))
	holyshitsound = tostring(et.trap_Cvar_Get("holyshitsound"))
	k_ds_message1 = tostring(et.trap_Cvar_Get("k_ds_message1"))
	k_ds_message2 = tostring(et.trap_Cvar_Get("k_ds_message2"))
	k_ds_message3 = tostring(et.trap_Cvar_Get("k_ds_message3"))
	k_ks_message1 = tostring(et.trap_Cvar_Get("k_ks_message1"))
	k_ks_message2 = tostring(et.trap_Cvar_Get("k_ks_message2"))
	k_ks_message3 = tostring(et.trap_Cvar_Get("k_ks_message3"))
	k_ks_message4 = tostring(et.trap_Cvar_Get("k_ks_message4"))
	k_ks_message5 = tostring(et.trap_Cvar_Get("k_ks_message5"))
	k_ks_message6 = tostring(et.trap_Cvar_Get("k_ks_message6"))
	k_mk_message1 = tostring(et.trap_Cvar_Get("k_mk_message1"))
	k_mk_message2 = tostring(et.trap_Cvar_Get("k_mk_message2"))
	k_mk_message3 = tostring(et.trap_Cvar_Get("k_mk_message3"))
	k_mk_message4 = tostring(et.trap_Cvar_Get("k_mk_message4"))
	k_mk_message5 = tostring(et.trap_Cvar_Get("k_mk_message5"))
	k_mk_message6 = tostring(et.trap_Cvar_Get("k_mk_message6"))
	k_mk_message7 = tostring(et.trap_Cvar_Get("k_mk_message7"))
	k_fm_message = tostring(et.trap_Cvar_Get("k_fm_message"))
	k_end_message1 = tostring(et.trap_Cvar_Get("k_end_message1"))
	k_end_message2 = tostring(et.trap_Cvar_Get("k_end_message2"))
	k_end_message3 = tostring(et.trap_Cvar_Get("k_end_message3"))
	k_end_message4 = tostring(et.trap_Cvar_Get("k_end_message4"))
	k_fb_message = tostring(et.trap_Cvar_Get("k_fb_message"))
	k_lb_message = tostring(et.trap_Cvar_Get("k_lb_message"))
	k_autopanzerdisable = tonumber(et.trap_Cvar_Get("k_autopanzerdisable"))
	k_panzerplayerlimit = tonumber(et.trap_Cvar_Get("k_panzerplayerlimit"))
	k_panzersperteam = tonumber(et.trap_Cvar_Get("k_panzersperteam"))
	k_spreesounds = tonumber(et.trap_Cvar_Get("k_spreesounds"))
	k_sprees = tonumber(et.trap_Cvar_Get("k_sprees"))
	k_multikillsounds = tonumber(et.trap_Cvar_Get("k_multikillsounds"))
	k_multikills = tonumber(et.trap_Cvar_Get("k_multikills"))
	k_flakmonkeysound = tonumber(et.trap_Cvar_Get("k_flakmonkeysound"))
	k_flakmonkey = tonumber(et.trap_Cvar_Get("k_flakmonkey"))
	k_firstbloodsound = tonumber(et.trap_Cvar_Get("k_firstbloodsound"))
	k_firstblood = tonumber(et.trap_Cvar_Get("k_firstblood"))
	k_lastblood = tonumber(et.trap_Cvar_Get("k_lastblood"))
	k_killerhpdisplay = tonumber(et.trap_Cvar_Get("k_killerhpdisplay"))
	k_deathsprees = tonumber(et.trap_Cvar_Get("k_deathsprees"))
	k_deathspreesounds = tonumber(et.trap_Cvar_Get("k_deathspreesounds"))
	k_spreerecord = tonumber(et.trap_Cvar_Get("k_spreerecord"))
	k_advplayers = tonumber(et.trap_Cvar_Get("k_advplayers"))
	k_crazygravityinterval = tonumber(et.trap_Cvar_Get("k_crazygravityinterval"))
	k_teamkillrestriction = tonumber(et.trap_Cvar_Get("k_teamkillrestriction"))
	k_tklimit_high = tonumber(et.trap_Cvar_Get("k_tklimit_high"))
	k_tklimit_low = tonumber(et.trap_Cvar_Get("k_tklimit_low"))
	k_tk_protect = tonumber(et.trap_Cvar_Get("k_tk_protect"))
	k_slashkilllimit = tonumber(et.trap_Cvar_Get("k_slashkilllimit"))
	k_slashkills = tonumber(et.trap_Cvar_Get("k_slashkills"))
	k_endroundshuffle = tonumber(et.trap_Cvar_Get("k_endroundshuffle"))
	k_noisereduction = tonumber(et.trap_Cvar_Get("k_noisereduction"))
	k_advancedpms = tonumber(et.trap_Cvar_Get("k_advancedpms"))
	k_logchat = tonumber(et.trap_Cvar_Get("k_logchat"))
	k_disablevotes = tonumber(et.trap_Cvar_Get("k_disablevotes"))
	k_dvmode = tonumber(et.trap_Cvar_Get("k_dvmode"))
	k_dvtime = tonumber(et.trap_Cvar_Get("k_dvtime"))
	k_adrensound = tonumber(et.trap_Cvar_Get("k_adrensound"))
	k_advancedadrenaline = tonumber(et.trap_Cvar_Get("k_advancedadrenaline"))
	k_antiunmute = tonumber(et.trap_Cvar_Get("k_antiunmute"))
	k_advancedspawn = tonumber(et.trap_Cvar_Get("k_advancedspawn"))
	k_deathspree1_amount = tonumber(et.trap_Cvar_Get("k_deathspree1_amount"))
	k_deathspree2_amount = tonumber(et.trap_Cvar_Get("k_deathspree2_amount"))
	k_deathspree3_amount = tonumber(et.trap_Cvar_Get("k_deathspree3_amount"))
	k_spree1_amount = tonumber(et.trap_Cvar_Get("k_spree1_amount"))
	k_spree2_amount = tonumber(et.trap_Cvar_Get("k_spree2_amount"))
	k_spree3_amount = tonumber(et.trap_Cvar_Get("k_spree3_amount"))
	k_spree4_amount = tonumber(et.trap_Cvar_Get("k_spree4_amount"))
	k_spree5_amount = tonumber(et.trap_Cvar_Get("k_spree5_amount"))
	k_spree6_amount = tonumber(et.trap_Cvar_Get("k_spree6_amount"))
	k_multikill_time = tonumber(et.trap_Cvar_Get("k_multikill_time"))
	k_ds_location = tonumber(et.trap_Cvar_Get("k_ds_location"))
	k_ks_location = tonumber(et.trap_Cvar_Get("k_ks_location"))
	k_mk_location = tonumber(et.trap_Cvar_Get("k_mk_location"))
	k_fm_location = tonumber(et.trap_Cvar_Get("k_fm_location"))
	k_fb_location = tonumber(et.trap_Cvar_Get("k_fb_location"))
	k_lb_location = tonumber(et.trap_Cvar_Get("k_lb_location"))


	if k_ds_location == 2 then
		ds_location = "cp"
	elseif k_ds_location == 3 then
		ds_location = "bp"
	else
		ds_location = "qsay"
	end

	if k_ks_location == 2 then
		ks_location = "cp"
	elseif k_ks_location == 3 then
		ks_location = "bp"
	else
		ks_location = "qsay"
	end

	if k_mk_location == 2 then
		mk_location = "cp"
	elseif k_mk_location == 3 then
		mk_location = "bp"
	else
		mk_location = "qsay"
	end

	if k_fm_location == 2 then
		fm_location = "cp"
	elseif k_fm_location == 3 then
		fm_location = "bp"
	else
		fm_location = "qsay"
	end

	if k_fb_location == 2 then
		fb_location = "cp"
	elseif k_fb_location == 3 then
		fb_location = "bp"
	else
		fb_location = "qsay"
	end

	if k_lb_location == 2 then
		lb_location = "cp"
	elseif k_lb_location == 3 then
		lb_location = "bp"
	else
		lb_location = "qsay"
	end

	if k_advancedspawn == 1 and tonumber(et.trap_Cvar_Get("g_inactivity")) > 0 then -- g_inactivity is required or this will not work
		for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			if switchteam[i] == 1 then et.gentity_set(i,"ps.powerups", 1, 0) end
			switchteam[i] = 0
		end

		for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			if clientrespawn[i] == 1 then
				if (switchteam[i] == 0 and et.gentity_get(i,"ps.powerups", 1) > 0) then
					if invincDummy[i] == 0 then
						invincStart[i] = tonumber(et.gentity_get(i,"client.inactivityTime"))
						invincDummy[i] = 1
					end
					local inactivity = tonumber(et.gentity_get(i,"client.inactivityTime"))
					if inactivity == invincStart[i] then
						local timer = mtime + 3000
						et.gentity_set(i,"ps.powerups", 1, timer)
					else
						clientrespawn[i] = 0
						invincDummy[i] = 0
					end
				else
					clientrespawn[i] = 0
					invincDummy[i] = 0
				end
			end
		end
	end

	if panzdv == 1 or frenzdv == 1 or grendv == 1 or snipdv == 1 then
		if timedelay_antiloop == 0 then
			refreshrate = mtime
			timedelay_antiloop = 1
		end

		if ((mtime-refreshrate)/1000) >= 0.25 then -- reset ammo and stuff every 0.25 of a second rather than 0.05 of a second (which caused lag)
			egamemodes = 1
			timedelay_antiloop = 0
		else
			egamemodes = 0
		end
	else
		egamemodes = 0
	end

	if tonumber(et.trap_Cvar_Get("g_spectatorInactivity")) > 0 then
		for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			if AdminUserLevel(i) >= 1 then
				if et.gentity_get(i,"sess.sessionTeam") >= 3 or et.gentity_get(i,"sess.sessionTeam") < 1 then
					et.gentity_set(i,"client.inactivityTime", mtime)
					et.gentity_set(i,"client.inactivityWarning", 1)
				end
			end
		end
	end

	if panzdv == 1 then
		if egamemodes == 1 then
		for q=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			et.gentity_set(q,"sess.latchPlayerWeapon",5)
			for i=1,(et.MAX_WEAPONS-1),1 do
				if not pweapons[i] then
					et.gentity_set(q,"ps.ammoclip",i,0)
					et.gentity_set(q,"ps.ammo",i,0)
				else
					et.gentity_set(q,"ps.ammoclip",i,999)
					et.gentity_set(q,"ps.ammo",i,999)
				end
			end
		end
		end
	elseif frenzdv == 1 then
		if egamemodes == 1 then
		for w=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			for i=1,(et.MAX_WEAPONS-1),1 do
				if not fweapons[i] then
					et.gentity_set(w,"ps.ammoclip",i,0)
					et.gentity_set(w,"ps.ammo",i,0)
				else
					et.gentity_set(w,"ps.ammoclip",i,999)
					et.gentity_set(w,"ps.ammo",i,999)
				end
			end
		end
		end
	elseif grendv == 1 then
		if egamemodes == 1 then
		for e=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			for i=1,(et.MAX_WEAPONS-1),1 do
				if not gweapons[i] then
					et.gentity_set(e,"ps.ammoclip",i,0)
					et.gentity_set(e,"ps.ammo",i,0)
				else
					et.gentity_set(e,"ps.ammoclip",i,999)
					et.gentity_set(e,"ps.ammo",i,999)
				end
			end
		end
		end
	elseif snipdv == 1 then
		if egamemodes == 1 then
		for r=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			if tonumber(et.gentity_get(r,"sess.latchPlayerType")) ~= 4 then
				et.gentity_set(r,"sess.latchPlayerType",4)
			end
			if tonumber(et.gentity_get(r,"sess.latchPlayerWeapon")) ~= 32 or tonumber(et.gentity_get(r,"sess.latchPlayerWeapon")) ~= 25 or tonumber(et.gentity_get(r,"sess.latchPlayerWeapon")) ~= 42 or tonumber(et.gentity_get(r,"sess.latchPlayerWeapon")) ~= 43 then
				if tonumber(et.gentity_get(r,"sess.latchPlayerWeapon")) ~= 33 then
					if tonumber(et.gentity_get(r,"sess.sessionTeam")) == 1 then
						et.gentity_set(r,"sess.latchPlayerWeapon", 32)
					elseif tonumber(et.gentity_get(r,"sess.sessionTeam")) == 2 then
						et.gentity_set(r,"sess.latchPlayerWeapon", 25)
					end
				end
			end
			for i=1,(et.MAX_WEAPONS-1),1 do
				if not sweapons[i] then
					et.gentity_set(r,"ps.ammoclip",i,0)
					et.gentity_set(r,"ps.ammo",i,0)
				else
					et.gentity_set(r,"ps.ammoclip",i,999)
					et.gentity_set(r,"ps.ammo",i,999)
				end
			end
		end
		end
	else
		panzers = tonumber(et.trap_Cvar_Get("team_maxpanzers"))
		medics = tonumber(et.trap_Cvar_Get("team_maxmedics"))
		cvops = tonumber(et.trap_Cvar_Get("team_maxcovertops"))
		fops = tonumber(et.trap_Cvar_Get("team_maxfieldops"))
		engie = tonumber(et.trap_Cvar_Get("team_maxengineers"))
		flamers = tonumber(et.trap_Cvar_Get("team_maxflamers"))
		mortars = tonumber(et.trap_Cvar_Get("team_maxmortars"))
		mg42s = tonumber(et.trap_Cvar_Get("team_maxmg42s"))
		soldcharge = tonumber(et.trap_Cvar_Get("g_soldierchargetime"))
		speed = tonumber(et.trap_Cvar_Get("g_speed"))
	end

	if crazygravity then
		CGactive = 1
		crazygravity_gravity = math.random(10, 1200)


		if crazydv == 1 then
			crazytime = mtime + (k_crazygravityinterval*1000)
			crazydv = 0
		end



		if (crazytime-mtime)/1000 == 0 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Crazygravity: ^7The gravity has been changed to ^1" .. crazygravity_gravity .. "^7!\n" )
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "g_gravity " .. crazygravity_gravity .. "\n" )
			crazydv = 1
		elseif (crazytime-mtime)/1000 == 5 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Crazygravity: ^7The gravity will be changed in ^15^7 seconds!\n" )
		end
	end

	local ftime = ((mtime-initTime)/1000)

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		PlayerName[i] = et.gentity_get(i,"pers.netname")
		if not PlayerName[i] then
			PlayerName[i] = ""
		end
		if et.gentity_get(i,"pers.connected") ~= 2 then
			PlayerName[i] = ""
		end
	end

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		if not Bname[i] then
			Bname[i] = ""
		end
		if et.gentity_get(i,"pers.connected") ~= 2 then
			Bname[i] = ""
		end
	end

	if Gamestate == 3 then
		if k_lastblood == 1 then
			if antiloop == 0 then
				if lastblood then
					local str = string.gsub(k_lb_message, "#killer#", lastblood)

					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..lb_location.." "..str.."\n" )
				end
				for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
					local name = et.gentity_get(i,"pers.netname")
					if killingspree[i] >= 5 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^7" ..name.. "^1's Killing spree was ended! Due to Map's end.\n" )
					end
					killingspree[i] = 0
				end
				if k_spreerecord == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^" .. k_color .. "" .. tostring(intmrecord) .. "\n" )
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^" .. k_color .. "" .. tostring(intmMaprecord) .. "\n" )
				end
			antiloop = 1
			end
		end
		if k_endroundshuffle == 1 then
			if antiloopes == 0 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref shuffleteamsxp_norestart\n" )
				antiloopes = 1
			end
		end
		if panzdv == 1 or snipdv == 1 then
			if antilooppw == 0 then
				for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
					if et.gentity_get(p,"sess.sessionTeam") == 1 or et.gentity_get(p,"sess.sessionTeam") == 2 then
							et.gentity_set(p,"sess.latchPlayerType",originalclass[p])
							et.gentity_set(p,"sess.latchPlayerWeapon",originalweap[p])
					end
				end
				antilooppw = 1
			end
		end
	end

	if k_advancedpms == 1 then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "b_privatemessages 0\n" )
	else
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "b_privatemessages 2\n" )
	end

	if k_sprees == 0 then
		if antiloop2 == 0 then
			killingspreereset()
		end
		antiloop2 = 1
	elseif k_sprees == 1 then
		antiloop2 = 0
	end

	if k_deathsprees == 0 then
		if antiloop3 == 0 then
			dspreereset()
		end
		antiloop3 = 1
	elseif k_deathsprees == 1 then
		antiloop3 = 0
	end

	if k_flakmonkey == 0 then
		if antiloop4 == 0 then
			flakreset()
		end
		antiloop4 = 1
	elseif k_flakmonkey == 1 then
		antiloop4 = 0
	end

	if floodprotect == 1 then
		fpPtime = (mtime-fpProt)/1000
		if fpPtime >= 2 then
			floodprotect = 0
		end
	end

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		if et.gentity_get(i,"pers.connected") == 2 then
			if PlayerName[i] ~= Bname[i] then
				log_chat( Bname[i], "NAME_CHANGE", PlayerName[i] )
				Bname[i] = PlayerName[i]
			end
		end
	end

	if k_advancedadrenaline == 1 then
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		local adrentlimit = 10
		local adrensound = "sound/misc/regen.wav"
		if pausedv == 1 then
			adrendummy[i] = 1
		end

		if adrendummy[i] == 1 and tonumber(et.gentity_get(i,"ps.powerups", 12)) == 0 then
			adrendummy[i] = 0
		end
	
		if adrendummy[i] == 0 then
			if tonumber(et.gentity_get(i,"ps.powerups", 12)) > 0 then
	
				adrnum[i] = tonumber(et.gentity_get(i,"ps.powerups", 12))
				soundindex = et.G_SoundIndex(adrensound)
				local name = et.gentity_get(i,"pers.netname")
				if antiloopadr1[i] == 0 then
					adrtime[i] = mtime
					if k_adrensound == 1 then
						et.G_Sound( i,  soundindex)
					end
					antiloopadr1[i] = 1
				end
				if antiloopadr2[i] == 0 then
					adrtime2[i] = mtime
					adrnum2[i] = tonumber(et.gentity_get(i,"ps.powerups", 12))
					antiloopadr2[i] = 1
				end
				adrenaline[i] = 1
				local tottime = math.floor((((mtime - adrtime[i])/1000)+0.05))
				local tottime2 = math.floor((((mtime - adrtime2[i])/1000)+0.05))

				if tottime >= 1 then
					antiloopadr1[i] = 0
				end
				if adrnum[i] ~= adrnum2[i] then
					adrnum2[i] = tonumber(et.gentity_get(i,"ps.powerups", 12))
					if k_adrensound == 1 then
						et.G_Sound( i,  soundindex)
					end
					adrtime[i] = mtime
					adrtime2[i] = mtime
				end
				local atime = (adrentlimit - tottime2)

				et.trap_SendServerCommand(i, string.format("cp \"^3Adrenaline ^1".. atime .."\n\""))
			else
				adrenaline[i] = 0
				antiloopadr1[i] = 0
				antiloopadr2[i] = 0
				adrnum[i] = 0
				adrnum2[i] = 0
			end
		end
	end
	end


	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
--		if muted[i] > mtime then
--			if antiloopm == 0 then
--				et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7Countdown initated\n" )
--				antiloopm = 1
--			end
--		end

		local mute = et.gentity_get(i, "sess.muted")
		if muted[i] > 0 then
			if mtime > muted[i] then
				if mute == 1 then
					local name = et.gentity_get(i,"pers.netname")
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref unmute \""..i.."\"\n" )
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been auto unmuted.  Please watch your language!\n" )
				end
				muted[i] = 0
			elseif mtime < muted[i] then
				if mute == 0 then
					muted[i] = 0
					setMute(i, 0)
				end
			elseif mute == 0 then
				muted[i] = 0
			end
		elseif muted[i] == -1 then
			if mute == 0 then
				muted[i] = 0
			end
		end
	end


	if k_disablevotes == 1 then
		local timelimit = tonumber(et.trap_Cvar_Get("timelimit"))
		if k_dvmode == 1 then
			local cancel_time = ( timelimit - k_dvtime )
			if timecounter >= (cancel_time * 60) then
				if votedis == 0 then
					votedis = 1
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay XP-Shuffle / Map Restart / Swap Teams  / Match Reset and New Campaign votings are now DISABLED\n")
				end
			else
				if votedis == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay XP-Shuffle / Map Restart / Swap Teams  / Match Reset and New Campaign votings have been reenabled due to timelimit change\n")
				end
				votedis = 0
			end
		elseif k_dvmode == 3 then
			if timecounter >= (k_dvtime * 60) then
				if votedis == 0 then
					votedis = 1
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay XP-Shuffle / Map Restart / Swap Teams  / Match Reset and New Campaign votings are now DISABLED\n")
				end
			else
				if votedis == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay XP-Shuffle / Map Restart / Swap Teams  / Match Reset and New Campaign votings have been reenabled due to timelimit change\n")
				end
				votedis = 0
			end
		else
			local cancel_percent = ( timelimit * ( k_dvtime / 100 ) )
			if timecounter >= (cancel_percent * 60) then
				if votedis == 0 then
					votedis = 1
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay XP-Shuffle / Map Restart / Swap Teams  / Match Reset and New Campaign votings are now DISABLED\n")
				end
			else
				if votedis == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay XP-Shuffle / Map Restart / Swap Teams  / Match Reset and New Campaign votings have been reenabled due to timelimit change\n")
				end
				votedis = 0
			end
		end
	end

	numAxisPlayers = 0
	numAlliedPlayers = 0
	active_players = 0
	total_players = 0
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		if et.gentity_get(i,"pers.connected") == 2 then
			if et.gentity_get(i,"sess.sessionTeam") == 1 then
				numAxisPlayers = numAxisPlayers + 1
			elseif et.gentity_get(i,"sess.sessionTeam") == 2 then
				numAlliedPlayers = numAlliedPlayers + 1
			end

			if et.gentity_get(i,"sess.sessionTeam") == 1 or et.gentity_get(i,"sess.sessionTeam") == 2 then
				active_players = active_players + 1
			end
			total_players = total_players + 1
		end
	end

--	local k_autopanzerdisable = 0
--	local k_panzerplayerlimit = tonumber(et.trap_Cvar_Get("k_panzerplayerlimit"))
--	local k_panzerplayerlimit = 2
	local k_panzerwarning1 = "^3Panzerlimit:^7  Please switch to a different weapon or be automatically moved to spec in ^11^3 minute!"
	local k_panzerwarning2 = "^3Panzerlimit:^7  Please switch to a different weapon or be automatically moved to spec in ^130^3 Seconds!"
	local k_panzermoved = "^1You have been moved to spectator for having a panzerfaust after being warned twice to switch!"
--	local k_panzersperteam = tonumber(et.trap_Cvar_Get("k_panzersperteam"))
--	local k_panzersperteam = 1
	local active_panzers = 0
	if k_autopanzerdisable == 1 then
		if panzdv == 0 and frenzdv == 0 and grendv == 0 and snipdv == 0 then
			if active_players < k_panzerplayerlimit then
				--if active_players ~= 0 then
					for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
						if tonumber(et.gentity_get(i,"sess.latchPlayerWeapon")) == 5 then
							active_panzers = 1
							break
						end
					end 

					if panzer_antiloop == 0 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Panzerlimit:  ^7Panzers have been disabled.\n" )
						panzer_antiloop = 1
						panzers_enabled = 0
					end

					if active_panzers == 1 then
						if panzer_antiloop1 == 0 then
							panzertime = mtime
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay "..k_panzerwarning1.."\n" )
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxpanzers 0\n" )
							panzer_antiloop1 = 1
						end

						if ((mtime-panzertime)/1000) > 30 then
							if panzer_antiloop2 == 0 then
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay "..k_panzerwarning2.."\n" )
								panzer_antiloop2 = 1
							end
						end

						if ((mtime-panzertime)/1000) > 60 then
							for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if tonumber(et.gentity_get(i,"sess.latchPlayerWeapon")) == 5 then
									et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref remove ".. i .."\n" )
									et.gentity_set(i,"sess.latchPlayerWeapon",3)
									if k_advancedpms == 1 then
										et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 ".. i .." "..k_panzermoved.."\n" )
									else
										local name = et.gentity_get(PlayerID,"pers.netname")
										et.trap_SendConsoleCommand( et.EXEC_APPEND, "m \""..name.."\" "..k_panzermoved.."\n" )
									end
								end
							end
							active_panzers = 0
							panzer_antiloop = 0
							panzer_antiloop1 = 0
							panzer_antiloop2 = 0
						end
					else
						--et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxpanzers ".. k_panzersperteam .."\n" )
						active_panzers = 0
						--panzer_antiloop = 0
						panzer_antiloop1 = 0
						panzer_antiloop2 = 0

						--if panzers_enabled == 0 then
						--	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Panzerlimit: ^7Panzers have been auto-enabled.  Each team is allowed only ^1".. k_panzersperteam .."^7 panzer(s) per team!\n" )
						--	panzers_enabled = 1
						--end
					end
				--else
				--	if panzer_antiloop == 0 then
				--		--et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Panzerlimit:  ^7Panzers have been disabled.\n" )
				--		--et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxpanzers 0\n" )
				--		panzers_enabled = 0
				--		panzer_antiloop = 1
				--	end
				--end
			else
				active_panzers = 0
				panzer_antiloop = 0
				panzer_antiloop1 = 0
				panzer_antiloop2 = 0
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxpanzers ".. k_panzersperteam .."\n" )

				if panzers_enabled == 0 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Panzerlimit: ^7Panzers have been auto-enabled.  Each team is allowed only ^1".. k_panzersperteam .."^7 panzer(s) per team!\n" )
					panzers_enabled = 1
				end
			end
		end
	end

	local k_playerdependantmines = 1
	local k_minesperSD = 2
	local k_playersSD = 2
	local minelimit = 10
	--if k_playerdependantmines == 1 then
	--	if 
	--end
end

function mutechange(PlayerID, muteTime)
	local fdin,lenin = et.trap_FS_FOpenFile( "mutes.cfg", et.FS_READ )
	local fdout,lenout = et.trap_FS_FOpenFile( "mutestmp.cfg", et.FS_WRITE )
	local IP2 = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
	s,e,IP2 = string.find(IP2,"(%d+%.%d+%.%d+%.%d+)")
	local i = 0
	if lenin <= 0 then
		et.G_Print("There is no Mutes to remove \n")
	else
		local filestr = et.trap_FS_Read( fdin, lenin )
		for time,ip,name in string.gfind(filestr, "(%-*%d+)%s%-%s(%d+%.%d+%.%d+%.%d+)%s%-%s*([^%\n]*)") do
			if (ip~=IP2) then
				
				mute = time .. " - " .. ip .. " - " .. name .. "\n"
				et.trap_FS_Write( mute, string.len(mute) ,fdout )
			end
			i=i+1

		end
	end
	confirm2 = 1
	et.trap_FS_FCloseFile( fdin ) 
	et.trap_FS_FCloseFile( fdout )
	et.trap_FS_Rename( "mutestmp.cfg", "mutes.cfg" )
	loadMutes()
end

function loadMutes()
	local i = 0
	local i2 = 0
	local dv = 1
	local dv2 = 1
	chkIP = {}
	local fd,len = et.trap_FS_FOpenFile( "mutes.cfg", et.FS_READ )

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		chkIP[i] = 0
	end
	if len <= 0 then
		et.G_Print("WARNING: No Mutes Defined! \n")
	else
		local filestr = et.trap_FS_Read( fd, len )
		local i = 0

		for time,ip in string.gfind(filestr, "(%-*%d+)%s%-%s(%d+%.%d+%.%d+%.%d+)%s%-%s*") do
			muteDuration[ip] = time
			chkIP[i] = ip
			if dv == 1 then
				i=i+1
			end
		end
	end
	et.trap_FS_FCloseFile( fd ) 
end

function setMute(PlayerID, muteTime)
	local time = math.ceil(muteTime)
	local fdadm,len = et.trap_FS_FOpenFile( "mutes.cfg", et.FS_APPEND )
	local Name = et.Q_CleanStr(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "name" ))
	local IP = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
	s,e,IP = string.find(IP,"(%d+%.%d+%.%d+%.%d+)")
	local dv = 0

	if len == -1 then
		confirm2 = 1
		et.trap_FS_FCloseFile( fdadm )
	else 
		local fdread,length = et.trap_FS_FOpenFile( "mutes.cfg", et.FS_READ )
		local n = 0
		if length ~= 0 then
			local filestr = et.trap_FS_Read( fdread, length )
			for ip in string.gfind(filestr, "%-*%d+%s%-%s(%d+%.%d+%.%d+%.%d+)%s%-%s*") do
				n = n + 1
			end
		end
		et.trap_FS_FCloseFile( fdread )


		et.trap_FS_FCloseFile( fdadm ) -- Close the mutes file so that mutechange() can open the mutes.cfg

		for i=0, n, 1 do
			if chkIP[i] == IP then
--				et.trap_FS_FCloseFile( fdadm )
				dv = 1

				if dv == 1 then
					mutechange(PlayerID, time)
				end
				break
			end
		end


		if muted[PlayerID] > 0 or muted[PlayerID] == -1 then
			confirm2 = 1
		else
			confirm2 = 0
		end
	end
	if confirm2 == 1 then
		fdadm,len = et.trap_FS_FOpenFile( "mutes.cfg", et.FS_APPEND )
		if muted[PlayerID] ~= 0 then
			ADMIN = time .. " - " .. IP .. " - " .. Name .. "\n"
			confirm2 = 0
			et.trap_FS_Write( ADMIN, string.len(ADMIN) ,fdadm )
		else
			loadMutes()
		end
		et.trap_FS_FCloseFile( fdadm )
		loadMutes()
--		return 1
	else
--		et.trap_FS_FCloseFile( fdadm )
--		return 1
	end
end

function MuteCheck(PlayerID)
	local ip = ""
	local name = et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "name" )
	local ref = tonumber(et.gentity_get(PlayerID,"sess.referee"))
	if(PlayerID~=-1) then
		ip = et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" )
		s,e,ip = string.find(ip,"(%d+%.%d+%.%d+%.%d+)")
	else
		ip=-1
	end

	local fd,len = et.trap_FS_FOpenFile( "mutes.cfg", et.FS_READ )

	if len > 0 then
		local filestr = et.trap_FS_Read( fd, len )
		local i = 0

		for time in string.gfind(filestr, "(%-*%d+)%s%-%s%d+%.%d+%.%d+%.%d+%s%-%s*") do
			if chkIP[i] == ip then
				if ref == 0 then
					if tonumber(muteDuration[ip]) > 0 then
						muted[PlayerID] = mtime + tonumber((muteDuration[ip])*1000)
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Curse Filter:  ^7"..name.."^7 has not yet finished his mute sentance.  (^1".. muteDuration[ip] .."^7) seconds.\n" )
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute "..PlayerID.."\n" )
					elseif tonumber(muteDuration[ip]) == -1 then
						muted[PlayerID] = -1
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Curse Filter:  ^7"..name.."^7 has been permanently muted\n" )
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute "..PlayerID.."\n" )
					end
				end
			end
			i=i+1
		end
	end
	et.trap_FS_FCloseFile( fd )
end



function et_Print( text )
	local t = ParseString(text)

	if t[1] == "saneClientCommand:" and t[3] == "callvote" then
		local caller = tonumber(t[2])
		local vote = t[4]
		local target = tonumber(t[5])

		if vote == "kick" or vote == "mute" then
			if AdminUserLevel(caller) < AdminUserLevel(target) then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "cancelvote ; qsay Admins cannot be vote kicked or vote muted!\n")
			end
		end
	end

	if t[1] == "Medic_Revive:" then
		local reviver = tonumber(t[2])
		teamkillr[reviver] = teamkillr[reviver] + 1
		if teamkillr[reviver] > k_tklimit_high then
			teamkillr[reviver] = k_tklimit_high
		end
	end
end

function ParseString(inputString)
	local i = 1
	local t = {}
	for w in string.gfind(inputString, "([^%s]+)%s*") do
		t[i]=w
		i=i+1
	end
	return t
 end

function et_ShutdownGame ( restart )
	if panzdv == 1 then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; g_speed " .. speed .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
	elseif frenzdv == 1 then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
	elseif grendv == 1 then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
	elseif snipdv == 1 then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
	end

	et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxpanzers ".. k_panzersperteam .."\n" )

	if panzdv == 1 then
		for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
			if et.gentity_get(p,"sess.sessionTeam") == 1 or et.gentity_get(p,"sess.sessionTeam") == 2 then
					et.gentity_set(p,"sess.latchPlayerType",originalclass[p])
					et.gentity_set(p,"sess.latchPlayerWeapon",originalweap[p])
			end
		end
	end

	if k_logchat == 1 then
		log_chat( "DV", "START", "DV" )
	end

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		if et.gentity_get(i,"pers.connected") == 2 then
			if muted[i] > 0 then
				local muteDur = (muted[i]-mtime)/1000
				setMute(i, muteDur)
			elseif muted[i] == 0 then
				IP = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "ip" )
				s,e,IP = string.find(IP,"(%d+%.%d+%.%d+%.%d+)")
				if muteDuration[IP] ~= 0 then
					local muteDur = 0
					setMute(i, muteDur)
				end
			end
		end
	end
end

function panzerwar_reset()
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; g_speed " .. speed .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
end

function frenzy_reset()
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
end

function grenadewar_reset()
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
end

function sniperwar_reset()
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics " .. medics .. " ; team_maxcovertops " .. cvops .. " ; team_maxfieldops " .. fops .. " ; team_maxengineers " .. engie .. " ; team_maxflamers " .. flamers .. " ; team_maxmortars " .. mortars .. " ; team_maxmg42s " .. mg42s .. " ; team_maxpanzers " .. panzers .. " ; forcecvar g_soldierchargetime " .. soldcharge .. "\n" )
end

function et_ClientSpawn(clientNum,revived)
	if panzdv == 1 then
		local doublehealth = tonumber(et.gentity_get(clientNum,"health"))*2
		if et.gentity_get(clientNum,"sess.sessionTeam") >= 1 and et.gentity_get(clientNum,"sess.sessionTeam") < 3 then
			et.gentity_set(clientNum,"health", doublehealth)
		end
	end

	if revived == 0 then
		if et.gentity_get(clientNum,"sess.sessionTeam") >= 1 and et.gentity_get(clientNum,"sess.sessionTeam") < 3 then
			clientrespawn[clientNum] = 1
		end
	end
end

function killingspreereset()
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		local name = et.gentity_get(i,"pers.netname")
		if killingspree[i] >= 5 then
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." ^7" ..name.. "^1's Killing spree was ended! Sprees Disabled!\n" )
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^1Killingspree's disabled.  All sprees reset.\n" )
		end

		killingspree[i] = 0
	end
end

function dspreereset()
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		deathspree[i] = 0
	end
end

function flakreset()
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		flakmonkey[i] = 0
	end
end

function floodprotector()
	floodprotect = 1
	fpProt = tonumber(mtime)
end

function et_InitGame( levelTime, randomSeed, restart )
	k_maxAdminLevels = tonumber(et.trap_Cvar_Get("k_maxAdminLevels"))

	initTime = levelTime
	loadAdmins()

	loadspreerecord()
	loadmapspreerecord()
--	loadcommands()
	loadMutes()

	local currentver = et.trap_Cvar_Get("mod_version")
	et.RegisterModname( "KMOD version " .. KMODversion .. " " .. et.FindSelf() )
	et.trap_SendConsoleCommand(et.EXEC_APPEND, "forcecvar mod_version \"" .. currentver .. " - KMOD" .. KMODversion2 .. "\"" .. "\n" )

	k_panzersperteam = tonumber(et.trap_Cvar_Get("team_maxpanzers"))

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		killingspree[i] = 0
		flakmonkey[i] = 0
		deathspree[i] = 0
		multikill[i] = 0
		muted[i] = 0
		nummutes[i] = 0
		antiloopadr1[i] = 0
		antiloopadr2[i] = 0
		adrenaline[i] = 0
		adrnum[i] = 0
		adrnum2[i] = 0
		adrtime[i] = 0
		adrtime2[i] = 0
		adrendummy[i] = 0
		clientrespawn[i] = 0
		invincDummy[i] = 0
		switchteam[i] = 0
		gibbed[i] = 0

		playerwhokilled[i] = 1022
		killedwithweapk[i] = ""
		killedwithweapv[i] = ""
		playerlastkilled[i] = 1022
		selfkills[i] = 0
		teamkillr[i] = 0
		khp[i] = 0
		AdminName[i] = ""
		originalclass[i] = ""
		originalweap[i] = ""

		killr[i] = 0
	end

	readconfig()

	et.G_Print( "KMOD version " .. KMODversion .. " has been initialized...\n" )
end

function et_ClientDisconnect( clientNum)
		killingspree[clientNum] = 0
		flakmonkey[clientNum] = 0
		deathspree[clientNum] = 0
		multikill[clientNum] = 0
		nummutes[clientNum] = 0
		antiloopadr1[clientNum] = 0
		antiloopadr2[clientNum] = 0
		adrenaline[clientNum] = 0
		adrnum[clientNum] = 0
		adrnum2[clientNum] = 0
		adrtime[clientNum] = 0
		adrtime2[clientNum] = 0
		adrendummy[clientNum] = 0
		clientrespawn[clientNum] = 0
		invincDummy[clientNum] = 0
		switchteam[clientNum] = 0
		gibbed[clientNum] = 0

		playerwhokilled[clientNum] = 1022
		killedwithweapk[clientNum] = ""
		killedwithweapv[clientNum] = ""
		playerlastkilled[clientNum] = 1022
		selfkills[clientNum] = 0
		teamkillr[clientNum] = 0
		khp[clientNum] = 0
		AdminName[clientNum] = ""
		originalclass[clientNum] = ""
		originalweap[clientNum] = ""

		killr[clientNum] = 0

	kill1[clientNum] = ""
	kill2[clientNum] = ""
	kill3[clientNum] = ""
	kill4[clientNum] = ""
	kill5[clientNum] = ""
	kill6[clientNum] = ""

	PlayerName[clientNum] = ""

	if k_logchat == 1 then
		log_chat(clientNum, "DISCONNECT", "DV2")
	end

	if muted[clientNum] > 0 then
		local muteDur = (muted[clientNum]-mtime)/1000
		setMute(clientNum, muteDur)
		muted[clientNum] = 0
	elseif muted[clientNum] == 0 then
		local muteDur = 0
		setMute(clientNum, muteDur)
		muted[clientNum] = 0
	end
end

function spreerecord(PlayerID, kills2)
	local fdadm,len = et.trap_FS_FOpenFile( "sprees/spree_record.dat", et.FS_WRITE )
	local Name = et.Q_CleanStr(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "name" ))
	local date = os.date("%x %I:%M:%S%p")
	local kills = tonumber(kills2)

	SPREE = kills .. "@" .. date .. "@" .. Name

	et.trap_FS_Write( SPREE, string.len(SPREE) ,fdadm )
	et.trap_FS_FCloseFile( fdadm )
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^1New spree record: ^7" ..Name.. " ^7with^3 " ..kills.. "^7 kills  ^7" ..tostring(oldspree).. "\n" )
	loadspreerecord()
end

function spreerecord_reset()
	local fdadm,len = et.trap_FS_FOpenFile( "sprees/spree_record.dat", et.FS_WRITE )

	SPREE = ""

	et.trap_FS_Write( SPREE, string.len(SPREE) ,fdadm )
	et.trap_FS_FCloseFile( fdadm )
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Spree record: ^7Spree record has been reset!\n" )
	loadspreerecord()
end

function loadspreerecord()
	local fd,len = et.trap_FS_FOpenFile( "sprees/spree_record.dat", et.FS_READ )
	local kills = 0
	local date = ""
	local name = ""

	if len <= 0 then
		et.G_Print("WARNING: No spree record found! \n")
		oldspree = "^3[Old: ^7N/A^3]"
		oldspree2 = "^3Spree Record: ^7There is no current spree record"
		spreerecordkills = 0
	else
		local filestr = et.trap_FS_Read( fd, len )

		s,e,kills,date,name = string.find(filestr, "(%d+)%@(%d+%/%d+%/%d+%s%d+%:%d+%:%d+%a+)%@([^%\n]*)")
		
		spreerecordkills = tonumber(kills)
		oldspree = "^3[Old: ^7" ..name.. "^3 " .. kills .. "^7 @ " ..date.. "^3]"
		oldspree2 = "^3Spree Record: ^7" ..name.. "^7 with ^3" .. kills .. "^7 kills at " ..date
		intmrecord = "Current spree record: ^7" ..name.. "^7 with ^3" .. kills .. "^7 kills at " ..date

	end

	et.trap_FS_FCloseFile( fd ) 
end

function mapspreerecord(PlayerID, kills2)
	local mapname = tostring(et.trap_Cvar_Get("mapname"))
	local fdadm,len = et.trap_FS_FOpenFile( "sprees/"..mapname..".record", et.FS_WRITE )
	local Name = et.Q_CleanStr(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "name" ))
	local date = os.date("%x %I:%M:%S%p")
	local kills = tonumber(kills2)

	SPREE = kills .. "@" .. date .. "@" .. Name

	et.trap_FS_Write( SPREE, string.len(SPREE) ,fdadm )
	et.trap_FS_FCloseFile( fdadm )
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^1New Map spree record: ^7" ..Name.. " ^7with^3 " ..kills.. "^7 kills on "..mapname.."  ^7" ..tostring(oldmapspree).. "\n" )
	loadmapspreerecord()
end

function loadmapspreerecord()
	local mapname = tostring(et.trap_Cvar_Get("mapname"))
	local fd,len = et.trap_FS_FOpenFile( "sprees/"..mapname..".record", et.FS_READ )
	local kills = 0
	local date = ""
	local name = ""

	if len <= 0 then
		et.G_Print("WARNING: No spree record found! \n")
		oldmapspree = "^3[Old: ^7N/A^3]"
		oldmapspree2 = "^3Map Spree Record: ^7There is no current spree record"
		mapspreerecordkills = 0
	else
		local filestr = et.trap_FS_Read( fd, len )

		s,e,kills,date,name = string.find(filestr, "(%d+)%@(%d+%/%d+%/%d+%s%d+%:%d+%:%d+%a+)%@([^%\n]*)")
		
		mapspreerecordkills = tonumber(kills)
		oldmapspree = "^3[Old: ^7" ..name.. "^3 " .. kills .. "^7 @ " ..date.. "^3]"
		oldmapspree2 = "^3Map Spree Record: ^7" ..name.. "^7 with ^3" .. kills .. "^7 kills at " ..date.. " on the map of " ..mapname
		intmMaprecord = "Current Map spree record: ^7" ..name.. "^7 with ^3" .. kills .. "^7 kills at " ..date

	end

	et.trap_FS_FCloseFile( fd ) 
end

function et_ConsoleCommand() 
		if string.lower(et.trap_Argv(0)) == k_commandprefix.."setlevel" then  
			if (et.trap_Argc() < 2) then 
				et.G_Print("Setlevel is used to set admin status to a player.\n") 
				et.G_Print("useage: !setlevel \[name/PID\] \[level 0-3\]\n")
			return 1 
			end 
			commandSaid = false
			sldv = 1
			setlevel(et.trap_Argv(1), 0) 
			sldv = 0
			setlevel(et.trap_Argv(1), et.trap_Argv(2)) 
		return 1
		elseif string.lower(et.trap_Argv(0)) == "goto" then
			if (et.trap_Argc() < 2) then 
				et.G_Print("Goto is used to teleport one player to another player\n") 
				et.G_Print("useage: goto \[name/PID\] \[name/PID\]\n")
			return 1 
			end 
			goto(et.trap_Argv(1), et.trap_Argv(2))
		return 1
		elseif string.lower(et.trap_Argv(0)) == "iwant" then
			if (et.trap_Argc() < 2) then 
				et.G_Print("Iwant is used to teleport one player to another player\n") 
				et.G_Print("useage: iwant \[name/PID - Destination\] \[name/PID\]\n")
			return 1 
			end 
			iwant(et.trap_Argv(1), et.trap_Argv(2))
		return 1
		elseif string.lower(et.trap_Argv(0)) == k_commandprefix.."showadmins" then  
			showAdmins() 
		return 1
		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."panzerwar" ) then
			if Cvarct < 3 then
				et.G_Print("^3Panzerwar:^7 Disable or enable panzerwar \[0-1\]\n" )
			else
				local panz = tonumber(et.trap_Argv(1))
				local dspeed = (speed*2)
				if panz >= 0 and panz <= 1 then
					if panz == 1 then
						if panzdv == 0 then
							if frenzdv == 1 then
								et.G_Print("^3Panzerwar:^7 Frenzy mode must be disabled first\n" )
							elseif grendv == 1 then
								et.G_Print("^3Panzerwar:^7 Grenadewar must be disabled first\n" )
							elseif snipdv == 1 then
								et.G_Print("^3Panzerwar:^7 Sniperwar must be disabled first\n" )
							else
								et.G_Print("^3Panzerwar:^7 Panzerwar has been Enabled\n" )
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics 0 ; team_maxcovertops 0 ; team_maxfieldops 0 ; team_maxengineers 0 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers -1 ; g_speed " .. dspeed .. " ; forcecvar g_soldierchargetime 0\n" )
								panzdv = 1
								for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
									if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
										originalclass[p] = tonumber(et.gentity_get(p,"sess.latchPlayerType"))
										originalweap[p] = tonumber(et.gentity_get(p,"sess.latchPlayerWeapon"))
										if et.gentity_get(p,"health") > 0 then
											et.G_Damage(p, p, 1022, 400, 24, 0)
											et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
										end
									end
									et.gentity_set(p,"sess.latchPlayerType",0)
									et.gentity_set(p,"sess.latchPlayerWeapon",5)
								end
							end
						else
							et.G_Print("^3Panzerwar:^7 Panzerwar is already active\n" )
						end
					else
						if panzdv == 1 then
							et.G_Print("^3Panzerwar:^7 Panzerwar has been Disabled.\n" )
							panzerwar_reset()
							panzdv = 0
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if et.gentity_get(p,"sess.sessionTeam") == 1 or et.gentity_get(p,"sess.sessionTeam") == 2 then
									if et.gentity_get(p,"health") >= 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
										et.gentity_set(p,"sess.latchPlayerType",originalclass[p])
										et.gentity_set(p,"sess.latchPlayerWeapon",originalweap[p])
									end
								end
							end
						else
							et.G_Print("^3Panzerwar:^7 Panzerwar has already been disabled\n" )
						end
					end
				else
					et.G_Print("^3Panzerwar:^7 Valid values are \[0-1\]\n" )
				end
			end
  		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."frenzy" ) then
			if Cvarct < 3 then
				et.G_Print("^3Frenzy:^7 Disable or enable frenzy \[0-1\]\n" )
			else
				local frenz = tonumber(et.trap_Argv(1))
				if frenz >= 0 and frenz <= 1 then
					if frenz == 1 then
						if frenzdv == 0 then
							if panzdv == 1 then
								et.G_Print("^3Frenzy:^7 Panzerwar must be disabled first\n" )
							elseif grendv == 1 then
								et.G_Print("^3Frenzy:^7 Grenadewar must be disabled first\n" )
							elseif snipdv == 1 then
								et.G_Print("^3Frenzy:^7 Sniperwar must be disabled first\n" )
							else
								et.G_Print("^3Frenzy:^7 Frenzy has been Enabled\n" )
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics -1 ; team_maxcovertops -1 ; team_maxfieldops -1 ; team_maxengineers -1 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers 0\n" )
								frenzdv = 1
								for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
									if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
										if et.gentity_get(p,"health") > 0 then
											et.G_Damage(p, p, 1022, 400, 24, 0)
											et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
										end
									end
								end
							end
						else
							et.G_Print("^3Frenzy:^7 Frenzy is already active\n" )
						end
					else
						if frenzdv == 1 then
							et.G_Print("^3Frenzy:^7 Frenzy has been Disabled.\n" )
							frenzdv = 0
							frenzy_reset()
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
									if et.gentity_get(p,"health") > 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									end
								end
							end
						else
							et.G_Print("^3Frenzy:^7 Frenzy has already been disabled\n" )
						end
					end
				else
					et.G_Print("^3Frenzy:^7 Valid values are \[0-1\]\n" )
				end
			end
  		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."grenadewar" ) then
			if Cvarct < 3 then
				et.G_Print("^3Grenadewar:^7 Disable or enable Grenadewar \[0-1\]\n" )
			else
				local gren = tonumber(et.trap_Argv(1))
				if gren >= 0 and gren <= 1 then
					if gren == 1 then
						if grendv == 0 then
							if panzdv == 1 then
								et.G_Print("^3Grenadewar:^7 Panzerwar must be disabled first\n" )
							elseif frenzdv == 1 then
								et.G_Print("^3Grenadewar:^7 Frenzy must be disabled first\n" )
							elseif snipdv == 1 then
								et.G_Print("^3Grenadewar:^7 Sniperwar must be disabled first\n" )
							else
								et.G_Print("^3Grenadewar:^7 Grenadewar has been Enabled\n" )
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics -1 ; team_maxcovertops -1 ; team_maxfieldops -1 ; team_maxengineers -1 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers 0\n" )
								grendv = 1
								for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
									if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
										if et.gentity_get(p,"health") > 0 then
											et.G_Damage(p, p, 1022, 400, 24, 0)
											et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
										end
									end
								end
							end
						else
							et.G_Print("^3Grenadewar:^7 Grenadewar is already active\n" )
						end
					else
						if grendv == 1 then
							et.G_Print("^3Grenadewar:^7 Grenadewar has been Disabled.\n" )
							grendv = 0
							grenadewar_reset()
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
									if et.gentity_get(p,"health") > 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									end
								end
							end
						else
							et.G_Print("^3Grenadewar:^7 Grenadewar has already been disabled\n" )
						end
					end
				else
					et.G_Print("^3Grenadewar:^7 Valid values are \[0-1\]\n" )
				end
			end
  		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."sniperwar" ) then
			if Cvarct < 3 then
				et.G_Print("^3Sniperwar:^7 Disable or enable Sniperwar \[0-1\]\n" )
			else
				local snip = tonumber(et.trap_Argv(1))
				if snip >= 0 and snip <= 1 then
					if snip == 1 then
						if snipdv == 0 then
							if panzdv == 1 then
								et.G_Print("^3Sniperwar:^7 Panzerwar must be disabled first\n" )
							elseif frenzdv == 1 then
								et.G_Print("^3Sniperwar:^7 Frenzy must be disabled first\n" )
							elseif grendv == 1 then
								et.G_Print("^3Sniperwar:^7 Grenadewar must be disabled first\n" )
							else
								et.G_Print("^3Sniperwar:^7 Sniperwar has been Enabled\n" )
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics -1 ; team_maxcovertops -1 ; team_maxfieldops -1 ; team_maxengineers -1 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers 0\n" )
								snipdv = 1
								for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
										originalclass[p] = tonumber(et.gentity_get(p,"sess.latchPlayerType"))
										originalweap[p] = tonumber(et.gentity_get(p,"sess.latchPlayerWeapon"))
									if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
										if et.gentity_get(p,"health") > 0 then
											et.G_Damage(p, p, 1022, 400, 24, 0)
											et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
										end
									end
								end
							end
						else
							et.G_Print("^3Sniperwar:^7 Sniperwar is already active\n" )
						end
					else
						if snipdv == 1 then
							et.G_Print("^3Sniperwar:^7 Sniperwar has been Disabled.\n" )
							snipdv = 0
							sniperwar_reset()
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
									if et.gentity_get(p,"health") > 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									end
										et.gentity_set(p,"sess.latchPlayerType",originalclass[p])
										et.gentity_set(p,"sess.latchPlayerWeapon",originalweap[p])
								end
							end
						else
							et.G_Print("^3Sniperwar:^7 Sniperwar has already been disabled\n" )
						end
					end
				else
					et.G_Print("^3Sniperwar:^7 Valid values are \[0-1\]\n" )
				end
			end
		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."readconfig" ) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "exec kmod.cfg\n" )
			et.G_Print("^3ReadConfig:^7 Config reloaded\n")
			readconfig()
		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."spree_restart" ) then
			spreerecord_reset()
		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."crazygravity" ) then
			if Cvarct < 3 then
				et.G_Print("^3Crazygravity:^7 Disable or enable crazygravity \[0-1\]\n" )
			else
				local crazy = tonumber(et.trap_Argv(1))
				if crazy >= 0 and crazy <= 1 then
					if crazy == 1 then
						if CGactive == 0 then
							et.G_Print("^3Crazygravity:^7 Crazygravity has been Enabled\n" )
							crazygravity = true
							crazydv = 1
						else
							et.G_Print("^3Crazygravity:^7 Crazygravity is already active\n" )
						end
					else
						if CGactive == 1 then
							et.G_Print("^3Crazygravity:^7 Crazygravity has been Disabled.  Resetting gravity\n" )
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "g_gravity 800\n" )
							crazygravity = false
							crazydv = 0
						else
							et.G_Print("^3Crazygravity:^7 Crazygravity has already been disabled\n" )
						end
					end
				else
					et.G_Print("^3Crazygravity:^7 Valid values are \[0-1\]\n" )
				end
			end
  		elseif (string.lower(et.trap_Argv(0)) == k_commandprefix.."spec999" ) then
			spec999()
		elseif string.lower(et.trap_Argv(0)) == k_commandprefix.."gib" then  
			if (et.trap_Argc() < 2) then 
				et.G_Print("Gib is used to instantly kill a player\n") 
				et.G_Print("useage: gib \[name/PID\]\n")
			return 1 
			end 
			Gib(et.trap_Argv(1)) 
		return 1 
		elseif string.lower(et.trap_Argv(0)) == k_commandprefix.."slap" then 
			if (et.trap_Argc() < 2) then 
				et.G_Print("Slap is used to slap a player\n")
				et.G_Print("useage: slap \[name/PID\]\n")
			return 1 
			end  
			burn(et.trap_Argv(1)) -- the command was originally !burn
		return 1
		elseif string.lower(et.trap_Argv(0)) == "k_commandprefix" then 
			et.G_Print("Unknown command in line k_commandprefix\n")
		return 1
		elseif string.lower(et.trap_Argv(0)) == "m2" then  -- used when advancedpms is enabled
			if k_advancedpms == 1 then
				if (et.trap_Argc() < 2) then 
					et.G_Print("Useage:  /m \[pname/ID\] \[message\]\n")
					return 1
				else
					private_message(1022, et.trap_Argv(1), et.ConcatArgs(2))
				end

				if k_logchat == 1 then
					log_chat( 1022, "PMESSAGE", et.ConcatArgs(2), et.trap_Argv(1) )
				end
			end
		return 1
		elseif string.lower(et.trap_Argv(0)) == "m" or string.lower(et.trap_Argv(0)) == "pm" or string.lower(et.trap_Argv(0)) == "msg" then
			if k_advancedpms == 0 then
				if k_logchat == 1 then
					log_chat( 1022, "PMESSAGE", et.ConcatArgs(2),  et.trap_Argv(1) )
				end
			end
		return 1
		elseif string.lower(et.trap_Argv(0)) == "ma" or string.lower(et.trap_Argv(0)) == "pma" then
				for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do
					if AdminUserLevel(i) >= 2 then
						et.trap_SendServerCommand(i, ("b 8 \"^dPm to admins from ^1SERVER^d --> ^3" .. et.ConcatArgs(1) .. "^7"))
						et.G_ClientSound(i, pmsound)
					end
				end
				if k_logchat == 1 then
					log_chat( 1022, "PMADMINS", et.ConcatArgs(1))
				end
				et.G_Print("Private message sent to admins\n")
				return 1
		elseif string.lower(et.trap_Argv(0)) == "ref" and string.lower(et.trap_Argv(1)) == "pause" and pausedv == 0 then
			GAMEPAUSED = 1
			dummypause = mtime
			return 0
		elseif string.lower(et.trap_Argv(0)) == "ref" and string.lower(et.trap_Argv(1)) == "unpause" and pausedv == 1 then
			GAMEPAUSED = 0
			return 0
		else
      		return 0 
      	end 
end 

function setlevel(client, level2) 
   local clientnum = tonumber(client) 
   local level = tonumber(level2)
   if clientnum then 
      if (clientnum >= 0) and (clientnum < 64) then 
         if et.gentity_get(clientnum,"pers.connected") ~= 2 then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^Setlevel: ^7There is no client associated with this slot number\n" )
			commandSaid = false
		else
            	et.G_Print("There is no client associated with this slot number\n") 
		end
         return 
         end 

      else              
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel: ^7Please enter a slot number between 0 and 63\n" )
			commandSaid = false
		else 
         		et.G_Print("Please enter a slot number between 0 and 63\n") 
		end
      return 
      end 
   else 
      if client then 
	   s,e=string.find(client, client)
	   if e <= 2 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel: ^7Player name requires more than 2 characters\n" )
			commandSaid = false
		else
			et.G_Print("Player name requires more than 2 characters\n")
		end
	   return
	   else
         	clientnum = getsetlvlidfname(client)
	   end
      end 
         if not clientnum then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel: ^7Try name again or use slot number\n" )
			commandSaid = false
		else
         		et.G_Print("Try name again or use slot number\n") 
		end
         return 
         end 
   end 
   if level < 0 or level > k_maxAdminLevels then
	if commandSaid then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel: ^7Admin level does not exist! \[0-3\]\n" )
		commandSaid = false
	else
	    	et.G_Print("Admin level does not exist! \[0-".. k_maxAdminLevels .."\]\n") 
	end
   return
   end

 	setAdmin(clientnum, level)

end 

function getsetlvlidfname(name) 
   local i = 0
   local slot = nil
   local matchcount = 0
   local cleanname = string.lower(et.Q_CleanStr( name ))
   local playerp = ""
   for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do 
	if PlayerName[i] then
 	   playerp = string.lower(et.Q_CleanStr( PlayerName[i] ))
 	   s,e=string.find(playerp, cleanname)
	    if s and e then 
		matchcount = matchcount + 1
			slot = i
          end 
      end 
   end 
   if matchcount >= 2 then
	if commandSaid then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7There are currently ^1" .. matchcount .. "^7 client\[s\] that match \"" ..name.. "\"\n" )
		commandSaid = false
	else
  	    	et.G_Print("There are currently ^1" .. matchcount .. "^7 client\[s\] that match \"" ..name.. "\"\n") 
	end
	return nil
   else
	return slot
   end
end

function setAdmin(PlayerID, levelv)
	-- gets file length
	local fdadm,len = et.trap_FS_FOpenFile( "shrubbot.cfg", et.FS_APPEND )
	et.trap_FS_FCloseFile( fdadm )
	local level = tonumber(levelv)
	local Name = et.Q_CleanStr(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "name" ))
	local GUID = et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
	local dv = 0
	local dvi = 0

	if len == -1 then
		confirm = 1
	else 
		for i=0, tonumber(table.getn(chkGUID)), 1 do
			if chkGUID[i] == GUID then
--				et.trap_FS_FCloseFile( fdadm )
				dv = 1

				if dv == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay changing admin!\n" )
					adminchange(PlayerID, level)
				end
				break
			end
				dvi = i
		end
		if dvi == tonumber(table.getn(chkGUID)) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay setting admin!\n" )
			confirm = 1
		end
	end

	local fdadm,len = et.trap_FS_FOpenFile( "shrubbot.cfg", et.FS_APPEND )

if confirm == 1 then
	for i=0, k_maxAdminLevels, 1 do
		if level == 0 then
			if sldv == 0 then
				if commandSaid then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel: ^7" .. Name .. "^7 is now a regular ^7user!\n" )
	  				commandSaid = false
				else
					et.G_Print( Name .. "^7 is now a regular ^7user!\n" )
					confirm = 0
				end
			end
			loadAdmins()
			return
		elseif level == i and level > 0 then
			for q=0, i, 1 do
				AdminLV[q][GUID] = true
				--AdminLV0[GUID] = true
				--AdminLV1[GUID] = true
				ADMIN = level .. " - " .. GUID .. " - " .. Name .. "\n"
				confirm = 0
			end
			break
		end
	end

--	if level == 0 then
--		if sldv == 0 then
--			if commandSaid then
--				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel: ^7" .. Name .. "^7 is now a regular ^7user!\n" )
--	  			commandSaid = false
--			else
--				et.G_Print( Name .. "^7 is now a regular ^7user!\n" )
--				confirm = 0
--			end
--		end
--		loadAdmins()
--		return
--	elseif level == 1 then
--		AdminLV0[GUID] = true
--		AdminLV1[GUID] = true
--		ADMIN = level .. " - " .. GUID .. " - " .. Name .. "\n"
--		confirm = 0
--	elseif level == 2 then
--		AdminLV0[GUID] = true
--		AdminLV1[GUID] = true
--		AdminLV2[GUID] = true
--		ADMIN = level .. " - " .. GUID .. " - " .. Name .. "\n"
--		confirm = 0
--	elseif level == 3 then
--		AdminLV0[GUID] = true
--		AdminLV1[GUID] = true
--		AdminLV2[GUID] = true
--		AdminLV3[GUID] = true
--		ADMIN = level .. " - " .. GUID .. " - " .. Name .. "\n"
--		confirm = 0
--	end
	et.trap_FS_Write( ADMIN, string.len(ADMIN) ,fdadm )
	et.trap_FS_FCloseFile( fdadm )
	if commandSaid then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel: ^7" .. Name .. "^7 is now a level ^1" .. level .. " ^7user!\n" )
  		commandSaid = false
	else
		et.G_Print( Name .. "^7 is now a level ^1" .. level .. " ^7user!\n" )
	end
	loadAdmins()
	return 1
else
	et.trap_FS_FCloseFile( fdadm )
	return 1
end
end

function addAdmin(GUID)
	s,e,level,GUID = string.find(GUID, "(%d)%s%-%s(%x+)") 
	level=tonumber(level)
	for i=0, k_maxAdminLevels, 1 do
		if level == i then
			for q=1, i, 1 do
				AdminLV[q][GUID] = true
			end
			break
		end
	end



--	if level == 1 then
--		AdminLV1[GUID] = true
--	elseif level == 2 then
--		AdminLV1[GUID] = true
--		AdminLV2[GUID] = true
--	elseif level == 3 then
--		AdminLV1[GUID] = true
--		AdminLV2[GUID] = true
--		AdminLV3[GUID] = true
--	end
end

function adminStatus(PlayerID)
--	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^1CODE HAS GOTTEN THIS FAR!!!\n" )
	local IP   = et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" )
	local GUID = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
	for i=k_maxAdminLevels, 0, -1 do
		if finger then
--	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^1FINGER\n" )
			local name = et.gentity_get(PlayerID,"pers.netname")
			if ( AdminLV[i][GUID] ) and i ~= 0 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Finger: ^7" .. name .. " ^7is an admin \[lvl ".. i .."\]\n" )
				finger = false
				commandSaid = false
				break
			elseif i == 0 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Finger: ^7" .. name .. " ^7is a guest \[lvl 0\]\n" )
				finger = false
				commandSaid = false
				break
			end
		else
--	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^1ADMINTEST\n" )
			if ( AdminLV[i][GUID] ) and i ~= 0 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Admintest: ^7You are an admin \[lvl ".. i .."\]\n" )
				break
			elseif i == 0 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Admintest: ^7You are a guest \[lvl 0\]\n" )
				break
			end
		end
	end

--	if finger then
--		local name = et.gentity_get(PlayerID,"pers.netname")
--		if ( AdminLV3[GUID] ) then
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Finger: ^7" .. name .. " ^7is an admin \[lvl 3\]\n" )
--		elseif ( AdminLV2[GUID] ) then
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Finger: ^7" .. name .. " ^7is an admin \[lvl 2\]\n" )
--		elseif ( AdminLV1[GUID] ) then
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Finger: ^7" .. name .. " ^7is an admin \[lvl 1\]\n" )
--		else
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Finger: ^7" .. name .. " ^7is a guest \[lvl 0\]\n" )
--		end
--		finger = false
--		commandSaid = false
--	else
--		if ( AdminLV3[GUID] ) then
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Admintest: ^7You are an admin \[lvl 3\]\n" )
--		elseif ( AdminLV2[GUID] ) then
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Admintest: ^7You are an admin \[lvl 2\]\n" )
--		elseif ( AdminLV1[GUID] ) then
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Admintest: ^7You are an admin \[lvl 1\]\n" )
--		else
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Admintest: ^7You are a guest \[lvl 0\]\n" )
--		end
--	end
end

function adminchange(PlayerID, level)
	local fdin,lenin = et.trap_FS_FOpenFile( "shrubbot.cfg", et.FS_READ )
	local fdout,lenout = et.trap_FS_FOpenFile( "shrubbottmp.cfg", et.FS_WRITE )
	local GUID2 = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
	local i = 0
	local IPRemove = ""
	local fname = ""
	local dv = 1
	if lenin == -1 then
		et.G_Print("There is no Power User IP to remove \n")
	else
		local filestr = et.trap_FS_Read( fdin, lenin )
		for lvl,guid,name in string.gfind(filestr, "(%d)%s%-%s(%x+)%s%-%s*([^%\n]*)") do
			if (guid==GUID2) then
				guid = string.upper(guid)
				fname = name
				for q=0, k_maxAdminLevels, 1 do
					AdminLV[q][GUID2] = false
				end
--				AdminLV0[GUID2] = false
--				AdminLV1[GUID2] = false
--				AdminLV2[GUID2] = false
--				AdminLV3[GUID2] = false
			else
				guid = lvl .. " - " .. guid .. " - " .. name .. "\n"
				et.trap_FS_Write( guid, string.len(guid) ,fdout )
			end
			i=i+1

		end
	end
	confirm = 1
	et.trap_FS_FCloseFile( fdin ) 
	et.trap_FS_FCloseFile( fdout )
	et.trap_FS_Rename( "shrubbottmp.cfg", "shrubbot.cfg" )
	if dv == 1 then
		loadAdmins()
	end
end

function AdminUserLevel(PlayerID)
	local guid = ""
	if(PlayerID~=-1) then
		guid = et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" )
	else
		guid=-1
	end

	for i=k_maxAdminLevels, 1, -1 do
		if ( AdminLV[i][guid] ) then
			return i
		end
	end


--	if ( AdminLV3[guid] ) then
--		return 3
--	elseif ( AdminLV2[guid] ) then
--		return 2
--	elseif ( AdminLV1[guid] ) then
--		return 1
--	end
	return 0
end

function showAdmins()
	local fd,len = et.trap_FS_FOpenFile( "shrubbot.cfg", et.FS_READ )
	if len <= 0 then
		et.G_Print("WARNING: No Admins's Defined! \n")
	else
		local filestr = et.trap_FS_Read( fd, len )

		for level,guid,Name in string.gfind(filestr, "(%d)%s%-%s(%x+)%s%-%s*([^%\n]*)") do
			-- upcase for exact matches
			GUID = string.upper(guid)
			et.G_Print( "Name  = " ..Name.. "\nLevel = " ..level.. "\n\n")
		end
	end
	et.trap_FS_FCloseFile( fd ) 
end

function loadAdmins()
	local i = 0
	local i2 = 0
	local dv = 1
	local dv2 = 1
	local fd,len = et.trap_FS_FOpenFile( "shrubbot_lua.cfg", et.FS_READ )

--	table.setn(chkGUID, 0)
--	table.setn(AdminName, 0)

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
--		chkGUID[i] = 0
--		AdminName[i] = ""
	end
	if len <= 0 then
		et.G_Print("WARNING: No Admins's Defined! \n")
	else
		local filestr = et.trap_FS_Read( fd, len )
		local i = 0

		for guid in string.gfind(filestr, "%d%s%-%s(%x+)%s%-%s*") do
			-- upcase for exact matches
			chkGUID[i] = string.upper(guid)

--			table.insert(chkGUID, string.upper(guid))
			if dv == 1 then
				i=i+1
			end
		end
		for guid2 in string.gfind(filestr, "(%d%s%-%s%x+)%s%-%s*") do
			-- upcase for exact matches
			guid2 = string.upper(guid2)
			addAdmin(guid2)
		end

		for guid3,Name in string.gfind(filestr, "%d%s%-%s(%x+)%s%-%s*([^%\n]*)") do
			AdminName[guid3] = Name

--			table.insert(AdminName, Name)
		end
	end
	et.trap_FS_FCloseFile( fd ) 
end

function burn(client) 
   local clientnum = tonumber(client) 
   if clientnum then 
      if (clientnum >= 0) and (clientnum < 64) then 
         if et.gentity_get(clientnum,"pers.connected") ~= 2 then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7There is no client associated with this slot number\n" )
			commandSaid = false
		else
            	et.G_Print("There is no client associated with this slot number\n") 
		end
         return 
         end 
         if et.gentity_get(clientnum,"sess.sessionTeam") >= 3 or et.gentity_get(clientnum,"sess.sessionTeam") < 1 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7Client is not actively playing\n" )
			commandSaid = false
		else
	            et.G_Print("Client is not actively playing\n") 
		end
         return 
         end 
	   if et.gentity_get(clientnum,"health") <= 0 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7Client is currently dead\n" )
			commandSaid = false
		else
	  	    	et.G_Print("Client is currently dead\n") 
		end
	   return
	   end
      else              
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7Please enter a slot number between 0 and 63\n" )
			commandSaid = false
		else 
         		et.G_Print("Please enter a slot number between 0 and 63\n") 
		end
      return 
      end 
   else 
      if client then 
	   s,e=string.find(client, client)
	   if e <= 2 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7Player name requires more than 2 characters\n" )
			commandSaid = false
		else
			et.G_Print("Player name requires more than 2 characters\n")
		end
	   return
	   else
         	clientnum = getPlayernameToIdburn(client)
	   end
      end 
         if not clientnum then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7Try name again or use slot number\n" )
			commandSaid = false
		else
         		et.G_Print("Try name again or use slot number\n") 
		end
         return 
         end 
   end 
         if et.gentity_get(clientnum,"sess.sessionTeam") >= 3 or et.gentity_get(clientnum,"sess.sessionTeam") < 1 then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7Client is not actively playing\n" )
			commandSaid = false
		else
	            et.G_Print("Client is not actively playing\n") 
		end
         return 
         end 
	   if et.gentity_get(clientnum,"health") <= 0 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap: ^7Client is currently dead\n" )
			commandSaid = false
		else
	  	    	et.G_Print("Client is currently dead\n") 
		end
	   return
	   end
        et.gentity_set(clientnum,"health",(et.gentity_get(clientnum,"health")-5))
		local	slapsound = "sound/player/hurt_barbwire.wav"
		soundindex = et.G_SoundIndex(slapsound)
		et.G_Sound( clientnum,  soundindex)

	   et.trap_SendServerCommand(-1, ("b 16 \"^7" .. et.gentity_get(clientnum,"pers.netname") .. " ^7was Slapped^7")) 
end 

function getPlayernameToIdburn(name) 
   local i = 0
   local slot = nil
   local matchcount = 0
   local cleanname = string.lower(et.Q_CleanStr( name ))
   local playerp = ""
   for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do 
	if PlayerName[i] then
 	   playerp = string.lower(et.Q_CleanStr( PlayerName[i] ))
 	   s,e=string.find(playerp, cleanname)
     		 if s and e then 
			matchcount = matchcount + 1
				slot = i
        	 end 
      end 
   end 
   if matchcount >= 2 then
	return nil
   else
	return slot
   end
end

function Gib(client) 
   local clientnum = tonumber(client) 
   if clientnum then 
      if (clientnum >= 0) and (clientnum < 64) then 
         if et.gentity_get(clientnum,"pers.connected") ~= 2 then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7There is no client associated with this slot number\n" )
			commandSaid = false
		else
            	et.G_Print("There is no client associated with this slot number\n") 
		end
         return 
         end 
         if et.gentity_get(clientnum,"sess.sessionTeam") >= 3 or et.gentity_get(clientnum,"sess.sessionTeam") < 1 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7Client is not actively playing\n" )
			commandSaid = false
		else
	            et.G_Print("Client is not actively playing\n") 
		end
         return 
         end 
	   if et.gentity_get(clientnum,"health") <= 0 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7Client is currently dead\n" )
			commandSaid = false
		else
	  	    	et.G_Print("Client is currently dead\n") 
		end
	   return
	   end
      else              
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7Please enter a slot number between 0 and 63\n" )
			commandSaid = false
		else 
         		et.G_Print("Please enter a slot number between 0 and 63\n") 
		end
      return 
      end 
   else 
      if client then 
	   s,e=string.find(client, client)
	   if e <= 2 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7Player name requires more than 2 characters\n" )
			commandSaid = false
		else
			et.G_Print("Player name requires more than 2 characters\n")
		end
	   return
	   else
         	clientnum = getPlayernameToId(client)
	   end
      end 
         if not clientnum then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7Try name again or use slot number\n" )
			commandSaid = false
		else
         		et.G_Print("Try name again or use slot number\n") 
		end
         return 
         end 
   end 
         if et.gentity_get(clientnum,"sess.sessionTeam") >= 3 or et.gentity_get(clientnum,"sess.sessionTeam") < 1 then 
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7Client is not actively playing\n" )
			commandSaid = false
		else
	            et.G_Print("Client is not actively playing\n") 
		end
         return 
         end 
	   if et.gentity_get(clientnum,"health") <= 0 then
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib: ^7Client is currently dead\n" )
			commandSaid = false
		else
	  	    	et.G_Print("Client is currently dead\n") 
		end
	   return
	   end
	   et.G_Damage(clientnum, clientnum, 1022, 400, 24, 0)

	   et.trap_SendServerCommand(-1, ("b 16 \"^7" .. et.gentity_get(clientnum,"pers.netname") .. " ^7was Gibbed^7")) 
end 

function getPlayernameToId(name) 
   local i = 0
   local slot = nil
   local matchcount = 0
   local cleanname = string.lower(et.Q_CleanStr( name ))
   local playerp = ""
   for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do 
	if PlayerName[i] then
 	   playerp = string.lower(et.Q_CleanStr( PlayerName[i] ))
 	   s,e=string.find(playerp, cleanname)
     		 if s and e then 
			matchcount = matchcount + 1
				slot = i
        	 end 
      end 
   end 
   if matchcount >= 2 then
	return nil
   else
	return slot
   end
end

function spec999()
	local matches = 0
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		local team = tonumber(et.gentity_get(i,"sess.sessionTeam"))
		local ping = tonumber(et.gentity_get(i,"ps.ping"))
		if team ~= 3 and team ~= 0 then
			if ping >= 999 then
				matches = matches + 1
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref remove " .. i .. "\n" )
			end
		end
	end
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Spec999: ^7Moving ^1" ..matches.. " ^7players to spectator\n" )
end

function et_ClientBegin( clientNum )
	local name=et.Info_ValueForKey( et.trap_GetUserinfo( clientNum ), "name" )

	ModInfo(clientNum)
	loadAdmins()
--	AdminUserLevel(clientNum)

	teamkillr[clientNum] = 0
	selfkills[clientNum] = 0
	muted[clientNum] = 0
	loadMutes()
	MuteCheck(clientNum)

	Bname[clientNum] = name

	if k_logchat == 1 then
		log_chat(clientNum, "CONN", "DV")
	end
end

function ModInfo(PlayerID)
	et.trap_SendServerCommand( PlayerID,"cpm \"This server is running the new KMOD version " .. KMODversion .. "\n\"")
	et.trap_SendServerCommand( PlayerID,"cpm \"Created by Clutch152.\n\"")
end

function advPlayers(PlayerID)
	et.trap_SendServerCommand(PlayerID, string.format("print \"^3 ID ^1: ^3Player                     Rate  Snaps\n"))
	et.trap_SendServerCommand(PlayerID, string.format("print \"^1--------------------------------------------\n"))
	local pteam = { "^1X" , "^4L" , " " }
	local playercount = 0
	local spa = 24

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		local teamnumber = tonumber(et.gentity_get(i,"sess.sessionTeam"))
		local cname = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "name" )
		local rate = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "rate" )
		local snaps = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "snaps" )
		local name = string.lower(et.Q_CleanStr( cname ))
		local namel = tonumber(string.len(name))-1
		local namespa = spa - namel
		local space = string.rep( " ", namespa)
		local ref = tonumber(et.gentity_get(PlayerID,"sess.referee"))
		if ref == 0 then
			ref = ""
		else
			ref = "^3REF"
		end

		if et.gentity_get(i,"pers.connected") ~= 2 then
		else
			et.trap_SendServerCommand(PlayerID, string.format('print "%s^7%2s ^1:^7 %s%s %5s  %5s %s\n"',pteam[teamnumber], i, name, space, rate, snaps, ref))
			playercount = playercount + 1
		end
	end

		et.trap_SendServerCommand(PlayerID, string.format("print \"\n^3 " ..playercount.. " ^7total players\n"))
end

function admins(PlayerID)
	et.trap_SendServerCommand(PlayerID, string.format("print \"^3 ID ^1:^3 Player                   ^1: ^3 Level ^1 : ^3 AdminName\n"))
	et.trap_SendServerCommand(PlayerID, string.format("print \"^1----------------------------------------------------------------\n"))
	local pteam = { "^1X" , "^4L" , " " }
	local playercount = 0
	local spa = 23
	local adname = ""

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		guis = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "cl_guid" )
		GUID = string.upper( guis )
		local teamnumber = tonumber(et.gentity_get(i,"sess.sessionTeam"))
		local cname = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "name" )
		local nudge = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "cl_timenudge" )
		local pitch = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "m_pitch" )
		local name = string.lower(et.Q_CleanStr( cname ))
		local namel = tonumber(string.len(name))-1
		local namespa = spa - namel
		local space = string.rep( " ", namespa)
		local fps = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "com_maxfps" )
		local sens = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "sensitivity" )
		local fov = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "cg_fov" )
		local pmove = et.Info_ValueForKey( et.trap_GetUserinfo( i ), "pmove_fixed" )
		local level = AdminUserLevel(i)

		adname = AdminName[GUID]

		if AdminUserLevel(i) == nil or AdminUserLevel(i) == " " or AdminUserLevel(i) < 1 then
		else
			if et.gentity_get(i,"pers.connected") ~= 2 then
			else
			et.trap_SendServerCommand(PlayerID, string.format('print "%s^7%2s ^1:^7 %s%s ^1:  %5s  ^1:^7  ^7%s\n"',pteam[teamnumber], i, name, space, level, adname))
			playercount = playercount + 1

--string.format('print "%2d : %6s : %s\n"',i, team, et.gentity_get(i,'pers.netname'))) 
			end

		end
	end

		et.trap_SendServerCommand(PlayerID, string.format("print \"\n^3 " ..playercount.. " ^7total admins\n"))
return 1
end

function private_message(PlayerID,client,message) 
   local clientnum = ""
   local clientnum = tonumber(client) 
   if clientnum then 
      if (clientnum >= 0) and (clientnum < 64) then 
         if et.gentity_get(clientnum,"pers.connected") ~= 2 then 
			if commandSaid then
				et.trap_SendServerCommand(PlayerID, string.format("print \"There is no client associated with this slot number\n"))
				commandSaid = false
			else
				et.G_Print("There is no client associated with this slot number\n")
			end
         return 
         end 
      else    
		if commandSaid then          
			et.trap_SendServerCommand(PlayerID, string.format("print \"Please enter a slot number between 0 and 63\n"))
			commandSaid = false
		else
			et.G_Print("Please enter a slot number between 0 and 63\n")
		end
      return 
      end 
   else 
      if client then 
	   s,e=string.find(client, client)
	   if e <= 2 then
			if commandSaid then
				et.trap_SendServerCommand(PlayerID, string.format("print \"Player name requires more than 2 characters\n"))
				commandSaid = false
			else
				et.G_Print("Player name requires more than 2 characters\n")
			end
	   return
	   else
         	clientnum = name2IDPM(client)
	   end
      end 
         if not clientnum then
		if commandSaid then
			et.trap_SendServerCommand(PlayerID, string.format("print \"Try name again or use slot number\n"))
			commandSaid = false
		else
			et.G_Print("Try name again or use slot number\n")
		end
         return 
         end 
   end 
		local name = et.gentity_get(PlayerID,"pers.netname")
		local rname = et.gentity_get(clientnum,"pers.netname")
		if PlayerID == 1022 then
			name = "^1SERVER"
		end
	   if commandSaid then
		if PlayerID ~= 1022 then
			et.trap_SendServerCommand(PlayerID, ("b 8 \"^dPrivate message sent to "..rname.."^d --> ^3" .. message .. "^7"))
		   	et.G_ClientSound(PlayerID, pmsound)
		end
	      commandSaid = false
	   else 
	  	et.G_Print("Private message sent to "..rname.."^d --> ^3" .. message .. "^7\n")
	   end
	   et.trap_SendServerCommand(clientnum, ("b 8 \"^dPrivate message from "..name.."^d --> ^3" .. message .. "^7"))
	   et.G_ClientSound(clientnum, pmsound)
end 

function name2IDPM(name) 
   local i = 0
   local slot = nil
   local matchcount = 0
   local cleanname = string.lower(et.Q_CleanStr( name ))
   local playerp = ""
   for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do 
	if PlayerName[i] then
 	   playerp = string.lower(et.Q_CleanStr( PlayerName[i] ))
 	   s,e=string.find(playerp, cleanname)
     		 if s and e then 
			matchcount = matchcount + 1
				slot = i
        	 end 
      end 
   end 
   if matchcount >= 2 then
	return nil
   else
	return slot
   end
end


function curse_filter( PlayerID )
--	local k_cursemode = 1
	local k_cursemode = tonumber(et.trap_Cvar_Get("k_cursemode"))
	local name = et.gentity_get(PlayerID,"pers.netname")
	local ref = tonumber(et.gentity_get(PlayerID,"sess.referee"))

	if (k_cursemode - 32) >= 0 then
		-- Override kill and slap
		if (k_cursemode - 32) > 7 then
			k_cursemode = 7
		else
			k_cursemode = k_cursemode - 32
		end
		if et.gentity_get(PlayerID,"pers.connected") == 2 then
			if et.gentity_get(PlayerID,"sess.sessionTeam") >= 3 or et.gentity_get(PlayerID,"sess.sessionTeam") < 1 then
			else
				Gib(PlayerID)
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been auto gibbed for language!\n" )
			end
		end
	end
	if (k_cursemode - 16) >= 0 then
		-- Override slap
		if (k_cursemode - 16) > 7 then
			k_cursemode = 7
		else
			k_cursemode = k_cursemode - 16
		end
		if et.gentity_get(PlayerID,"pers.connected") == 2 then
			if et.gentity_get(PlayerID,"sess.sessionTeam") >= 3 or et.gentity_get(PlayerID,"sess.sessionTeam") < 1 then
			else
				et.gentity_get(PlayerID,"health",-1)
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been auto killed for language!\n" )
			end
		end
	end
	if (k_cursemode - 8) >= 0 then
		k_cursemode = k_cursemode - 16
		if et.gentity_get(PlayerID,"pers.connected") == 2 then
			if et.gentity_get(PlayerID,"sess.sessionTeam") >= 3 or et.gentity_get(PlayerID,"sess.sessionTeam") < 1 then
			else
				burn(PlayerID)
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been auto slapped for language!\n" )
			end
		end
	end
	if (k_cursemode - 4) >= 0 then
		-- Override kill and slap
		if (k_cursemode - 4) > 0 then
			k_cursemode = 0
		end
		if ref == 0 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute " .. PlayerID .. "\n" )
			local mute = "-1"
			muted[PlayerID] = -1
			setMute(PlayerID, mute)
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been permanently muted for language!\n" )
		end
	end
	if (k_cursemode - 2) >= 0 then
		-- Override kill and slap
		if (k_cursemode - 2) > 0 then
			k_cursemode = 0
		end
		--Mute time starts at one then doubles each time thereafter
		if ref == 0 then
			if nummutes[PlayerID] == 0 then
				nummutes[PlayerID] = 1
				muted[PlayerID] = mtime + (1*60*1000)
			else
				nummutes[PlayerID] = nummutes[PlayerID] + nummutes[PlayerID]
				muted[PlayerID] = mtime + (nummutes[PlayerID]*60*1000)
			end
			--muted[PlayerID] = mtime + 300000         --5 minutes in milliseconds
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute "..PlayerID.."\n" )
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been auto muted for ^1".. nummutes[PlayerID] .."^7 minute(s)!\n" )
		end
	end
	if k_cursemode == 1 then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute " .. PlayerID .. "\n" )
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been auto muted!\n" )
	end



--	if k_cursemode == 1 then
--		--Mute time starts at one then doubles each time thereafter
--		if ref > 0 then
--			if nummutes[PlayerID] == 0 then
--				nummutes[PlayerID] = 1
--				muted[PlayerID] = mtime + (1*60*1000)
--			else
--				nummutes[PlayerID] = nummutes[PlayerID] + nummutes[PlayerID]
--				muted[PlayerID] = mtime + (nummutes[PlayerID]*60*1000)
--			end
--			--muted[PlayerID] = mtime + 300000         --5 minutes in milliseconds
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute "..PlayerID.."\n" )
--			et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3CurseFilter: ^7"..name.." ^7has been auto muted for ^1".. nummutes[PlayerID] .."^7 minute(s)!\n" )
--		end
--	end
end


function log_chat( PlayerID, mode, text, PMID )
	local text = et.Q_CleanStr(text)
	local fdadm,len = et.trap_FS_FOpenFile( "chat_log.log", et.FS_APPEND )
	local time = os.date("%x %I:%M:%S%p")
	local ip
	local guid
	if mode == et.SAY_ALL then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..name.. ": " .. text.. "\n"
	elseif mode == et.SAY_TEAM then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..name.." (TEAM): " ..text.. "\n"
	elseif mode == et.SAY_BUDDY then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..name.. " (BUDDY): " ..text.. "\n"
	elseif mode == et.SAY_TEAMNL then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..name.. " (TEAM): " ..text.. "\n"
	elseif mode == "VSAY_TEAM" then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..name.. " (VSAY_TEAM): " ..text.. "\n"
	elseif mode == "VSAY_BUDDY" then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..name.. " (VSAY_BUDDY): " ..text.. "\n"
	elseif mode == "VSAY_ALL" then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..name.. " (VSAY): " ..text.. "\n"
	elseif mode == "PMESSAGE" then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local from = "SERVER"
		if PlayerID ~= 1022 then
			from = et.gentity_get(PlayerID,"pers.netname")
		end
		local rec1 = part2id(PMID)
		if rec1 then
			local recipiant = et.gentity_get(rec1,"pers.netname")
			LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..from.." to "..recipiant.." (PRIVATE): " ..text.. "\n"
		end
	elseif mode == "PMADMINS" then
		local from = "SERVER"
		if PlayerID ~= 1022 then
			ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
			guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
			from = et.gentity_get(PlayerID,"pers.netname")
		else
			ip = "127.0.0.1"
			guid = "NONE!"
		end
		local recipiant = "ADMINS"
		LOG = "("..time..") (IP: " .. ip .. " GUID: " .. guid .. ") "..from.." to "..recipiant.." (PRIVATE): " ..text.. "\n"
	elseif mode == "CONN" then
		ip = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "ip" ))
		guid = string.upper(et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "*** " ..name.. " HAS ENTERED THE GAME  (IP: " .. ip .. " GUID: " .. guid .. ") ***\n"
	elseif mode == "NAME_CHANGE" then
		LOG = "*** " ..PlayerID.. " HAS RENAMED TO "..text.." ***\n"
	elseif mode == "DISCONNECT" then
		local name = et.gentity_get(PlayerID,"pers.netname")
		local name = et.Q_CleanStr( name )
		LOG = "*** " ..name.. " HAS DISCONNECTED ***\n"
	elseif mode == "START" then
		LOG = "\n	***SERVER RESTART OR MAP CHANGE***\n\n"
	end

	et.trap_FS_Write( LOG, string.len(LOG) ,fdadm )
	et.trap_FS_FCloseFile( fdadm )
end

function goto(PlayerID, target)
	target = part2id(target)
	if target == nil then
		--durt
	else
--		local target_origin = et.gentity_get(target, "r.currentOrigin")
		if et.gentity_get(PlayerID,"pers.connected") == 2 then
			if et.gentity_get(PlayerID,"sess.sessionTeam") >= 3 or et.gentity_get(PlayerID,"sess.sessionTeam") < 1 then
			else
				local target_origin = et.gentity_get(target, "origin")
				target_origin[2] = target_origin[2] + 40
				et.gentity_set(PlayerID, "origin", target_origin)
			end
		end

--		et.gentity_set(PlayerID, "r.currentOrigin", target_origin)
	end



--   		pos[30]=et.gentity_get(et.trap_Argv(2),"origin")
--   		et.gentity_set(et.trap_Argv(1), "origin", pos[30])
end

function iwant(PlayerID, target)
	PlayerID = part2id(PlayerID)
	if PlayerID == nil then
		--durt
	else
--		local PlayerID_origin = et.gentity_get(PlayerID, "r.currentOrigin")
		if et.gentity_get(PlayerID,"pers.connected") == 2 then
			if et.gentity_get(PlayerID,"sess.sessionTeam") >= 3 or et.gentity_get(PlayerID,"sess.sessionTeam") < 1 then
			else
				local PlayerID_origin = et.gentity_get(PlayerID, "origin")
				PlayerID_origin[2] = PlayerID_origin[2] + 40
				et.gentity_set(target, "origin", PlayerID_origin)
			end
		end

--		et.gentity_set(target, "r.currentOrigin", PlayerID_origin)
	end
end

function dmg_test( PlayerID )
	local damage = et.gentity_get(PlayerID, "damage")
	local sdmgFlags = et.gentity_get(PlayerID, "s.dmgFlags")
	local sessdamage_given = et.gentity_get(PlayerID, "sess.damage_given")
	local sessdamage_received = et.gentity_get(PlayerID, "sess.damage_received")

	et.trap_SendServerCommand("print \"damage = " .. damage .. "\nsdmgflags = " .. sdmgflags .. "\nsessdamage_given = " .. sessdamage_given .. "\nsessdamage_received = " .. sessdamage_received .. "\n\"")
end

function et_ClientCommand( clientNum, command )
	local name2 = et.gentity_get(clientNum,"pers.netname")
	local name2 = et.Q_CleanStr( name2 )
	local arg0 = string.lower(et.trap_Argv(0))
	local arg1 = string.lower(et.trap_Argv(1))
	local muted = et.gentity_get(clientNum, "sess.muted")
	if muted == 0 then
	if arg0 == "say" then
		if k_logchat == 1 then
			log_chat( clientNum, et.SAY_ALL, et.ConcatArgs(1))
		end

		say_parms = "qsay"

		et_ClientSay( clientNum, et.SAY_ALL, et.ConcatArgs(1))
	elseif arg0 == "say_team" then
		if k_logchat == 1 then
			log_chat( clientNum, et.SAY_TEAM, et.ConcatArgs(1))
		end

		say_parms = "qsay"

		et_ClientSay( clientNum, et.SAY_TEAM, et.ConcatArgs(1))
	elseif arg0 == "st" then
		--speaker_test( clientNum, et.ConcatArgs(1))
		speaker_test( clientNum, et.ConcatArgs(1))
		return 1
	elseif arg0 == "say_buddy" then
		if k_logchat == 1 then
			log_chat( clientNum, et.SAY_BUDDY, et.ConcatArgs(1))
		end

		say_parms = "qsay"

		et_ClientSay( clientNum, et.SAY_BUDDY, et.ConcatArgs(1))
	elseif arg0 == "say_teamnl" then
		if k_logchat == 1 then
			log_chat( clientNum, et.SAY_TEAMNL, et.ConcatArgs(1))
		end

		say_parms = "qsay"

		et_ClientSay( clientNum, et.SAY_TEAMNL, et.ConcatArgs(1))
	elseif arg0 == "sc" then
		if AdminUserLevel(clientNum) == 3 then
			local name = et.gentity_get(clientNum,"pers.netname")

			if k_advancedpms == 1 then
				say_parms = "m2 ".. clientNum
			else
				say_parms = "m "..name
			end

			et_ClientSay( clientNum, SC, et.ConcatArgs(1))
			return 1
		end
	elseif arg0 == "vsay" then
		if k_logchat == 1 then
			log_chat( clientNum, "VSAY_ALL", et.ConcatArgs(1))
		end
	elseif arg0 == "vsay_team" then
		if k_logchat == 1 then
			log_chat( clientNum, "VSAY_TEAM", et.ConcatArgs(1))
		end
	elseif arg0 == "vsay_buddy" then
		if k_logchat == 1 then
			log_chat( clientNum, "VSAY_BUDDY", et.ConcatArgs(1))
		end
	end
	end

	if arg0 == "dmg_test" then
		dmg_test(clientNum)
		return 1
	end

	if arg0 == "m" or arg0 == "pm"  then
		if k_logchat == 1 then
			log_chat( clientNum, "PMESSAGE", et.ConcatArgs(2), et.trap_Argv(1) )
		end
	elseif arg0 == "ma" or arg0 == "pma" or arg0 == "msg" then
		if k_logchat == 1 then
			log_chat( clientNum, "PMADMINS", et.ConcatArgs(1))
		end
	end

	if k_advplayers == 1 then
		if string.lower(command) == "players" then
   			advPlayers(clientNum)
		return 1 
		end
		if AdminUserLevel(clientNum) >= 2 then
			if string.lower(command) == "admins" then
   				admins(clientNum)
			return 1
  			end
		end
	end

		local ref = tonumber(et.gentity_get(clientNum,"sess.referee"))

	if tonumber(et.gentity_get(clientNum,"sess.sessionTeam")) == 3 then
		if et.trap_Argv(0) == "team" and et.trap_Argv(1) then
			switchteam[clientNum] = 1
		end
	end

	if votedis == 1 then
		local vote = et.trap_Argv(1)

		if AdminUserLevel(clientNum) < 3 then
			if et.trap_Argv(0) == "callvote" then
				if vote == "shuffleteamsxp" or vote == "shuffleteamsxp_norestart" or vote == "nextmap" or vote == "swapteams" or vote == "matchreset" or vote == "maprestart" or vote == "map" then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "cancelvote ; qsay Voting has been disabled!\n")
				end
			end
		end

	end

	local vote = et.trap_Argv(1)
	if k_antiunmute == 1 then
		if vote == "unmute" then
			local client = et.trap_Argv(2)
			local clientnumber = part2id(client)
			local targetmuted = et.gentity_get(clientnumber, "sess.muted")
			if targetmuted == 1 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "cancelvote ; qsay Cannot vote to unmute a muted person!\n")
			end
		end
	end

	if arg0 == "ref" and arg1 == "pause" and pausedv == 0 then
		GAMEPAUSED = 1
		dummypause = mtime
	elseif arg0 == "ref" and arg1 == "unpause" and pausedv == 1 then
		if ((mtime - dummypause)/1000) >= 5 then
			GAMEPAUSED = 0
		end
	end

	if k_advancedpms == 1 then
		if string.lower(command) == "m" or string.lower(command) == "msg" or string.lower(command) == "pm" then
			if et.trap_Argv(1) == nil or et.trap_Argv(1) == "" or et.trap_Argv(1) == " " then 
				et.trap_SendServerCommand(clientNum, string.format("print \"Useage:  /m \[pname/ID\] \[message\]\n"))
			else
				commandSaid = true
				private_message(clientNum, et.trap_Argv(1), et.ConcatArgs(2))
				return 1
			end
		end
	end

	if string.lower(command) == "ma" or string.lower(command) == "pma" then
		for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do
			if AdminUserLevel(i) >= 2 then
				local name = et.gentity_get(clientNum,"pers.netname") 
				et.trap_SendServerCommand(i, ("b 8 \"^dPm to admins from "..name.."^d --> ^3" .. et.ConcatArgs(1) .. "^7"))
				if k_advancedpms == 1 then
					et.G_ClientSound(i, pmsound)
				end
			end
		end
		if AdminUserLevel(clientNum) < 2 then
			et.trap_SendServerCommand(clientNum, ("b 8 \"^dPm to admins has been sent^d --> ^3" .. et.ConcatArgs(1) .. "^7"))
			if k_advancedpms == 1 then
				et.G_ClientSound(clientNum, pmsound)
			end
		end
		return 1
	end

	if k_slashkilllimit == 1 then
		local name = et.gentity_get(clientNum,"pers.netname")
		local teamnumber = tonumber(et.gentity_get(clientNum,"sess.sessionTeam"))
		if string.lower(command) == "kill" then
			if teamnumber ~= 3 then
				if et.gentity_get(clientNum,"health") > 0 then
					if selfkills[clientNum] < k_slashkills then
						selfkills[clientNum] = selfkills[clientNum] + 1
						if selfkills[clientNum] == k_slashkills then
							if k_advancedpms == 1 then
--								et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 " .. name .. " ^7You have reached your /kill limit!  You can no longer /kill for the rest of this map.\n" )
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 " .. clientNum .. " ^7You have reached your /kill limit!  You can no longer /kill for the rest of this map.\n" )
								et.G_ClientSound(clientNum, pmsound)
							else
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "m " .. name .. " ^7You have reached your /kill limit!  You can no longer /kill for the rest of this map.\n" )
							end
						elseif selfkills[clientNum] == (k_slashkills - 1) then
							if k_advancedpms == 1 then
--								et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 " .. name .. " ^7You have ^11^7 /kill left for this map.\n" )
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 " .. clientNum .. " ^7You have ^11^7 /kill left for this map.\n" )
								et.G_ClientSound(clientNum, pmsound)
							else
								et.trap_SendConsoleCommand( et.EXEC_APPEND, "m " .. name .. " ^7You have ^11^7 /kill left for this map.\n" )
							end
						end
					else
						if k_advancedpms == 1 then
--							et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 " .. name .. " ^7You may no longer /kill for the rest of this map!\n" )
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 " .. clientNum .. " ^7You may no longer /kill for the rest of this map!\n" )
							et.G_ClientSound(clientNum, pmsound)
						else
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "m " .. name .. " ^7You may no longer /kill for the rest of this map!\n" )
						end
						return 1
					end
				end
			end
		end
	end

	return 0
end

function et_ClientSay(clientNum,mode,text)
	local command1=""
	local commands = 0
	local first = ""
	local second = ""
	local third = ""
	local returnVal = 0
	s,e,first,second,third = string.find(text,"%s*([%p]?[.%S]*)%s+([.%S]*)%s+(.*)")

	local fd,len = et.trap_FS_FOpenFile( "badwords.list", et.FS_READ )
	if len > 0 then
		local filestr = et.trap_FS_Read( fd, len )

--		for level,comm,str in string.gfind(filestr, "[^%#](%d)%s*%-%s*([%w%_]*)%s*%=%s*([^%\n]*)") do
--		for Bword in string.gfind(filestr, "[^%#]([^%\n]*)[^%#]") do
--		for Bword in string.gfind(filestr, "([^%\n]+)%S*") do
		for Bword in string.gfind(filestr, "(%w*)") do
--			Bword = string.gsub(Bword, "(.+)%\n", "%1")
--			d,v,Bword = string.find(Bword, "([%w]*)")
--			if dv == nil then
--				et.G_Print("Bword = ".. Bword .." and it is ".. string.len(Bword) .." long\n" )
				for word in string.gfind(text, "([^%s]+)%p*") do
--					t,f=string.find(word, Bword)
--					if f ~= nil then
--						if f ~= 0 then
						if word == Bword then
--							et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay moo\n" )
							curse_filter( clientNum )
							break
						end
--					end
				end
--			end
		end
	end
	et.trap_FS_FCloseFile( fd )


	if(mode==et.SAY_ALL) then
		command1="say"
	elseif (mode==et.SAY_TEAM or mode==et.SAY_TEAMNL) then
		command1="say_team"
	elseif (mode==et.SAY_BUDDY) then
		command1="say_buddy"
	elseif (mode==SC) then
		command1="SC"
	end
	if(third~=nil) then
		commands=4
	else
		s,e,first,second = string.find(text,"%s*([^%s+]+)%s+(.+)%s*")
			third=""
			if(second~=nil) then
				commands = 3
			else
				second=""
				first = et.ConcatArgs(1)
				commands = 2
			end
	end
	return ClientUserCommand(clientNum, command1, first, second, third, commands)
end

function comds(client, cvar1, caller) 
	local clientnum = tonumber(client) 
	if clientnum then 
	local wname = ""
      if (clientnum >= 0) and (clientnum < 64) then 
      	if et.gentity_get(clientnum,"pers.connected") ~= 2 then 
			if commandSaid then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ": ^7There is no client associated with this slot number\n" )
				commandSaid = false
			else
            		et.G_Print("There is no client associated with this slot number\n") 
			end
      	return 
         	end 

      else              
		if commandSaid then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ": ^7Please enter a slot number between 0 and 63\n" )
			commandSaid = false
		else 
         		et.G_Print("Please enter a slot number between 0 and 63\n") 
		end
      return 
      end 
	else 
      	if client then 
			s,e=string.find(client, client)
				if e <= 2 then
					if commandSaid then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ": ^7Player name requires more than 2 characters\n" )
						commandSaid = false
					else
						et.G_Print("Player name requires more than 2 characters\n")
					end
	   			return
				else
			         	clientnum = getsetlvlidfpname(client)
					wname = clientnum
				end
      	end 
         	if not clientnum then 
			if commandSaid then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ": ^7Try name again or use slot number\n" )
				commandSaid = false
			else
         			et.G_Print("Try name again or use slot number\n") 
			end
         	return 
         	end 
   	end 

 	if kick then
		local client2 = clientnum+1
		if AdminUserLevel(caller) > AdminUserLevel(clientnum) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "pb_sv_kick " .. client2 .. " " .. cvar1 .. "\n" )
		else
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ":^7 Cannot kick a higher admin\n" )
		end
		kick = false
		commandSaid = false
 	elseif ban then
		local client2 = clientnum+1
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "pb_sv_ban " .. client2 .. " " .. cvar1 .. "\n" )
		ban = false
		commandSaid = false
 	elseif warn then
		local name = et.gentity_get(clientnum,"pers.netname")
		wname = string.lower(et.Q_CleanStr( name ))
		local cvar1 = tostring( cvar1 )
		if AdminUserLevel(caller) > AdminUserLevel(clientnum) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref warn \"" .. wname .. "\" \"" .. cvar1 .. "\"\n" )
		else
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ":^7 Cannot warn a higher admin\n" )
		end
		warn = false
		commandSaid = false
 	elseif mute then
		local name = et.gentity_get(clientnum,"pers.netname")
		if AdminUserLevel(caller) > AdminUserLevel(clientnum) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute " .. clientnum .. "\n" )
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Mute: ^7" ..name.. " ^7has been muted\n" )
		else
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ":^7 Cannot mute a higher admin\n" )
		end
		mute = false
		commandSaid = false
 	elseif pmute then
		local name = et.gentity_get(clientnum,"pers.netname")
		if AdminUserLevel(caller) >= AdminUserLevel(clientnum) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref mute " .. clientnum .. "\n" )
			local mute = "-1"
			muted[clientnum] = -1
			setMute(clientnum, mute)
		else
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3" .. fullcom .. ":^7 Cannot mute a higher admin\n" )
		end
		pmute = false
		commandSaid = false
 	elseif unmute then
		local name = et.gentity_get(clientnum,"pers.netname")
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref unmute " .. clientnum .. "\n" )
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay ^3Unmute: ^7" ..name.. " ^7has been unmuted\n" )
		unmute = false
		commandSaid = false
 	elseif putaxis then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref putaxis " .. clientnum .. "\n" )
		putaxis = false
		commandSaid = false
 	elseif putallies then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref putallies " .. clientnum .. "\n" )
		putallies = false
		commandSaid = false
 	elseif putspec then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref remove " .. clientnum .. "\n" )
		putspec = false
		commandSaid = false
 	elseif makeshoutcaster then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref makeshoutcaster " .. clientnum .. "\n" )
		makeshoutcaster = false
		commandSaid = false
 	elseif removeshoutcaster then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref removeshoutcaster " .. clientnum .. "\n" )
		removeshoutcaster = false
		commandSaid = false
 	elseif makereferee then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref referee " .. clientnum .. "\n" )
		makereferee = false
		commandSaid = false
 	elseif removereferee then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "ref unreferee " .. clientnum .. "\n" )
		removereferee = false
		commandSaid = false
 	elseif finger then
		adminStatus(clientnum)
 	elseif getip then
		local IP   = et.Info_ValueForKey( et.trap_GetUserinfo( clientnum ), "ip" )
		local cvar1 = tonumber(cvar1)
		local name = et.gentity_get(clientnum,"pers.netname")
		et.trap_SendServerCommand(cvar1, string.format("b 8 \"^3Getip: " .. name .. "^7's IP is " .. IP .. ""))
		getip = false
		commandSaid = false
 	elseif getguid then
		local GUID   = et.Info_ValueForKey( et.trap_GetUserinfo( clientnum ), "cl_guid" )
		local cvar1 = tonumber(cvar1)
		local name = et.gentity_get(clientnum,"pers.netname")
		et.trap_SendServerCommand(cvar1, string.format("b 8 \"^3Getguid: " .. name .. "^7's GUID is " .. GUID .. ""))
		getguid = false
		commandSaid = false
	end


end 

--[[
function getsetlvlidfpname(name) 
   	local i 
   	local matchcount = 0
   	local cleanname = string.lower(et.Q_CleanStr( name ))
   	for i=0,63,1 do 
   	playeri = et.gentity_get(i,"pers.netname")
      	if playeri then 
 	   	playerp = string.lower(et.Q_CleanStr( playeri ))
 	   	s,e=string.find(playerp, cleanname)
		 	if s and e then 
				matchcount = matchcount + 1
				if matchcount == 1 then
					slot = i
				else
					slot = nil
				end
        	 	end 
      	end 
   	end 
   	if matchcount >= 2 then
		return nil
   	else
		return slot
   	end
end
--]]

function getsetlvlidfpname(name) 
   local i = 0
   local slot = nil
   local matchcount = 0
   local cleanname = string.lower(et.Q_CleanStr( name ))
   local playerp = ""
   for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do 
	if PlayerName[i] then
 	   playerp = string.lower(et.Q_CleanStr( PlayerName[i] ))
 	   s,e=string.find(playerp, cleanname)
     		 if s and e then 
			matchcount = matchcount + 1
				slot = i
        	 end 
      end 
   end 
   if matchcount >= 2 then
	return nil
   else
	return slot
   end
end

function part2id(client) 
	local clientnum = tonumber(client) 
	if clientnum then 
		if (clientnum >= 0) and (clientnum < 64) then 
      		if et.gentity_get(clientnum,"pers.connected") ~= 2 then 
      			return nil
         		end 
	    	  return clientnum
      	end 
	else 
		local client = string.lower(et.Q_CleanStr( client ))
      	if client then 
			s,e=string.find(client, client)
				if e <= 2 then
		   			return nil
				else
			         	clientnum = nameforID(client)
				end
      	end 
         	if not clientnum then 
	         	return nil
         	end 
   	end 

	return clientnum

end 

--[[
function nameforID(name) 
   	local i 
   	local matchcount = 0
   	local cleanname = string.lower(et.Q_CleanStr( name ))
   	for i=0,63,1 do 
   	playeri = et.gentity_get(i,"pers.netname")
      	if playeri then 
 	   	playerp = string.lower(et.Q_CleanStr( playeri ))
 	   	s,e=string.find(playerp, cleanname)
		 	if s and e then 
				matchcount = matchcount + 1
				if matchcount == 1 then
					slot = i
				else
					slot = nil
				end
        	 	end 
      	end 
   	end 
   	if matchcount >= 2 then
		return nil
   	else
		return slot
   	end
end
--]]

function nameforID(name) 
   local i = 0
   local slot = nil
   local matchcount = 0
   local cleanname = string.lower(et.Q_CleanStr( name ))
   local playerp = ""
   for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do 
	if PlayerName[i] then
 	   playerp = string.lower(et.Q_CleanStr( PlayerName[i] ))
 	   s,e=string.find(playerp, cleanname)
     		 if s and e then 
			matchcount = matchcount + 1
				slot = i
        	 end 
      end 
   end 
   if matchcount >= 2 then
	return nil
   else
	return slot
   end
end

function loadcommands()
	local fd,len = et.trap_FS_FOpenFile( "commands.cfg", et.FS_READ )
	if len > 0 then
		local filestr = et.trap_FS_Read( fd, len )
		local counter = {}
		local d = {}
		for i=0, k_maxAdminLevels, 1 do
			counter[i] = 0
			d[i] = lvlsc[i]
		end
--		local counter0 = 0
--		local counter1 = 0
--		local counter2 = 0
--		local counter3 = 0
		for level,comm in string.gfind(filestr, "[^%#](%d)%s*%-%s*(%w+)%s*%=%s*[^%\n]*") do
			local comm2 = k_commandprefix..comm

			for i=0, k_maxAdminLevels, 1 do
				if tonumber(level) == i then
--					counter[i] = counter[i] + 1
--					lvlsc[i] = d[i] + counter[i]
					lvlsc[i] = lvlsc[i] + 1
					lvls[i][lvlsc[i]] = comm2
				end
			end

--			if tonumber(level) == 0 then
--				counter0 = counter0 + 1
--				lvl0c = d0 + counter0
--				lvl0[lvl0c] = comm2
--			elseif tonumber(level) == 1 then
--				counter1 = counter1 + 1
--				lvl1c = d1 + counter1
--				lvl1[lvl1c] = comm2
--			elseif tonumber(level) == 2 then
--				counter2 = counter2 + 1
--				lvl2c = d2 + counter2
--				lvl2[lvl2c] = comm2
--			elseif tonumber(level) == 3 then
--				counter3 = counter3 + 1
--				lvl3c = d3 + counter3
--				lvl3[lvl3c] = comm2
--			end
		end
	end
	et.trap_FS_FCloseFile( fd )
end

function readconfig()

	loadAdmins()
	loadspreerecord()
	loadmapspreerecord()

	k_maxAdminLevels = tonumber(et.trap_Cvar_Get("k_maxAdminLevels"))

	for i=0, k_maxAdminLevels, 1 do
		t = tostring(et.trap_Cvar_Get("k_Admin" .. i))
		local c = 1
		lvls[i] = {}
		for w in string.gfind(t, "([^%s]+)%s*") do
			lvls[i][c]=k_commandprefix .. w
--			if type(lvls[i][c]) ~= "string" then
--				et.G_Print("i and c = " .. i .. " and " .. c .. " = nil\n")
--			else
--				et.G_Print("i and c = " .. i .. " and " .. c .. " = " .. lvls[i][c] .. "\n")
--			end
			c=c+1
		end
		lvlsc[i] = c-1
--		et.G_Print("lvlsc = " .. lvlsc[i] .. "\n")
	end

--[[	
	lvl0c = 5
	lvl0 = {
		k_commandprefix.."admintest",
		k_commandprefix.."time",
		k_commandprefix.."date",
		k_commandprefix.."spree_record",
		k_commandprefix.."tk_index",
	}

	lvl1c = 1
	lvl1 = {
		k_commandprefix.."spec999",
	}

	lvl2c = 7
	lvl2 = {
		k_commandprefix.."finger",
		k_commandprefix.."kick",
		k_commandprefix.."warn",
		k_commandprefix.."mute",
		k_commandprefix.."pmute",
		k_commandprefix.."unmute",
		k_commandprefix.."timelimit",
	}

	lvl3c = 23
	lvl3 = {
		k_commandprefix.."ban",
		k_commandprefix.."readconfig",
		k_commandprefix.."makeshoutcaster",
		k_commandprefix.."makereferee",
		k_commandprefix.."removeshoutcaster",
		k_commandprefix.."removereferee",
		k_commandprefix.."gravity",
		k_commandprefix.."speed",
		k_commandprefix.."knifeonly",
		k_commandprefix.."knockback",
		k_commandprefix.."cheats",
		k_commandprefix.."laser",
		k_commandprefix.."spree_restart",
		k_commandprefix.."getip",
		k_commandprefix.."crazygravity",
		k_commandprefix.."getguid",
		k_commandprefix.."setlevel",
		k_commandprefix.."gib",
		k_commandprefix.."slap",
		k_commandprefix.."panzerwar",
		k_commandprefix.."frenzy",
		k_commandprefix.."grenadewar",
		k_commandprefix.."sniperwar",
	}
--]]

	loadcommands()
end

function randomClientFinder()
	randomClient = {}
	local m = 0
	
	for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do
		if et.gentity_get(i,"pers.connected") == 2 then
			randomClient[m] = i
			m = m + 1
		end
	end

	local dv1 = m - 1 

	local dv = math.random(0, dv1)
	local dv2 = randomClient[dv]

	return dv2
end

function ClientUserCommand(PlayerID, Command, BangCommand, Cvar1, Cvar2, Cvarct)
	local admin_req = k_maxAdminLevels + 1

	local fd,len = et.trap_FS_FOpenFile( "commands.cfg", et.FS_READ )
	if len > 0 then
		local filestr = et.trap_FS_Read( fd, len )
		for level,comm,str in string.gfind(filestr, "[^%#](%d)%s*%-%s*([%w%_]*)%s*%=%s*([^%\n]*)") do
			local strnumber = {}
			local strnumber = ParseString(str)

			local comm2 = k_commandprefix..comm
			local t = tonumber(et.gentity_get(PlayerID,"sess.sessionTeam"))
			local c = tonumber(et.gentity_get(PlayerID,"sess.latchPlayerType"))
			local player_last_victim_name = ""
			local player_last_killer_name = ""
			local player_last_victim_cname = ""
			local player_last_killer_cname = ""
			if playerlastkilled[PlayerID] == 1022 then
				player_last_victim_name = "NO ONE"
				player_last_victim_cname = "NO ONE"
			else
				player_last_victim_name = et.Q_CleanStr(et.gentity_get(playerlastkilled[PlayerID],"pers.netname"))
				player_last_victim_cname = et.gentity_get(playerlastkilled[PlayerID],"pers.netname")
			end
			if playerwhokilled[PlayerID] == 1022 then
				player_last_killer_name = "NO ONE"
				player_last_killer_cname = "NO ONE"
			else
				player_last_killer_name = et.Q_CleanStr(et.gentity_get(playerwhokilled[PlayerID],"pers.netname"))
				player_last_killer_cname = et.gentity_get(playerwhokilled[PlayerID],"pers.netname")
			end

			local pnameID = part2id(Cvar1)
			if not pnameID then
				pnameID = 1021
			end
			local PBpnameID = pnameID + 1
			local PBID = PlayerID + 1

			local randomC = randomClientFinder()
			local randomTeam = team[tonumber(et.gentity_get(randomC,"sess.sessionTeam"))]
			local randomCName = et.gentity_get(randomC,"pers.netname")
			local randomName = et.Q_CleanStr(et.gentity_get(randomC,"pers.netname"))
			local randomClass = class[tonumber(et.gentity_get(randomC,"sess.latchPlayerType"))]

			local str = string.gsub(str, "<CLIENT_ID>", PlayerID)
			local str = string.gsub(str, "<GUID>", et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "cl_guid" ))
			local str = string.gsub(str, "<COLOR_PLAYER>", et.gentity_get(PlayerID,"pers.netname"))
			local str = string.gsub(str, "<ADMINLEVEL>", AdminUserLevel(PlayerID))
			local str = string.gsub(str, "<PLAYER>", et.Q_CleanStr(et.gentity_get(PlayerID,"pers.netname")))
			local str = string.gsub(str, "<PLAYER_CLASS>", class[c])
			local str = string.gsub(str, "<PLAYER_TEAM>", team[t])
			local str = string.gsub(str, "<PARAMETER>", Cvar1..Cvar2)
			local str = string.gsub(str, "<PLAYER_LAST_KILLER_ID>", playerwhokilled[PlayerID])
			local str = string.gsub(str, "<PLAYER_LAST_KILLER_NAME>", player_last_killer_name)
			local str = string.gsub(str, "<PLAYER_LAST_KILLER_CNAME>", player_last_killer_cname)
			local str = string.gsub(str, "<PLAYER_LAST_KILLER_WEAPON>", killedwithweapv[PlayerID])
			local str = string.gsub(str, "<PLAYER_LAST_VICTIM_ID>", playerlastkilled[PlayerID])
			local str = string.gsub(str, "<PLAYER_LAST_VICTIM_NAME>", player_last_victim_name)
			local str = string.gsub(str, "<PLAYER_LAST_VICTIM_CNAME>", player_last_victim_cname)
			local str = string.gsub(str, "<PLAYER_LAST_VICTIM_WEAPON>", killedwithweapk[PlayerID])
			local str = string.gsub(str, "<PNAME2ID>", pnameID)
			local str = string.gsub(str, "<PBPNAME2ID>", PBpnameID)
			local str = string.gsub(str, "<PB_ID>", PBID)
			local str = string.gsub(str, "<RANDOM_ID>", randomC)
			local str = string.gsub(str, "<RANDOM_CNAME>", randomCName)
			local str = string.gsub(str, "<RANDOM_NAME>", randomName)
			local str = string.gsub(str, "<RANDOM_CLASS>", randomClass)
			local str = string.gsub(str, "<RANDOM_TEAM>", randomTeam)
			local teamnumber = tonumber(et.gentity_get(PlayerID,"sess.sessionTeam"))
			local classnumber = tonumber(et.gentity_get(PlayerID,"sess.latchPlayerType"))


			if (string.lower(BangCommand) == comm2 ) then
				if tonumber(level) <= AdminUserLevel(PlayerID) then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "".. str .. "\n" )
					if strnumber[1] == "forcecvar" then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3etpro svcmd: ^7forcing client cvar ["..strnumber[2].."] to [".. Cvar1 .."]\n" )
					end						
				else
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^7Insufficient Admin status\n" )
				end
			end
		end
	end
	et.trap_FS_FCloseFile( fd )


	for i=0, k_maxAdminLevels, 1 do
		for q=1, lvlsc[i], 1 do
--			if type(lvls[i][q]) ~= "string" then
--				et.G_Print("i and q = " .. i .. " and " .. q .. " = nil\n")
--			else
--				et.G_Print("i and q = " .. i .. " and " .. q .. " = " .. lvls[i][q] .. "\n")
--			end
--et.G_Print(lvls[i][q] .. " and it is " .. string.len(lvls[i][q]) .. " long\n")
--et.G_Print(BangCommand .. " and it is " .. string.len(BangCommand) .. " long\n")
			if lvls[i][q] == BangCommand then
				admin_req = i
				break
			end
		end
	end

--et.G_Print("******************************    " .. AdminUserLevel(PlayerID) .. "\n")
--et.G_Print("******************************    " .. admin_req .. "\n")

	if AdminUserLevel(PlayerID) >= admin_req then
		if (string.lower(BangCommand) == k_commandprefix.."admintest") then
			adminStatus(PlayerID)
		elseif (string.lower(BangCommand) == k_commandprefix.."time" ) then
		 	local time = os.date("%I:%M:%S%p")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." The server time is " ..time.. "\n" )
		elseif (string.lower(BangCommand) == k_commandprefix.."date" ) then
			local date = os.date("%x %I:%M:%S%p")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." The server date is " ..date.. "\n" )
		elseif (string.lower(BangCommand) == k_commandprefix.."spree_record" ) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ".. tostring(oldspree2) .."\n" )
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ".. tostring(oldmapspree2) .."\n" )
		elseif (string.lower(BangCommand) == k_commandprefix.."spec999" ) then
			spec999()
		elseif (string.lower(BangCommand) == k_commandprefix.."tk_index" ) then
			local status = ""
			local name = et.Info_ValueForKey( et.trap_GetUserinfo( PlayerID ), "name" )
			if teamkillr[PlayerID] < -1 then
				status = "^1NOT OK"
			else
				status = "^2OK"
			end

			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Tk_index: ^7" .. name .. "^7 has a tk index of ^3" ..teamkillr[PlayerID] .. "^7 \[" .. status .. "^7\] \n" )
		elseif (string.lower(BangCommand) == k_commandprefix.."listcmds" ) then
			lstcomds( PlayerID )
		elseif (string.lower(BangCommand) == k_commandprefix.."durt" ) then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." k_panzersperteam = " .. k_panzersperteam .. " " .. k_panzersperteam2 .. "\n" )
		end
--	end

--Admin commands
	--level 3
  if (string.lower(BangCommand) == k_commandprefix.."gib" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			Gib(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gib:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."slap" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			burn(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Slap:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."setlevel" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel:^7 \[partname/id#\] \[level\]\n" )
		else
			commandSaid = true
			sldv = 1
			setlevel(Cvar1, 0)
			sldv = 0
			setlevel(Cvar1, Cvar2)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Setlevel:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."readconfig" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		et.trap_SendConsoleCommand( et.EXEC_APPEND, "exec kmod.cfg ; qsay ^3ReadConfig:^7 Config reloaded\n" )
		readconfig()
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3ReadConfig:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."spree_restart" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		spreerecord_reset()
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Spree reset:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."ban" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Ban:^7 \[partname/id#\] \[reason\]\n" )
		else
			commandSaid = true
			ban = true
			fullcom = "Ban"
			comds(Cvar1, Cvar2)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Ban:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."getip" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Getip:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			getip = true
			fullcom = "Getip"
			comds(Cvar1, PlayerID)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Getip:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."getguid" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Getguid:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			getguid = true
			fullcom = "Getguid"
			comds(Cvar1, PlayerID)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Getguid:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."makeshoutcaster" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Makeshoutcaster:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			makeshoutcaster = true
			fullcom = "Makeshoutcaster"
			comds(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Makeshoutcaster:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."removeshoutcaster" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Removeshoutcaster:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			removeshoutcaster = true
			fullcom = "Removeshoutcaster"
			comds(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Removeshoutcaster:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."makereferee" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Makereferee:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			makereferee = true
			fullcom = "Makereferee"
			comds(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Makereferee:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."removereferee" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Removereferee:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			removereferee = true
			fullcom = "Removereferee"
			comds(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Removereferee:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."gravity" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gravity:^7 Changes the gravity \[default = 800\]\n" )
		else
			local grav = tonumber(Cvar1)
			if grav then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "g_gravity " .. grav .. "\n" )
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gravity:^7 Gravity has been changed to " .. grav .. "\n" )
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gravity:^7 Please enter in only numbers\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Gravity:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."knifeonly" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knifeonly:^7 Disable or enable g_knifeonly \[0-1\]\n" )
		else
			local knife = tonumber(Cvar1)
			if knife >= 0 and knife <= 1 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "g_knifeonly " .. knife .. "\n" )
				if knife == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knifeonly:^7Knifeonly has been Enabled\n" )
				else
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knifeonly:^7Knifeonly has been Disabled\n" )
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knifeonly:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knifeonly:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."speed" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Speed:^7 Changes game speed \[default = 320\]\n" )
		else
			local speed2 = tonumber(Cvar1)
			if speed then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "g_speed " .. speed2 .. "\n" )
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Speed:^7 Game speed has been changed to " .. speed2 .. "\n" )
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Speed:^7 Please enter in only numbers\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Speed:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."knockback" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knockback:^7 Changes knockback \[default = 1000\]\n" )
		else
			local knock = tonumber(Cvar1)
			if knock then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "g_knockback " .. knock .. "\n" )
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knockback:^7 Knockback has been changed to " .. knock .. "\n" )
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knockback:^7 Please enter in only numbers\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Knockback:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."cheats" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Cheats:^7 Disable or enable cheats \[0-1\]\n" )
		else
			local cheat = tonumber(Cvar1)
			if cheat >= 0 and cheat <= 1 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "forcecvar sv_cheats " .. cheat .. "\n" )
				if cheat == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Cheats:^7Cheats have been Enabled\n" )
				else
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Cheats:^7Cheats have been Disabled\n" )
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Cheats:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Cheats:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."laser" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Laser:^7 Disable or enable g_debugbullets \[0-1\]\n" )
		else
			local laser = tonumber(Cvar1)
			if laser >= 0 and laser <= 1 then
				et.trap_SendConsoleCommand( et.EXEC_APPEND, "forcecvar g_debugbullets " .. laser .. "\n" )
				if laser == 1 then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Laser:^7Laser has been Enabled\n" )
				else
					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Laser:^7Laser has been Disabled\n" )
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Laser:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Laser:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."crazygravity" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Crazygravity:^7 Disable or enable crazygravity \[0-1\]\n" )
		else
			local crazy = tonumber(Cvar1)
			if crazy >= 0 and crazy <= 1 then
				if crazy == 1 then
					if CGactive == 0 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Crazygravity:^7 Crazygravity has been Enabled\n" )
						crazygravity = true
						crazydv = 1
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Crazygravity:^7 Crazygravity is already active\n" )
					end
				else
					if CGactive == 1 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Crazygravity:^7 Crazygravity has been Disabled.  Resetting gravity\n" )
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "g_gravity 800\n" )
						crazygravity = false
						crazydv = 0
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Crazygravity:^7 Crazygravity has already been disabled\n" )
					end
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Crazygravity:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Crazygravity:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."panzerwar" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Disable or enable panzerwar \[0-1\]\n" )
		else
			local panz = tonumber(Cvar1)
			local dspeed = (speed*2)
			if panz >= 0 and panz <= 1 then
				if panz == 1 then
					if panzdv == 0 then
						if frenzdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Frenzy mode must be disabled first\n" )
						elseif grendv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Grenadewar must be disabled first\n" )
						elseif snipdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Sniperwar must be disabled first\n" )
						else
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Panzerwar has been Enabled\n" )
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics 0 ; team_maxcovertops 0 ; team_maxfieldops 0 ; team_maxengineers 0 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers -1 ; g_speed " .. dspeed .. " ; forcecvar g_soldierchargetime 0\n" )
							panzdv = 1
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
									originalclass[p] = tonumber(et.gentity_get(p,"sess.latchPlayerType"))
									originalweap[p] = tonumber(et.gentity_get(p,"sess.latchPlayerWeapon"))
									if et.gentity_get(p,"health") > 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									end
								end
								et.gentity_set(p,"sess.latchPlayerType",0)
								et.gentity_set(p,"sess.latchPlayerWeapon",5)
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Panzerwar is already active\n" )
					end
				else
					if panzdv == 1 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Panzerwar has been Disabled.\n" )
						panzerwar_reset()
						panzdv = 0
						for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
							if et.gentity_get(p,"sess.sessionTeam") == 1 or et.gentity_get(p,"sess.sessionTeam") == 2 then
								if et.gentity_get(p,"health") >= 0 then
									et.G_Damage(p, p, 1022, 400, 24, 0)
									et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									et.gentity_set(p,"sess.latchPlayerType",originalclass[p])
									et.gentity_set(p,"sess.latchPlayerWeapon",originalweap[p])
								end
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Panzerwar has already been disabled\n" )
					end
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Panzerwar:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."frenzy" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Disable or enable frenzy \[0-1\]\n" )
		else
			local frenz = tonumber(Cvar1)
			if frenz >= 0 and frenz <= 1 then
				if frenz == 1 then
					if frenzdv == 0 then
						if panzdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Panzerwar must be disabled first\n" )
						elseif grendv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Grenadewar must be disabled first\n" )
						elseif snipdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Sniperwar must be disabled first\n" )
						else
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Frenzy has been Enabled\n" )
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics -1 ; team_maxcovertops -1 ; team_maxfieldops -1 ; team_maxengineers -1 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers 0\n" )
							frenzdv = 1
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
									if et.gentity_get(p,"health") > 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									end
								end
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Frenzy is already active\n" )
					end
				else
					if frenzdv == 1 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Frenzy has been Disabled.\n" )
						frenzdv = 0
						frenzy_reset()
						for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
							if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
								if et.gentity_get(p,"health") > 0 then
									et.G_Damage(p, p, 1022, 400, 24, 0)
									et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
								end
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Frenzy has already been disabled\n" )
					end
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Frenzy:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."grenadewar" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Disable or enable Grenadewar \[0-1\]\n" )
		else
			local gren = tonumber(Cvar1)
			if gren >= 0 and gren <= 1 then
				if gren == 1 then
					if grendv == 0 then
						if panzdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Panzerwar must be disabled first\n" )
						elseif frenzdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Frenzy must be disabled first\n" )
						elseif snipdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Sniperwar must be disabled first\n" )
						else
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Grenadewar has been Enabled\n" )
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics -1 ; team_maxcovertops -1 ; team_maxfieldops -1 ; team_maxengineers -1 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers 0\n" )
							grendv = 1
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
								if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
									if et.gentity_get(p,"health") > 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									end
								end
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Grenadewar is already active\n" )
					end
				else
					if grendv == 1 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Grenadewar has been Disabled.\n" )
						grendv = 0
						grenadewar_reset()
						for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
							if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
								if et.gentity_get(p,"health") > 0 then
									et.G_Damage(p, p, 1022, 400, 24, 0)
									et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
								end
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Grenadewar has already been disabled\n" )
					end
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Grenadewar:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."sniperwar" ) then
--	if AdminUserLevel(PlayerID) == 3 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Disable or enable Sniperwar \[0-1\]\n" )
		else
			local snip = tonumber(Cvar1)
			if snip >= 0 and snip <= 1 then
				if snip == 1 then
					if snipdv == 0 then
						if panzdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Panzerwar must be disabled first\n" )
						elseif frenzdv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Frenzy must be disabled first\n" )
						elseif grendv == 1 then
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Grenadewar must be disabled first\n" )
						else
							et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Sniperwar has been Enabled\n" )
							et.trap_SendConsoleCommand( et.EXEC_APPEND, "team_maxmedics -1 ; team_maxcovertops -1 ; team_maxfieldops -1 ; team_maxengineers -1 ; team_maxflamers 0 ; team_maxmortars 0 ; team_maxmg42s 0 ; team_maxpanzers 0\n" )
							snipdv = 1
							for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
									originalclass[p] = tonumber(et.gentity_get(p,"sess.latchPlayerType"))
									originalweap[p] = tonumber(et.gentity_get(p,"sess.latchPlayerWeapon"))
								if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
									if et.gentity_get(p,"health") > 0 then
										et.G_Damage(p, p, 1022, 400, 24, 0)
										et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
									end
								end
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Sniperwar is already active\n" )
					end
				else
					if snipdv == 1 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Sniperwar has been Disabled.\n" )
						snipdv = 0
						sniperwar_reset()
						for p=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
							if et.gentity_get(p,"sess.sessionTeam") >= 1 and et.gentity_get(p,"sess.sessionTeam") < 3 then
								if et.gentity_get(p,"health") > 0 then
									et.G_Damage(p, p, 1022, 400, 24, 0)
									et.gentity_set(p,"health",(et.gentity_get(p,"health")-400)) -- in case they recently spawned and are protected by spawn shield
								end
									et.gentity_set(p,"sess.latchPlayerType",originalclass[p])
									et.gentity_set(p,"sess.latchPlayerWeapon",originalweap[p])
							end
						end
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Sniperwar has already been disabled\n" )
					end
				end
			else
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 Valid values are \[0-1\]\n" )
			end
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Sniperwar:^7 command unavailible due to lack of required admin status!\n" )
--	end
  end





	--level 2
  if (string.lower(BangCommand) == k_commandprefix.."kick" ) then
--	if AdminUserLevel(PlayerID) >= 2 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Kick:^7 \[partname/id#\] \[time\] \[reason\]\n" )
		else
			commandSaid = true
			kick = true
			fullcom = "Kick"
			comds(Cvar1, Cvar2, PlayerID)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Kick:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."warn" ) then
--	if AdminUserLevel(PlayerID) >= 2 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Warn:^7 \[partname/id#\] \[reason\]\n" )
		else
			commandSaid = true
			warn = true
			fullcom = "Warn"
			comds(Cvar1, Cvar2, PlayerID)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Warn:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."mute" ) then
--	if AdminUserLevel(PlayerID) >= 2 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Mute:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			mute = true
			fullcom = "Mute"
			comds(Cvar1, "dv", PlayerID)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Mute:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."pmute" ) then
--	if AdminUserLevel(PlayerID) >= 2 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Pmute:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			pmute = true
			fullcom = "Pmute"
			comds(Cvar1, "dv", PlayerID)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Pmute:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."timelimit" ) then
--	if AdminUserLevel(PlayerID) >= 2 then
		local timel = tonumber(Cvar1)
		if timel then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, "timelimit " .. timel .. "\n" )
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Timelimit:^7 Timelimit has been changed to " .. timel .. "\n" )
		else
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Timelimit:^7 Please enter in only numbers\n" )
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Unlock:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."unmute" ) then
--	if AdminUserLevel(PlayerID) >= 2 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Unmute:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			unmute = true
			fullcom = "Unmute"
			comds(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Unmute:^7 command unavailible due to lack of required admin status!\n" )
--	end
  elseif (string.lower(BangCommand) == k_commandprefix.."finger" ) then
--	if AdminUserLevel(PlayerID) >= 2 then
		if Cvarct < 3 then
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Finger:^7 \[partname/id#\]\n" )
		else
			commandSaid = true
			finger = true
			fullcom = "Finger"
			comds(Cvar1)
		end
--	else
--		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..say_parms.." ^3Map_restart:^7 command unavailible due to lack of required admin status!\n" )
--	end
  end
end
end

function lstcomds( PlayerID )
	for i=0, k_maxAdminLevels, 1 do
		if AdminUserLevel(PlayerID) >= i then
			et.trap_SendServerCommand(PlayerID, string.format("print \"Level " .. i .. " Commands\n"))
			et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n"))
			for q=1, tonumber(lvlsc[i]), 3 do
				local b = q
				local m = q + 1
				local e = q + 2
				if lvls[i][b] == nil then
					lvls[i][b] = " "
				end
				if lvls[i][m] == nil then
					lvls[i][m] = " "
				end
				if lvls[i][e] == nil then
					lvls[i][e] = " "
				end
				et.trap_SendServerCommand(PlayerID, string.format('print \"%21s^1|^7 %21s^1|^7 %21s^1|^7\n"', lvls[i][b], lvls[i][m], lvls[i][e]))
			end
			et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n\n"))
		end
	end


--	if AdminUserLevel(PlayerID) >= 0 then
--		et.trap_SendServerCommand(PlayerID, string.format("print \"Level 0 Commands\n"))
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n"))
--		for q=1, tonumber(lvl0c), 3 do
--			local b = q
--			local m = q + 1
--			local e = q + 2
--			if lvl0[b] == nil then
--				lvl0[b] = " "
--			end
--			if lvl0[m] == nil then
--				lvl0[m] = " "
--			end
--			if lvl0[e] == nil then
--				lvl0[e] = " "
--			end
--			et.trap_SendServerCommand(PlayerID, string.format('print \"%21s^1|^7 %21s^1|^7 %21s^1|^7\n"', lvl0[b], lvl0[m], lvl0[e]))
--		end
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n\n"))
--	end
--	if AdminUserLevel(PlayerID) >= 1 then
--		et.trap_SendServerCommand(PlayerID, string.format("print \"Level 1 Commands \[PROTECTED USER\]\n"))
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n"))
--		for w=1, tonumber(lvl1c), 3 do
--			local b = w
--			local m = w + 1
--			local e = w + 2
--			if lvl1[b] == nil then
--				lvl1[b] = " "
--			end
--			if lvl1[m] == nil then
--				lvl1[m] = " "
--			end
--			if lvl1[e] == nil then
--				lvl1[e] = " "
--			end
--			et.trap_SendServerCommand(PlayerID, string.format('print \"%21s^1|^7 %21s^1|^7 %21s^1|^7\n"', lvl1[b], lvl1[m], lvl1[e]))
--		end
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n\n"))
--	end
--	if AdminUserLevel(PlayerID) >= 2 then
--		et.trap_SendServerCommand(PlayerID, string.format("print \"Level 2 Commands\n"))
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n"))
--		for r=1, tonumber(lvl2c), 3 do
--			local b = r
--			local m = r + 1
--			local e = r + 2
--			if lvl2[b] == nil then
--				lvl2[b] = " "
--			end
--			if lvl2[m] == nil then
--				lvl2[m] = " "
--			end
--			if lvl2[e] == nil then
--				lvl2[e] = " "
--			end
--			et.trap_SendServerCommand(PlayerID, string.format('print \"%21s^1|^7 %21s^1|^7 %21s^1|^7\n"', lvl2[b], lvl2[m], lvl2[e]))
--		end
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n\n"))
--	end
--	if AdminUserLevel(PlayerID) == 3 then
--		et.trap_SendServerCommand(PlayerID, string.format("print \"Level 3 Commands \[SILENT COMMANDS WITH /sc \(command\)\]\n"))
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n"))
--		for t=1, tonumber(lvl3c), 3 do
--			local b = t
--			local m = t + 1
--			local e = t + 2
--			if lvl3[b] == nil then
--				lvl3[b] = " "
--			end
--			if lvl3[m] == nil then
--				lvl3[m] = " "
--			end
--			if lvl3[e] == nil then
--				lvl3[e] = " "
--			end
--			et.trap_SendServerCommand(PlayerID, string.format('print \"%21s^1|^7 %21s^1|^7 %21s^1|^7\n"', lvl3[b], lvl3[m], lvl3[e]))
--		end
--		et.trap_SendServerCommand(PlayerID, string.format("print \"^1-------------------------------------------------------------------\n\n"))
--	end
end

function et_Obituary( victim, killer, meansOfDeath )
	local killername= ""

	local killedname=et.Info_ValueForKey( et.trap_GetUserinfo( victim ), "name" )
      local victimteam = tonumber(et.gentity_get(victim, "sess.sessionTeam")) 
      local killerteam = tonumber(et.gentity_get(killer, "sess.sessionTeam"))
	weapon = ""

	if victimteam ~= killerteam and killer ~= 1022 and killer ~= victim then
		killername=et.Info_ValueForKey( et.trap_GetUserinfo( killer ), "name" )
		lastblood = killername
		khp[killer] = (mtime + 5000)
		if khp[victim] == nil then
			khp[victim] = 0
		end
	end

	if victimteam ~= killerteam and killer ~= 1022 and killer ~= victim then
		if killer ~= victim then
			if firstblood == 0 then
				firstblood = 1
					if k_firstblood == 1 then
						local str = string.gsub(k_fb_message, "#killer#", killername)

						et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..fb_location.." "..str.."\n" )
						if k_firstbloodsound == 1 then
							if k_noisereduction == 1 then
								et.G_ClientSound(killer, firstbloodsound)
							else
								et.G_globalSound(firstbloodsound)
							end
						end
					end
			end
		end
 
      	local killerhp = et.gentity_get(killer, "health") 
		if k_killerhpdisplay == 1 then
			if khp[victim] < mtime then
				if (killerhp>=75) then
--         				et.trap_SendServerCommand(victim, ("b 8 \"^" .. k_color .. "You got owned!"))
					et.trap_SendServerCommand(victim, ("b 8 \"^7" .. killername .. "^" .. k_color .. "'s hp (^o" ..killerhp .. "^" .. k_color .. ")"))
					if adrenaline[killer] == 1 then
						et.trap_SendServerCommand(victim, ("b 8 \"^7" .. killername .. "^" .. k_color .. " is an adrenaline junkie!\""))
					end
   				elseif (killerhp>=50 and killerhp<=74) then
--					et.trap_SendServerCommand(victim, string.format("b 8 \"^" .. k_color .. "You're not a total newb."))
					et.trap_SendServerCommand(victim, string.format("b 8 \"^7" ..killername .. "^" .. k_color .. "'s hp (^o" ..killerhp.. "^" .. k_color .. ")"))
					if adrenaline[killer] == 1 then
						et.trap_SendServerCommand(victim, ("b 8 \"^7" .. killername .. "^" .. k_color .. " is an adrenaline junkie!\""))
					end
				elseif (killerhp>=25 and killerhp<=49) then
--					et.trap_SendServerCommand(victim, string.format("b 8 \"^" .. k_color .. "Try Harder!"))
					et.trap_SendServerCommand(victim, string.format("b 8 \"^7" ..killername.. "^" .. k_color .. "'s hp (^o" ..killerhp.. "^" .. k_color .. ")"))
					if adrenaline[killer] == 1 then
						et.trap_SendServerCommand(victim, ("b 8 \"^7" .. killername .. "^" .. k_color .. " is an adrenaline junkie!\""))
					end
				elseif (killerhp>0 and killerhp<=24) then
--					et.trap_SendServerCommand(victim, string.format("b 8 \"^" .. k_color .. "Almost!"))
					et.trap_SendServerCommand(victim, string.format("b 8 \"^7" ..killername.. "^" .. k_color .. "'s hp (^o" ..killerhp.. "^" .. k_color .. ")"))
					if adrenaline[killer] == 1 then
						et.trap_SendServerCommand(victim, ("b 8 \"^7" .. killername .. "^" .. k_color .. " is an adrenaline junkie!\""))
					end
				end 
			end
				if (killerhp<=0) then
					if meansOfDeath == 4 or meansOfDeath == 18 or meansOfDeath == 18 or meansOfDeath == 26 or meansOfDeath == 27 or meansOfDeath == 30 or meansOfDeath == 44 or meansOfDeath == 43 then
						et.trap_SendServerCommand(victim, string.format("b 8 \"^" .. k_color .. "You were owned by ^7" .. killername .. "^" .. k_color .. "'s explosive inheritance"))
					end
				end
		end
	end

	kills(victim, killer, meansOfDeath, weapon)
	deaths(victim, killer, meansOfDeath, weapon)

	if meansOfDeath == 64 or meansOfDeath == 63 then
		switchteam[victim] = 1
	else
		switchteam[victim] = 0
	end

--Weapons used!

	if (meansOfDeath==0) then
		weapon="UNKNOWN"
	elseif (meansOfDeath==1) then
		weapon="MACHINEGUN"
	elseif (meansOfDeath==2) then
		weapon="BROWNING"
	elseif (meansOfDeath==3) then
		weapon="MG42"
	elseif (meansOfDeath==4) then
		weapon="GRENADE"
	elseif (meansOfDeath==5) then
		weapon="ROCKET"
	elseif (meansOfDeath==6) then
		weapon="KNIFE"
	elseif (meansOfDeath==7) then
		weapon="LUGER"
	elseif (meansOfDeath==8) then
		weapon="COLT"
	elseif (meansOfDeath==9) then
		weapon="MP40"
	elseif (meansOfDeath==10) then
		weapon="THOMPSON"
	elseif (meansOfDeath==11) then
		weapon="STEN"
	elseif (meansOfDeath==12) then
		weapon="GARAND"
	elseif (meansOfDeath==13) then
		weapon="SNOOPERSCOPE"
	elseif (meansOfDeath==14) then
		weapon="SILENCER"
	elseif (meansOfDeath==15) then
		weapon="FG42"
	elseif (meansOfDeath==16) then
		weapon="FG42SCOPE"
	elseif (meansOfDeath==17) then
		weapon="PANZERFAUST"
	elseif (meansOfDeath==18) then
		weapon="GRENADE_LAUNCHER"
	elseif (meansOfDeath==19) then
		weapon="FLAMETHROWER"
	elseif (meansOfDeath==20) then
		weapon="GRENADE_PINEAPPLE"
	elseif (meansOfDeath==21) then
		weapon="CROSS"
	elseif (meansOfDeath==22) then
		weapon="MAPMORTAR"
	elseif (meansOfDeath==23) then
		weapon="MAPMORTAR_SPLASH"
	elseif (meansOfDeath==24) then
		weapon="KICKED"
	elseif (meansOfDeath==25) then
		weapon="GRABBER"
	elseif (meansOfDeath==26) then
		weapon="DYNAMITE"
	elseif (meansOfDeath==27) then
		weapon="AIRSTRIKE"
	elseif (meansOfDeath==28) then
		weapon="SYRINGE"
	elseif (meansOfDeath==29) then
		weapon="AMMO"
	elseif (meansOfDeath==30) then
		weapon="ARTY"
	elseif (meansOfDeath==31) then
		weapon="WATER"
	elseif (meansOfDeath==32) then
		weapon="SLIME"
	elseif (meansOfDeath==33) then
		weapon="LAVA"
	elseif (meansOfDeath==34) then
		weapon="CRUSH"
	elseif (meansOfDeath==35) then
		weapon="TELEFRAG"
	elseif (meansOfDeath==36) then
		weapon="FALLING"
	elseif (meansOfDeath==37) then
		weapon = "SUICIDE"
	elseif (meansOfDeath==38) then
		weapon="TARGET_LASER"
	elseif (meansOfDeath==39) then
		weapon="TRIGGER_HURT"
	elseif (meansOfDeath==40) then
		weapon="EXPLOSIVE"
	elseif (meansOfDeath==41) then
		weapon="CARBINE"
	elseif (meansOfDeath==42) then
		weapon="KAR98"
	elseif (meansOfDeath==43) then
		weapon="GPG40"
	elseif (meansOfDeath==44) then
		weapon="M7"
	elseif (meansOfDeath==45) then
		weapon="LANDMINE"
	elseif (meansOfDeath==46) then
		weapon="SATCHEL"
	elseif (meansOfDeath==47) then
		weapon="TRIPMINE"
	elseif (meansOfDeath==48) then
		weapon="SMOKEBOMB"
	elseif (meansOfDeath==49) then
		weapon="MOBILE_MG42"
	elseif (meansOfDeath==50) then
		weapon="SILENCED_COLT"
	elseif (meansOfDeath==51) then
		weapon="GARAND_SCOPE"
	elseif (meansOfDeath==52) then
		weapon="CRUSH_CONSTRUCTION"
	elseif (meansOfDeath==53) then
		weapon="CRUSH_CONSTRUCTIONDEATH"
	elseif (meansOfDeath==54) then
		weapon="CRUSH_CONSTRUCTIONDEATH_NOATTACKER"
	elseif (meansOfDeath==55) then
		weapon="K43"
	elseif (meansOfDeath==56) then
		weapon="K43_SCOPE"
	elseif (meansOfDeath==57) then
		weapon="MORTAR"
	elseif (meansOfDeath==58) then
		weapon="AKIMBO_COLT"
	elseif (meansOfDeath==59) then
		weapon="AKIMBO_LUGER"
	elseif (meansOfDeath==60) then
		weapon="AKIMBO_SILENCEDCOLT"
	elseif (meansOfDeath==61) then
		weapon="AKIMBO_SILENCEDLUGER"
	elseif (meansOfDeath==62) then
		weapon="SMOKEGRENADE"
	elseif (meansOfDeath==63) then
		weapon="SWAP_SPACES"
	elseif (meansOfDeath==64) then
		weapon="SWITCH_TEAM"
	end

--et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay <<<<<<INSERT OBITUARY HERE>>>>>>\n" )
--return 1
--et.trap_SendServerCommand(-1, string.format("play sound/misc/ludicrouskill.wav"))
--return
end

function kills(victim, killer, meansOfDeath, weapon)

	if (meansOfDeath==0) then
		weapon="UNKNOWN"
	elseif (meansOfDeath==1) then
		weapon="MACHINEGUN"
	elseif (meansOfDeath==2) then
		weapon="BROWNING"
	elseif (meansOfDeath==3) then
		weapon="MG42"
	elseif (meansOfDeath==4) then
		weapon="GRENADE"
	elseif (meansOfDeath==5) then
		weapon="ROCKET"
	elseif (meansOfDeath==6) then
		weapon="KNIFE"
	elseif (meansOfDeath==7) then
		weapon="LUGER"
	elseif (meansOfDeath==8) then
		weapon="COLT"
	elseif (meansOfDeath==9) then
		weapon="MP40"
	elseif (meansOfDeath==10) then
		weapon="THOMPSON"
	elseif (meansOfDeath==11) then
		weapon="STEN"
	elseif (meansOfDeath==12) then
		weapon="GARAND"
	elseif (meansOfDeath==13) then
		weapon="SNOOPERSCOPE"
	elseif (meansOfDeath==14) then
		weapon="SILENCER"
	elseif (meansOfDeath==15) then
		weapon="FG42"
	elseif (meansOfDeath==16) then
		weapon="FG42SCOPE"
	elseif (meansOfDeath==17) then
		weapon="PANZERFAUST"
	elseif (meansOfDeath==18) then
		weapon="GRENADE_LAUNCHER"
	elseif (meansOfDeath==19) then
		weapon="FLAMETHROWER"
	elseif (meansOfDeath==20) then
		weapon="GRENADE_PINEAPPLE"
	elseif (meansOfDeath==21) then
		weapon="CROSS"
	elseif (meansOfDeath==22) then
		weapon="MAPMORTAR"
	elseif (meansOfDeath==23) then
		weapon="MAPMORTAR_SPLASH"
	elseif (meansOfDeath==24) then
		weapon="KICKED"
	elseif (meansOfDeath==25) then
		weapon="GRABBER"
	elseif (meansOfDeath==26) then
		weapon="DYNAMITE"
	elseif (meansOfDeath==27) then
		weapon="AIRSTRIKE"
	elseif (meansOfDeath==28) then
		weapon="SYRINGE"
	elseif (meansOfDeath==29) then
		weapon="AMMO"
	elseif (meansOfDeath==30) then
		weapon="ARTY"
	elseif (meansOfDeath==31) then
		weapon="WATER"
	elseif (meansOfDeath==32) then
		weapon="SLIME"
	elseif (meansOfDeath==33) then
		weapon="LAVA"
	elseif (meansOfDeath==34) then
		weapon="CRUSH"
	elseif (meansOfDeath==35) then
		weapon="TELEFRAG"
	elseif (meansOfDeath==36) then
		weapon="FALLING"
	elseif (meansOfDeath==37) then
		weapon = "SUICIDE"
	elseif (meansOfDeath==38) then
		weapon="TARGET_LASER"
	elseif (meansOfDeath==39) then
		weapon="TRIGGER_HURT"
	elseif (meansOfDeath==40) then
		weapon="EXPLOSIVE"
	elseif (meansOfDeath==41) then
		weapon="CARBINE"
	elseif (meansOfDeath==42) then
		weapon="KAR98"
	elseif (meansOfDeath==43) then
		weapon="GPG40"
	elseif (meansOfDeath==44) then
		weapon="M7"
	elseif (meansOfDeath==45) then
		weapon="LANDMINE"
	elseif (meansOfDeath==46) then
		weapon="SATCHEL"
	elseif (meansOfDeath==47) then
		weapon="TRIPMINE"
	elseif (meansOfDeath==48) then
		weapon="SMOKEBOMB"
	elseif (meansOfDeath==49) then
		weapon="MOBILE_MG42"
	elseif (meansOfDeath==50) then
		weapon="SILENCED_COLT"
	elseif (meansOfDeath==51) then
		weapon="GARAND_SCOPE"
	elseif (meansOfDeath==52) then
		weapon="CRUSH_CONSTRUCTION"
	elseif (meansOfDeath==53) then
		weapon="CRUSH_CONSTRUCTIONDEATH"
	elseif (meansOfDeath==54) then
		weapon="CRUSH_CONSTRUCTIONDEATH_NOATTACKER"
	elseif (meansOfDeath==55) then
		weapon="K43"
	elseif (meansOfDeath==56) then
		weapon="K43_SCOPE"
	elseif (meansOfDeath==57) then
		weapon="MORTAR"
	elseif (meansOfDeath==58) then
		weapon="AKIMBO_COLT"
	elseif (meansOfDeath==59) then
		weapon="AKIMBO_LUGER"
	elseif (meansOfDeath==60) then
		weapon="AKIMBO_SILENCEDCOLT"
	elseif (meansOfDeath==61) then
		weapon="AKIMBO_SILENCEDLUGER"
	elseif (meansOfDeath==62) then
		weapon="SMOKEGRENADE"
	elseif (meansOfDeath==63) then
		weapon="SWAP_SPACES"
	elseif (meansOfDeath==64) then
		weapon="SWITCH_TEAM"
	end

	local kil = tonumber(killer)
	local killername = ""

	victimname=et.Info_ValueForKey( et.trap_GetUserinfo( victim ), "name" )
	playerlastkilled[killer] = victim
	killedwithweapk[killer] = tostring(weapon)
	local victimteam = tonumber(et.gentity_get(victim, "sess.sessionTeam")) 
	local killerteam = tonumber(et.gentity_get(killer, "sess.sessionTeam"))

	if killer == 1022 then
		killername="The World"
		if k_sprees == 1 then
			if gibbed[victim] == 0 then
				if (killingspree[victim]>=5) then
					local str = string.gsub(k_end_message3, "#victim#", victimname)
					local str = string.gsub(str, "#kills#", killingspree[victim])
					local str = string.gsub(str, "#killer#", killername)

					et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
					killingspree[victim]=0
				else
					killingspree[victim]=0
				end
			else
				gibbed[victim] = 0
			end
		end

		if k_spreerecord == 1 then
			if killr[victim] > spreerecordkills then
				spreerecord(victim, killr[victim])
				--killr[victim] = 0
			else
				--killr[victim] = 0
			end
			if killr[victim] > mapspreerecordkills then
				mapspreerecord(victim, killr[victim])
				killr[victim] = 0
			else
				killr[victim] = 0
			end
		end


--		multikill[victim]=0
		flakmonkey[victim]=0
	else
		killername=et.Info_ValueForKey( et.trap_GetUserinfo( killer ), "name" )
	end

	if (killer~=victim) then
		if k_sprees == 1 then
			deathspree[killer]=0
		end
	else
	end

if Gamestate == 0 then
	if k_teamkillrestriction == 1 then
		if victimteam == killerteam and killer ~= victim and killer ~= 1022 then
			if AdminUserLevel(killer) < k_tk_protect then

				local warning = (k_tklimit_low + 1)
				local pbkiller = killer + 1
				if meansOfDeath == 17 or meansOfDeath == 43 or meansOfDeath == 44 or meansOfDeath == 4 or meansOfDeath == 18 or meansOfDeath == 57 or meansOfDeath == 30  or meansOfDeath == 27 or meansOfDeath == 49 or meansOfDeath == 3 then
					teamkillr[killer] = teamkillr[killer] - 0.75
				elseif meansOfDeath == 30  or meansOfDeath == 27 then
					teamkillr[killer] = teamkillr[killer] - 0.65
				elseif meansOfDeath == 45 then    -- Mines do nothing!
				else
					teamkillr[killer] = teamkillr[killer] - 1
				end
				if warning > teamkillr[killer] and k_tklimit_low < teamkillr[killer] then
					if k_advancedpms == 1 then
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "m2 " .. killername .. " ^1You are making to many teamkills please be more careful or you will be kicked!\n" )
						et.G_ClientSound(killer, pmsound)
					else
						et.trap_SendConsoleCommand( et.EXEC_APPEND, "m " .. killername .. " ^1You are making to many teamkills please be more careful or you will be kicked!\n" )
					end
				elseif teamkillr[killer] <= k_tklimit_low then
					et.trap_SendConsoleCommand( et.EXEC_APPEND, "pb_sv_kick " .. pbkiller .. " 10 Too many teamkills\n" )
				end
			end
		else
			if killer ~= victim then
				if killer ~= 1022 then
					teamkillr[killer] = teamkillr[killer] + 1
					if teamkillr[killer] > k_tklimit_high then
						teamkillr[killer] = k_tklimit_high
					end
				end
			end
		end
	end
end


	
if k_multikills == 1 then
	if killer ~= victim and victimteam ~= killerteam and killer ~= 1022 then
		local dvtime = (k_multikill_time * 1000)

		multikill[killer]=multikill[killer]+1

		if (multikill[killer]==1) then 
			kill1[killer]=mtime

		elseif (multikill[killer]==2) then 
			kill2[killer]=mtime
			local mktime=((kill2[killer]-kill1[killer])/1000)

			if ((kill2[killer]-kill1[killer])<=dvtime) then 
				local str = string.gsub(k_mk_message1, "#killer#", killername)

				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..mk_location.." "..str.."\n")
				if k_multikillsounds == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, doublekillsound)
					else
						et.G_globalSound(doublekillsound) 
					end
				end
			else 
				multikill[killer]=0
				multikill[killer]=multikill[killer]+1 
				kill1[killer]=mtime
			end 
		elseif (multikill[killer]==3) then 
			kill3[killer]=tonumber(mtime)
			local mktime=((kill3[killer]-kill2[killer])/1000) 

			if ((kill3[killer]-kill2[killer])<=dvtime) then 
				local str = string.gsub(k_mk_message2, "#killer#", killername)
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..mk_location.." "..str.."\n")
				if k_multikillsounds == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, multikillsound)
					else
						et.G_globalSound(multikillsound) 
					end
				end
			else 
				multikill[killer]=0 
				multikill[killer]=multikill[killer]+1
				kill1[killer]=mtime
			end
		elseif (multikill[killer]==4) then 
			kill4[killer]=tonumber(mtime)
			local mktime=((kill4[killer]-kill3[killer])/1000) 

			if ((kill4[killer]-kill3[killer])<=dvtime) then 
				local str = string.gsub(k_mk_message3, "#killer#", killername)

				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..mk_location.." "..str.."\n")
				if k_multikillsounds == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, megakillsound)
					else
						et.G_globalSound(megakillsound) 
					end
				end
			else 
				multikill[killer]=0 
				multikill[killer]=multikill[killer]+1
				kill1[killer]=mtime
			end
		elseif (multikill[killer]==5) then 
			kill5[killer]=tonumber(mtime)
			local mktime=((kill5[killer]-kill4[killer])/1000) 

			if ((kill5[killer]-kill4[killer])<=dvtime) then 
				local str = string.gsub(k_mk_message4, "#killer#", killername)

				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..mk_location.." "..str.."\n")
				if k_multikillsounds == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, ultrakillsound)
					else
						et.G_globalSound(ultrakillsound) 
					end
				end
			else 
				multikill[killer]=0 
				multikill[killer]=multikill[killer]+1
				kill1[killer]=mtime
			end
		elseif (multikill[killer]==6) then 
			kill6[killer]=tonumber(mtime)
			local mktime=((kill6[killer]-kill5[killer])/1000) 

			if ((kill6[killer]-kill5[killer])<=dvtime) then 
				local str = string.gsub(k_mk_message5, "#killer#", killername)

				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..mk_location.." "..str.."\n")
				if k_multikillsounds == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, monsterkillsound)
					else
						et.G_globalSound(monsterkillsound)
					end
				end
			else 
				multikill[killer]=0
				multikill[killer]=multikill[killer]+1 
				kill1[killer]=mtime
			end
		elseif (multikill[killer]==7) then 
			kill7[killer]=tonumber(mtime)
			local mktime=((kill7[killer]-kill6[killer])/1000) 

			if ((kill6[killer]-kill5[killer])<=dvtime) then 
				local str = string.gsub(k_mk_message6, "#killer#", killername)

				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..mk_location.." "..str.."\n")
				if k_multikillsounds == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, ludicrouskillsound)
					else
						et.G_globalSound(ludicrouskillsound)
					end
				end
			else 
				multikill[killer]=0
				multikill[killer]=multikill[killer]+1 
				kill1[killer]=mtime
			end
		elseif (multikill[killer]==8) then 
			kill8[killer]=tonumber(mtime)
			local mktime=((kill8[killer]-kill7[killer])/1000) 

			if ((kill6[killer]-kill5[killer])<=dvtime) then 
				local str = string.gsub(k_mk_message7, "#killer#", killername)

				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..mk_location.." "..str.."\n")
				if k_multikillsounds == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, holyshitsound)
					else
						et.G_globalSound(holyshitsound)
					end
				end

				multikill[killer]=0
			else 
				multikill[killer]=0
				multikill[killer]=multikill[killer]+1 
				kill1[killer]=mtime
			end
		end
	else
		multikill[killer]=0
	end
end


if killer ~= 1022 then
if killer ~= victim and victimteam ~= killerteam then
	if k_spreerecord == 1 then
		killr[killer] = killr[killer] + 1
	end

	if k_sprees == 1 then
		killingspree[killer]=killingspree[killer]+1

		if (killingspree[killer]==k_spree1_amount) then
			local str = string.gsub(k_ks_message1, "#killer#", killername)
			local str = string.gsub(str, "#kills#", killingspree[killer])

--			et.trap_SendServerCommand( killer,"cp \"^1KILLINGSPREE!\"\n")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			if k_spreesounds == 1 then
				if k_noisereduction == 1 then
					et.G_ClientSound(killer, killingspreesound)
				else
					et.G_globalSound(killingspreesound)
				end
			end
		elseif (killingspree[killer]==k_spree2_amount) then
			local str = string.gsub(k_ks_message2, "#killer#", killername)
			local str = string.gsub(str, "#kills#", killingspree[killer])

--			et.trap_SendServerCommand( killer,"cp \"^1RAMPAGE!\"\n")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			if k_spreesounds == 1 then
				if k_noisereduction == 1 then
					et.G_ClientSound(killer, rampagesound)
				else
					et.G_globalSound(rampagesound)
				end
			end
		elseif (killingspree[killer]==k_spree3_amount) then
			local str = string.gsub(k_ks_message3, "#killer#", killername)
			local str = string.gsub(str, "#kills#", killingspree[killer])

--			et.trap_SendServerCommand( killer,"cp \"^1DOMINATION!\"\n")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			if k_spreesounds == 1 then
				if k_noisereduction == 1 then
					et.G_ClientSound(killer, dominatingsound)
				else
					et.G_globalSound(dominatingsound)
				end
			end
		elseif (killingspree[killer]==k_spree4_amount) then
			local str = string.gsub(k_ks_message4, "#killer#", killername)
			local str = string.gsub(str, "#kills#", killingspree[killer])

--			et.trap_SendServerCommand( killer,"cp \"^1UNSTOPABLE!\"\n")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			if k_spreesounds == 1 then
				if k_noisereduction == 1 then
					et.G_ClientSound(killer, unstopablesound)
				else
					et.G_globalSound(unstopablesound)
				end
			end
		elseif (killingspree[killer]==k_spree5_amount) then
			local str = string.gsub(k_ks_message5, "#killer#", killername)
			local str = string.gsub(str, "#kills#", killingspree[killer])

--			et.trap_SendServerCommand( killer,"cp \"^1GODLIKE!\"\n")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			if k_spreesounds == 1 then
				if k_noisereduction == 1 then
					et.G_ClientSound(killer, godlikesound)
				else
					et.G_globalSound(godlikesound)
				end
			end
		elseif (killingspree[killer]==k_spree6_amount) then
			local str = string.gsub(k_ks_message6, "#killer#", killername)
			local str = string.gsub(str, "#kills#", killingspree[killer])

--			et.trap_SendServerCommand( killer,"cp \"^1WICKED SICK!\"\n")
			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			if k_spreesounds == 1 then
				if k_noisereduction == 1 then
					et.G_ClientSound(killer, wickedsicksound)
				else
					et.G_globalSound(wickedsicksound)
				end
			end
		end

	end
else
	if (killingspree[killer]>=5) then
		if killer == victim then
			local str = string.gsub(k_end_message2, "#victim#", victimname)
			local str = string.gsub(str, "#kills#", killingspree[victim])
			local str = string.gsub(str, "#killer#", killername)

			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			killingspree[killer]=0
		else
			local str = string.gsub(k_end_message4, "#victim#", victimname)
			local str = string.gsub(str, "#kills#", killingspree[killer])
			local str = string.gsub(str, "#killer#", killername)

			et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
			killingspree[killer]=0
		end
	else
		killingspree[killer]=0
	end

	if k_spreerecord == 1 then
		if killr[victim] > spreerecordkills then
			spreerecord(victim, killr[victim])
			--killr[victim] = 0
		else
			--killr[victim] = 0
		end
		if killr[victim] > mapspreerecordkills then
			mapspreerecord(victim, killr[victim])
			killr[victim] = 0
		else
			killr[victim] = 0
		end
	end
end
end

if k_flakmonkey == 1 then
	if (meansOfDeath==17 or meansOfDeath==43 or meansOfDeath==44) then
		if killer ~= victim and victimteam ~= killerteam then
			flakmonkey[killer]=flakmonkey[killer]+1

			if (flakmonkey[killer]==3) then
				local str = string.gsub(k_fm_message, "#killer#", killername)

--				et.trap_SendServerCommand( killer,"cp \"^1FLAKMONKEY!\"\n")
				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..fm_location.." "..str.."\n" )
				if k_flakmonkeysound == 1 then
					if k_noisereduction == 1 then
						et.G_ClientSound(killer, flakmonkeysound)
					else
						et.G_globalSound(flakmonkeysound)
					end
				end

				flakmonkey[killer]=0
			end

		else
				flakmonkey[killer]=0
		end
	else
				flakmonkey[killer]=0
	end
end

end

function deaths(victim, killer, meansOfDeath, weapon)

	if (meansOfDeath==0) then
		weapon="UNKNOWN"
	elseif (meansOfDeath==1) then
		weapon="MACHINEGUN"
	elseif (meansOfDeath==2) then
		weapon="BROWNING"
	elseif (meansOfDeath==3) then
		weapon="MG42"
	elseif (meansOfDeath==4) then
		weapon="GRENADE"
	elseif (meansOfDeath==5) then
		weapon="ROCKET"
	elseif (meansOfDeath==6) then
		weapon="KNIFE"
	elseif (meansOfDeath==7) then
		weapon="LUGER"
	elseif (meansOfDeath==8) then
		weapon="COLT"
	elseif (meansOfDeath==9) then
		weapon="MP40"
	elseif (meansOfDeath==10) then
		weapon="THOMPSON"
	elseif (meansOfDeath==11) then
		weapon="STEN"
	elseif (meansOfDeath==12) then
		weapon="GARAND"
	elseif (meansOfDeath==13) then
		weapon="SNOOPERSCOPE"
	elseif (meansOfDeath==14) then
		weapon="SILENCER"
	elseif (meansOfDeath==15) then
		weapon="FG42"
	elseif (meansOfDeath==16) then
		weapon="FG42SCOPE"
	elseif (meansOfDeath==17) then
		weapon="PANZERFAUST"
	elseif (meansOfDeath==18) then
		weapon="GRENADE_LAUNCHER"
	elseif (meansOfDeath==19) then
		weapon="FLAMETHROWER"
	elseif (meansOfDeath==20) then
		weapon="GRENADE_PINEAPPLE"
	elseif (meansOfDeath==21) then
		weapon="CROSS"
	elseif (meansOfDeath==22) then
		weapon="MAPMORTAR"
	elseif (meansOfDeath==23) then
		weapon="MAPMORTAR_SPLASH"
	elseif (meansOfDeath==24) then
		weapon="KICKED"
	elseif (meansOfDeath==25) then
		weapon="GRABBER"
	elseif (meansOfDeath==26) then
		weapon="DYNAMITE"
	elseif (meansOfDeath==27) then
		weapon="AIRSTRIKE"
	elseif (meansOfDeath==28) then
		weapon="SYRINGE"
	elseif (meansOfDeath==29) then
		weapon="AMMO"
	elseif (meansOfDeath==30) then
		weapon="ARTY"
	elseif (meansOfDeath==31) then
		weapon="WATER"
	elseif (meansOfDeath==32) then
		weapon="SLIME"
	elseif (meansOfDeath==33) then
		weapon="LAVA"
	elseif (meansOfDeath==34) then
		weapon="CRUSH"
	elseif (meansOfDeath==35) then
		weapon="TELEFRAG"
	elseif (meansOfDeath==36) then
		weapon="FALLING"
	elseif (meansOfDeath==37) then
		weapon = "SUICIDE"
	elseif (meansOfDeath==38) then
		weapon="TARGET_LASER"
	elseif (meansOfDeath==39) then
		weapon="TRIGGER_HURT"
	elseif (meansOfDeath==40) then
		weapon="EXPLOSIVE"
	elseif (meansOfDeath==41) then
		weapon="CARBINE"
	elseif (meansOfDeath==42) then
		weapon="KAR98"
	elseif (meansOfDeath==43) then
		weapon="GPG40"
	elseif (meansOfDeath==44) then
		weapon="M7"
	elseif (meansOfDeath==45) then
		weapon="LANDMINE"
	elseif (meansOfDeath==46) then
		weapon="SATCHEL"
	elseif (meansOfDeath==47) then
		weapon="TRIPMINE"
	elseif (meansOfDeath==48) then
		weapon="SMOKEBOMB"
	elseif (meansOfDeath==49) then
		weapon="MOBILE_MG42"
	elseif (meansOfDeath==50) then
		weapon="SILENCED_COLT"
	elseif (meansOfDeath==51) then
		weapon="GARAND_SCOPE"
	elseif (meansOfDeath==52) then
		weapon="CRUSH_CONSTRUCTION"
	elseif (meansOfDeath==53) then
		weapon="CRUSH_CONSTRUCTIONDEATH"
	elseif (meansOfDeath==54) then
		weapon="CRUSH_CONSTRUCTIONDEATH_NOATTACKER"
	elseif (meansOfDeath==55) then
		weapon="K43"
	elseif (meansOfDeath==56) then
		weapon="K43_SCOPE"
	elseif (meansOfDeath==57) then
		weapon="MORTAR"
	elseif (meansOfDeath==58) then
		weapon="AKIMBO_COLT"
	elseif (meansOfDeath==59) then
		weapon="AKIMBO_LUGER"
	elseif (meansOfDeath==60) then
		weapon="AKIMBO_SILENCEDCOLT"
	elseif (meansOfDeath==61) then
		weapon="AKIMBO_SILENCEDLUGER"
	elseif (meansOfDeath==62) then
		weapon="SMOKEGRENADE"
	elseif (meansOfDeath==63) then
		weapon="SWAP_SPACES"
	elseif (meansOfDeath==64) then
		weapon="SWITCH_TEAM"
	end

	local kil = tonumber(killer)
	local killername = ""

	if killer == 1022 then
		killername="The World"
	else
		killername=et.Info_ValueForKey( et.trap_GetUserinfo( killer ), "name" )
	end
	playerwhokilled[victim] = killer
	killedwithweapv[victim] = tostring(weapon)

	local victimteam = tonumber(et.gentity_get(victim, "sess.sessionTeam")) 
	local killerteam = tonumber(et.gentity_get(killer, "sess.sessionTeam"))
	local killedname=et.Info_ValueForKey( et.trap_GetUserinfo( victim ), "name" )

		if k_spreerecord == 1 then
			if killr[victim] > spreerecordkills then
				spreerecord(victim, killr[victim])
				--killr[victim] = 0
			else
				--killr[victim] = 0
			end
			if killr[victim] > mapspreerecordkills then
				mapspreerecord(victim, killr[victim])
				killr[victim] = 0
			else
				killr[victim] = 0
			end
		end



if k_deathsprees == 1 then
	deathspree[victim]=deathspree[victim]+1
end
		if k_sprees == 1 then
			if (killingspree[victim]>=5) then
				local str = string.gsub(k_end_message1, "#victim#", killedname)
				local str = string.gsub(str, "#kills#", killingspree[victim])
				local str = string.gsub(str, "#killer#", killername)

				et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ks_location.." "..str.."\n" )
				killingspree[victim]=0
			else
				killingspree[victim]=0
			end
		end

	flakmonkey[victim]=0
--	multikill[victim]=0

if k_deathsprees == 1 then
	if (deathspree[victim]==k_deathspree1_amount) then
		local str = string.gsub(k_ds_message1, "#victim#", killedname)
		local str = string.gsub(str, "#deaths#", deathspree[victim])

		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ds_location.." "..str.."\n" )
		if k_deathspreesounds == 1 then
			if k_noisereduction == 1 then
				et.G_ClientSound(victim, deathspreesound1)
			else
				et.G_globalSound(deathspreesound1)
			end
		end
	elseif (deathspree[victim]==k_deathspree2_amount) then
		local str = string.gsub(k_ds_message2, "#victim#", killedname)
		local str = string.gsub(str, "#deaths#", deathspree[victim])

		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ds_location.." "..str.."\n" )
		if k_deathspreesounds == 1 then
			if k_noisereduction == 1 then
				et.G_ClientSound(victim, deathspreesound2)
			else
				et.G_globalSound(deathspreesound2)
			end
		end
	elseif (deathspree[victim]==k_deathspree3_amount) then
		local str = string.gsub(k_ds_message3, "#victim#", killedname)
		local str = string.gsub(str, "#deaths#", deathspree[victim])

		et.trap_SendConsoleCommand( et.EXEC_APPEND, ""..ds_location.." "..str.."\n" )
		if k_deathspreesounds == 1 then
			if k_noisereduction == 1 then
				et.G_ClientSound(victim, deathspreesound3)
			else
				et.G_globalSound(deathspreesound3)
			end
		end
	end
end

end

function et.G_ClientSound(clientnum, soundfile)
	local tempentity = et.G_TempEntity(et.gentity_get(clientnum, "r.currentOrigin"), EV_GLOBAL_CLIENT_SOUND)
	et.gentity_set(tempentity, "s.teamNum", clientnum)
	et.gentity_set(tempentity, "s.eventParm", et.G_SoundIndex(soundfile))
end

speaker = {}
--for pse = 0, 63, 1 do
--	speaker[pse] = et.G_Spawn()
--	et.G_AddEvent(pse, EV_GLOBAL_CLIENT_SOUND, 0)
--end

--for pse = 0, 63, 1 do
--	speaker[pse] = et.G_TempEntity(et.gentity_get(pse, "r.currentOrigin"), EV_GLOBAL_CLIENT_SOUND)
--end

function speaker_test(clientnum, soundfile)
	local tempentity
	
	if speaker[clientnum] == nil then
		tempentity = et.G_TempEntity(et.gentity_get(clientnum, "r.currentOrigin"), EV_GLOBAL_CLIENT_SOUND)
		speaker[clientnum] = tempentity
	else
		local origin3 = {}
		origin3[1] = -65536
		origin3[2] = -65536
		origin3[3] = -65536

		et.gentity_set(speaker[clientnum], "r.currentOrigin", origin3)

		tempentity = et.G_TempEntity(et.gentity_get(clientnum, "r.currentOrigin"), EV_GLOBAL_CLIENT_SOUND)
		speaker[clientnum] = tempentity
	end


	et.gentity_set(tempentity, "s.teamNum", clientnum)
	et.gentity_set(tempentity, "s.eventParm", et.G_SoundIndex(soundfile))

	local origin = et.gentity_get(tempentity, "origin")
	local origin2 = et.gentity_get(clientnum, "r.currentOrigin")
	et.trap_SendConsoleCommand( et.EXEC_APPEND, "qsay " .. soundfile .. " was played at speaker " .. origin[1] .. ", " .. origin[2] .. ", " .. origin[3] .. " and you are at " .. origin2[1] .. ", " .. origin2[2] .. ", " .. origin2[3] .. "\n" )
end