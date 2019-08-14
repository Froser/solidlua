#include "stdafx.h"
#include <include_solid.h>

#define TEST_OUTPUT 0
#if TEST_OUTPUT
#define SOLID_SDK_UNLOCKCODE				L""
#define SOLID_SDK_NAME						L""
#define SOLID_SDK_ORGANIZATION				L""
#define SOLID_SDK_EMAIL						L""
#else
#include "include_lua.h"
#include <codecvt>
#endif

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Not enough arguments.");
		return -1;
	}

	std::string filename = argv[1];

#if TEST_OUTPUT
	bool isSupport = SolidFramework::Platform::Platform::SetSupportDirectory(L"G:\\Win32");
	if (isSupport)
	{
		SolidFramework::License::Import(SOLID_SDK_NAME,
			SOLID_SDK_EMAIL,
			SOLID_SDK_ORGANIZATION,
			SOLID_SDK_UNLOCKCODE);
		SolidFramework::Converters::PdfToWordConverter converter;

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> c;
		std::wstring wideFilename = c.from_bytes(filename);
		converter.AddSourceFile(wideFilename);
		converter.SetOutputType(SolidFramework::Converters::Plumbing::WordDocumentType::DocX);
		converter.ConvertTo(wideFilename + L".docx", true);
		return 0;
	}
#else
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	glue::solid_openlibs(L);

	if (luaL_loadfile(L, filename.c_str()) == LUA_OK)
	{
		if (!lua_pcall(L, 0, LUA_MULTRET, 0) == LUA_OK)
		{
			std::string message = lua_tostring(L, -1);
			printf("Error occurs: %s\n", message.c_str());
			lua_close(L);
			return -1;
		}
	}
	else
	{
		printf("Load Failed. File: '%s'.\nReason: %s\n", filename.c_str(), lua_tostring(L, lua_gettop(L)));
		lua_close(L);
		return -1;
	}
	lua_close(L);
	return 0;
	
#endif
}