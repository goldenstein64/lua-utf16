#define LUA_LIB

#if defined(_WIN32)

#include <windows.h>

#else

#include <limits.h>

#if (SHRT_MAX == 0x7fff)

typedef unsigned short UINT16;

#elif (INT_MAX == 0x7fff)

typedef unsigned int UINT16;

#else

#error no 16-bit integer type available

#endif

#if (INT_MAX == 0x7fffffff)

typedef int INT32;

#elif (LONG_MAX == 0x7fffffff)

typedef long INT32;

#else

#error no 32-bit integer type available

#endif

#endif

#include <lauxlib.h>

#define SUR1 0xd800
#define SUR2 0xdc00
#define SUR3 0xe000

#define END1 0x10000
#define MAX2 0x10ffff

#define SUR1TO2(c) (SUR1 <= (c) && (c) < SUR2)
#define SUR2TO3(c) (SUR2 <= (c) && (c) < SUR3)
#define SUR1TO3(c) (SUR1 <= (c) && (c) < SUR3)

static const UINT16 *utf16_decode(const UINT16 *p, INT32 *r)
{
    INT32 u1 = *p;
    if (SUR1TO2(u1)) {
        INT32 u2 = *++p;
        if (!SUR2TO3(u2)) return NULL;
        u1 = END1 + ((0x3ff & u1) << 10) + (0x3ff & u2);
    }
    else if (SUR2TO3(u1)) return NULL;
    if (r != NULL) *r = u1;
    return ++p;
}

static size_t utf16_encode(UINT16 *p, INT32 c)
{
    if (c < 0 || SUR1TO3(c) || c > MAX2) return 0;
    if (c >= END1) {
        if (p != NULL) {
            c -= END1;
            p[0] = SUR1 + (UINT16)(0x3ff & (c >> 10));
            p[1] = SUR2 + (UINT16)(0x3ff & c);
        }
        return 2;
    }
    if (p != NULL) p[0] = (UINT16)c;
    return 1;
}

/*
** Converts the given pos in bytes [-len, -1] or [1, len] to an offset as in
** [0, len-1].
*/
static lua_Integer offset(lua_Integer pos, size_t len)
{
    return pos > 0 ? pos - 1 : pos + (lua_Integer)len;
}

/*
** utf16.len(s, [, i [, j]]) -> number of code points that start in the range
** [i, j] (byte positions) or nil and the position of the first byte of the
** first invalid code point. j defaults to -1.
*/
static int utf16_len(lua_State *L)
{
    lua_Integer i, j, n = 0;
    size_t size;
    const UINT16 *s = (const UINT16 *)luaL_checklstring(L, 1, &size);
    i = offset(luaL_optinteger(L, 2, 1), size);
    luaL_argcheck(L, 0 <= i && i < (lua_Integer)size, 2, 
        "initial position out of range");
    j = offset(luaL_optinteger(L, 3, -1), size);
    luaL_argcheck(L, 0 <= j && j < (lua_Integer)size, 3, 
        "final position out of range");
    if (i % sizeof(UINT16)) return luaL_error(L, 
        "initial position not aligned to UTF-16 unit");
    /* Convert the starting byte offset to the UTF-16 unit offset */
    i /= sizeof(UINT16); 
    /* Convert the ending byte offset to the UTF-16 unit offset */
    j /= sizeof(UINT16);
    while (i <= j) {
        const UINT16 *t = utf16_decode(s + i, NULL);
        if (t == NULL) {
            lua_pushnil(L);
            lua_pushinteger(L, i * sizeof(UINT16) + 1);
            return 2;
        }
        i = t - s;
        n++;
    }
    lua_pushinteger(L, n);
    return 1;
}

/*
** utf16.codepoint(s, [, i [, j]]) -> values of the code points that start in
** the range [i, j] (byte positions). i defaults to 1 and j defaults to i.
*/
static int utf16_codepoint(lua_State *L)
{
    lua_Integer i, j;
    int n = 0;
    size_t size;
    const UINT16 *s = (const UINT16 *)luaL_checklstring(L, 1, &size);
    i = offset(luaL_optinteger(L, 2, 1), size);
    luaL_argcheck(L, 0 <= i && i < (lua_Integer)size, 2, 
        "initial position out of range");
    /* Use i + 1 (to get byte position, not offset) as default for arg #2. */
    j = offset(luaL_optinteger(L, 3, i + 1), size);
    luaL_argcheck(L, 0 <= j && j < (lua_Integer)size, 3, 
        "final position out of range");
    if (i % sizeof(UINT16)) return luaL_error(L, 
        "initial position not aligned to UTF-16 unit");
    /* Convert the starting byte offset to the UTF-16 unit offset */
    i /= sizeof(UINT16); 
    /* Convert the ending byte offset to the UTF-16 unit offset */
    j /= sizeof(UINT16);
    if (i <= j) {
        /* Check for the worst case, when each code unit is a code point. */
        luaL_checkstack(L, (int)(j - i), "string slice too long");
        while (i <= j) {
            INT32 c;
            const UINT16 *t = utf16_decode(&s[i], &c);
            if (t == NULL) return luaL_error(L, "invalid UTF-16 sequence");
            lua_pushinteger(L, c);
            n++;
            i = t - s;
        }
    }
    return n;
}

