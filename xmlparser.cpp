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

// node
static void ParseNode(lua_State *L, xml_node<>* pNode)
{
	xml_attribute<> *pAttribute = pNode->first_attribute();
	lua_newtable(L);	// node nt
	lua_pushvalue(L, -1);	// node nt, nt
	lua_setfield(L, -3, "attribute"); // node, node.attribute
	while (pAttribute)
	{
		lua_pushstring(L, pAttribute->value()); // node, node.attribute, value
		lua_setfield(L, -2, pAttribute->name());	// node, node.attribute
		pAttribute = pAttribute->next_attribute();
	}
	lua_pop(L, 1);	// node

	xml_node<> *pChildNode = pNode->first_node();
	lua_newtable(L);	// node, nt
	lua_pushvalue(L, -1);	// node, nt, nt
	lua_setfield(L, -3, "children"); // node, node.children
	while (pChildNode)
	{
		lua_pushstring(L, pChildNode->name());	// node, node.children, name
		lua_rawget(L, -2);	// node, node.children, node.children[name]
		if (lua_type(L, -1) == LUA_TNIL) {
			lua_pop(L, 1);	// node, node.children
			lua_newtable(L);	// node, node.children, nt
			lua_pushvalue(L, -1);	// node, node.children, nt, nt
			lua_setfield(L, -3, pChildNode->name()); // node, node.children, node.children[name]
		}

		lua_newtable(L);	// node, node.children, node.children[name], nt
		lua_pushvalue(L, -1);	// node, node.children, node.children[name], nt, nt
		lua_rawseti(L, -3, lua_rawlen(L, -3) + 1);	// node, node.children, node.children[name], node.children[name][n]
		ParseNode(L, pChildNode);
		lua_pop(L, 2);	// node, node.children
		pChildNode = pChildNode->next_sibling();
	}
	lua_pop(L, 1);	// node
}

static void ParseData(lua_State *L, char *pFileData) {
	xml_document<> XmlDoc;

	lua_newtable(L);	// root
	XmlDoc.parse<0>(pFileData);
	xml_node<> *pRootNode = XmlDoc.first_node();
	while (pRootNode)
	{
		lua_pushstring(L, pRootNode->name());	// root, node_name
		lua_rawget(L, -2);	// root, root[node_name]
		if (lua_type(L, -1) == LUA_TNIL) {
			lua_pop(L, 1);	// root
			lua_newtable(L);	// root, nt
			lua_pushvalue(L, -1); // root, nt, nt
			lua_setfield(L, -3, pRootNode->name());	// root, root[node_name]
		}

		lua_newtable(L);	// root, root[node_name], nt
		lua_pushvalue(L, -1);	// root, root[node_name], nt, nt
		lua_rawseti(L, -3, lua_rawlen(L, -3) + 1);	// root, root[node_name], root[node_name][n]
		ParseNode(L, pRootNode);
		lua_pop(L, 2);	// root
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
