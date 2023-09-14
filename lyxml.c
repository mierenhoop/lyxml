#include <yxml.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>

#define DEFSTACKSZ 4096

int parse(lua_State *L) {
    yxml_t* x = luaL_checkudata(L, 1, "yxml");
    int c = luaL_checkinteger(L, 2);
    int nret = 1;

    int ret = yxml_parse(x, c);
    lua_pushinteger(L, ret);

    switch (ret) {
    case YXML_ELEMSTART:
        /* fallthrough */
    case YXML_ELEMEND:
        lua_pushstring(L, x->elem);
        nret++;
        break;
    case YXML_ATTRSTART:
        lua_pushstring(L, x->attr);
        nret++;
        break;
    case YXML_CONTENT:
        /* fallthrough */
    case YXML_ATTRVAL:
        /* fallthrough */
    case YXML_PICONTENT:
        lua_pushstring(L, x->data);
        nret++;
        break;
    }

    return nret;
}

int eof(lua_State *L) {
    yxml_t *x = luaL_checkudata(L, 1, "yxml");

    lua_pushinteger(L, yxml_eof(x));

    return 1;
}

static const luaL_Reg yxmlfn[] = {
    {"parse", parse},
    {"eof", eof},
    {0},
};

int gc(lua_State *L) {
    yxml_t* x = luaL_checkudata(L, 1, "yxml");

    free(x->stack);

    return 0;
}

static const luaL_Reg metamethods[] = {
    {"__gc", gc},
    {0},
};

int init(lua_State *L) {
    yxml_t *x = lua_newuserdata(L, sizeof(yxml_t));
    luaL_setmetatable(L, "yxml");

    unsigned char *stack = malloc(DEFSTACKSZ);
    yxml_init(x, stack, DEFSTACKSZ);

    return 1;
}

static const luaL_Reg luayxml[] = {
    {"init", init},
    {0},
};

#define PUSHCONST(v) (lua_pushinteger(L, YXML_##v), lua_setfield(L, -2, #v))

int luaopen_lyxml(lua_State *L) {
    luaL_newlib(L, luayxml);

    luaL_newmetatable(L, "yxml");
    luaL_setfuncs(L, metamethods, 0);
    luaL_newlibtable(L, yxmlfn);
    luaL_setfuncs(L, yxmlfn, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    PUSHCONST(EEOF     );
    PUSHCONST(EREF     );
    PUSHCONST(ECLOSE   );
    PUSHCONST(EMEM     );
    PUSHCONST(ESYN     );
    PUSHCONST(OK       );
    PUSHCONST(ELEMSTART);
    PUSHCONST(CONTENT  );
    PUSHCONST(ELEMEND  );
    PUSHCONST(ATTRSTART);
    PUSHCONST(ATTRVAL  );
    PUSHCONST(ATTREND  );
    PUSHCONST(PISTART  );
    PUSHCONST(PICONTENT);
    PUSHCONST(PIEND    );

    return 1;
}