static void utf16_pushchar(lua_State *L, int index)
{
    UINT16 buf[2];
    size_t n;
    INT32 c = (INT32)luaL_checkinteger(L, index);
    n = utf16_encode(&buf[0], c);
    luaL_argcheck(L, n > 0, index, "invalid code point");
    lua_pushlstring(L, (const char *)&buf[0], n * sizeof(UINT16));
}

/*
** utf16.char(u1 [, u2 [, ...]]) -> char(u1)..char(u2)..[...]
*/
static int utf16_char(lua_State *L)
{
    int n = lua_gettop(L);
    if (n == 1)
        utf16_pushchar(L, 1);
    else {
        luaL_Buffer b;
        int i = 0;
        luaL_buffinit(L, &b);
        while (++i <= n) {
            utf16_pushchar(L, i);
            luaL_addvalue(&b);
        }
        luaL_pushresult(&b);
    }
    return 1;
}

static int utf16_iter(lua_State *L)
{
    lua_Integer i, n;
    size_t size;
    const UINT16 *s = (const UINT16 *)luaL_checklstring(L, 1, &size);
    /* n gets the number of UINT16 units. */
    n = (lua_Integer)(size / sizeof(UINT16));
    /* i gets the offset in bytes. */
    i = lua_tointeger(L, 2) - 1;
    /* On the first iteration, i is -1, adjust it to 0. */
    if (i < 0) i = 0;
    else {
        /* Translate i to a number of UINT16 units. */
        i /= sizeof(UINT16);
        /* Skip the current code point: skip the current UINT16 unit. */
        i++;
        /* Then skip the next UINT16 unit if it's a second surrogate. */
        if (SUR2TO3(s[i])) i++;
    }
    if (i < n) {
        INT32 c;
        const UINT16 *t = utf16_decode(&s[i], &c);
        if (t == NULL) return luaL_error(L, "invalid UTF-16 sequence");
        /* Push the current offset as position in bytes starting at 1. */
        lua_pushinteger(L, i * sizeof(UINT16) + 1);
        lua_pushinteger(L, c);
        return 2;
    }
    return 0;
}

static int utf16_codes(lua_State*L)
{
    luaL_checkstring(L, 1);
    lua_pushcfunction(L, utf16_iter);
    lua_pushvalue(L, 1);
    lua_pushinteger(L, 0);
    return 3;
}

/*
** utf16.offset(s, i, [pos]) -> position (as bytes) where the i-th code point 
** counting from pos begins; an i of 0 means the code point at pos. If the 
** specified character is neither in the subject nor right after its end, the
** function returns nil.
*/
static int utf16_offset(lua_State *L)
{
    lua_Integer i, pos, n;
    size_t size;
    const UINT16 *s = (const UINT16 *)luaL_checklstring(L, 1, &size);
    i = luaL_checkinteger(L, 2);
    pos = (i >= 0) ? 1 : (lua_Integer)(size + 1);
    pos = offset(luaL_optinteger(L, 3, pos), size);
    /* pos should be [0, size] */
    luaL_argcheck(L, 0 <= pos && pos <= (lua_Integer)size, 3, 
        "position out of range");
    if (pos % sizeof(UINT16)) return luaL_error(L, 
        "position not aligned to UTF-16 unit");
    /* n gets the number of UINT16 units. */
    n = (lua_Integer)(size / sizeof(UINT16));
    /* Translate pos to a number of UINT16 units. */
    pos /= sizeof(UINT16);
    /* pos is in [0, n] so check bounds before access. */
    if (i == 0) {
        if (0 < pos && pos < n && SUR2TO3(s[pos])) pos--;
    }
    else {
        if (pos < n && SUR2TO3(s[pos])) return luaL_error(L,
            "position starting a low UTF-16 surrogate");
        if (i > 0) {
            while (pos < n) {
                if (!SUR2TO3(s[pos])) i--; 
                if (i == 0) break;
                pos++;
            }
        }
        else {
            while (pos > 0) {
                pos--;
                if (!SUR2TO3(s[pos])) i++; 
                if (i == 0) break;
            }
        }
    }
    /* 
    ** When a code point position is found, i is 0 (no more search). Return
    ** pos converted to a byte position starting with 1, or nil.
    */
    if (i == 0) lua_pushinteger(L, pos * sizeof(UINT16) + 1);
    else lua_pushnil(L);
    return 1;
}

LUAMOD_API int luaopen_utf16(lua_State *L)
{
    lua_newtable(L);
    lua_pushcfunction(L, utf16_char);
    lua_setfield(L, -2, "char");
    lua_pushcfunction(L, utf16_codepoint);
    lua_setfield(L, -2, "codepoint");
    lua_pushcfunction(L, utf16_codes);
    lua_setfield(L, -2, "codes");
    lua_pushcfunction(L, utf16_len);
    lua_setfield(L, -2, "len");
    lua_pushcfunction(L, utf16_offset);
    lua_setfield(L, -2, "offset");
    return 1;
}

