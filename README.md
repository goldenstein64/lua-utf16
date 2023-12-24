# UTF-16 Support for Lua

## Description

This library provides minimal support for the UTF-16 encoding. Operations that query the meaning of a character, such as character classification, are outside its scope. Since strings in Lua can have embedded zeros, the library uses the default Lua string type to hold the UTF-16 data. The encoding of a particular string is the user's responsibility.
Unless specifically mentioned, all functions expect a byte (and not UTF-16 code unit) position as an argument. They assume that the given position is either the start of a code point (a self-represented UTF-16 character or a high UTF-16 surrogate) or one plus the length (in bytes) of the subject string. Negative positions (in bytes) count from the end of the string.

### utf16.char(···)
Gets zero or more integers, converts each one to its corresponding UTF-16 code units and returns a string with the concatenation of all these code units.

### utf16.codes(s)
Returns values so that the construction `for p, c in utf8.codes(s) do body end` will iterate over all code points in string s, with p being the position (in bytes) and c the code point of each character. It raises an error if it meets any invalid UTF-16 sequence.

### utf16.codepoint(s [, i [, j]])
Returns the code points (as integers) from all UTF-16 characters in s that start between byte position i and j (both included). The default for i is 1 and for j is i. It raises an error if it meets any invalid UF-16 sequence.

### utf16.len(s [, i [, j]])
Returns the number of UTF-8 characters in string s that start between positions i and j (both inclusive). The default for i is 1 and for j is -1. If it finds any invalid byte sequence, returns a false value plus the position of the first invalid byte.

### utf16.offset (s, i [, pos])
Returns the position (in bytes) where the encoding of the `i`-th code point of `s` (counting from `pos`) starts. A negative `i` gets characters before `pos`. The default for `pos` is 1 when `i` is non-negative and `#s + 1` otherwise, so that `utf16.offset(s, -i)` gets the offset of the `i`-th character from the end of the string. If the specified character is neither in the UTF-16 string nor right after its end, the function returns `nil`. As a special case, when `i` is 0 the function returns the start of the encoding of the character that contains the `i`-th byte of `s`. This function assumes that `s` is a valid UTF-16 string.

## Installation

### On Microsoft Windows with Microsoft Visual Studio

Open a Visual Studio Command Prompt (choosing the same platform as the one used when compiling Lua) in the source directory and execute `nmake`. The variables `LUA_INCLUDE`, `LUA_LIBNAME` and `LUA_LIBPATH` can be set either in `Makefile` or at the command line, for example with `nmake LUA_INCLUDE=C:\Lua54\include LUA_LIBNAME=lua54.lib LUA_LIBPATH=C:\Lua54\lib`. Lua must have been compiled as DLL (to allow using modules). The resulting `utf16.dll` must be manually moved to the target directory.

### On Microsoft Windows with MinGW

At the command prompt, in the source directory, execute `make -f GNUmakefile mingw`. The variables LUA_CFLAGS and LUA_LIBS can be set either in GNUmakefile or at the command line, for example with `make -f GNUmakefile LUA_CFLAGS=-I/opt/local/include LUA_LIBS=-L/opt/local/lib -llua54`. The resulting `utf16.dll` must be manually moved to the target directory.

### On macOS

At the command prompt, in the source directory, execute `make -f GNUmakefile macos`. The variable LUA_CFLAGS can be set either in GNUmakefile or at the command line, for example with `make -f GNUmakefile LUA_CFLAGS=-I/opt/local/include`. LUA_LIBS is ignored (this configuration uses -undefined dynamic_lookup linker option). The resulting `utf16.so` must be manually moved to the target directory.

### On Linux and other Unix platforms

At the command prompt, in the source directory, execute `make -f GNUmakefile`. The variables LUA_CFLAGS and LUA_LIBS can be set either in GNUmakefile or at the command line, for example with `make -f GNUmakefile LUA_CFLAGS=-I/opt/local/include LUA_LIBS=-L/opt/local/lib -llua54`. The resulting `utf16.so` must be manually moved to the target directory.

### With LuaRocks

At the command prompt, in the source directory, execute `luarocks make ./lua-utf16-dev-1.rockspec`. A `utf16.{dll,so}` should be compiled in the nearest LuaRocks tree.

This works with MinGW on Windows and Debian WSL2.
