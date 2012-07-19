add_definitions("-DLUA_USE_LINUX")

add_library(lua lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c 
            	lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c  
            	lundump.c lvm.c lzio.c
                lauxlib.c lbaselib.c ldblib.c liolib.c lmathlib.c loslib.c ltablib.c 
            	lstrlib.c loadlib.c linit.c luasql.c ls_sqlite3.c sqlite3.c)
