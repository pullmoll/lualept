require "lua/tools"

header("Main program")
print("Hello, world!")

--[[[
local pdffile = tmpdir .. "/multi.pdf"
local pos = "first"
local title = "Converted JPEG"
local pdd = nil
local xp = 0
local yp = 0
local res = 150

for i = 2, #arg do
	local base = basename(arg[i])
	if i == #arg then
		pos = "last"
	end
	print(arg[i], pos)
	if pos == "first" then
		ppd = Pix(arg[i]):ConvertToPdf("jpeg", 90, pdffile, xp, yp, res, title, nil, pos)
	else
		ppd = Pix(arg[i]):ConvertToPdf("jpeg", 90, pdffile, xp, yp, res, title, ppd, pos)
	end
	pos = "next"
end
--]]

local res = io.popen("mkdir " .. tmpdir)
print("mkdir " .. tmpdir, res:read("*a"))
res.close()

print(pad("LuaLept"), LuaLept)
LuaLept:DebugOn("udata", "array")
print(pad("ll:Debug()"), LuaLept:Debug())
print(pad("ll:Version()"), LuaLept:Version())
print(pad("ll:Version('Lua')"), LuaLept:Version('Lua'))
print(pad("ll:Version('Leptonica')"), LuaLept:Version('Leptonica'))

print(pad("global sa"), sa)
print(pad("global box"), box)

-- require "lua/aset"
-- require "lua/amap"
-- require "lua/bbuffer"
require "lua/bmf"
-- require "lua/box"
-- require "lua/pta"
-- require "lua/numa"
-- require "lua/dna"
require "lua/pix"
-- require "lua/pix2"
-- require "lua/pix3"
-- require "lua/fpix"
-- require "lua/dpix"

---[[
local pix = Pix(4,4,32)
pix:SetAllArbitrary("Medium Violet Red")
pix:SetPixel(1, 1, "Blue")
pix:SetPixel(2, 2, "Yellow")

local pixc = PixComp(pix);
print (pad("pixc = PixComp(pix)"), pixc)
local rootname = tmpdir .. "/pixc"
local ok = pixc:WriteFile(rootname)
print (pad("pixcWriteFile(" .. rootname ..")"), pixc)
--]]

bytes = "The quick brown \0\1\2\3 fox jumps over the lazy dog! 1234567890? - ryryryry\0\0\x00"
ok = true

hex_dump(bytes)
