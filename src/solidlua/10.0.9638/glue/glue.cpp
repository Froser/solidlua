#include "stdafx.h"
#include "glue.h"
#include <codecvt>
#include <include_solid.h>

#define GLUE_CHECK_ARGUMENTS_COUNT(L, expected) \
	{ int __count__ = lua_gettop(L); \
	if (__count__ != expected) \
		return luaL_error(L, "In function %s: %d arguments count are expected, but current is %d.", __function__, expected, __count__); }

#define GLUE_FUNCTION_NAME(name) constexpr char* __function__ = #name

#define GLUE_GET_ARG(expr, err) if (!(expr)) return err;

#define GLUE_CHECK_CONVERTER(L) if (!s_session.converter) return luaL_error(L, "You have to create a converter before convert.");

struct glue_Int_Constants
{
	const char* name;
	lua_Integer value;
};

enum ConverterType
{
	PdfToWord,
};

namespace
{
	struct session_t
	{
		SolidFramework::Converters::Converter* converter;
		ConverterType type;
	} s_session;

	static const luaL_Reg solid_funcs[] = {
		{ "platform_init", glue::platform_init },
		{ "converters_new", glue::converters_new },
		{ "converters_convert", glue::converters_convert },
		{ "converters_setProperty", glue::converters_setProperty },
		{ "converters_getProperty", glue::converters_getProperty },
		{ NULL, NULL }
	};

	static const glue_Int_Constants constants[] = {
		{ "OutputType", static_cast<lua_Integer>(glue::ConvertProperties::OutputType) },
		{ "DetectToc", static_cast<lua_Integer>(glue::ConvertProperties::DetectToc) },
		{ "DetectLists", static_cast<lua_Integer>(glue::ConvertProperties::DetectLists) },
		{ "DetectTables", static_cast<lua_Integer>(glue::ConvertProperties::DetectTables) },
		{ "DetectTaggedTables", static_cast<lua_Integer>(glue::ConvertProperties::DetectTaggedTables) },
		{ "DetectTiledPages", static_cast<lua_Integer>(glue::ConvertProperties::DetectTiledPages) },
		{ "DetectStyles", static_cast<lua_Integer>(glue::ConvertProperties::DetectStyles) },
		{ "DetectLanguage", static_cast<lua_Integer>(glue::ConvertProperties::DetectLanguage) },
		{ "KeepCharacterSpacing", static_cast<lua_Integer>(glue::ConvertProperties::KeepCharacterSpacing) },
		{ "AverageCharacterScaling", static_cast<lua_Integer>(glue::ConvertProperties::AverageCharacterScaling) },
		{ "SupportRightToLeftWritingDirection", static_cast<lua_Integer>(glue::ConvertProperties::SupportRightToLeftWritingDirection) },
		{ "AutoRotate", static_cast<lua_Integer>(glue::ConvertProperties::AutoRotate) },
		{ "TextRecoverySuspects", static_cast<lua_Integer>(glue::ConvertProperties::TextRecoverySuspects) },
		{ "DetectSoftHyphens", static_cast<lua_Integer>(glue::ConvertProperties::DetectSoftHyphens) },
		{ "NoRepairing", static_cast<lua_Integer>(glue::ConvertProperties::NoRepairing) },
		{ "GraphicsAsImage", static_cast<lua_Integer>(glue::ConvertProperties::GraphicsAsImages) },
		{ "KeepInvisibleText", static_cast<lua_Integer>(glue::ConvertProperties::KeepInvisibleText) },
		{ "KeepBackgroundColorText", static_cast<lua_Integer>(glue::ConvertProperties::KeepBackgroundColorText) },
		{ "Password", static_cast<lua_Integer>(glue::ConvertProperties::Password) },


		{ "WordML", static_cast<lua_Integer>(glue::DocumentType::WordML) },
		{ "Rtf", static_cast<lua_Integer>(glue::DocumentType::Rtf) },
		{ "Txt", static_cast<lua_Integer>(glue::DocumentType::Txt) },
		{ "Doc", static_cast<lua_Integer>(glue::DocumentType::Doc) },
		{ "Docx", static_cast<lua_Integer>(glue::DocumentType::Docx) },
		{ NULL, NULL }
	};

