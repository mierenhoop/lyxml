#include <lua.h>
#include <lauxlib.h>

#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#include "yxml.h"

struct Decoder {
  yxml_t x;
  const char *p;
  const char *end;
  bool trimcontent; // trim whitespace around content
  bool acceptpartial; // for use in streaming xml, TODO: have streaming decoder function?
  const char *tagfield;
};

// Returns last yxml status
static int ParseElement(lua_State *L, struct Decoder *d) {
  struct luaL_Buffer b;
  int n = 0;
  int c = 0;
  int nspace;
  bool incontent = false;
#define FinishContent do { \
      if (incontent) { \
        int l = luaL_bufflen(&b); \
        incontent = false; \
        if (d->trimcontent && nspace) \
          luaL_buffsub(&b, nspace); \
        luaL_pushresult(&b); \
        if (l) \
          lua_rawseti(L, -2, ++n); \
        else \
          lua_pop(L, 1); \
      } \
    } while (0)
  lua_pushstring(L, d->x.elem);
  if (d->tagfield)
    lua_setfield(L, -2, d->tagfield);
  else
    lua_rawseti(L, -2, 0);
  for (;d->p < d->end; d->p++) {
    switch ((c = yxml_parse(&d->x, *d->p))) {
    case YXML_OK:
      break;
    case YXML_ELEMSTART:
      FinishContent;
      lua_newtable(L);
      lua_pushvalue(L, -1);
      lua_rawseti(L, -3, ++n);
      if ((c = ParseElement(L, d)) < 0)
        goto end;
      break;
    case YXML_ELEMEND:
      FinishContent;
      goto end;
    case YXML_ATTRSTART:
      luaL_buffinit(L, &b);
      break;
    case YXML_ATTRVAL:
      luaL_addstring(&b, d->x.data);
      break;
    case YXML_ATTREND:
      luaL_pushresult(&b);
      lua_setfield(L, -2, d->x.attr);
      break;
    case YXML_CONTENT:
      if (!incontent) { // skip the first >
        incontent = true;
        luaL_buffinit(L, &b);
        nspace = 0;
        break;
      }
      if (isspace(d->x.data[0]))
        nspace++;
      else
        nspace = 0;
      luaL_addstring(&b, d->x.data);
      break;
    case YXML_PISTART:
    case YXML_PICONTENT:
    case YXML_PIEND:
      break;
    default:
      goto end;
    }
  }
end:
  FinishContent;
  lua_pop(L, 1);
  return c;
}

static int PushError(lua_State *L, int err) {
  const char *s = "unknown error";
  switch (err) {
  case YXML_EEOF:
    s = "unexpected end of document";
    break;
  case YXML_EREF:
    s = "invalid character";
    break;
  case YXML_ECLOSE:
    s = "wrong close tag";
    break;
  case YXML_EMEM:
    s = "out of memory";
    break;
  case YXML_ESYN:
    s = "syntax error";
    break;
  }
  lua_pushnil(L);
  lua_pushstring(L, s);
  return 2;
}

static int DecodeXml(lua_State *L) {
  struct Decoder d = {
    .trimcontent = true,
  };
  int c;
  char *s;
  d.p = luaL_checkstring(L, 1);
  d.end = d.p + luaL_len(L, 1);
  if (lua_istable(L, 2)) {
    lua_settop(L, 2);
    lua_getfield(L, 2, "tagfield");
    if (!lua_isnoneornil(L, -1))
      d.tagfield = luaL_checkstring(L, -1);
    lua_getfield(L, 2, "trimcontent");
    if (!lua_isnoneornil(L, -1))
      d.trimcontent = lua_toboolean(L, -1);
  }
  lua_settop(L, 1);
  if (!(s = malloc(1024)))
    return PushError(L, YXML_EMEM);
  yxml_init(&d.x, s, 1024);
  while (d.p < d.end && (c = yxml_parse(&d.x, *d.p++)) == YXML_OK) {}
  if (c != YXML_ELEMSTART) {
    free(s);
    return PushError(L, YXML_EEOF);
  }
  lua_newtable(L);
  lua_pushvalue(L, -1);
  if ((c = ParseElement(L, &d)) < 0) {
    lua_pop(L, 1);
    free(s);
    return PushError(L, c);
  }
  free(s);
  return 1;
}

static const luaL_Reg lyxmlfuncs[] = {
    {"decode", DecodeXml},
    {0},
};

int luaopen_lyxml(lua_State *L) {
  luaL_newlib(L, lyxmlfuncs);
  return 1;
}

