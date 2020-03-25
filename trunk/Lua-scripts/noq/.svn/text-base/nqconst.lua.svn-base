-- IRATA [*] - based on etconst.lua from ETPro but slightly modified to fit for NQ

-- misc q_shared.h 
et.MAX_CLIENTS 						= 64
et.MAX_MODELS 						= 256
et.MAX_SOUNDS 						= 256
et.MAX_CS_SKINS 					= 64
et.MAX_CSSTRINGS 					= 32
et.MAX_CS_SHADERS 					= 32
et.MAX_SERVER_TAGS 					= 256
et.MAX_TAG_FILES 					= 64
et.MAX_MULTI_SPAWNTARGETS 			= 16
et.MAX_DLIGHT_CONFIGSTRINGS 		= 16
et.MAX_SPLINE_CONFIGSTRINGS 		= 8
-- misc bg_public.h
et.MAX_OID_TRIGGERS 				= 18
et.MAX_CHARACTERS 					= 16
et.MAX_TAGCONNECTS 					= 64
et.MAX_FIRETEAMS 					= 12
et.MAX_MOTDLINES 					= 6

-- Config string:
-- q_shared.h
et.CS_SERVERINFO 					= 0  -- an info string with all the serverinfo cvars
et.CS_SYSTEMINFO 					= 1  -- an info string for server system to client system configuration (timescale, etc)
-- bg_public.h
et.CS_MUSIC 						= 2
et.CS_WARMUP 						= 5  -- server time when the match will be restarted
et.CS_VOTE_TIME 					= 6
et.CS_VOTE_STRING 					= 7
et.CS_VOTE_YES 						= 8
et.CS_VOTE_NO 						= 9
et.CS_GAME_VERSION 					= 10
et.CS_LEVEL_START_TIME 				= 11  -- so the timer only shows the current level
et.CS_INTERMISSION 					= 12  -- when 1, intermission will start in a second or two
et.CS_MULTI_INFO 					= 13
et.CS_MULTI_MAPWINNER 				= 14
et.CS_MULTI_OBJECTIVE 				= 15

et.CS_SCREENFADE 					= 17  -- Ridah, used to tell clients to fade their screen to black/normal
et.CS_FOGVARS 						= 18  -- (SA) used for saving the current state/settings of the fog
et.CS_SKYBOXORG 					= 19  -- this is where we should view the skybox from
et.CS_TARGETEFFECT 					= 20 
et.CS_WOLFINFO 						= 21
et.CS_FIRSTBLOOD 					= 22  -- Team that has first blood
et.CS_ROUNDSCORES1  				= 23  -- Axis round wins
et.CS_ROUNDSCORES2  				= 24  -- Allied round wins
et.CS_MUSIC_QUEUE 					= 25  
et.CS_SCRIPT_MOVER_NAMES 			= 26
et.CS_CONSTRUCTION_NAMES 			= 27
et.CS_REINFSEEDS 					= 28  -- Reinforcement seeds
et.CS_SERVERTOGGLES 				= 29  -- Shows current enable/disabled settings (for voting UI)
et.CS_GLOBALFOGVARS 				= 30
et.CS_AXIS_MAPS_XP 					= 31
et.CS_ALLIED_MAPS_XP 				= 32
et.CS_INTERMISSION_START_TIME 		= 33
et.CS_ENDGAME_STATS 				= 34
et.CS_CHARGETIMES 					= 35
et.CS_FILTERCAMS 					= 36
et.CS_NOQUARTERINFO 				= 37
et.CS_SKILLLEVELS 					= 38
et.CS_FORCECVAR 					= 39
et.CS_SVCVAR 						= 40
et.CS_CONFIGNAME 					= 41
et.CS_CSMETHODINFO 					= 42

et.CS_MODELS 						=      64
et.CS_SOUNDS 						= ( et.CS_MODELS + et.MAX_MODELS     )
et.CS_SHADERS 						= ( et.CS_SOUNDS + et.MAX_SOUNDS     )
et.CS_SHADERSTATE 					= ( et.CS_SHADERS + et.MAX_CS_SHADERS    )
et.CS_SKINS 						= ( et.CS_SHADERSTATE +   1       )
et.CS_CHARACTERS 					= ( et.CS_SKINS + et.MAX_CS_SKINS    )
et.CS_PLAYERS 						= ( et.CS_CHARACTERS + et.MAX_CHARACTERS    )
et.CS_MULTI_SPAWNTARGETS 			= ( et.CS_PLAYERS + et.MAX_CLIENTS     )
et.CS_OID_TRIGGERS 					= ( et.CS_MULTI_SPAWNTARGETS + et.MAX_MULTI_SPAWNTARGETS  )
et.CS_OID_DATA 						= ( et.CS_OID_TRIGGERS + et.MAX_OID_TRIGGERS   )
et.CS_DLIGHTS 						= ( et.CS_OID_DATA + et.MAX_OID_TRIGGERS   )
et.CS_SPLINES 						= ( et.CS_DLIGHTS + et.MAX_DLIGHT_CONFIGSTRINGS )
et.CS_TAGCONNECTS 					= ( et.CS_SPLINES + et.MAX_SPLINE_CONFIGSTRINGS )
et.CS_FIRETEAMS 					= ( et.CS_TAGCONNECTS + et.MAX_TAGCONNECTS    )
et.CS_CUSTMOTD 						= ( et.CS_FIRETEAMS + et.MAX_FIRETEAMS    )
et.CS_STRINGS 						= ( et.CS_CUSTMOTD + et.MAX_MOTDLINES    )
et.CS_MAX 							= ( et.CS_STRINGS + et.MAX_CSSTRINGS    )

return 1