	int solidopen(lua_State *L)
	{
		luaL_newlib(L, solid_funcs);
		const glue_Int_Constants* s;
		for (s = constants; s->name; s++) {
			lua_pushinteger(L, s->value);
			lua_setfield(L, -2, s->name);
		}
		return 1;
	}

	static const luaL_Reg loadedlibs[] = {
		{ "Solid", solidopen },
		{ NULL, NULL }
	};

	std::wstring towstring(const std::string& s)
	{
		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> c;
		return c.from_bytes(s);
	}

	bool getstring(lua_State* L, int* err, const std::string& function, int index, std::string& str)
	{
		if (!lua_isstring(L, index))
		{
			int e = luaL_error(L, "In function %s: Wrong argument type at argument %d. The expected type is string.", function.c_str(), index);
			if (err) *err = e;
			return false;
		}
		str = lua_tostring(L, index);
		return true;
	}

	bool getint(lua_State* L, int* err, const std::string& function, int index, lua_Integer& result)
	{
		if (!lua_isinteger(L, index))
		{
			int e = luaL_error(L, "In function %s: Wrong argument type at argument %d. The expected type is integer.", function.c_str(), index);
			if (err) *err = e;
			return false;
		}
		result = lua_tointeger(L, index);
		return true;
	}

	bool getbool(lua_State* L, int* err, const std::string& function, int index, bool& result)
	{
		if (!lua_isboolean(L, index))
		{
			int e = luaL_error(L, "In function %s: Wrong argument type at argument %d. The expected type is boolean.", function.c_str(), index);
			if (err) *err = e;
			return false;
		}
		result = !!lua_toboolean(L, index);
		return true;
	}

	void disposeConverter()
	{
		if (s_session.converter)
			delete s_session.converter;
	}
}

void glue::solid_openlibs(lua_State* L)
{
	const luaL_Reg *lib;
	for (lib = loadedlibs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);  /* remove lib */
	}
}

void glue::solid_close()
{
	disposeConverter();
}

int glue::platform_init(lua_State* L)
{
	GLUE_FUNCTION_NAME(platform_init);
	GLUE_CHECK_ARGUMENTS_COUNT(L, 5);
	int err;
	std::string path, name, email, organization, unlockcode;
	GLUE_GET_ARG(getstring(L, &err, __function__, 1, path), err);
	GLUE_GET_ARG(getstring(L, &err, __function__, 2, name), err);
	GLUE_GET_ARG(getstring(L, &err, __function__, 3, email), err);
	GLUE_GET_ARG(getstring(L, &err, __function__, 4, organization), err);
	GLUE_GET_ARG(getstring(L, &err, __function__, 5, unlockcode), err);

	bool isSupport = SolidFramework::Platform::Platform::SetSupportDirectory(towstring(path));
	if (isSupport)
	{
		try
		{
			SolidFramework::License::Import(towstring(name),
				towstring(email),
				towstring(organization),
				towstring(unlockcode));
		}
		catch (std::runtime_error)
		{
			isSupport = false;
		}
	}
	lua_pushboolean(L, isSupport);
	return 1;
}

int glue::converters_new(lua_State* L)
{
	GLUE_FUNCTION_NAME(converters_new);
	GLUE_CHECK_ARGUMENTS_COUNT(L, 1);
	int err;
	lua_Integer type;
	GLUE_GET_ARG(getint(L, &err, __function__, 1, type), err);
	if (type != PdfToWord)
		luaL_error(L, "Converter type is not supported.");

	disposeConverter();
	s_session.type = PdfToWord;
	if (!s_session.converter)
	{
		try
		{
			s_session.converter = new SolidFramework::Converters::PdfToWordConverter();
		}
		catch (SolidFramework::InvalidLicenseException)
		{
			luaL_error(L, "Invalid license.");
		}
	}
	return 0;
}

