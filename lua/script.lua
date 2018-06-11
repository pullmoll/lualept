require "lua/tools"

header("Main program")
print("Hello, world!")

local pdffile = tmpdir .. "/multi.pdf"
local pos = "first"
local pdd = nil

for i = 3, #arg do
	local pix = Pix(arg[i])
	if i == #arg then
		pos = "last"
	end
	print(arg[i], pos)
	pdd = pix:ConvertToPdf("png", 50, pdffile, 0, 0, 150, arg[i], pdd, pos)
	pix:Destroy()
	pos = "next"
end

local res = io.popen("mkdir " .. tmpdir)
print("mkdir " .. tmpdir, res:read("*a"))
res.close()

LuaLept:DebugOn("udata", "array")
print(pad("LuaLept:Debug()"), LuaLept:Debug())
print(pad("LuaLept:Version()"), LuaLept:Version())
print(pad("LuaLept:Version('Lua')"), LuaLept:Version('Lua'))
print(pad("LuaLept:Version('Leptonica')"), LuaLept:Version('Leptonica'))

print(pad("global sa"), sa)
print(pad("global box"), box)

--require "lua/aset"
--require "lua/amap"
--require "lua/bbuffer"
--require "lua/bmf"
--require "lua/box"
--require "lua/pta"
--require "lua/numa"
--require "lua/dna"
--require "lua/pix"
--require "lua/pix2"
--require "lua/pix3"
--require "lua/fpix"
--require "lua/dpix"

local pix = Pix(4,4,32)
pix:SetAllArbitrary("Medium Violet Red")
pix:SetPixel(1, 1, "Blue")
pix:SetPixel(2, 2, "Yellow")

local pixc = PixComp(pix);
print (pad("pixc = PixComp(pix)"), pixc)
local rootname = tmpdir .. "/pixc"
local ok = pixc:WriteFile(rootname)
print (pad("pixcWriteFile(" .. rootname ..")"), pixc)

pix:View("4x4 32bpp",0,0,64.0)
bytes = "The quick brown \0\1\2\3 fox jumps over the lazy dog! 1234567890? - ryryryry\0\0\x00"
ok = true

hex_dump(bytes)
