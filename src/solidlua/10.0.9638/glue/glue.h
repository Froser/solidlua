#pragma once
#include "include_lua.h"

#define GLUE_BEGIN_NS namespace glue {
#define GLUE_END_NS }

#define GLUE_BEGIN_EXTERN_C extern "C" {
#define GLUE_BEGIN_END_C }

GLUE_BEGIN_NS

#define GLUE_SOLID_API

enum DocumentType
{
	WordML,
	Rtf,
	Txt,
	Doc,
	Docx,
};

enum class ConvertProperties
{
	/*!
	  输出文件类型。[glue::DocumentType, glue::Docx]
	*/
	OutputType,

	/*!
	  是否检测文档中的表格中的内容。[bool]
	*/
	DetectToc,

	/*!
	  是否检测文档中的列表。[bool]
	*/
	DetectLists,

	/*!
	  是否文档中的表格应当视为表格，而不是纯文本。 [bool]
	*/
	DetectTables,

	/*!
	  是否检测文档中的Tagged表格。[bool]
	*/
	DetectTaggedTables,

	/*!
	  是否检测文档中的跨页表。[bool]
	*/
	DetectTiledPages,

	/*!
	  是否检测文档中的样式。[bool]
	*/
	DetectStyles,

	/*!
	  是否检测文档中的语言。[bool]
	*/
	DetectLanguage,

	/*!
	  是否应该保持字符间距。[bool]
	*/
	KeepCharacterSpacing,

	/*!
	  是否将字符调整到平均大小。一定要为true，否则输出效果和PDF会不一样。[bool]
	*/
	AverageCharacterScaling,

	/*!
	  是否合并段落缩进。[bool]
	*/
	SupportRightToLeftWritingDirection,
};

extern "C"
{
	void solid_openlibs(lua_State* L);
	void solid_close();

	//! 初始化solid sdk。
	/*
	 p1 [string]: solid sdk路径
	 p2 [string]: solid 授权name
	 p3 [string]: solid 授权email
	 p4 [string]: solid 授权organization
	 p5 [string]: solid 授权码
	 r1 [bool]: 是否成功
	*/
	GLUE_SOLID_API int platform_init(lua_State* L);

	//! 创建一个转换器。在进行任何操作之前，应该先创建一个转换器。
	/*
	 p1 [int]: 转换器类型。
	    目前类型仅支持传入0，表示PDF转Word文件。
	*/
	GLUE_SOLID_API int converters_new(lua_State* L);

	//! 开始转换。
	/*
	 p1 [string]: 文件输入路径
	 p2 [string]: 文件输出路径
	*/
	GLUE_SOLID_API int converters_convert(lua_State* L);

	//! 设置转换选项
	/*
	 所有生成的选项由此函数
	 p1 [int]: 设置属性的类型
	 p2 [bool, int]: 设置的属性的值。具体的值请查阅glue::ConvertProperties。
	 r1 [bool]: 值是否设置成功。不成功可能是转换器不支持导致。
	*/
	GLUE_SOLID_API int converters_setProperty(lua_State* L);
}

GLUE_END_NS