LUA_CFLAGS= 
LUA_LIBS=

RM= rm -f

# -------------------------- End of user settings --------------------------#

CFLAGS= -O2 -pedantic -Wall $(LUA_CFLAGS)
LDFLAGS= -shared
LIBS= $(LUA_LIBS)

T= utf16

T_OUT= $(T).so

T_OBJ= $(T).o

$(T): $(T_OUT)

$(T_OUT): $(T_OBJ)
	$(CC) -o $@ $(LDFLAGS) $(T_OBJ) $(LIBS)

clean:
	$(RM) $(T_OUT) $(T_OBJ)

ios macos macosx:
	$(MAKE) $(T) LIBS="" LDFLAGS="$(LDFLAGS) -undefined dynamic_lookup"

mingw:
	$(MAKE) "CFLAGS=$(CFLAGS) -DLUA_BUILD_AS_DLL" "T_OUT=$(T).dll"
    
.c.o:
	$(CC) $(CFLAGS) -c $*.c

