# solidlua
An Lua wrapper for solid framework sdk.

Here is a demo of lua script:

canLoadSolid = Solid.platform_init(
	"G:\\Win32", -- Solid SDK Path
	"Name",
	"Email",
	"Organziation",
	"Code")

if (not canLoadSolid) then
	print("Cannot load solid sdk.")
end

Solid.converters_new(0)
Solid.converters_setProperty(Solid.ConvertProperties.DetectTables, false) -- Treats tables as plain text
Solid.converters_convert("G:\\cacm.pdf", "G:\\cacm2.docx")