GLUE_SOLID_API int glue::converters_convert(lua_State* L)
{
	GLUE_CHECK_CONVERTER(L);
	GLUE_FUNCTION_NAME(converters_convert);
	GLUE_CHECK_ARGUMENTS_COUNT(L, 2);
	int err;
	std::string input, output;
	GLUE_GET_ARG(getstring(L, &err, __function__, 1, input), err);
	GLUE_GET_ARG(getstring(L, &err, __function__, 2, output), err);

	s_session.converter->AddSourceFile(towstring(input));
	s_session.converter->ConvertTo(towstring(output), true);
	return 0;
}

#define GLUE_FAST_SETPROPERTY_BOOL(type, method) \
	{ bool v; \
	GLUE_GET_ARG(getbool(L, &err, __function__, 2, v), err); \
	type* converter = static_cast<type*>(s_session.converter); \
	converter-> method (v); }

#define GLUE_FAST_SETPROPERTY_STRING(type, method) \
	{ std::string v; \
	GLUE_GET_ARG(getstring(L, &err, __function__, 2, v), err); \
	type* converter = static_cast<type*>(s_session.converter); \
	converter-> method (towstring(v)); }

#define GLUE_FAST_SETPROPERTY_ENUM(type, method, argtype) \
	{ lua_Integer v; \
	GLUE_GET_ARG(getint(L, &err, __function__, 2, v), err); \
	type* converter = static_cast<type*>(s_session.converter); \
	converter-> method (static_cast<argtype>(v)); }

#define GLUE_FAST_GETPROPERTY_BOOL(type, method) \
	{ ret = 1; type* converter = static_cast<type*>(s_session.converter); \
	lua_pushboolean(L, converter-> method ()); }

#define GLUE_FAST_GETPROPERTY_STRING(type, method) \
	{ lua_pushnil(L); lua_assert(false);  }

#define GLUE_FAST_GETPROPERTY_ENUM(type, method) \
	{ ret = 1; type* converter = static_cast<type*>(s_session.converter); \
	lua_pushinteger(L, static_cast<lua_Integer>(converter-> method ())); }

GLUE_SOLID_API int glue::converters_setProperty(lua_State* L)
{
	GLUE_CHECK_CONVERTER(L);
	GLUE_FUNCTION_NAME(converters_setProperty);
	GLUE_CHECK_ARGUMENTS_COUNT(L, 2);
	int err;
	lua_Integer type;
	GLUE_GET_ARG(getint(L, &err, __function__, 1, type), err);
	bool ret = true;

	ConvertProperties propertyType = static_cast<ConvertProperties>(type);
	switch (propertyType)
	{
	case ConvertProperties::OutputType:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_ENUM(SolidFramework::Converters::PdfToWordConverter, SetOutputType, SolidFramework::Converters::Plumbing::WordDocumentType);
		break;
	}
	case ConvertProperties::DetectToc:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectToc);
		break;
	}
	case ConvertProperties::DetectLists:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectLists);
		break;
	}
	case ConvertProperties::DetectTables:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectTables);
		break;
	}
	case ConvertProperties::DetectTaggedTables:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectTaggedTables);
		break;
	}
	case ConvertProperties::DetectTiledPages:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectTiledPages);
		break;
	}
	case ConvertProperties::DetectStyles:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectStyles);
		break;
	}
	case ConvertProperties::DetectLanguage:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectLanguage);
		break;
	}
	case ConvertProperties::KeepCharacterSpacing:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetKeepCharacterSpacing);
		break;
	}
	case ConvertProperties::AverageCharacterScaling:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetAverageCharacterScaling);
		break;
	}
	case ConvertProperties::SupportRightToLeftWritingDirection:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetSupportRightToLeftWritingDirection);
		break;
	}
	case ConvertProperties::AutoRotate:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetAutoRotate);
		break;
	}
	case ConvertProperties::TextRecoverySuspects:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetTextRecoverySuspects);
		break;
	}
	case ConvertProperties::DetectSoftHyphens:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetDetectSoftHyphens);
		break;
	}
	case ConvertProperties::NoRepairing:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetNoRepairing);
		break;
	}
	case ConvertProperties::GraphicsAsImages:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetGraphicsAsImages);
		break;
	}
	case ConvertProperties::KeepInvisibleText:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetKeepInvisibleText);
		break;
	}
	case ConvertProperties::KeepBackgroundColorText:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, SetKeepBackgroundColorText);
		break;
	}
	case ConvertProperties::Password:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_SETPROPERTY_STRING(SolidFramework::Converters::PdfToWordConverter, SetPassword);
		break;
	}
	default:
		luaL_error(L, "In function %s: Property type %d is not supported.", __function__, type);
	}

	lua_pushboolean(L, ret);
	return 1;
}

