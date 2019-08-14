#pragma once
#include "include_lua.h"
#include "include_solid.h"

#define GLUE_BEGIN_NS namespace glue {
#define GLUE_END_NS }

#define GLUE_BEGIN_EXTERN_C extern "C" {
#define GLUE_BEGIN_END_C }

GLUE_BEGIN_NS

#define GLUE_SOLID_API

enum DocumentType
{
	WordML = SolidFramework::Converters::Plumbing::WordDocumentType::WordML,
	Rtf = SolidFramework::Converters::Plumbing::WordDocumentType::Rtf,
	Txt = SolidFramework::Converters::Plumbing::WordDocumentType::Txt,
	Doc = SolidFramework::Converters::Plumbing::WordDocumentType::Doc,
	Docx = SolidFramework::Converters::Plumbing::WordDocumentType::DocX,
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

	/*!
	 是否自动旋转。[bool]
	*/
	AutoRotate,

	/*!
	 是否标记可疑文字。对于一些转换成功率较低的文字部分，设置是否用黄色底色标记出来。
	 只有OCR可用时词功能才有用。
	*/
	TextRecoverySuspects,

	/*!
	 是否检测软连字符。[bool]
	*/
	DetectSoftHyphens,

	/*!
	 是否在PDF损坏时不进行修复而继续进行转换。[bool]
	*/
	NoRepairing,

	/*!
	 矢量图是否自动转换为位图。
	*/
	GraphicsAsImages,

	/*!
	 设置是否应该恢复没有描边或填充的文字。[bool]
	*/
	KeepInvisibleText,

	/*!
	 当背景和文字颜色一致时，是否要保持文字。[bool]
	*/
	KeepBackgroundColorText,

	/*!
	 设置PDF文档的密码。[string]
	*/
	Password,
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
	 所有生成的选项由此函数设置。
	 p1 [int]: 设置属性的类型
	 p2 [bool | string]: 设置的属性的值。具体的值请查阅glue::ConvertProperties。
	 r1 [bool]: 值是否设置成功。不成功可能是转换器不支持导致。
	*/
	GLUE_SOLID_API int converters_setProperty(lua_State* L);

	//! 设置输出选项
	/*
	 查询输出的选项。
	 p2 [int]: 需要查询的选项。具体的值请查阅glue::ConvertProperties。
	 r1 [int, string, bool] 如果成功的话，获取到的值。
	 r2 [bool]: 值是否获取成功。不成功可能是转换器不支持导致。
	*/
	GLUE_SOLID_API int converters_getProperty(lua_State* L);
}

GLUE_END_NS