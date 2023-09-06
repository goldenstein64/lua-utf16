LUA_INCLUDE= 
LUA_LIBNAME= 
LUA_LIBPATH= 

RM= del /F

# -------------------------- End of user settings --------------------------#

CFLAGS= /DLUA_BUILD_AS_DLL /I"$(LUA_INCLUDE)" /MD /O2 /TC /W4

T= utf16

T_OUT= $(T).dll

T_OBJ= $(T).obj

$(T): $(T_OUT)

$(T_OUT): $(T_OBJ)
    $(CC) /Fe:$(T_OUT) $(T_OBJ) /link /DLL /NOEXP /NOIMPLIB \
        /LIBPATH:"$(LUA_LIBPATH)" $(LUA_LIBNAME)

clean:
    $(RM) $(T_OUT) $(T_OBJ)

.c.obj:
  $(CC) /c $(CFLAGS) $*.c