GLUE_SOLID_API int glue::converters_getProperty(lua_State* L)
{
	GLUE_CHECK_CONVERTER(L);
	GLUE_FUNCTION_NAME(converters_getProperty);
	GLUE_CHECK_ARGUMENTS_COUNT(L, 1);
	int err;
	lua_Integer type;
	GLUE_GET_ARG(getint(L, &err, __function__, 1, type), err);

	int ret = 0;
	ConvertProperties propertyType = static_cast<ConvertProperties>(type);
	switch (propertyType)
	{
	case ConvertProperties::OutputType:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_ENUM(SolidFramework::Converters::PdfToWordConverter, GetOutputType);
		break;
	}
	case ConvertProperties::DetectToc:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectToc);
		break;
	}
	case ConvertProperties::DetectLists:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectLists);
		break;
	}
	case ConvertProperties::DetectTables:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectTables);
		break;
	}
	case ConvertProperties::DetectTaggedTables:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectTaggedTables);
		break;
	}
	case ConvertProperties::DetectTiledPages:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectTiledPages);
		break;
	}
	case ConvertProperties::DetectStyles:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectStyles);
		break;
	}
	case ConvertProperties::DetectLanguage:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectLanguage);
		break;
	}
	case ConvertProperties::KeepCharacterSpacing:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetKeepCharacterSpacing);
		break;
	}
	case ConvertProperties::AverageCharacterScaling:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetAverageCharacterScaling);
		break;
	}
	case ConvertProperties::SupportRightToLeftWritingDirection:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetSupportRightToLeftWritingDirection);
		break;
	}
	case ConvertProperties::AutoRotate:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetAutoRotate);
		break;
	}
	case ConvertProperties::TextRecoverySuspects:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetTextRecoverySuspects);
		break;
	}
	case ConvertProperties::DetectSoftHyphens:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetDetectSoftHyphens);
		break;
	}
	case ConvertProperties::NoRepairing:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetNoRepairing);
		break;
	}
	case ConvertProperties::GraphicsAsImages:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetGraphicsAsImages);
		break;
	}
	case ConvertProperties::KeepInvisibleText:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetKeepInvisibleText);
		break;
	}
	case ConvertProperties::KeepBackgroundColorText:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_BOOL(SolidFramework::Converters::PdfToWordConverter, GetKeepBackgroundColorText);
		break;
	}
	case ConvertProperties::Password:
	{
		if (s_session.type == PdfToWord)
			GLUE_FAST_GETPROPERTY_STRING(SolidFramework::Converters::PdfToWordConverter, GetPassword);
		break;
	}
	default:
		luaL_error(L, "In function %s: Property type %d is not supported.", __function__, type);
	}

	lua_pushboolean(L, ret);
	return 2;
}
