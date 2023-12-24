package = "utf16"
version = "dev-1"
source = {
   url = "git+https://github.com/vtudorache/lua-utf16.git"
}
description = {
   summary = "Minimal support for the UTF-16 encoding for Lua.",
   detailed = [[
      This library provides minimal support for the UTF-16 encoding. Operations
      that query the meaning of a character, such as character classification,
      are outside its scope. Since strings in Lua can have embedded zeros, the
      library uses the default Lua string type to hold the UTF-16 data. The
      encoding of a particular string is the user's responsibility.

      Unless specifically mentioned, all functions expect a byte (and not
      UTF-16 code unit) position as an argument. They assume that the given
      position is either the start of a code point (a self-represented UTF-16
      character or a high UTF-16 surrogate) or one plus the length (in bytes)
      of the subject string. Negative positions (in bytes) count from the end
      of the string.
   ]],
   homepage = "https://github.com/vtudorache/lua-utf16",
   license = "MIT"
}
dependencies = {
   "lua ~> 5.1"
}
build = {
   type = "builtin",
   modules = {
      utf16 = {
         sources = "utf16.c"
      }
   }
}
