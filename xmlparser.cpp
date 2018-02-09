#define LUA_LIB
#define LUA_BUILD_AS_DLL

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include <stdio.h>
#include "rapidxml.hpp"
#include "common.h"

using namespace rapidxml;

static void ParseNode(lua_State *L, xml_node<>* pNode) {
	lua_checkstack(L, 8);

	lua_pushstring(L, pNode->name());
	lua_rawget(L, -2);
	if (lua_type(L, -1) == LUA_TNIL) {
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, pNode->name());
	}
	if (lua_type(L, -1) != LUA_TTABLE) {
		luaL_error(L, "do not support *attr* and *child* has same name: %s", pNode->name());
	}

	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_rawseti(L, -3, lua_rawlen(L, -3) + 1);

	xml_attribute<> *pAttribute = pNode->first_attribute();
	while (pAttribute) {
		lua_pushstring(L, pAttribute->value());
		lua_setfield(L, -2, pAttribute->name());
		pAttribute = pAttribute->next_attribute();
	}

	xml_node<> *pChildNode = pNode->first_node();
	while (pChildNode) {
		ParseNode(L, pChildNode);
		pChildNode = pChildNode->next_sibling();
	}

	lua_pop(L, 2);
}

static void ParseData(lua_State *L, char *pFileData) {
	xml_document<> XmlDoc;

	lua_newtable(L);

	XmlDoc.parse<0>(pFileData);
	xml_node<> *pRootNode = XmlDoc.first_node();
	while (pRootNode) {
		ParseNode(L, pRootNode);
		pRootNode = pRootNode->next_sibling();
	}
}

static int ParseXmlFile(lua_State *L)
{
	int nRet = 0;
	FILE *pFile = NULL;
	int nFileSize = 0;
	char *pFileData = NULL;

	const char* szXmlFileName = luaL_checkstring(L,1);
	pFile = fopen(szXmlFileName, "r");
	PROCESS_ERROR(pFile);
	nFileSize = GetFileSize(pFile);
	pFileData = (char *)malloc(nFileSize + 1);
	int nReadSize = fread(pFileData, 1, nFileSize, pFile);
	pFileData[nReadSize] = '\0';
	ParseData(L, pFileData);

	nRet = 1;
Exit0:
	if (pFile)
		fclose(pFile);

	if (pFileData)
		free(pFileData);

	return nRet;
}

static int ParseXmlData(lua_State *L) {
	char *pFileData = (char *)luaL_checkstring(L, 1);
	ParseData(L, pFileData);

	return 1;
}

extern "C" LUAMOD_API int luaopen_lxmlparser(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{"parse_file", ParseXmlFile},
		{"parse_data", ParseXmlData},

		{NULL, NULL},
	};
	luaL_newlib(L, l);
	return 1;
}
