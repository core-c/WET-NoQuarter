onlyonce = 0
cyclecounter = 0

function et_RunFrame( levelTime )
       
	if onlyonce == 100 then
		for i = 64, 1021 do

			if cyclecounter > i then
				return
			end

			-- Test for G_SetSpawnVal
			local message = et.G_GetSpawnVar( i, "message" )
			
			if message == "Axis Command Post" then
				
				local popup = et.G_GetSpawnVar( i, "popup" )
				local book = et.G_GetSpawnVar( i, "book" )

				et.G_Print( "BEFORE THE CHANGES\n")

				et.G_Print( "message = "..message.."\n")

				if popup == nil then
					popup = "nil"
				end
				et.G_Print( "popup = "..popup.."\n")

				if book == nil then
					book = "nil"
				end
				et.G_Print( "book = "..book.."\n")

				et.G_SetSpawnVar( i, "message", "IlDuca's Radio Box" )
				et.G_SetSpawnVar( i, "popup", "IlDuca's Radio Box" )
				et.G_SetSpawnVar( i, "book", "IlDuca's Radio Box" )
			
				-- Check if the values changed
				message = et.G_GetSpawnVar( i, "message" )
				popup = et.G_GetSpawnVar( i, "popup" )
				book = et.G_GetSpawnVar( i, "book" )

				et.G_Print( "AFTER THE CHANGES\n")

				if message == nil then
					message = "nil"
				end
				et.G_Print( "message = "..message.."\n")

				if popup == nil then
					popup = "nil"
				end
				et.G_Print( "popup = "..popup.."\n")

				if book == nil then
					book = "nil"
				end
				et.G_Print( "book = "..book.."\n")
			end


			cyclecounter = i		

		end
	end
	onlyonce = onlyonce + 1
end