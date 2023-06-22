microcompute_lua: microcompute_lua.c microcompute.h
	gcc -shared -fPIC $@.c -o $@.so