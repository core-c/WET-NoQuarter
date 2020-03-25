function et_ConsoleCommand()

	if et.trap_Argv(0) == "luaxpset" then

		if et.trap_Argc() ~= 5 then
			et.G_LogPrint( "Usage : luaxpset clientNum xp skill add\n" )
			return 1
		end

		local clientNum = et.trap_Argv(1)
		local xp = et.trap_Argv(2)
		local skill = et.trap_Argv(3)
		local add = et.trap_Argv(4)


		et.G_LogPrint( "command : luaxpset clientNum = "..clientNum.." xp = "..xp.." skill = "..skill.." add = "..add.."\n" )

		et.G_XP_Set ( clientNum , xp, skill, add )

		return 1
	end
	return 0
end
