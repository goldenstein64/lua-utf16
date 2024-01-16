---@meta

---This library provides minimal support for the UTF-16 encoding. Operations
---that query the meaning of a character, such as character classification, are
---outside its scope. Since strings in Lua can have embedded zeros, the library
---uses the default Lua string type to hold the UTF-16 data. The encoding of a
---particular string is the user's responsibility.
---
---Unless specifically mentioned, all functions expect a byte (and not UTF-16
---code unit) position as an argument. They assume that the given position is
---either the start of a code point (a self-represented UTF-16 character or a
---high UTF-16 surrogate) or one plus the length (in bytes) of the subject
---string. Negative positions (in bytes) count from the end of the string.
local utf16 = {}

---Gets zero or more integers, converts each one to its corresponding UTF-16
---code units and returns a string with the concatenation of all these code
---units.
---@param ... integer
---@return string
function utf16.char(...) end

---Returns values so that the construction
---`for p, c in utf8.codes(s) do body end` will iterate over all code points
---in string s, with p being the position (in bytes) and c the code point of
---each character. It raises an error if it meets any invalid UTF-16 sequence.
---@param s string
---@return fun(s: string, p: integer) iter, string state, integer control
function utf16.codes(s) end

---@param s string
---@param i? integer
---@param j? integer
---@return integer ...
function utf16.codepoint(s, i, j) end

---Returns the number of UTF-8 characters in string s that start between
---positions i and j (both inclusive). The default for i is 1 and for j is -1.
---If it finds any invalid byte sequence, returns a false value plus the
---position of the first invalid byte.
---@param s string
---@param i? integer
---@param j? integer
---@return integer len
function utf16.len(s, i, j) end

---Returns the position (in bytes) where the encoding of the `i`-th code point
---of `s` (counting from `pos`) starts. A negative `i` gets characters before
---`pos`. The default for `pos` is 1 when `i` is non-negative and `#s + 1`
---otherwise, so that `utf16.offset(s, -i)` gets the offset of the `i`-th
---character from the end of the string. If the specified character is neither
---in the UTF-16 string nor right after its end, the function returns `nil`. As
---a special case, when `i` is 0 the function returns the start of the encoding
---of the character that contains the `i`-th byte of `s`. This function assumes
---that `s` is a valid UTF-16 string.
---@param s string
---@param i integer
---@param pos? integer
---@return integer
function utf16.offset(s, i, pos) end

return utf16
