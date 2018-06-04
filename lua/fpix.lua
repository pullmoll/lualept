require "lua/tools"

local filename = tmpdir .. "/fpix-test.fpix"
local filename2 = tmpdir .. "/fpix-test.png"
header("FPix")
local fpix = FPix(160,100)
print(pad("fpix = FPix(160,100)"), fpix)
fpix:SetResolution(150,150)
print(pad("fpix:SetResolution(150,150)"), fpix)
fpix:SetAllArbitrary(0.95)
for x=0,159 do
	local y = 100*x//160
	fpix:SetPixel(x, y, 0.55)
end
local ok = fpix:Write(filename)
print(pad("fpix:Write('" .. filename .."')"), ok)

local fpix2 = FPix(filename)
print(pad("fpix2 = FPix('" .. filename .. "')"), fpix2)
local fpix3 = FPix(fpix2)
print(pad("fpix3 = FPix(fpix2)"), fpix3)

local pix = fpix2:ThresholdToPix(0.80)
pix:Write(filename2)
print(pad("pix:Write('" .. filename2 .."')"), ok)

header()
