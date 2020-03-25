# Mac NQ Build with LUA/LUASQL
# I'm not a fully-qualified Mac dev, just somewhat of a wanderer ! :o
############# UPDATED 08/2009 #################
NOW NO NEED TO COMPILE EVERYTIME !!!
Just place liblua5.1.so in /Library/Frameworks   where  / is your root directory of HD ! :)



####################################### @@@
####################################### @@@@
#############  @@@   @     @@	############# 
############# @   @  @     @ @	############# 
#############  @@@   @@@@  @@	############# 
####################################### 
#### 	LUA BUID	########
For now to use Lua on Mac, you've to re-compile and link dylib against your NoQuarter  User folder.

For instance, look with otool in a bash shell, about my liblua5.1.so :
$~ otool -L lua/Mac/liblua5.1.so
lua/Mac/liblua5.1.so:
	/Users/djfou/Library/Application Support/Wolfenstein ET/noquarter/liblua5.1.so (compatibility version 0.0.0, current version 0.0.0)
	/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 111.1.4)
	/usr/lib/libgcc_s.1.dylib (compatibility version 1.0.0, current version 1.0.0)

In the lua_mac.zip provided in the lua/Mac folder, you'll found a modified Makefile to compile/link liblua5.1.so to your NQ directory. Just remplace the old Makefile from your luasource/src/Makefile with this one.
/!\ The Makefile in src/ directory, not above.

Compile with :  make macosx
		make liblua

The first will output the necesseray liblua5.1.a and lua binaries.
The 2nd will create liblua5.1.so, linked to your NQ Dir.

To build NQ, you'll need scons (>0.97   you can find it with MacPorts).
Place the resulting liblua5.1.so you've just made in NQ Source folder  /src/lua/Mac/liblua5.1.so then edit SConscript.game libluapath, and finally build.

## LUA SQL	##
LuaSQL should be OK (mysql.so)
The library link is /usr/local/mysql/lib/libmysqlclient.16.dylib
You may need to install MySQL first... (Pre-packaged exists on mysql site)


## Final ##
All procedures used for linux about Lua, serve for Mac too. This means that :
- Copy your src/lua/Mac/liblua5.1.so in fs_gamepath ( /Users/USERNAME/Library/Applications Support/Wolfenstein ET/noquarter/ ) where USERNAME is your username (ah obvious eh!)
- Copy src/luasql/Mac/mysql.so  in fs_gamepath too, but this time create a directory luasql… like (blabla/Wolfenstein ET/noquarter/luasql/mysql.so)


##
Well, it *should* run fine. Of course, you're graciously invited to participate & improve this tricky tweaked Mac build... :)

enjoy ! 
##
Links : 
http://www.lua.org/ftp/lua-5.1.4.tar.gz				Lua Sources (5.1.4)
http://www.keplerproject.org/luasql/				Lua SQL

