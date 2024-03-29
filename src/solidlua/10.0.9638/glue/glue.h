﻿#pragma once
#include "include_lua.h"
#include "include_solid.h"

#define GLUE_BEGIN_NS namespace glue {
#define GLUE_END_NS }

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

typedef SolidFramework::Plumbing::LicensePermissions LicensePermissions;
typedef SolidFramework::Converters::Plumbing::TextRecovery TextRecovery;
typedef SolidFramework::Converters::Plumbing::TextRecoveryNSE TextRecoveryNSE;
typedef SolidFramework::Converters::Plumbing::TextRecoveryEngine TextRecoveryEngine;
typedef SolidFramework::Converters::Plumbing::TextRecoveryEngineNse TextRecoveryEngineNse;
typedef SolidFramework::Converters::Plumbing::ImageAnchoringMode ImageAnchoringMode;
typedef SolidFramework::Converters::Plumbing::ReconstructionMode ReconstructionMode;
typedef SolidFramework::Converters::Plumbing::HeaderAndFooterMode HeaderAndFooterMode;
typedef SolidFramework::Converters::Plumbing::FootnotesMode FootnotesMode;
typedef SolidFramework::Converters::Plumbing::MarkupAnnotConversionType MarkupAnnotConversionType;

enum class ConvertProperties
{
	/*!
	 输出文件类型。[glue::DocumentType]
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
	 进行OCR时的语言。默认为空。[string]
	*/
	TextRecoveryLanguage,

	/*!
	 进行OCR转换时的策略。[glue::TextRecovery]
	*/
	TextRecoveryType,

	/*!
	 进行非标准编码(NSE)OCR转换时的策略。[glue::TextRecoveryNSE]
	*/
	TextRecoveryNseType,

	/*!
	 进行OCR转换的引擎。[glue::TextRecoveryEngine]
	*/
	TextRecoveryEngine,

	/*!
	 进行非标准编码(NSE)OCR转换时的引擎。[glue::TextRecoveryEngineNse]
	*/
	TextRecoveryEngineNse,

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
	 矢量图是否自动转换为位图。[bool]
	*/
	GraphicsAsImages,

	/*!
	 是否应该恢复没有描边或填充的文字。[bool]
	*/
	KeepInvisibleText,

	/*!
	 当背景和文字颜色一致时，是否要保持文字。[bool]
	*/
	KeepBackgroundColorText,

	/*!
	 PDF文档的密码。[string]
	*/
	Password,

	/*!
	 图像锚点模式。[glue::ImageAnchoringMode]
	*/
	ImageAnchoringMode,

	/*!
	 文档重构模式。[glue::ReconstructionMode]
	*/
	ReconstructionMode,

	/*!
	 页眉页脚模式。[glue::HeaderAndFooterMode]
	*/
	HeaderAndFooterMode,

	/*!
	 脚注模式。[glue::FootnotesMode]
	*/
	FootnotesMode,

	/*!
		注解模式。[glue::MarkupAnnotConversionType]
	*/
	MarkupAnnotConversionType,

	/*!
	 设置文档输出范围时，可输入形如1-2,3,4的字符串。[string]
	 获取文档输出范围时，将获取一个文档页码的列表。[int array]
	*/
	PageRange,
};

extern "C"
{
	void solidlua_appendargs(const char*);
	void solid_openlibs(lua_State* L);
	void solid_close();

	//! 获取程序的命令行参数
	/*!
		r1 [string array]: 命令行参数列表
	*/
	GLUE_SOLID_API int args(lua_State* L);

	//! 初始化solid sdk。
	/*!
	 p1 [string]: solid sdk路径
	 p2 [string]: solid 授权name
	 p3 [string]: solid 授权email
	 p4 [string]: solid 授权organization
	 p5 [string]: solid 授权码
	 r1 [bool]: 是否成功
	*/
	GLUE_SOLID_API int platform_init(lua_State* L);

	//! 使用默认的验证信息初始化solid sdk。
	/*!
	 p1 [string]: solid sdk路径
	 r1 [bool]: 是否成功
	*/
	GLUE_SOLID_API int platform_init_preset(lua_State* L);

	//! 查询是否拥有某个能力。
	/*
	 p1 [int]: 需要查询的某个能力，参考参考glue::LicensePermissions。
	 r1 [bool]: 是否拥有这个能力。
	*/
	GLUE_SOLID_API int license_allows(lua_State* L);

	//! 初始化一个OCR环境。如果不调用，则无法使用OCR相关功能。
	/*
	 p1 [string]: OCR训练数据文件夹。
	 r1 [string array]: 返回当前支持的OCR语言。
	*/
	GLUE_SOLID_API int ocr_init(lua_State* L);

	//! 创建一个转换器。在进行任何操作之前，应该先创建一个转换器。
	/*
	 p1 [int]: 转换器类型。
	    目前类型仅支持传入0，表示PDF转Word文件。
	 r1 [int]: 当前创建的转换器索引。
	*/
	GLUE_SOLID_API int converters_new(lua_State* L);

	//! 选择一个转换器。
	/*
	 p1 [int]: 转换器索引。如果转换器不存在，则选择失败。
	 r1 [bool]: 是否选择转换器成功。
	*/
	GLUE_SOLID_API int converters_select(lua_State* L);

	//! 开始转换。
	/*
	 p1 [string]: 文件输入路径
	 p2 [string]: 文件输出路径
	 r1 [int]: 转换结果，参考SolidFramework::Converters::Plumbing::ConversionStatus
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