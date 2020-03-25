-- RoadKillPuppy's PlaySound / etpro lua module
-- ex. 'rcon rconpass playsound -1 sound/etpro/hit.wav'
--     'rcon rconpass playsound_env 10 sound/etpro/hit.wav'
--
-- copypasted 99% of the code from ReyalP's wolfwiki page
-- thx r0f`deej for testing...  May your sprees be remembered!
--
-- 0.1 rkp
-- 0.2 rkp: dirty implementation of -1 parameter used by etadminmod
-- 0.3 rkp: workaround for etpro beta racecondition
-- 0.4 rkp: cleaned-up version for etpro 3.2.3
-- 0.5 rkp: added playsound_env command

-- global varlist
PlaySoundVersion = "0.5"

-- called when game inits
function et_InitGame( levelTime, randomSeed, restart )
        et.RegisterModname( "rkp_playsound-" .. PlaySoundVersion .. " " .. et.FindSelf() )
        et.G_Print( "RoadKillPuppy's PlaySound version " ..  PlaySoundVersion .. " activated...\n" )
end

-- some help
function PlaySoundHelp()
        et.G_Print("playsound -1 plays a sound that everybody on the server can hear\n");
        et.G_Print("usage: playsound -1 path_to_sound.wav\n");
end

function PlaySoundEnvHelp()
        et.G_Print("playsound_env plays a sound that you can hear in the proximity of the player with slot -playerslot-\n");
        et.G_Print("usage: playsound_env playerslot path_to_sound.wav\n");
end

-- react on new console command
function et_ConsoleCommand()
        if et.trap_Argv(0) == "playsound" then
                if et.trap_Argc() ~= 3 then
                        PlaySoundHelp()
                else
                        et.G_globalSound(et.trap_Argv(2));
                end
                return 1
        end

        if et.trap_Argv(0) == "playsound_env" then
                if et.trap_Argc() ~= 3 then
                        PlaySoundEnvHelp()
                else
                        soundindex = et.G_SoundIndex( et.trap_Argv(2) )
                        et.G_Sound( et.trap_Argv(1) , soundindex )
                end
                return 1
        end

        return 0
end
